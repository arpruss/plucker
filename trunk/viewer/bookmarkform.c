/*
 * $Id: bookmarkform.c,v 1.45 2006/11/12 02:00:55 prussar Exp $
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

#include <TxtGlue.h>

#include "bookmark.h"
#include "debug.h"
#include "document.h"
#include "fullscreenform.h"
#include "hires.h"
#include "image.h"
#include "prefsdata.h"
#include "resourceids.h"
#include "table.h"
#include "util.h"
#include "bookmarkform.h"
#include "annotation.h"


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define DELETE_BOOKMARK         0
#define MAX_BOOKMARK_LIST_LEN   22


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void BookmarksFormInit( void ) BOOKMARKFORM_SECTION;


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static UInt16       recordID  = NO_RECORD;
static Boolean      deleted   = false;



/* Return record ID for selected bookmark */
UInt16 GetBookmarkRecordID( void )
{
    return recordID;
}



/* Initialize the bookmark form */
static void BookmarksFormInit( void )
{
    FormType* bookmarksForm;
    ListType* list;

    bookmarksForm = FrmGetFormPtr( frmBookmarks );
    deleted = false;

    list = GetObjectPtr( frmBookmarksList );
    CreateBookmarkList( list );
//    LstSetHeight( list, MAX_BOOKMARK_LIST_LEN );

    FrmDrawForm( bookmarksForm );
}



/* Event handler for the bookmark form */
Boolean BookmarksFormHandleEvent
    (
    EventType* event  /* pointer to an EventType structure */
    )
{
    Boolean handled    = false;

    switch ( event->eType ) {
        case winEnterEvent:
            handled = ResizeHandleWinEnterEvent();
            break;

        case winDisplayChangedEvent:
            handled = ResizeHandleWinDisplayChangedEvent();
            break;

        case winExitEvent:
            handled = ResizeHandleWinExitEvent();
            break;

        case frmOpenEvent:
#ifdef HAVE_SILKSCREEN
            ResizeHandleFrmOpenEvent();
#endif
            BookmarksFormInit();
            handled = true;
            break;

        case frmCloseEvent:
            ReleaseBookmarkList();
#ifdef HAVE_SILKSCREEN
            ResizeHandleFrmCloseEvent();
#endif
            handled = false;
            break;

        case ctlSelectEvent:
            if ( event->data.ctlEnter.controlID == frmBookmarksDelete ) {
                ListType*   list;
                Int16       selection;

                list        = GetObjectPtr( frmBookmarksList );
                selection   = LstGetSelection( list );
                if ( selection != noListSelection ) {
                    BookmarkListEntry* e;

                    e = GetBookmarkListEntry( selection );

                    if ( e->kind == BOOKMARK_ANNOTATION ||
                         e->kind == BOOKMARK_BOOKMARK ) {
                        UInt16 choice;

                        choice = FrmCustomAlert( confirmDelete,
                                    LstGetSelectionText( list, selection ), NULL,
                                        NULL );
                        if ( choice == DELETE_BOOKMARK ) {
                            ErrTry {
                                DeleteAnnotationByRecordNum( e->recordNum );
                                ReleaseBookmarkList();
                                CreateBookmarkList( list );
                                LstDrawList( list );
                                deleted = true;
                            }
                            ErrCatch( UNUSED_PARAM( err ) ) {
                                FrmCustomAlert( errCannotDeleteBookmark,
                                    LstGetSelectionText( list, selection ), NULL,
                                    NULL );
                            } ErrEndCatch
                        }
                     }
                }
                handled = true;
                break;
            }
            else if ( event->data.ctlEnter.controlID == frmBookmarksGo ) {
                ListType*   list;
                Int16       selection;

                list        = GetObjectPtr( frmBookmarksList );
                selection   = LstGetSelection( list );

                GoToBookmark( selection );
                ReleaseBookmarkList();
                handled = true;
                break;
            }
            else if ( event->data.ctlEnter.controlID == frmBookmarksDone ) {
                ReleaseBookmarkList();
                FrmReturnToForm( PREVIOUS_FORM );
                if ( deleted && IsFormMain( Prefs()->lastForm ) )
                    FrmUpdateForm( GetMainFormId(), frmRedrawUpdateCode );
                handled = true;
                break;
            }
            break;

        default:
            handled = false;
    }

    return handled;
}

