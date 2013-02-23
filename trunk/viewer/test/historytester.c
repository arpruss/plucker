/*
 * $Id: historytester.c,v 1.8 2004/01/04 12:03:18 nordstrom Exp $
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
#include "genericfile.h"
#include "metadocument.h"
#include "history.h"
#include "testutil.h"
#include "mock_Device.h"

#include "historytester.h"

#define NO_RECORD       0xFFFF

static DocumentInfo docInfo;

static void InitializeTestData( void ) UNIT_TEST_SECTION;
static void ReleaseTestData( void ) UNIT_TEST_SECTION;

/* Open test document, add meta records to meta database, and
   initialize history structure */
static void InitializeTestData( void )
{
    Err err;

    err = OpenTestDocument( "UnitTest", &docInfo );
    if ( err == errNone ) {
        History* history;

        history = GetHistoryPtr();
        ASSERT( history != NULL );
        history->addHistory = true;

        ASSERT( ReturnMetaHandle( 2, 2 ) != NULL );
        ASSERT( ReturnMetaHandle( 11, 2 ) != NULL ); 
        ASSERT( ReturnMetaHandle( 12, 2 ) != NULL );
        AddToHistory( 11 );
        AddToHistory( 12 );
    }
}



static void ReleaseTestData( void )
{
    CloseTestDocument( "UnitTest" );
}



int HistorySuiteSetUp( void )
{
    /* Check that we have access to the test documents we require */
    if ( DmFindDatabase( 0, "UnitTest" ) == 0 )
        return 1;

    return 0;
}



int HistorySuiteTearDown( void )
{
    return 0;
}



int HistoryInitTest( void )
{
    InitHistory();

    return 0;
}



int HistoryCleanupTest( void )
{
    return 0;
}



void TestAddToHistory( void )
{
    History* history;

    history = GetHistoryPtr();
    ASSERT_MSG( "Add to history: ", history->addHistory == false );
    ASSERT_UINT16_EQUAL_MSG( "CurrentRecord: ", 0, history->currentRecord );
    ASSERT_UINT16_EQUAL_MSG( "First record: ", 0, history->firstRecord );
    ASSERT_UINT16_EQUAL_MSG( "Last record: ", 0, history->lastRecord );
    ASSERT_UINT16_EQUAL_MSG( "Record ID for first record: ", HOME_PAGE_ID,
        history->records[ 0 ].recordId );
    ASSERT_UINT16_EQUAL_MSG( "Record ID for second record: ", 0,
        history->records[ 1 ].recordId );

    /* First time we call AddToHistory it shouldn't do anything but
       setting the addHistory flag */
    AddToHistory( 11 );
    ASSERT_MSG( "Add to history: ", history->addHistory == true );
    ASSERT_UINT16_EQUAL_MSG( "CurrentRecord: ", 0, history->currentRecord );
    ASSERT_UINT16_EQUAL_MSG( "First record: ", 0, history->firstRecord );
    ASSERT_UINT16_EQUAL_MSG( "Last record: ", 0, history->lastRecord );
    ASSERT_UINT16_EQUAL_MSG( "Record ID for first record: ", HOME_PAGE_ID,
        history->records[ 0 ].recordId );
    ASSERT_UINT16_EQUAL_MSG( "Record ID for second record: ", 0,
        history->records[ 1 ].recordId );

    AddToHistory( 11 );
    ASSERT_MSG( "Add to history: ", history->addHistory == true );
    ASSERT_UINT16_EQUAL_MSG( "CurrentRecord: ", 1, history->currentRecord );
    ASSERT_UINT16_EQUAL_MSG( "First record: ", 0, history->firstRecord );
    ASSERT_UINT16_EQUAL_MSG( "Last record: ", 1, history->lastRecord );
    ASSERT_UINT16_EQUAL_MSG( "Record ID for first record: ", HOME_PAGE_ID,
        history->records[ 0 ].recordId );
    ASSERT_UINT16_EQUAL_MSG( "Record ID for second record: ", 11,
        history->records[ 1 ].recordId );
}



void TestGetFromHistory( void )
{
    History*    history;
    UInt16      recordId;

    InitializeTestData();

    history = GetHistoryPtr();

    recordId = GetHistoryNext();
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "GetHistoryNext: ", NO_RECORD, recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "CurrentRecord: ", 2,
        history->currentRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "First record: ", 0,
        history->firstRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Last record: ", 2, history->lastRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for first record: ", 2,
        history->records[ 0 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for second record: ", 11,
        history->records[ 1 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for third record: ", 12,
        history->records[ 2 ].recordId );

    recordId = GetHistoryPrev();
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "GetHistoryPrev: ", 11, recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "CurrentRecord: ", 1,
        history->currentRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "First record: ", 0,
        history->firstRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Last record: ", 2, history->lastRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for first record: ", 2,
        history->records[ 0 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for second record: ", 11,
        history->records[ 1 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for third record: ", 12,
        history->records[ 2 ].recordId );

    recordId = GetHistoryNext();
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "GetHistoryNext: ", 12, recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "CurrentRecord: ", 2,
        history->currentRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "First record: ", 0,
        history->firstRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Last record: ", 2, history->lastRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for first record: ", 2,
        history->records[ 0 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for second record: ", 11,
        history->records[ 1 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for third record: ", 12,
        history->records[ 2 ].recordId );

    recordId = GetHistoryPrev();
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "GetHistoryPrev: ", 11, recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "CurrentRecord: ", 1,
        history->currentRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "First record: ", 0,
        history->firstRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Last record: ", 2, history->lastRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for first record: ", 2,
        history->records[ 0 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for second record: ", 11,
        history->records[ 1 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for third record: ", 12,
        history->records[ 2 ].recordId );

    recordId = GetHistoryPrev();
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "GetHistoryPrev: ", 2, recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "CurrentRecord: ", 0,
        history->currentRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "First record: ", 0,
        history->firstRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Last record: ", 2, history->lastRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for first record: ", 2,
        history->records[ 0 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for second record: ", 11,
        history->records[ 1 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for third record: ", 12,
        history->records[ 2 ].recordId );

    recordId = GetHistoryPrev();
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "GetHistoryPrev: ", NO_RECORD, recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "CurrentRecord: ", 0,
        history->currentRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "First record: ", 0,
        history->firstRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Last record: ", 2, history->lastRecord );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for first record: ", 2,
        history->records[ 0 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for second record: ", 11,
        history->records[ 1 ].recordId );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Record ID for third record: ", 12,
        history->records[ 2 ].recordId );

ERR:
    ReleaseTestData();
}



void InitializeHistorySuite( void )
{
    TestGroup* group;

    /* History Tester */
    group = add_test_group( "History", HistorySuiteSetUp, HistorySuiteTearDown,
                HistoryInitTest, HistoryCleanupTest );
    add_test_case( group, "Add", TestAddToHistory );
    add_test_case( group, "Get", TestGetFromHistory );
}



void TerminateHistorySuite( void )
{
}

