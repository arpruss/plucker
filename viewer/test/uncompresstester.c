/*
 * $Id: uncompresstester.c,v 1.6 2004/01/11 17:33:32 nordstrom Exp $
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
#include "document.h"
#include "uncompress.h"
#include "mock_Device.h"

#include "uncompresstester.h"


#define ZLIB_DOCUMENT       "UnitTest"
#define DOC_DOCUMENT        "UnitTestDOC"



static DocumentInfo docInfo;

/* Known data from the records in UnitTest.pdb */
/* Note: record 12 is not compressed */
static UInt16 record2Size = 96;
static UInt8* record2Data = "This is a test document used for unit test. It has one link of depth 2.  S     
 Link A1   8  ";
static UInt16 record11Size = 96;
static UInt8* record11Data = "This is a test document used for unit test. It has one link of depth 1.  S     
 Link A2   8  ";


/* Known data from the records in UnitTestDOC.pdb */
static UInt16 DOCrecord2Size = 96;
static UInt8* DOCrecord2Data = "This is a test document used for unit test. It has one link of depth 2.  S     
 Link A1   8  ";
static UInt16 DOCrecord11Size = 96;
static UInt8* DOCrecord11Data = "This is a test document used for unit test. It has one link of depth 1.  S     
 Link A2   8  ";
static UInt16 DOCrecord12Size = 94;
static UInt8* DOCrecord12Data = "This is a test document used for unit test. It has one external link.  S     
 Link A3   8  ";



int UncompressSuiteSetUp( void )
{
    /* Check that we have access to the test documents we require */
    if ( DmFindDatabase( 0, ZLIB_DOCUMENT ) == 0 ||
         DmFindDatabase( 0, DOC_DOCUMENT ) == 0 )
        return 1;

    return OpenTestDocument( ZLIB_DOCUMENT, &docInfo );
}



int UncompressSuiteTearDown( void )
{
    CloseTestDocument( ZLIB_DOCUMENT );

    return 0;
}



int UncompressInitTest( void )
{
    CreateUncompressBuffer();

    return 0;
}



int UncompressCleanupTest( void )
{
    DeleteUncompressBuffer();

    return 0;
}



void TestCreateBuffer( void )
{
    LocalID     dbID;
    DmOpenRef   dbRef;

    DeleteUncompressBuffer();

    CreateUncompressBuffer();

    dbID = DmFindDatabase( 0, "Plkr-UnCoMpReSS" );
    ASSERT_MSG( "No buffer created", dbID != 0 );

    dbRef = DmOpenDatabase( 0, dbID, dmModeReadOnly );
    ASSERT_MSG( "Couldn't open buffer", dbRef != NULL );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong number of records", 3, DmNumRecords( dbRef ) );

ERR:
    DmCloseDatabase( dbRef );
}



void TestDeleteBuffer( void )
{
    LocalID dbID;

    DeleteUncompressBuffer();

    dbID = DmFindDatabase( 0, "Plkr-UnCoMpReSS" );
    ASSERT_MSG( "Buffer wasn't deleted", dbID == 0 );
}



