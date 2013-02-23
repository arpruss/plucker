/*
 * $Id: ramfile.c,v 1.26 2008/01/20 22:21:33 prussar Exp $
 *
 * Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 1998-2002, Mark Ian Lillywhite and Michael Nordstrom
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "const.h"
#include "debug.h"
#include "resourceids.h"
#include "doclist.h"
#include "document.h"
#include "metadocument.h"
#include "list.h"
#include "annotation.h"

#include "ramfile.h"

/* #define DEBUG_RECORDS */


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define UNFILED_CATEGORY    1
#define PDB_SUFFIX          ".pdb"
#define PDB_SUFFIX_LEN      4
#define IGNORE_INDEX      0xFFFF
#define ALL_DBS           NULL

/* MIME type for Plucker documents */
#define ViewerMIMEType  "application/prs.plucker"


/***********************************************************************
 *
 *      Internal types
 *
 ***********************************************************************/
typedef struct {
    DmOpenRef dbRef;
    UInt16    card;
    LocalID   id;
    MemHandle handle;
    UInt32    openCount;
    UInt16    index;
    Boolean   dirty;
} OpenRecordType;


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static Err OpenRAMDatabase( const LocalID dbID, const UInt16 cardNo,
                DmOpenRef* db, UInt16* version, Char* name, UInt32* date,
                UInt16* numRecords ) RAMFILE_SECTION;
static Err WritePluckerDocument( const void* dataP, UInt32* sizeP,
            void* userDataP ) RAMFILE_SECTION;
static void RAMRenameMetaDocument( Char* newName, UInt16 oldLocation,
                DocumentInfo* docInfo ) RAMFILE_SECTION;
static LocalID GetDocumentID( DocumentInfo *docInfo ) RAMFILE_SECTION;
static void AppendRecordInfo( DmOpenRef dbRef, UInt16 index,
                MemHandle handle, Boolean dirty ) RAMFILE_SECTION;
static OpenRecordType* FindRecordInfo( DmOpenRef dbRef, UInt16 index,
                MemHandle handle ) RAMFILE_SECTION;
static LocalID LocateDatabase( DmOpenRef ref, UInt16* cardP ) RAMFILE_SECTION;


/* These functions must be in the main code section */
static Err ReadPluckerDocument( void* dataP, UInt32* sizeP, void* userDataP );
static Boolean DeletePluckerDocument( const char* nameP, UInt16 version,
                UInt16 cardNo, LocalID dbID, void* userDataP );


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static DmOpenRef        ramDocument;
static DocumentInfo*    lastDocInfo;
static LinkedList       openRecordsList = NULL;



/*
   The following functions implement an NVFS-safe method of opening and
   closing records.  Calls to NewRecord(), OpenRecord(), ResizeRecord()
   should be paired with calls to CloseRecord().  The importance of these
   functions is that they are nestable so you can do
     OpenRecord();
     OpenRecord();
     CloseRecord();
     CloseRecord();
   on the same record.  Unfortunately DmGetRecord()/DmReleaseRecord() are
   not documented as having this functionality.  It is crucial that calls
   to these functions not be mixed with calls to the
   DmGet/DmResize/DmNew/DmRelease functions.  Furthermore, to ensure all
   records get closed, you should call CloseDatabase() in place of
   DmCloseDatabase().

   However, you must not open the same database twice at the same time with
   these functions.
 */


/* returns NULL if dbRef is NULL */
static LocalID LocateDatabase
    (
    DmOpenRef  dbRef,
    UInt16*    cardP
    )
{
    LocalID id;

    id = NULL;

    if ( dbRef != NULL )
        DmOpenDatabaseInfo( dbRef, &id, NULL, NULL, cardP, NULL );

    return id;
}



