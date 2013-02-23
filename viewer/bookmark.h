/*
 * $Id: bookmark.h,v 1.19 2006/11/12 02:00:55 prussar Exp $
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

#ifndef PLUCKER_BOOKMARK_H
#define PLUCKER_BOOKMARK_H

#include "viewer.h"

/* commands in dropdown list */
#define ADD_BOOKMARK            0
#ifdef SUPPORT_ANNOTATION
# define ADD_ANNOTATION          1
# define EDIT_BOOKMARK           2
#else
# define EDIT_BOOKMARK          1
#endif 

#define BOOKMARKLISTWIDTH 154

typedef enum {
    BOOKMARK_EXT_BOOKMARK = 0,
    BOOKMARK_ANNOTATION,
    BOOKMARK_BOOKMARK,
    BOOKMARK_ADD_BOOKMARK,
#ifdef SUPPORT_ANNOTATION
    BOOKMARK_ADD_ANNOTATION,
#endif
    BOOKMARK_EDIT_BOOKMARK, /* really should be called BOOKMARK_VIEW_BOOKMARKS */
    BOOKMARK_INVALID
} BookmarkKind;

typedef struct {
    BookmarkKind kind;
    Char*   text;
    UInt16  recordNum;
} BookmarkListEntry;


/* Restore data for current bookmark, return the record ID or if there are
   no bookmarks NOT_FOUND */
extern UInt16 RestoreBookmarkData(UInt16 index) BOOKMARK_SECTION;

/* Populate list with bookmarks ( including Add/View Bookmarks ), return number
   of bookmarks in list */
extern UInt16 CreatePopupBookmarkList(ListType *list) BOOKMARK_SECTION;

/* Populate list with bookmarks ( skip Add/View Bookmarks ), return number
   of bookmarks in list */
extern UInt16 CreateBookmarkList(ListType *list) BOOKMARK_SECTION;

/* Delete bookmark, return true if list is empty */
extern void DeleteBookmark(UInt16 index) BOOKMARK_SECTION;

/* Unlock list handle and free allocated memory */
extern void ReleaseBookmarkList(void) BOOKMARK_SECTION;

/* Reset the verticalOffset fields in all the bookmarks */
extern void ResetBookmarkVerticalOffsets(void) BOOKMARK_SECTION;

/* Return the number of external bookmarks */
extern UInt16 CountExtBookmarks(void) BOOKMARK_SECTION;

/* Go to a bookmark, internal or external */
void GoToBookmark ( UInt16 index ) CONTROL_SECTION;

/* Go to an external bookmark */
extern UInt16 GoToExtBookmark(UInt16 index) BOOKMARK_SECTION;

/* Send a goto event for internal bookmark */
void SendBookmarkGoToEvent ( BookmarkListEntry* listEntry,
    Boolean returnToForm ) BOOKMARK_SECTION;

/* Move old bookmarks into annotations */
extern void UpgradeBookmarks( void ) BOOKMARK_SECTION;

/* Get an entry from the bookmark list */
extern BookmarkListEntry* GetBookmarkListEntry( UInt16 selection )
    BOOKMARK_SECTION;

#endif

