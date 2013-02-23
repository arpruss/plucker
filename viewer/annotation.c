/*
 * $Id: annotation.c,v 1.19 2008/01/21 00:22:17 prussar Exp $
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

#include "document.h"
#include "metadocument.h"
#include "paragraph.h"

#include "annotation.h"

#define NO_RECORD_FOUND    0xFFFFu

#define OVERLAP( a,b,x,y ) ( ! ( (y)<=(a) || (b)<=(x) ) )
#define START_MARKER       0xFFFFFFFFl

#define INDEX_LEVEL     0
#define PARAGRAPH_LEVEL 1

/* options for confirmDeleteOldAnnotations */
#define DELETE_OLD_ANNOTATIONS        0
#define DO_NOT_DELETE_OLD_ANNOTATIONS 1
#define DO_NOT_EVEN_ASK               2

/* options for confirmDeleteAnnotations */
#define DELETE_ANNOTATIONS_YES    0
#define DELETE_ANNOTATIONS_NO     1
#define DELETE_ANNOTATIONS_ALWAYS 2

/***********************************************************************
 *
 *      Internal Types
 *
 ***********************************************************************/

/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void GetAnnotationIdentifier( MemHandle h, AnnotationIdentifier* id )
    ANNOTATION_SECTION;

static Int16 CompareAnnotations( AnnotationIdentifier* id1,
    AnnotationIdentifier* id2,
    Int32 mode ) ANNOTATION_SECTION;

static void OpenDocAnnotations( void ) ANNOTATION_SECTION;

static Boolean CreateDocAnnotations( void ) ANNOTATION_SECTION;

static void AddToIndexes( UInt16 recordNum, UInt16 uid,
     UInt16 paragraphNum, Int16 delta ) ANNOTATION_SECTION;

static MemHandle AllocateNewAnnotation( AnnotationEntry* header, UInt32 size )
     ANNOTATION_SECTION;

static Boolean IsAbove( UInt16 recordNum,
     AnnotationEntry* newEntry, Boolean* overlapP ) ANNOTATION_SECTION;

static void GetAnnotationsName( Char* annName, Char* docName, 
     UInt16 location ) ANNOTATION_SECTION;

static void DoDeleteAnnotations( UInt16 cardNo, LocalID id, Char* annName )
     ANNOTATION_SECTION;


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static DmOpenRef         annotationRef = NULL;
static Boolean           lastValid;
static Boolean           needToCheckAnnotations;
static AnnotationIdentifier lastId;
static UInt16            lastRecordNum;
static Char				 annotationName[ dmDBNameLength ];


static void GetAnnotationIdentifier
    (
    MemHandle             h,
    AnnotationIdentifier* id
    )
{
    AnnotationEntry* idP;

    idP = MemHandleLock( h );

    *id = idP->id;

    MemHandleUnlock( h );
}



static Int16 CompareAnnotations
    (
    AnnotationIdentifier*     id1,
    AnnotationIdentifier*     id2,
    Int32     mode
    )
{
    if ( id1->uid < id2->uid )
        return -1;
    else if ( id2->uid < id1->uid )
        return 1;
    else if ( id1->paragraphNum < id2->paragraphNum )
        return -1;
    else if ( id2->paragraphNum < id1->paragraphNum )
        return 1;
    else if ( id1->indexInParagraph == id2->indexInParagraph || mode == PARAGRAPH_LEVEL )
        return 0;
    else if ( id2->indexInParagraph < id1->indexInParagraph )
        return 1;
    else
        return -1;
}