static OpenRecordType* FindRecordInfo
    (
    DmOpenRef  dbRef,
    UInt16     index,
    MemHandle  handle
    )
{
    UInt16 i;
    UInt16 size;
    LocalID id;
    UInt16  card;
    
    id = LocateDatabase( dbRef, &card );

/* TO DO: Optimize this.  Current performance is O(size^2) because of
   how ListGet() works.  Hopefully, size() is pretty small, though. */

    size = ListSize( openRecordsList );

    for ( i = 1 ; i <= size ; i++ ) {
         OpenRecordType* rec;

         rec = ListGet( openRecordsList, i );

         if ( ( dbRef == NULL || ( rec->id == id && rec->card == card ) ) &&
              ( index == IGNORE_INDEX || rec->index == index ) &&
              ( handle == NULL || rec->handle == handle ) ) {
             return rec;
         }
    }

    return NULL;
}



static void AppendRecordInfo
    (
    DmOpenRef dbRef,
    UInt16    index,
    MemHandle handle,
    Boolean   dirty
    )
{
    OpenRecordType* rec;
    UInt16          card;


    if ( openRecordsList == NULL ) {
        openRecordsList = ListCreate();
    }

    rec = SafeMemPtrNew( sizeof( OpenRecordType ) );

    rec->dbRef  = dbRef;
    rec->id     = LocateDatabase( dbRef, &card );
    rec->card   = card;
    rec->index  = index;
    rec->handle = handle;
    rec->dirty  = dirty;
    rec->openCount = 1;

    ListAppend( openRecordsList, rec );
}



void MarkRecordDirty
    (
    MemHandle  handle
    )
{
    OpenRecordType* rec;


    rec = FindRecordInfo( NULL, IGNORE_INDEX, handle );

    if ( rec != NULL )
        rec->dirty = true;
}




static void AdjustIndexes
    (
    DmOpenRef   dbRef,
    UInt16      fromIndex,
    Int16       delta
    )
{
    UInt16 i;
    UInt16 size;
    UInt16 card;
    LocalID id;


    id = LocateDatabase( dbRef, &card );

    size = ListSize( openRecordsList );

    for ( i = 1 ; i <= size ; i++ ) {
        OpenRecordType* rec;

        rec = ListGet( openRecordsList, i );

        if ( rec->id == id && rec->card == card && fromIndex <= rec->index )
            rec->index += delta;
    }
}




MemHandle NewRecord
    (
    DmOpenRef   dbRef,
    UInt16*     indexP,
    UInt32      size
    )
{
    MemHandle handle;


    AdjustIndexes( dbRef, *indexP, 1 );

    handle = DmNewRecord( dbRef, indexP, size );

    if ( handle != NULL )
        AppendRecordInfo( dbRef, *indexP, handle, true );

    return handle;
}



MemHandle ResizeRecord
    (
    DmOpenRef   dbRef,
    UInt16      index,
    UInt32      size
    )
{
    MemHandle       handle;
    OpenRecordType* rec;


    rec = FindRecordInfo( dbRef, index, NULL );

    if ( rec != NULL ) {
        DmReleaseRecord( dbRef, index, true );
        ListTakeOut( openRecordsList, rec );
        SafeMemPtrFree( rec );
    }

    handle = SafeDmResizeRecord( dbRef, index, size );

    if ( handle != NULL )
        AppendRecordInfo( dbRef, index, handle, true );

    return handle;
}



Err RemoveRecord
    (
    DmOpenRef       dbRef,
    UInt16          index
    )
{
    UInt16 i;
    UInt16 size;
    Err    err;
    UInt16 card;
    LocalID id;


    id = LocateDatabase( dbRef, &card );

    err = DmRemoveRecord( dbRef, index );

    if ( errNone != err )
        return err;

    size = ListSize( openRecordsList );

    for ( i = 1 ; i <= size ; i++ ) {
        OpenRecordType* rec;

        rec = ListGet( openRecordsList, i );

        if ( rec->id == id && rec->card == card ) {
            if ( rec->index == index ) {
                ListTakeOut( openRecordsList, rec );
                SafeMemPtrFree( rec );
                break;
            }
        }
    }

    AdjustIndexes( dbRef, index, -1 );

    return errNone;
}



