/*
 * $Id: bookmark.c,v 1.51 2006/11/12 02:00:55 prussar Exp $
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

// TODO: Replace SafeMemPtrNew() with explicit error handling.

#include "control.h"
#include "debug.h"
#include "ramfile.h"
#include "document.h"
#include "fullscreenform.h"
#include "genericfile.h"
#include "history.h"
#include "resourceids.h"
#include "util.h"
#include "metadocument.h"
#include "annotation.h"
#include "paragraph.h"
#include "mainform.h"
#include "link.h"

#include "bookmark.h"


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define ADD_BOOKMARK_LEN        25
#define ADD_ANNOTATION_LEN      30
#define EDIT_BOOKMARK_LEN       25
#define MAX_BOOKMARK_ENTRY_LENGTH   32
#define NO_PARAGRAPHS           0
#define NO_BOOKMARKS            0
#define MAX_BOOKMARK_LIST_LEN   12
#define BOOKMARK_HEADER_LEN     ( 3 * sizeof( UInt16 ) )

#define GET_ENTRIES( ptr )         ( ( ptr )[ 2 ] * 256 + ( ptr )[ 3 ] )
#define GET_OFFSET( ptr )          ( ( ptr )[ 4 ] * 256 + ( ptr )[ 5 ] )
#define GET_EXTFIRSTFIELD( ptr )   ( ( ptr )[ 0 ] * 256 + ( ptr )[ 1 ] )
#define GET_EXTSECONDFIELD( ptr )  ( ( ptr )[ 2 ] * 256 + ( ptr )[ 3 ] )


/***********************************************************************
 *
 *      Internal Types
 *
 ***********************************************************************/
typedef HistoryData OldBookmarkData;

/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void DrawListItem( Int16 itemNum, RectangleType* bounds,
                    Char** itemsText ) BOOKMARK_SECTION;
static UInt16 InitBookmarkList( ListType* list ) BOOKMARK_SECTION;
static UInt16 InitExtBookmarkList( Char** nameList, UInt16 offset )
                    BOOKMARK_SECTION;
static UInt16 ConvertBookmarksForRecord( UInt16 recordId, UInt8* bookmarkPtr,
                    OldBookmarkData* dataPtr, UInt16 entries, Boolean* done )
                    BOOKMARK_SECTION;
static UInt16 CountBookmarks( void ) BOOKMARK_SECTION;


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static UInt16              numEntries;
static BookmarkListEntry** bookmarkList;
static Char**             nameList;
static Boolean      isPopupList;
static Char         addBookmark[ ADD_BOOKMARK_LEN ];
#ifdef SUPPORT_ANNOTATION
static Char         addAnnotation[ ADD_ANNOTATION_LEN ];
#endif
static Char         editBookmark[ EDIT_BOOKMARK_LEN ];



/* Callback function that draws list items */
static void DrawListItem
    (
    Int16           itemNum,    /* item number */
    RectangleType*  bounds,     /* pointer to a RectangleType structure that
                                   specifies the bounds for the list item */
    Char**          itemsText   /* pointer to an array of text strings */
    )
{
    FontID oldFont;
    FontID newFont;

    if ( bookmarkList[ itemNum ] == NULL )
        return;

    switch ( bookmarkList[ itemNum ]->kind ) {
        case BOOKMARK_ANNOTATION:
        case BOOKMARK_BOOKMARK:
            newFont = stdFont;
            break;
        default:
            newFont = boldFont;
            break;
    }

    oldFont = FntSetFont( newFont );

    if ( itemsText[ itemNum ] != NULL )
        WinDrawChars( itemsText[ itemNum ], StrLen( itemsText[ itemNum ] ),
            bounds->topLeft.x, bounds->topLeft.y );

    FntSetFont( oldFont );
}



BookmarkListEntry* GetBookmarkListEntry
    (
    UInt16 selection
    )
{
    return bookmarkList[ selection ];
}



static UInt16 ConvertBookmarksForRecord
         (
         UInt16        recordId,
         UInt8*        bookmarkPtr,
         OldBookmarkData* dataPtr,
         UInt16        entries,
         Boolean*      done
         )
{
     UInt16    i;
     Header*   record = NULL;
     MemHandle handle = NULL;
     Boolean   success = true;

     for ( i = 0 ; i < entries ; i++ ) {
          if ( dataPtr->recordId == recordId ) {
              AnnotationEntry e;

              if ( handle == NULL ) {
                  handle = ReturnRecordHandle( recordId );

                  if ( handle == NULL )
                      return false;

                  record = MemHandleLock( handle );
              }
              MemSet( &e, sizeof( AnnotationEntry ), 0 );

              e.flags = ANNOTATION_BOOKMARK | ANNOTATION_HIDDEN;

              e.triggerStart = dataPtr->characterPosition;
              e.triggerStop  = dataPtr->characterPosition;

              e.id.uid              = dataPtr->recordId;

              e.id.paragraphNum     = GetParagraphNumber( record, e.triggerStart );
              e.id.indexInParagraph = NEW_ANNOTATION;

              success = AddAnnotation( &e, bookmarkPtr ) && success;

              done[ i ] = true;
          }

          bookmarkPtr    += StrLen( bookmarkPtr ) + 1;
          dataPtr++;
     }

     if ( handle != NULL ) {
         MemHandleUnlock( handle );
         FreeRecordHandle( &handle );
     }

     return success;
}



