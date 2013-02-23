/*
 * $Id: genericfiletester.c,v 1.3 2004/01/04 12:03:18 nordstrom Exp $
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
#include "mock_Device.h"
#include "genericfile.h"
#include "metadocument.h"
#include "os.h"
#include "const.h"
#include "util.h"
#include "prefsdata.h"

#include "genericfiletester.h"


#define TEST_DOCUMENT       "UnitTest"


int GenericFileSuiteSetUp( void )
{
    return 0;
}



int GenericFileSuiteTearDown( void )
{
    return 0;
}



int GenericFileInitTest( void )
{
    return 0;
}



int GenericFileCleanupTest( void )
{
    return 0;
}



void TestOpenDocument( void )
{
    Err             err;
    DocumentInfo    docInfo;

    OS_Init();
    ReadPrefs();
    SetStandardFunctions();

    DeleteMetaDocument( TEST_DOCUMENT, 0 );

    StrCopy( docInfo.name, TEST_DOCUMENT );
    docInfo.cardNo      = 0;
    docInfo.size        = 550;
    docInfo.created     = -1139389298;
    docInfo.attributes  = 512;
    docInfo.categories  = 1;
    docInfo.location    = 0;
    docInfo.active      = true;
    docInfo.timestamp   = 0;
    docInfo.volumeRef   = 0;
    docInfo.numRecords  = 0;

    err = OpenDocument( &docInfo );
    ASSERT_MSG( "UnitTest.pdb must be installed", err == errNone );

    CloseDocument();
    OS_Release();
}



void InitializeGenericFileSuite( void )
{
    TestGroup* group;

    /* GenericFile Tester */
    group = add_test_group( "GenericFile", GenericFileSuiteSetUp,
                GenericFileSuiteTearDown, GenericFileInitTest,
                GenericFileCleanupTest );
    add_test_case( group, "Open", TestOpenDocument );
}



void TerminateGenericFileSuite( void )
{
}