void TestUnDoc( void )
{
    MemHandle   inHandle    = NULL;
    MemHandle   outHandle   = NULL;
    Header*     header      = NULL;
    UInt8*      data        = NULL;

    /* We must use a DOC compressed document instead of the default
       (ZLib) compressed document */
    CloseTestDocument( ZLIB_DOCUMENT );
    OpenTestDocument( DOC_DOCUMENT, &docInfo );

    /* Get handle for home page and lock it */
    inHandle    = GetRecordHandle( 2 );
    header      = MemHandleLock( inHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong record ID: ", 2, header->uid );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong number of paragraphs: ", 2,
        header->paragraphs );

    /* Uncompress the data and check the result */
    outHandle   = UnDoc( header );
    data        = MemHandleLock( outHandle );
    ASSERT_MSG_GOTOERR( "Wrong handle", GetUncompressTextHandle() == outHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong size: ", header->size,
        MemPtrSize( data ) );
    ASSERT_MSG_GOTOERR( "Wrong data: ", CheckRecordData( data, DOCrecord2Data,
                                            DOCrecord2Size ) );

    MemHandleUnlock( outHandle );
    MemHandleUnlock( inHandle );

    /* Get handle for record 11 and lock it */
    inHandle    = GetRecordHandle( 11 );
    header      = MemHandleLock( inHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong record ID: ", 11, header->uid );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong number of paragraphs: ", 2,
        header->paragraphs );

    /* Uncompress the data and check the result */
    outHandle   = UnDoc( header );
    data        = MemHandleLock( outHandle );
    ASSERT_MSG_GOTOERR( "Wrong handle", GetUncompressTextHandle() == outHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong size: ", header->size,
        MemPtrSize( data ) );
    ASSERT_MSG_GOTOERR( "Wrong data: ", CheckRecordData( data, DOCrecord11Data,
                                            DOCrecord11Size ) );

    MemHandleUnlock( outHandle );
    MemHandleUnlock( inHandle );

    /* Get handle for record 12 and lock it */
    inHandle    = GetRecordHandle( 12 );
    header      = MemHandleLock( inHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong record ID: ", 12, header->uid );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong number of paragraphs: ", 2,
        header->paragraphs );

    /* Uncompress the data and check the result */
    outHandle   = UnDoc( header );
    data        = MemHandleLock( outHandle );
    ASSERT_MSG_GOTOERR( "Wrong handle", GetUncompressTextHandle() == outHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong size: ", header->size,
        MemPtrSize( data ) );
    ASSERT_MSG_GOTOERR( "Wrong data: ", CheckRecordData( data, DOCrecord12Data,
                                            DOCrecord12Size ) );

ERR:
    MemHandleUnlock( outHandle );
    MemHandleUnlock( inHandle );
    CloseTestDocument( DOC_DOCUMENT );
    OpenTestDocument( ZLIB_DOCUMENT, &docInfo );
}



void TestUnZip( void )
{
    MemHandle   inHandle    = NULL;
    MemHandle   outHandle   = NULL;
    Header*     header      = NULL;
    UInt8*      data        = NULL;

    SetUncompressKey( NULL );

    /* Get handle for home page and lock it */
    inHandle    = GetRecordHandle( 2 );
    header      = MemHandleLock( inHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong record ID: ", 2, header->uid );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong number of paragraphs: ", 2,
        header->paragraphs );

    /* Uncompress the data and check the result */
    outHandle   = UnZip( header );
    data        = MemHandleLock( outHandle );
    ASSERT_MSG_GOTOERR( "Wrong handle", GetUncompressTextHandle() == outHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong size: ", header->size,
        MemPtrSize( data ) );
    ASSERT_MSG_GOTOERR( "Wrong data: ", CheckRecordData( data, record2Data,
                                            record2Size ) );

    MemHandleUnlock( outHandle );
    MemHandleUnlock( inHandle );

    /* Get handle for record 11 and lock it */
    inHandle    = GetRecordHandle( 11 );
    header      = MemHandleLock( inHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong record ID: ", 11, header->uid );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong number of paragraphs: ", 2,
        header->paragraphs );

    /* Uncompress the data and check the result */
    outHandle   = UnZip( header );
    data        = MemHandleLock( outHandle );
    ASSERT_MSG_GOTOERR( "Wrong handle", GetUncompressTextHandle() == outHandle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong size: ", header->size,
        MemPtrSize( data ) );
    ASSERT_MSG_GOTOERR( "Wrong data: ", CheckRecordData( data, record11Data,
                                            record11Size ) );

ERR:
    MemHandleUnlock( outHandle );
    MemHandleUnlock( inHandle );
    ReleaseUncompressKey();
}



void InitializeUncompressSuite( void )
{
    TestGroup* group;

    /* Uncompress Tester */
    group = add_test_group( "Uncompress", UncompressSuiteSetUp,
                UncompressSuiteTearDown, UncompressInitTest,
                UncompressCleanupTest );
    add_test_case( group, "Create buffer", TestCreateBuffer );
    add_test_case( group, "Delete buffer", TestDeleteBuffer );
    add_test_case( group, "UnDoc", TestUnDoc );
    add_test_case( group, "UnZip", TestUnZip );
}



void TerminateUncompressSuite( void )
{
}