/* Upgrade bookmarks from the old system to the current bookmark system */
void UpgradeBookmarks( void )
{
    MemHandle   handle;
    UInt8*      startBookmarkPtr;
    OldBookmarkData* startDataPtr;
    UInt8*      bookmarkPtr;
    UInt16      entries;
    Boolean     success;
    OldBookmarkData* dataPtr;
    UInt16      i;
    Boolean*    done;

    handle = ReturnMetaHandle( INTERNAL_BOOKMARKS_ID, NO_PARAGRAPHS );

    if ( handle == NULL )
        return;

    bookmarkPtr     = MemHandleLock( handle );
    dataPtr         = ( OldBookmarkData* )( bookmarkPtr + GET_OFFSET( bookmarkPtr ) );
    entries         = GET_ENTRIES( bookmarkPtr );
    bookmarkPtr    += BOOKMARK_HEADER_LEN;

    startDataPtr     = dataPtr;
    startBookmarkPtr = bookmarkPtr;

    if ( 0 == entries ) {
        MemHandleUnlock( handle );
        CloseRecord( handle, true );
        DeleteMetaRecord( INTERNAL_BOOKMARKS_ID );
        return;
    }

    success = false;

    done = SafeMemPtrNew( sizeof( Boolean ) * entries );
    MemSet( done, sizeof( Boolean ) * entries, 0 );

    success = true;

    for ( i = 0 ; i < entries ; i++ ) {
        if ( ! done[ i ] )
            success = ConvertBookmarksForRecord( dataPtr->recordId,
                startBookmarkPtr, startDataPtr, entries, done ) &&
                success;

        bookmarkPtr    += StrLen( bookmarkPtr ) + 1;
        dataPtr++;
    }

    SafeMemPtrFree( done );

    MemHandleUnlock( handle );
    CloseRecord( handle, true );

    DeleteMetaRecord( INTERNAL_BOOKMARKS_ID );
}


