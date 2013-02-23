/*
 * $Id: metadocumenttester.c,v 1.10 2004/02/01 11:26:33 nordstrom Exp $
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
#include "const.h"
#include "util.h"
#include "testutil.h"
#include "metadocument.h"
#include "mock_Device.h"

#include "metadocumenttester.h"


#define TEST_DOCUMENT       "UnitTest"
#define META_TEST_DOCUMENT  "Plkr-UnitTest"


static DocumentInfo docInfo;


int MetaDocumentSuiteSetUp( void )
{
    Err err;

    /* Check that we have access to the test documents we require */
    if ( DmFindDatabase( 0, TEST_DOCUMENT ) == 0 )
        return 1;

    err = OpenTestDocument( TEST_DOCUMENT, &docInfo );
    if ( err == errNone ) {
        /* When we open the test document a meta document will be
           created and opened; we should close it before we continue
           with the test */
        CloseMetaDocument();
    }

    return err;
}



int MetaDocumentSuiteTearDown( void )
{
    CloseTestDocument( TEST_DOCUMENT );

    return 0;
}



int MetaDocumentInitTest( void )
{
    LocalID dbID;

    dbID = DmFindDatabase( 0, META_TEST_DOCUMENT );
    if ( dbID != 0 ) {
        DmDeleteDatabase( 0, dbID );
    }

    return 0;
}



int MetaDocumentCleanupTest( void )
{
    return 0;
}



void TestOpenMetaDocument( void )
{
    LocalID metaID;
    Err     err;
    UInt32  metaDate;
    UInt16  metaVersion;

    /* Since we start out with no meta document it should be created when
       we call OpenMetaDocument */
    ErrTry {
        OpenMetaDocument( TEST_DOCUMENT, 0, docInfo.created );
    }
    ErrCatch( err ) {
        ASSERT_MSG_GOTOERR( "Caught exception\n", false );
    } ErrEndCatch

    /* Check created meta document */
    metaID = DmFindDatabase( 0, META_TEST_DOCUMENT );
    ASSERT_GOTOERR( metaID != 0 );
    err = DmDatabaseInfo( 0, metaID, NULL, NULL, &metaVersion, &metaDate,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL );
    ASSERT_GOTOERR( err == errNone );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong version:", MetaDocumentVersion,
        metaVersion );
    ASSERT_UINT32_EQUAL_MSG_GOTOERR( "Wrong date:", docInfo.created,
        metaDate );

    CloseMetaDocument();

    /* This time we have an existing meta document that should be opened when
       we call OpenMetaDocument */
    ErrTry {
        OpenMetaDocument( TEST_DOCUMENT, 0, docInfo.created );
    }
    ErrCatch( err ) {
        ASSERT_MSG_GOTOERR( "Caught exception\n", false );
    } ErrEndCatch

    /* Check meta document */
    metaID = DmFindDatabase( 0, META_TEST_DOCUMENT );
    ASSERT_GOTOERR( metaID != 0 );
    err = DmDatabaseInfo( 0, metaID, NULL, NULL, &metaVersion, &metaDate,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL );
    ASSERT_GOTOERR( err == errNone );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong version:", MetaDocumentVersion,
        metaVersion );
    ASSERT_UINT32_EQUAL_MSG_GOTOERR( "Wrong date:", docInfo.created,
        metaDate );

    CloseMetaDocument();

    /* Simulate a new document by changing the date argument; this should
       delete the current meta document and create a new meta document */
    ErrTry {
        OpenMetaDocument( TEST_DOCUMENT, 0, docInfo.created + 10 );
    }
    ErrCatch( err ) {
        ASSERT_MSG_GOTOERR( "Caught exception\n", false );
    } ErrEndCatch

    /* Check created meta document */
    metaID = DmFindDatabase( 0, META_TEST_DOCUMENT );
    ASSERT_GOTOERR( metaID != 0 );
    err = DmDatabaseInfo( 0, metaID, NULL, NULL, &metaVersion, &metaDate,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL );
    ASSERT_GOTOERR( err == errNone );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong version:", MetaDocumentVersion,
        metaVersion );
    ASSERT_UINT32_EQUAL_MSG_GOTOERR( "Wrong date: ", docInfo.created + 10,
        metaDate );

    CloseMetaDocument();

    /* Simulate a new document by changing the version before calling
       OpenMetaDocument; this should delete the current meta document
       and create a new meta document */
    metaVersion = MetaDocumentVersion - 1;
    metaID      = DmFindDatabase( 0, META_TEST_DOCUMENT );
    err         = DmSetDatabaseInfo( 0, metaID, NULL, NULL, &metaVersion,
                    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
    ASSERT_GOTOERR( err == errNone );
    ErrTry {
        OpenMetaDocument( TEST_DOCUMENT, 0, docInfo.created );
    }
    ErrCatch( err ) {
        ASSERT_MSG_GOTOERR( "Caught exception\n", false );
    } ErrEndCatch

    /* Check created meta document */
    metaID = DmFindDatabase( 0, META_TEST_DOCUMENT );
    ASSERT_GOTOERR( metaID != 0 );
    err = DmDatabaseInfo( 0, metaID, NULL, NULL, &metaVersion, &metaDate,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL );
    ASSERT_GOTOERR( err == errNone );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong version:", MetaDocumentVersion,
        metaVersion );
    ASSERT_UINT32_EQUAL_MSG_GOTOERR( "Wrong date:", docInfo.created,
        metaDate );

ERR:
    CloseMetaDocument();
}



void TestRemoveMetaDocument( void )
{
    Err err;

    /* Create meta document */
    err = DmCreateDatabase( 0, META_TEST_DOCUMENT, ViewerAppID,
            MetaDocumentType, false );
    ASSERT_MSG( "Failed to create meta document", err == errNone );
    ASSERT_MSG( "Couldn't find meta document",
        DmFindDatabase( 0, META_TEST_DOCUMENT ) != 0 );

    /* Delete the meta document and check that it is gone */
    DeleteMetaDocument( TEST_DOCUMENT, 0 );
    ASSERT_MSG( "Meta document isn't removed",
        DmFindDatabase( 0, META_TEST_DOCUMENT ) == 0 );
}



void InitializeMetaDocumentSuite( void )
{
    TestGroup* group;

    /* MetaDocument Tester */
    group = add_test_group( "MetaDocument", MetaDocumentSuiteSetUp,
                MetaDocumentSuiteTearDown, MetaDocumentInitTest,
                MetaDocumentCleanupTest );
    add_test_case( group, "Open", TestOpenMetaDocument );
    add_test_case( group, "Remove", TestRemoveMetaDocument );
}



void TerminateMetaDocumentSuite( void )
{
}