/* Get a record, recording where it is. */
MemHandle OpenRecord
    (
    DmOpenRef       dbRef,
    UInt16          index
    )
{
    MemHandle handle;
    OpenRecordType* rec;

    rec = FindRecordInfo( dbRef, index, NULL );

    if ( rec != NULL ) {
        rec->openCount++;
#ifdef DEBUG_RECORDS
        if ( 10 < rec->openCount ) {
            SysFatalAlert( "Overopened record" );
        }
#endif

        return rec->handle;
    }

    handle = DmGetRecord( dbRef, index );

    if ( handle != NULL ) {
        AppendRecordInfo( dbRef, index, handle, false );
    }


    return handle;
}



void CloseRecord
    (
    MemHandle handle,
    Boolean   dirty
    )
{
    OpenRecordType* rec;
    

#ifdef DEBUG_RECORDS
    if ( handle == NULL )
        SysFatalAlert("Closing NULL handle.");
#endif

    rec = FindRecordInfo( NULL, IGNORE_INDEX, handle );

    if ( rec == NULL ) {
#ifdef DEBUG_RECORDS
        SysFatalAlert( "Overclosed record." );
#endif
        return;
    }

    rec->openCount--;
    rec->dirty = rec->dirty || dirty;

    if ( rec->openCount == 0 ) {
        DmReleaseRecord( rec->dbRef, rec->index, rec->dirty );
        ListTakeOut( openRecordsList, rec );
        SafeMemPtrFree( rec );
    }
}



Err CloseDatabase
    (
    DmOpenRef dbRef
    )
{
    UInt16  size;
    UInt16  i;

    size = ListSize( openRecordsList );

    for ( i = size ; 0 < i ; i-- ) {
         OpenRecordType* rec;

         rec = ListGet( openRecordsList, i );

         if ( dbRef == ALL_DBS || rec->dbRef == dbRef ) {
             DmReleaseRecord( rec->dbRef, rec->index, rec->dirty );
             ListTakeOut( openRecordsList, rec );
             SafeMemPtrFree( rec );
         }
    }

    if ( dbRef != NULL )
        return DmCloseDatabase( dbRef );
    else
        return errNone;

}



void DestroyOpenRecordsList( void )
{
    CloseDatabase( ALL_DBS );
    ListRelease( openRecordsList );
}



/* Open the specified database, return the result from the database call */
static Err OpenRAMDatabase
    (
    const LocalID   dbID,       /* database ID */
    const UInt16    cardNo,     /* card number database resides on */
    DmOpenRef*      db,         /* upon successful return, the access pointer */
    UInt16*         version,    /* upon successful return, the specific
                                   version number */
    Char*           name,       /* upon successful return, the name */
    UInt32*         date,       /* upon successful return, the creation date */
    UInt16*         numRecords  /* upon successful return, the number of
                                   records */
    )
{
    Err err;

    if ( dbID == 0 || db == NULL || version == NULL || name == NULL ||
         date == NULL || numRecords == NULL )
       return dmErrInvalidParam;

    *db = DmOpenDatabase( cardNo, dbID, dmModeReadWrite );
    if ( *db == NULL ) {
        *db = DmOpenDatabase( cardNo, dbID, dmModeReadOnly );
        if ( *db == NULL )
           return DmGetLastErr();
    }

    *numRecords = DmNumRecords( *db );

    err = DmDatabaseInfo( cardNo, dbID, name, NULL, version, date, 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL );
    
    return err;
}



/* Return ID for document in RAM */
static LocalID GetDocumentID
    (
    DocumentInfo* docInfo
    )
    /* THROWS */
{
    LocalID id;

    THROW_IF( docInfo == NULL, dmErrInvalidParam );

    id = DmFindDatabase( docInfo->cardNo, docInfo->name );

    THROW_IF( id == 0, DmGetLastErr() );

    return id;
}