/* Populate list with bookmarks, return number of bookmarks in list */
static UInt16 InitBookmarkList
    (
    ListType* list  /* pointer to list */
    )
{
    UInt16      recordNum;
    UInt16      numRecords;
    UInt16      extEntries;
    UInt16      bookmarks;
    UInt16      extraListItems;
    UInt16      index;
    UInt16      i;

    if ( isPopupList ) {
#ifdef SUPPORT_ANNOTATION
        extraListItems  = 3;
#else
        extraListItems  = 2;
#endif
    }
    else {
        extraListItems  = 0;   
    }

    ReleaseBookmarkList();

    extEntries  = CountExtBookmarks();
    bookmarks   = CountBookmarks();
    numRecords  = GetNumberOfAnnotations();

    numEntries  = extraListItems + extEntries + bookmarks;

    if ( numEntries == 0 ) {
        LstSetListChoices( list, nameList, NO_BOOKMARKS );
        LstSetDrawFunction( list, DrawListItem );

        return NO_BOOKMARKS;
    }

    bookmarkList = SafeMemPtrNew( numEntries *
                            sizeof( BookmarkListEntry* ) );

    for ( i = 0 ; i < numEntries ; i++ ) {
         bookmarkList[ i ] = SafeMemPtrNew( sizeof( BookmarkListEntry ) );
         MemSet( bookmarkList[ i ], sizeof( BookmarkListEntry ), 0 );
    }

    /* Allocate arrays for name list */
    nameList = SafeMemPtrNew( numEntries * sizeof( Char* ) );

    if ( isPopupList ) {
        SysCopyStringResource( addBookmark, strMainAddBookmark );
        nameList[ ADD_BOOKMARK ] = addBookmark;
        bookmarkList[ ADD_BOOKMARK ]->kind = BOOKMARK_ADD_BOOKMARK;
#ifdef SUPPORT_ANNOTATION
        SysCopyStringResource( addAnnotation, strMainAddAnnotation );
        nameList[ ADD_ANNOTATION ] = addAnnotation;
        bookmarkList[ ADD_ANNOTATION ]->kind = BOOKMARK_ADD_ANNOTATION;
#endif
        SysCopyStringResource( editBookmark, strMainViewBookmark );
        nameList[ EDIT_BOOKMARK ] = editBookmark;
        bookmarkList[ EDIT_BOOKMARK ]->kind = BOOKMARK_EDIT_BOOKMARK;
    }

    if ( 0 < extEntries ) {
        InitExtBookmarkList( nameList, extraListItems );
        for ( i = extraListItems ; i < extraListItems + extEntries ; i++ ) {
             bookmarkList[ i ]->kind      = BOOKMARK_EXT_BOOKMARK;
             bookmarkList[ i ]->recordNum = i - extraListItems;
        }
    }
    
    index = extraListItems + extEntries;
    
    for ( recordNum = 0 ; recordNum < numRecords && index < numEntries ;
          recordNum++ ) {

         MemHandle h;
         
         h = GetAnnotationByRecordNum( recordNum );
         
         if ( h != NULL ) {
             AnnotationEntry* entryP;

             entryP = MemHandleLock( h );
             
             if ( 1 < entryP->dataLength &&
                  ! ( ! ( entryP->flags & ANNOTATION_BOOKMARK ) &&
                    Prefs()->noAnnotationsInBookmarkList ) ) {
                 UInt32 labelSize;

                 labelSize = entryP->dataLength - 1;

                 if ( MAX_BOOKMARK_ENTRY_LENGTH < labelSize )
                     labelSize = MAX_BOOKMARK_ENTRY_LENGTH;

                 bookmarkList[ index ]->text = MemPtrNew( labelSize + 1 );

                 if ( bookmarkList[ index ]->text != NULL ) {
                     bookmarkList[ index ]->kind =
                         ( entryP->flags & ANNOTATION_BOOKMARK ) ?
                                BOOKMARK_BOOKMARK :
                                BOOKMARK_ANNOTATION;
                     bookmarkList[ index ]->recordNum = recordNum;

                     StrNCopy( bookmarkList[ index ]->text,
                         ( Char* )entryP + entryP->dataOffset, labelSize + 1 );
                     bookmarkList[ index ]->text[ labelSize ] = 0;

                     nameList[ index ] = bookmarkList[ index ]->text;

                     index++;
                 }
             }

             MemHandleUnlock( h );
         }
    }
    
    numEntries = index;

    LstSetListChoices( list, nameList, numEntries );
    LstSetDrawFunction( list, DrawListItem );

    if ( isPopupList )
        LstSetHeight( list, numEntries );

    return numEntries;
}



/* Count the displayable bookmarks */
static UInt16 CountBookmarks( void )
{
    UInt16 i;
    UInt16 n;
    UInt16 count;

    n = GetNumberOfAnnotations();

    count = 0;

    for ( i = 0 ; i < n ; i++ ) {
         AnnotationEntry e;

         if ( GetAnnotationHeaderByRecordNum( &e, i ) && 1 < e.dataLength &&
              ! ( Prefs()->noAnnotationsInBookmarkList &&
                  ! ( e.flags & ANNOTATION_BOOKMARK ) ) ) {
             count++;
         }
    }

    return count;
}



/* Populate list with bookmarks ( including Add/View Bookmarks ), return number
   of bookmarks in list */
UInt16 CreatePopupBookmarkList
    (
    ListType* list  /* pointer to list */
    )
{
    isPopupList = true;
    return InitBookmarkList( list );
}



/* Populate list with bookmarks ( skip Add/View Bookmarks ), return number 
   of bookmarks in list */
UInt16 CreateBookmarkList
    (
    ListType* list
    )
{
    isPopupList = false;
    return InitBookmarkList( list );
}



/* Unlock list handle and free allocated memory */
void ReleaseBookmarkList( void )
{
    if ( bookmarkList != NULL ) {
        UInt16 i;

        for ( i = 0 ; i < numEntries ; i++ ) {
             if ( bookmarkList[ i ] != NULL ) {
                  SafeMemPtrFree( bookmarkList[ i ]->text );
                  MemPtrFree( bookmarkList[ i ] );
             }
        }
        MemPtrFree( bookmarkList );
        bookmarkList = NULL;
    }

    SafeMemPtrFree( nameList );
    nameList = NULL;
}



