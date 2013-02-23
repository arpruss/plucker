/*
 * $Id: mock_VFS.c,v 1.3 2004/02/01 11:26:33 nordstrom Exp $
 *
 * Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 1998-2003, Mark Ian Lillywhite and Michael Nordstrom
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

#include "CUnit.h"
#include "list.h"
#include "util.h"

#include "mock_VFS.h"


#define LABEL_LEN  256


/* Used to store data about open files */
typedef struct {
    Char    name[ LABEL_LEN ];
    FileRef fileRef;
} mockFileNode;



static Err palmVFSFileOpen( UInt16 volRefNum, const Char* pathName, UInt16 openMode,
            FileRef* fileRefPtr ) VFSMGR_TRAP( vfsTrapFileOpen );
static Err palmVFSFileClose( FileRef fileRef ) VFSMGR_TRAP( vfsTrapFileClose );



/* Used to keep track of what files we have opened */
static LinkedList vfsList;



void mock_InitVFS( void )
{
    if ( vfsList == NULL ) {
        vfsList = ListCreate();
    }
}



void mock_ReleaseVFS( void )
{
    Boolean hasNoOpenFiles = true;

    if ( vfsList != NULL ) {
        hasNoOpenFiles = ListIsEmpty( vfsList );
        ListRelease( vfsList );
        vfsList = NULL;
    }
    ASSERT_MSG( "Found not closed files", hasNoOpenFiles );
}



static mockFileNode* mock_FindFileName
    (
    const Char* name
    )
{
    mockFileNode* item;

    item = ListFirst( vfsList );
    while ( item != NULL ) {
        if ( STREQ( item->name, name ) ) {
            break;
        }
        item = ListNext( vfsList, item );
    }
    return item;
}



static mockFileNode* mock_FindFileRef
    (
    FileRef fileRef
    )
{
    mockFileNode* item;

    item = ListFirst( vfsList );
    while ( item != NULL ) {
        if ( item->fileRef == fileRef ) {
            break;
        }
        item = ListNext( vfsList, item );
    }
    return item;
}



Err mock_VFSFileOpen
    (
    UInt16      volRefNum,
    const Char* pathName,
    UInt16      openMode,
    FileRef*    fileRefPtr
    )
{
    Err             err = vfsErrFilePermissionDenied;
    mockFileNode*   mockFile;

    ASSERT_MSG_GOTOERR( "File is already open",
        mock_FindFileName( pathName ) == NULL );

    err = palmVFSFileOpen( volRefNum, pathName, openMode, fileRefPtr );
    if ( err == errNone ) {
        mockFile = SafeMemPtrNew( sizeof *mockFile );
        StrCopy( mockFile->name, pathName );
        mockFile->fileRef = *fileRefPtr;
        ListAppend( vfsList, mockFile );
    }
    
ERR:
    return err;
}



Err mock_VFSFileClose
    (
    FileRef fileRef
    )
{
    mockFileNode*   mockFile;

    mockFile = mock_FindFileRef( fileRef );
    ASSERT_MSG_GOTOERR( "File is not open", mockFile != NULL );
    ListTakeOut( vfsList, mockFile );
    SafeMemPtrFree( mockFile );

    return palmVFSFileClose( fileRef );

ERR:
    return vfsErrFilePermissionDenied;
}