/* Callback function for ExgDBWrite that writes out the Plucker document.
   Return error code from ExgSend, or 0 if there is no error */
static Err WritePluckerDocument
    (
    const void* dataP,      /* pointer to buffer containing the data, placed 
                               there by ExgDBWrite */
    UInt32*     sizeP,      /* the number of bytes placed in dataP by
                               ExgDBWrite. If not all data was sent in this
                               chunk, on exit, sizeP will contain the number
                               of bytes that was sent */
    void*       userDataP   /* pointer to the socket */
    )
{
    Err err;

    *sizeP = ExgSend( (ExgSocketType *) userDataP, dataP, *sizeP, &err );

    return err;
}



/* Callback function for ExgDBRead that reads the received Plucker document.
   Return error code from ExgReceive, or 0 if there is no error */
static Err ReadPluckerDocument
    (
    void*   dataP,      /* pointer to buffer containing the data, placed 
                           there by ExgDBWrite */
    UInt32* sizeP,      /* the number of bytes placed in dataP by
                           ExgDBWrite. If not all data was sent in this
                           chunk, on exit, sizeP will contain the number
                           of bytes that was sent */
    void*   userDataP   /* pointer to the socket */
    )
{
    Err err;

    *sizeP = ExgReceive( (ExgSocketType *) userDataP, dataP, *sizeP, &err );

    return err;
}



/* Callback function for ExgDBRead that handles the case where a Plucker
   document with an identical name already exists on the device.
   Currently, this functions always return false, i.e. it will not accept
   the received Plucker document. In a future version this will be changed
   to allow the user to rename the existing document, and/or send it to an
   external card */
static Boolean DeletePluckerDocument
    (
    const char* nameP,
    UInt16      version,
    UInt16      cardNo,
    LocalID     dbID,
    void*       userDataP
    )
{
    return false;
}



/* Open document */
Err OpenRAMDocument
    (
    DocumentInfo* docInfo
    )
{
    UInt32  created;
    UInt16  version;
    UInt16  cardNo;
    LocalID dbID;
    Err     err;

    if ( docInfo == NULL )
       return dmErrInvalidParam;

    dbID    = GetDocumentID( docInfo );
    cardNo  = docInfo->cardNo;
    err = OpenRAMDatabase( dbID, cardNo, &ramDocument, &version, docInfo->name,
        &created, &docInfo->numRecords );
    if ( err != errNone )
        return err;

    lastDocInfo = docInfo;

    FindRecord              = FindRAMRecord;
    GetNumOfRecords         = GetRAMNumOfRecords;
    ReturnRecordHandle      = ReturnRAMRecordHandle;
    ReturnRecordHandleByIndex = ReturnRAMRecordHandleByIndex;
    ReturnNextRecordHandle  = ReturnNextRAMRecordHandle;
    GetMaxUID               = GetRAMMaxUID;
    FreeRecordHandle        = FreeRAMRecordHandle;

    return errNone;
}



/* Close document */
void CloseRAMDocument( void )
{
    if ( ramDocument == NULL )
        return;

    CloseDatabase( ramDocument );
    ramDocument = NULL;
}



/* Delete document */
void DeleteRAMDocument
    (
    DocumentInfo* docInfo
    )
    /* THROWS */
{
    Err err;

    THROW_IF( docInfo == NULL, dmErrInvalidParam );
    THROW_IF( ramDocument != NULL, dmErrDatabaseOpen );

    err = DmDeleteDatabase( docInfo->cardNo, GetDocumentID( docInfo ) );

    THROW_IF( err != errNone, err );
}