/* Send a goto event for internal bookmark */
void SendBookmarkGoToEvent
     (
     BookmarkListEntry* listEntry,
     Boolean            returnToForm
     )
{
    AnnotationEntry e;
    UInt16         offset;
    Header*        header;
    MemHandle      handle;
    EventType      match;

    if ( ! GetAnnotationHeaderByRecordNum( &e,
              listEntry->recordNum ) )
        return;

    handle = ReturnRecordHandle( e.id.uid );

    if ( handle == NULL )
        return;

    header = MemHandleLock( handle );

    offset = e.triggerStart +
                 GetParagraphOffset( header,
                     GET_PARAGRAPH( header, e.id.paragraphNum ) );

    MemHandleUnlock( handle );
    FreeRecordHandle( &handle );

    if ( returnToForm )
        FrmReturnToForm( GetMainFormId() );

    MemSet( &match, sizeof( EventType ), 0 );

    match.eType                         = frmGotoEvent;
    match.data.frmGoto.formID           = GetMainFormId();
    match.data.frmGoto.recordNum        =
                e.id.uid;
    match.data.frmGoto.matchPos         =
                offset;
    match.data.frmGoto.matchCustom      = GOTO_BOOKMARK;

    EvtAddEventToQueue( &match );
}



/* Restore data for current bookmark */
void GoToBookmark
    (
    UInt16 index     /* index in bookmarks list */
    )
{
    switch ( bookmarkList[ index ]->kind ) {
        case BOOKMARK_EXT_BOOKMARK:
            GoToExtBookmark( bookmarkList[ index ]->recordNum );
            break;

        case BOOKMARK_BOOKMARK:
        case BOOKMARK_ANNOTATION:
            SendBookmarkGoToEvent( bookmarkList[ index ], true );
            break;
        default:
            break;
    }
}



/* Populate array with external bookmarks, return number of bookmarks */
static UInt16 InitExtBookmarkList
    (
    Char** nameList,
    UInt16 offset     /* where to start in the list */
    )
{
    UInt16      entries;
    UInt16      i;
    MemHandle   handle;
    UInt8*      bookmarkPtr;

    handle          = NULL;
    bookmarkPtr     = NULL;

    handle  = ReturnRecordHandle( EXTERNAL_BOOKMARKS_ID );

    if ( handle == NULL )
        return NO_BOOKMARKS;

    bookmarkPtr     = MemHandleLock( handle );
    bookmarkPtr    += sizeof( Header );
    entries         = GET_EXTFIRSTFIELD( bookmarkPtr );
    bookmarkPtr    += 4;

    for ( i = offset; i < entries + offset; i++ ) {
        nameList[ i ]   = bookmarkPtr;
        bookmarkPtr    += StrLen( bookmarkPtr ) + 1;
    }

    MemHandleUnlock( handle );
    FreeRecordHandle( &handle );

    return entries;
}



/* Return the number of external bookmarks */
UInt16 CountExtBookmarks( void )
{
    UInt16      entries;
    MemHandle   handle;
    UInt8*      bookmarkPtr;

    handle          = NULL;
    bookmarkPtr     = NULL;
    entries         = 0;

    ErrTry {
        handle  = ReturnRecordHandle( EXTERNAL_BOOKMARKS_ID );
    }
    ErrCatch( UNUSED_PARAM( err ) ) {
        FreeRecordHandle( &handle );
        return 0;
    } ErrEndCatch

    if ( handle == NULL )
        return NO_BOOKMARKS;

    bookmarkPtr     = MemHandleLock( handle );
    bookmarkPtr    += sizeof( Header );
    entries         = GET_EXTFIRSTFIELD( bookmarkPtr );

    MemHandleUnlock( handle );
    FreeRecordHandle( &handle );
    return entries;
}



/* Restore data for external bookmark, return the record ID or
   NO_RECORD if it doesn't exist */
UInt16 GoToExtBookmark
    (
    UInt16 index     /* index in external bookmarks list */
    )
{

    UInt16      uid;
    UInt16      offset;
    MemHandle   handle;
    UInt8*      bookmarkPtr;

    handle          = NULL;
    bookmarkPtr     = NULL;
    offset          = 0;

    handle  = ReturnRecordHandle( EXTERNAL_BOOKMARKS_ID );

    if ( handle == NULL )
        return NO_RECORD;

    bookmarkPtr     = MemHandleLock( handle );
    bookmarkPtr    += sizeof( Header );
    bookmarkPtr    += GET_EXTSECONDFIELD( bookmarkPtr ) - sizeof( Header );
    bookmarkPtr    += index * 4;

    uid             = GET_EXTFIRSTFIELD( bookmarkPtr );
    offset          = GET_EXTSECONDFIELD( bookmarkPtr );

    SetVisitedLink( uid );

    if ( IsFullscreenformActive() )
        FsFrmGotoForm ( GetMainFormId() );
    JumpToRecord( uid, offset, NO_OFFSET );
    MemHandleUnlock( handle );
    FreeRecordHandle( &handle );

    return uid;
}
