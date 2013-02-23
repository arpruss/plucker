/*
 * $Id: vfsfiletester.c,v 1.3 2004/01/04 12:03:18 nordstrom Exp $
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

#include "TestDB.h"
#include "os.h"
#include "vfsfile.h"
#include "doclist.h"
#include "prefsdata.h"
#include "mock_VFS.h"
#include "mock_Feature.h"
#include "mock_Device.h"

#include "vfsfiletester.h"



int VFSFileSuiteSetUp( void )
{
    OS_Init();
    ReadPrefs();
    Prefs()->syncPolicy = SYNC_MANUAL;
    OpenDocList();

    return 0;
}



int VFSFileSuiteTearDown( void )
{
    CloseDocList();
    OS_Release();

    return 0;
}



int VFSFileInitTest( void )
{
    mock_InitVFS();

    return 0;
}



int VFSFileCleanupTest( void )
{
    mock_ReleaseVFS();

    return 0;
}



void TestEnumerateDocs( void )
{
    ErrTry {
        EnumerateCards( ENUMERATECARD_DOCS );
    }
    ErrCatch( err ) {
        ASSERT_UINT16_EQUAL_MSG( "Failed to enumerate card", errNone, err );
    } ErrEndCatch
}



void TestEnumerateFonts( void )
{
    ErrTry {
        EnumerateCards( ENUMERATECARD_FONTS );
    }
    ErrCatch( UNUSED_PARAM( err ) ) {
        ASSERT_UINT16_EQUAL_MSG( "Failed to enumerate card", errNone, err );
    } ErrEndCatch
}



void TestEnumerateDocsFonts( void )
{
    ErrTry {
        EnumerateCards( ENUMERATECARD_DOCS | ENUMERATECARD_FONTS );
    }
    ErrCatch( UNUSED_PARAM( err ) ) {
        ASSERT_UINT16_EQUAL_MSG( "Failed to enumerate card", errNone, err );
    } ErrEndCatch
}



void InitializeVFSFileSuite( void )
{
    TestGroup* group;

    /* We can only test the VFS support on a device with VFS support */
    if ( mock_GetRealVFSVersion() != 0 ) {
        /* VFS File Tester */
        group = add_test_group( "VFSFile", VFSFileSuiteSetUp,
                    VFSFileSuiteTearDown, VFSFileInitTest,
                    VFSFileCleanupTest );
        add_test_case( group, "Enumerate card (docs)", TestEnumerateDocs );
        add_test_case( group, "Enumerate card (fonts)", TestEnumerateFonts );
        add_test_case( group, "Enumerate card (docs | fonts)",
            TestEnumerateDocsFonts );
    }
}



void TerminateVFSFileSuite( void )
{
}