/* Beam Plucker document in RAM, return error code, or 0 if there is no error */
void BeamRAMDocument
    (
    DocumentInfo* docInfo
    )
    /* THROWS */
{
    ExgSocketType   s;
    Err             err;
    LocalID         dbID;
    UInt16          cardNo;

    THROW_IF( docInfo == NULL, dmErrInvalidParam );

    dbID    = GetDocumentID( docInfo );
    cardNo  = docInfo->cardNo;

    MemSet( &s, sizeof( ExgSocketType ), 0 );

    s.name          = docInfo->name;
    s.type          = ViewerMIMEType;
    s.description   = docInfo->name;
    s.target        = ViewerAppID;

    err = ExgPut( &s );
    if ( err == errNone ) {
        err = ExgDBWrite( WritePluckerDocument, &s, docInfo->name, dbID,
                cardNo );
        err = ExgDisconnect( &s, err );
    }
    THROW_IF( err != errNone, err );
}



/* Receive Plucker document, return error code, or 0 if there is no error */
Err ReceiveRAMDocument
    (
    ExgSocketType*  s,
    Boolean         globalsAvailable
    )
{
    Err     err;
    LocalID dbID;
    UInt16  cardNo;
    Boolean wantReset;

    wantReset   = false;
    cardNo      = 0;

    err = ExgAccept( s );
    err = ExgDBRead( ReadPluckerDocument, DeletePluckerDocument, s,
            &dbID, cardNo, &wantReset, true );
    err = ExgDisconnect( s, err );

    s->goToCreator = 0;

    if ( err == errNone && wantReset ) {
        SysReset();
    }

    return err;
}



/* return UID for last record in document, i.e. the max UID */
UInt16 GetRAMMaxUID( void )
{
    UID*        uid;
    MemHandle   handle;
    UInt16      lastIndex;
    UInt16      maxUID;

    lastIndex   = lastDocInfo->numRecords - 1;
    handle      = OpenRecord( ramDocument, lastIndex );
    if ( handle == NULL )
        return 1;

    uid     = MemHandleLock( handle );
    maxUID  = uid->uid;
    MemHandleUnlock( handle );

    CloseRecord( handle, false );
    
    return maxUID;
}



/* Return a handle to a record */
/* Note: You must release the record when done by using CloseRecord() */
MemHandle ReturnRAMRecordHandle
    (
    UInt16 uid    /* unique ID to search for */
    )
    /* THROWS */
{
    THROW_IF( uid == 0, dmErrInvalidParam );
    return SearchRAMDocument( ramDocument, CompareUID, &uid,
            lastDocInfo->numRecords, NULL );
}



/* Return a handle to next record */
/* Note: You must release the record when done, e.g., by using 
   CloseRecord() */
MemHandle ReturnNextRAMRecordHandle
    (
    UInt16* index  /* pointer to index of a known record */
    )
    /* THROWS */
{
    MemHandle   handle;
    Header*     record;
    UInt16      lastIndex;

    if ( *index == 0 )
        *index = 1;

    handle      = NULL;
    lastIndex   = lastDocInfo->numRecords - 1;
    while ( *index <= lastIndex ) {
        handle = OpenRecord( ramDocument, *index );
        if ( handle == NULL )
            ErrThrow( DmGetLastErr() );

        record = MemHandleLock( handle );
        if ( record->type == DATATYPE_PHTML ||
             record->type == DATATYPE_PHTML_COMPRESSED ) {
            MemHandleUnlock( handle );
            break;
        }
        MemHandleUnlock( handle );
        CloseRecord( handle, false );
        handle = NULL;

        ( *index )++;
    }
    return handle;
}



/* Return a handle to record */
/* Note: You must release the record when done, e.g.,
   by using CloseRecord() */
MemHandle ReturnRAMRecordHandleByIndex
    (
    const UInt16 index  /* index of record */
    )
{
    return OpenRecord( ramDocument, index );
}