MemHandle GetAnnotation
    (
    UInt16 uid,
    UInt16 paragraph,
    UInt16 index,
    UInt16* recordNumP
    )
{
    MemHandle             h;
    AnnotationIdentifier  currentId;
    Int16                 currentRecordNum;
    AnnotationIdentifier  searchId;
    Int16                 topRecordNum;
    Int16                 bottomRecordNum;
    Int16                 difference;
    UInt16                numRecords;

    if ( annotationRef == NULL )
        return NULL;

    numRecords = DmNumRecords( annotationRef );

    if ( numRecords == 0 ) {
        return NULL;
    }

    searchId.uid          = uid;
    searchId.paragraphNum = paragraph;
    searchId.indexInParagraph        = index;

    if ( lastValid && lastId.uid == uid && lastId.paragraphNum == paragraph ) {
        Boolean               found;
        AnnotationIdentifier* record;
		UInt16				  recordNum;

        recordNum = lastRecordNum + index - lastId.indexInParagraph;

        h = DmQueryRecord( annotationRef, recordNum );
        if ( h == NULL )
            return NULL;

        record = MemHandleLock( h );

        found = ( 0 == CompareAnnotations( record, &searchId, INDEX_LEVEL ) );

        if ( found ) {
            lastId = *record;
            lastRecordNum = recordNum;
        }

        MemHandleUnlock( h );

        if ( recordNumP != NULL )
            *recordNumP = recordNum;

        return found ? h : NULL;
    }

    topRecordNum    = 0;
    bottomRecordNum = numRecords - 1;

    if ( lastValid ) {
        if ( CompareAnnotations( &lastId, &searchId, INDEX_LEVEL ) < 0 ) {
            /* Typical use is often sequential, so go to the
            next record */

            AnnotationIdentifier  topId;

            topRecordNum = lastRecordNum + 1;
            if ( numRecords <= topRecordNum )
                return NULL;

            h = DmQueryRecord( annotationRef, topRecordNum );
            if ( h == NULL )
                return NULL;

            GetAnnotationIdentifier( h, &topId );

            if ( 0 == CompareAnnotations( &topId, &searchId, INDEX_LEVEL ) ) {
                lastRecordNum = topRecordNum;
                lastId        = topId;
                if ( recordNumP != NULL )
                    *recordNumP = lastRecordNum;
                return h;
            }
        }
        else {
            bottomRecordNum = lastRecordNum;
            h = DmQueryRecord( annotationRef, bottomRecordNum );
            if ( h == NULL )
                return NULL;
        }
    }

    while ( topRecordNum < bottomRecordNum ) {
        Int16 delta;

        currentRecordNum = ( ( UInt32 )topRecordNum +
                             ( UInt32 )bottomRecordNum ) / 2;
        h = DmQueryRecord( annotationRef, currentRecordNum );
        if ( h == NULL )
            return NULL;
        GetAnnotationIdentifier( h, &currentId );

        delta = CompareAnnotations( &currentId, &searchId, INDEX_LEVEL );

        if ( delta < 0 ) {
            topRecordNum       = currentRecordNum + 1;
        }
        else if ( 0 < delta ) {
            bottomRecordNum    = currentRecordNum - 1;
        }
        else {
            bottomRecordNum    = currentRecordNum;
            topRecordNum       = currentRecordNum;
        }
    }

    currentRecordNum = bottomRecordNum;

    h = DmQueryRecord( annotationRef, bottomRecordNum );
    if ( h == NULL )
        return NULL;

    GetAnnotationIdentifier( h, &currentId );

    difference = CompareAnnotations( &currentId, &searchId, INDEX_LEVEL );

    if ( difference == 0 ) {
        lastRecordNum = currentRecordNum;
        lastId        = currentId;
        lastValid     = true;
        if ( recordNumP != NULL )
            *recordNumP = currentRecordNum;
        return h;
    }
    else {
        return NULL;
    }
}



Boolean GetAnnotationHeader
    (
    AnnotationEntry* outP,
    UInt16 uid,
    UInt16 paragraphNum,
    UInt16 index
    )
{
    MemHandle h;
    
    h = GetAnnotation( uid, paragraphNum, index, NULL );
    
    if ( h != NULL ) {
        AnnotationEntry* e;

        e = MemHandleLock( h );

        MemMove( outP, e, sizeof( AnnotationEntry ) );
        
        MemHandleUnlock( h );

        return true;
    }
    else {
        return false;
    }
}
 
 
 

UInt16 GetNumberOfAnnotations( void )
{
    if ( annotationRef == NULL )
        return 0;
    else
        return DmNumRecords( annotationRef );
}


MemHandle GetAnnotationByRecordNum
    (
    UInt16 recordNum
    )
{
    if ( annotationRef == NULL ) 
        return NULL;
    else
        return DmQueryRecord( annotationRef, recordNum );
}



Boolean GetAnnotationHeaderByRecordNum
    (
    AnnotationEntry* e,
    UInt16 recordNum
    )
{
    MemHandle h;
    
    h = GetAnnotationByRecordNum( recordNum );

    if ( h != NULL ) {
        AnnotationEntry* in;
        UInt32           size;
    
        size = MemHandleSize( h );
        if ( sizeof( AnnotationEntry ) < size )
            size = sizeof( AnnotationEntry );
    
        in   = MemHandleLock( h );

        MemMove( e, in, size );
    
        MemHandleUnlock( h );
    
        return true;
    }
    else {
        return false;
    }
}