/* Find record in document, returns handle to record or NULL if not found */
/* Note: You must release the record when done by using CloseRecord() */
MemHandle SearchRAMDocument
    (
    DmOpenRef       dbRef,      /* document reference */
    CompareFunc*    compare,    /* pointer to compare function */
    void*           item,       /* item to search for */
    UInt16          numRecords, /* number of records to search */
    Int16*          index       /* upon successful return, the index of the
                                   record, pass NULL for this parameter if
                                   you don't want to retrieve this value */
    )
    /* THROWS */
{
    MemHandle   handle;
    Int16      l;
    Int16      r;
    Int16      x;

    THROW_IF( compare == NULL || item == NULL, dmErrInvalidParam );

    if ( dbRef == NULL )
        return NULL;

    handle  = NULL;
    l       = 0;
    r       = numRecords - 1;
    x       = 0;
    while ( l <= r ) {
        Int16 result;

        x = ( r + l ) / 2;

        handle = DmQueryRecord( dbRef, x );
        if ( handle == NULL )
            break;

        result = compare( item, handle );
        if ( result == 0 ) {
            if ( index != NULL )
                *index = x;
            break;
        }
        if ( result < 0 )
            r = x - 1;
        else
            l = x + 1;
        handle = NULL;
    }

    if ( handle != NULL )
        return OpenRecord( dbRef, x );
    else
        return NULL;
}



/* Find record with given uid in document */
/* Note: You must release the record when done by using CloseRecord() */
MemHandle FindRAMRecord
    (
    UInt16          uid,        /* uid to search for */
    Int16*          index       /* upon successful return, the index of the
                                   record, pass NULL for this parameter if
                                   you don't want to retrieve this value */
    )
    /* THROWS */
{
    THROW_IF( uid == 0, dmErrInvalidParam );
    return SearchRAMDocument( ramDocument, CompareUID, &uid,
            lastDocInfo->numRecords, index );
}



/* update document list with documents in RAM */
void EnumerateRAM( void )
    /* THROWS */
{
    SysDBListItemType*  dbIDs;
    MemHandle           dbHandle;
    UInt16              i;
    UInt16              dbCount;
    UInt16              numOfElementsInDocList;
    Boolean             foundDocuments;

    /* find all Plucker documents in RAM */
    foundDocuments = SysCreateDataBaseList( (UInt32) ViewerDocumentType,
        (UInt32) ViewerAppID, &dbCount, &dbHandle, false );

    if ( foundDocuments && 0 < dbCount && dbHandle != NULL ) {
        numOfElementsInDocList  = GetNumOfDocuments();
        dbIDs                   = MemHandleLock( dbHandle );

        /* iterate list of found documents and add to document list if a
           new document is encountered or update changed documents */
        for ( i = 0; i < dbCount; i++ ) {
            DocumentInfo*   handlePtr;
            DocumentInfo    docInfo;
            MemHandle       handle;
            UInt16          index;
            UInt32          created;

            /* get name, attributes and date */
            DmDatabaseInfo( dbIDs[ i ].cardNo, dbIDs[ i ].dbID, docInfo.name,
                       &docInfo.attributes, NULL, &created,
                       NULL, NULL, NULL, NULL, NULL, NULL, NULL );
            /* Try to retrieve the publication date from the document and, if
            found, use it instead of the file's creation date */
            docInfo.location    = RAM;
            docInfo.cardNo      = dbIDs[ i ].cardNo;
            ErrTry {
                docInfo.created = GetPublicationDate( &docInfo );
            }
            ErrCatch( UNUSED_PARAM( err ) ) {
                docInfo.created = 0xffff;
            } ErrEndCatch
            if ( docInfo.created == 0xffff ) {
                docInfo.created = created;
            }

            handlePtr = NULL;
            ErrTry {
                handle = FindDocData( docInfo.name, numOfElementsInDocList,
                            &index );
                if ( handle != NULL ) {
                    SetFoundDocument( index );
                    handlePtr = MemHandleLock( handle );
                    /* if the document is newer than the stored one, is on a
                       different card, or moved from external card then
                       updated info */
                    if ( ! handlePtr->active ||
                         handlePtr->created < docInfo.created ||
                         handlePtr->cardNo != dbIDs[ i ].cardNo ||
                         handlePtr->location != RAM ) {

                        DmDatabaseSize( dbIDs[ i ].cardNo, dbIDs[ i ].dbID,
                            NULL, &docInfo.size, NULL );
                        docInfo.categories  = handlePtr->categories;
                        docInfo.active      = true;
                        /* a "new" document should be indicated as unread */
                        if ( ! handlePtr->active ||
                             handlePtr->created < docInfo.created )
                            docInfo.timestamp = 0;
                        else
                            docInfo.timestamp = handlePtr->timestamp;

                        /* if document has been moved from external card
                           the meta database must be renamed */
                        if ( handlePtr->location != RAM )
                            RAMRenameMetaDocument( docInfo.name,
                                handlePtr->location, &docInfo );

                        MemHandleUnlock( handle );
                        UpdateDocument( &docInfo, NULL, index, &handle );
                    }
                    else {
                        MemHandleUnlock( handle );
                    }
                    CloseRecord( handle, false );
                }
                else {
                    DmDatabaseSize( dbIDs[ i ].cardNo, dbIDs[ i ].dbID, NULL,
                        &docInfo.size, NULL );
                    docInfo.active      = true;
                    docInfo.timestamp   = 0;

                    ErrTry {
                        docInfo.categories = GetDefaultCategories( &docInfo );
                    }
                    ErrCatch( UNUSED_PARAM( err ) ) {
                        docInfo.categories = UNFILED_CATEGORY;
                    } ErrEndCatch
                    AddDocument( &docInfo, NULL );
                }
            } ErrCatch( err ) {
                MemHandleUnlock( dbHandle );
                MemHandleFree( dbHandle );
                ErrThrow( err );
            } ErrEndCatch
        }
        MemHandleUnlock( dbHandle );
        MemHandleFree( dbHandle );
        dbHandle = NULL;
    }
}



/* Rename document and its associated meta document */
void RenameRAMDocument
    (
    Char*           newName,    /* new document name */
    DocumentInfo*   docInfo
    )
    /* THROWS */
{
    Err     err;
    LocalID dbID;
    UInt16  cardNo;

    THROW_IF( newName == NULL || *newName == '\0', errNoDocumentName );
    THROW_IF( docInfo == NULL, dmErrInvalidParam );

    cardNo = docInfo->cardNo;

    /* If a document with the given name doesn't already exists, rename it */
    dbID = DmFindDatabase( cardNo, newName );
    THROW_IF( dbID != 0, dmErrAlreadyExists );

    dbID = DmFindDatabase( cardNo, docInfo->name );
    err  = DmSetDatabaseInfo( cardNo, dbID, newName, NULL, NULL,
                NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
    THROW_IF( err != errNone, err );

    RAMRenameMetaDocument( newName, docInfo->location, docInfo );
}


static void RAMRenameMetaDocument
    (
    Char*           newName,    /* new document name */
    UInt16          oldLocation,
    DocumentInfo*   docInfo
    )
{
    Char metaNewName[ dmDBNameLength ];

    MakeMetaName( metaNewName, META_PREFIX, RAM, newName );
    RenameMetaDocument( metaNewName, oldLocation, docInfo );
    MakeMetaName( metaNewName, ANN_PREFIX, RAM, newName );
    RenameAnnotations( metaNewName, oldLocation, docInfo );
}



/* Return number of records in document */
UInt16 GetRAMNumOfRecords( void )
{
    return lastDocInfo->numRecords;
}



/* Free allocated handle */
void FreeRAMRecordHandle
    (
    MemHandle* handleP
    )
{
    if ( handleP != NULL ) {
        CloseRecord( *handleP, false );
        *handleP = NULL;
    }
}