static void OpenDocAnnotations( void )
    /* THROWS */
{
    UInt16   cardNo;
    LocalID  id;

    cardNo = 0; /* FIX ME: Fix for older devices */

    id = DmFindDatabase( cardNo, annotationName );

    if ( id != NULL ) {
        DBSetBackup( cardNo, id );

        annotationRef = DmOpenDatabase( cardNo, id, dmModeReadWrite );

        THROW_IF( annotationRef == NULL, dmErrMemError );
    }
    else {
        annotationRef = NULL;
    }
}


/* Make sure that the annotations are not out of date for the document */
void CheckAnnotationsDate( void )
{
    UInt16  cardNo;
    UInt32  annDate;
    UInt32  docDate;
    LocalID id;
    Err     err;

    if ( ! needToCheckAnnotations )
        return;

    needToCheckAnnotations = false;

    cardNo = 0;

    id = DmFindDatabase( cardNo, annotationName );

    if ( id == NULL )
        return;
        
    docDate = GetDocumentDate();

    err = DmDatabaseInfo( cardNo, id, NULL, NULL, NULL,
            &annDate, NULL, NULL, NULL, NULL, NULL, NULL, NULL );

    if ( annDate < docDate ) {
        switch ( FrmAlert( confirmDeleteOldAnnotations ) ) {
            case DELETE_OLD_ANNOTATIONS:
                if ( annotationRef != NULL ) {
                    DmCloseDatabase( annotationRef );
                    annotationRef = NULL;
                }
                DmDeleteDatabase( cardNo, id );
                break;
            case DO_NOT_EVEN_ASK:
                DmSetDatabaseInfo( cardNo, id, NULL, NULL, NULL,
                    &docDate, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
                break;
            default:
                break;
        }
    }
}



static void GetAnnotationsName
    (
    Char*        annName,
    Char*        docName,
    UInt16       location
    )
{
    MakeMetaName( annName, ANN_PREFIX, location, docName );
    LegacyFixMetaOrAnnotation( annName, docName );
}



void RenameAnnotations
    (
    Char*           newName,
    UInt16          oldLocation,
    DocumentInfo*   docInfo
    )
{
    Err     err;
    Char    annOldName[ dmDBNameLength ];
    LocalID annID;
    UInt16  annCardNo;

    err = errNone;

    /* Remove any "left-overs" */
    annCardNo  = 0;
    annID      = DmFindDatabase( annCardNo, newName );
    if ( annID != 0 ) {
        err = DmDeleteDatabase( annCardNo, annID );
    }
    if ( err == errNone ) {
        /* Rename ann document ( if it exists ) */
        MakeMetaName( annOldName, ANN_PREFIX, oldLocation, docInfo->name );
        LegacyFixMetaOrAnnotation( annOldName, docInfo->name );

        annID = DmFindDatabase( annCardNo, annOldName );
        if ( annID != 0 )
            err = DmSetDatabaseInfo( annCardNo, annID, newName, NULL,
                                    NULL, NULL, NULL, NULL, NULL, NULL,
                                    NULL, NULL, NULL );
        else
            err = DmGetLastErr();
    }
    THROW_IF( err != errNone && err != dmErrCantFind, err );
}





void InitDocAnnotations
    (
    Char*  docName,
    UInt16 location,
    UInt32 docDate
    )
    /* THROWS */
{
    GetAnnotationsName( annotationName, docName, location );

    OpenDocAnnotations();

    needToCheckAnnotations = true;

    lastValid = false;
}


void DoDeleteAnnotations
    ( 
    UInt16 cardNo, 
    LocalID id, 
    Char* annName 
    )
{
    if ( annotationRef != NULL && ! StrCompare( annName, annotationName ) ) {
        CloseDocAnnotations();
    }
    
    DmDeleteDatabase( cardNo, id );
}



void DeleteAnnotations
    ( 
    char* docName, 
    UInt16 location 
    )
{
    LocalID id;
    UInt16  cardNo;

    Char annName[ dmDBNameLength ];

    GetAnnotationsName( annName, docName, location );
    
    cardNo = 0;
    id     = DmFindDatabase( 0, annName );
    
    if ( id == NULL )
        return;

    if ( Prefs()->alwaysDeleteAnnotations ) {
        DoDeleteAnnotations( cardNo, id, annName );
        return;
    }

    switch ( FrmCustomAlert( confirmDeleteAnnotations, docName, "", "" ) ) {
        case DELETE_ANNOTATIONS_ALWAYS:
            Prefs()->alwaysDeleteAnnotations = true;
            /* fall through to next case */
        case DELETE_ANNOTATIONS_YES:
            DoDeleteAnnotations( cardNo, id, annName );
            break;
        default:
            break;
    }
}



void CloseDocAnnotations( void )
{
    if ( annotationRef != NULL )
        DmCloseDatabase( annotationRef );

    annotationRef = NULL;
}



/* This can be safely called even if the annotation database exists.
   You must call InitDocAnnotations() before calling this. */
static Boolean CreateDocAnnotations( void )
{
    UInt16  cardNo;
    LocalID id;
    Err     err;

    if ( annotationRef != NULL )
        return true;

    cardNo = 0;

    err = DmCreateDatabase( cardNo, annotationName, ViewerAppID,
		       AnnotationDocumentType, false );

    if ( err != errNone )
        return false;

    id  = DmFindDatabase( cardNo, annotationName );

    if ( id == NULL )
        return false;

    DBSetBackup( cardNo, id );

    annotationRef = DmOpenDatabase( cardNo, id, dmModeReadWrite );

    return annotationRef != NULL;
}



/* This adds delta to indexInParagraph fields for records
   for the same paragraph starting at recordNum and moving right. */
static void AddToIndexes
    (
    UInt16 recordNum,
    UInt16 uid,
    UInt16 paragraphNum,
    Int16  delta
    )
{
    UInt16 numRecords;

    numRecords = DmNumRecords( annotationRef );

    while ( recordNum < numRecords ) {
        MemHandle h;

        h = DmQueryRecord( annotationRef, recordNum );

        if ( h != NULL ) {
            AnnotationEntry* e;
            AnnotationIdentifier  id;

            e = MemHandleLock( h );

            if ( paragraphNum != e->id.paragraphNum || uid != e->id.uid ) {
                MemHandleUnlock( h );
                return;
            }

            id = e->id;

            id.indexInParagraph += delta;

            DmWrite( e, 0, &id, sizeof( AnnotationIdentifier ) );

            MemHandleUnlock( h );

            lastValid = false;

        }

        recordNum++;
    }
}


/* check if recordNum is above newEntry */
static Boolean IsAbove
    (
    UInt16                recordNum,
    AnnotationEntry*	  newEntry,
    Boolean*	          overlapP
    )
{
    AnnotationIdentifier id;
    AnnotationEntry*	 oldEntry;
    MemHandle            h;
    Boolean              returnValue;

    GetAnnotationIdentifier( DmQueryRecord( annotationRef, recordNum ), &id );

    if ( overlapP != NULL )
        *overlapP = false;

    if ( id.uid < newEntry->id.uid )
        return true;

    if ( newEntry->id.uid < id.uid )
        return false;

    if ( id.paragraphNum < newEntry->id.paragraphNum )
        return true;

    if ( newEntry->id.paragraphNum < id.paragraphNum )
	return false;

    h = GetAnnotation( id.uid, id.paragraphNum, id.indexInParagraph, NULL );

    if ( h == NULL )
        return false;

    oldEntry = MemHandleLock( h );

    if ( oldEntry->triggerStop <= newEntry->triggerStart ) {
        MemHandleUnlock( h );
        return true;
    }

    if ( newEntry->triggerStop <= oldEntry->triggerStart ) {
        MemHandleUnlock( h );
        return false;
    }

    /* We now have overlap, unless at least one is hidden */
    if ( overlapP != NULL ) {
        *overlapP = ! ( oldEntry->flags & ANNOTATION_HIDDEN ) &&
                    ! ( newEntry->flags & ANNOTATION_HIDDEN );
    }

    if ( oldEntry->triggerStart < newEntry->triggerStart ) {
        MemHandleUnlock( h );
        return true;
    }

    if ( newEntry->triggerStart < oldEntry->triggerStart ) {
        MemHandleUnlock( h );
        return false;
    }

    returnValue = oldEntry->triggerStop < newEntry->triggerStop;

    MemHandleUnlock( h );
    return returnValue;
}



/* This will also set header->id.indexInParagraph */
static MemHandle AllocateNewAnnotation
    (
    AnnotationEntry* header,
    UInt32           size
    )
{
    UInt16    recordNum;
    UInt16    numRecords;
    Boolean   overlap;
    MemHandle h;

    numRecords = DmNumRecords( annotationRef );
    overlap    = false;

    for ( recordNum = 0 ;
             recordNum < numRecords &&
                 IsAbove( recordNum, header, &overlap ) &&
                 ! overlap ;
             recordNum++ ) ;

    if ( overlap )
        return NULL;

    if ( 0 < recordNum ) {
        AnnotationIdentifier prevID;
        GetAnnotationIdentifier( DmQueryRecord( annotationRef, recordNum - 1 ),
            &prevID );
        if ( prevID.paragraphNum == header->id.paragraphNum &&
             prevID.uid == header->id.uid )
            header->id.indexInParagraph = prevID.indexInParagraph + 1;
        else
            header->id.indexInParagraph = 0;
    }
    else {
        header->id.indexInParagraph = 0;
    }

    h = DmNewRecord( annotationRef, &recordNum, size );

    if ( h == NULL )
        return NULL;

    lastValid = false;

    AddToIndexes( recordNum + 1, header->id.uid, header->id.paragraphNum, 1 );

    return h;
}



/* The newEntry->id field must have uid and paragraphNum subfields filled
   out.  If indexInParagraph is set to NEW_ANNOTATION, then a new one is
   created and the indexInParagraph field is filled out.  Otherwise, the
   old annotation is revised.  The date, headerLength, dataLength and 
   dataOffset fields are also automatically set, so newEntry must be 
   editable.  Returns true on success. */
Boolean AddAnnotation
    (
    AnnotationEntry*  header,
    Char*             text
    )
{
    AnnotationEntry*  e;
    MemHandle         h;
    UInt16            recordNum;

    if ( ! CreateDocAnnotations() )
        return false;

    header->headerLength = sizeof( AnnotationEntry );

    if ( text == NULL ) {
        header->dataLength = 0;
        header->dataOffset = 0;
    }
    else {
        header->dataLength = StrLen( text ) + 1;
        header->dataOffset = header->headerLength;
    }

    if ( NEW_ANNOTATION != header->id.indexInParagraph ) {
        h = GetAnnotation( header->id.uid, header->id.paragraphNum,
                header->id.indexInParagraph, &recordNum );

        if ( h == NULL )
            return false;

        h = DmResizeRecord( annotationRef, recordNum,
                header->headerLength + header->dataLength );

        if ( h == NULL )
            return false;
    }
    else {
        header->createDate = TimGetSeconds();

        h = AllocateNewAnnotation( header,
                sizeof( AnnotationEntry ) + header->dataLength );

        if ( h == NULL )
            return false;
    }

    header->modificationDate = TimGetSeconds();

    e = MemHandleLock( h );

    DmWrite( e, 0, header, header->headerLength );
    if ( 0 < header->dataLength )
        DmWrite( e, header->dataOffset, text, header->dataLength );

    MemHandleUnlock( h );

    DmReleaseRecord( annotationRef, recordNum, true );

    return true;
}



Boolean DeleteAnnotation
    (
    AnnotationIdentifier* id
    )
{
    MemHandle h;
    UInt16    recordNum;

    h = GetAnnotation( id->uid, id->paragraphNum, id->indexInParagraph,
            &recordNum );

    if ( h == NULL )
        return false;

    /* Maybe this should be DmDeleteRecord??  But then numbering may be
       off. */
    DmRemoveRecord( annotationRef, recordNum );

    lastValid = false;

    AddToIndexes( recordNum, id->uid, id->paragraphNum, -1 );

    return true;
}



/* Find the first annotation that overlaps with the specified area. */
Boolean FindAnnotation
    (
    AnnotationEntry* annotationHeader,
    UInt16       uid,
    UInt16       paragraph,
    UInt16       start,
    UInt16       stop
    )
{
    UInt16 index;

    for ( index = 0 ; 
          GetAnnotationHeader( annotationHeader, uid, paragraph, index ) &&
              annotationHeader->triggerStart <= stop ; 
          index++ ) {
        
        if ( stop < annotationHeader->triggerStart )
            return false;
            
        if ( OVERLAP( start, stop, annotationHeader->triggerStart, 
                 annotationHeader->triggerStop ) )
            return true;
        
    }
    
    return false;
}




void DeleteAnnotationByRecordNum
    (
    UInt16 recordNum
    )
{
    if ( annotationRef != NULL ) {
        AnnotationEntry e;
        
        if ( GetAnnotationHeaderByRecordNum( &e, recordNum ) ) {
            DmRemoveRecord( annotationRef, recordNum );

            AddToIndexes( recordNum, e.id.uid, e.id.paragraphNum, -1 );
        }
    }
}
