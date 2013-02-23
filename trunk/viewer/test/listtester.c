/*
 * $Id: listtester.c,v 1.3 2004/01/01 21:51:21 nordstrom Exp $
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
#include "list.h"

#include "listtester.h"

static void AddTwoItems( void ) UNIT_TEST_SECTION;


static LinkedList list;
 

static void AddTwoItems( void )
{
    UInt16* item1;
    UInt16* item2;

    item1 = MemPtrNew( sizeof *item1 );
    item2 = MemPtrNew( sizeof *item2 );
    *item1 = 6;
    *item2 = 10;
    ListAppend( list, item1 );
    ListAppend( list, item2 );
}



int ListSuiteSetUp( void )
{
    return 0;
}



int ListSuiteTearDown( void )
{
    return 0;
}



int ListInitTest( void )
{
    list = ListCreate();

    return 0;
}



int ListCleanupTest( void )
{
    ListRelease( list );

    return 0;
}



void TestAppendItem( void )
{
    UInt16* item1;
    UInt16* item2;
    UInt16* data1;
    UInt16* data2;

    ASSERT( ListIsEmpty( list ));

    data1 = MemPtrNew( sizeof *data1 );
    *data1 = 6;
    ListAppend( list, data1 );
    ASSERT( ListSize( list ) == 1 );
    ASSERT( ! ListIsEmpty( list ));

    data2 = MemPtrNew( sizeof *data2 );
    *data2 = 10;
    ListAppend( list, data2 );
    ASSERT( ListSize( list ) == 2 );

    item1 = ListGet( list, 1 );
    item2 = ListGet( list, 2 );
    ASSERT_UINT16_EQUAL_MSG( "First item: ", 6, *item1 );
    ASSERT_UINT16_EQUAL_MSG( "Second item: ", 10, *item2 );
}



void TestGetItem( void )
{
    UInt16* item1;
    UInt16* item2;

    ASSERT( ListIsEmpty( list ));

    AddTwoItems();

    item1 = ListGet( list, 3 );
    ASSERT_MSG( "Item shouldn't be in list: ", item1 == NULL );

    item1 = ListGet( list, 1 );
    item2 = ListGet( list, 2 );
    ASSERT_UINT16_EQUAL_MSG( "First item: ", 6, *item1 );
    ASSERT_UINT16_EQUAL_MSG( "Second item: ", 10, *item2 );

    item1 = ListGet( list, 0 );
    ASSERT_MSG( "Item shouldn't be in list: ", item1 == NULL );
}



void TestTraverse( void )
{
    UInt16* item1;
    UInt16* item2;

    ASSERT( ListIsEmpty( list ));

    AddTwoItems();

    item1 = ListFirst( list );
    item2 = ListNext( list, item1 );
    ASSERT_UINT16_EQUAL_MSG( "First item: ", 6, *item1 );
    ASSERT_UINT16_EQUAL_MSG( "Next item: ", 10, *item2 );

    item1 = ListLast( list );
    ASSERT_UINT16_EQUAL_MSG( "Last item: ", 10, *item1 );

    item2 = ListNext( list, item1 );
    ASSERT_MSG( "Item after last in list: ", item2 == NULL );
}



void TestRemove( void )
{
    UInt16* item1;
    UInt16* item2;

    ASSERT( ListIsEmpty( list ));

    AddTwoItems();

    item1 = ListFirst( list );
    ListTakeOut( list, item1 );
    ASSERT( ListSize( list ) == 1 );

    MemPtrFree( item1 );

    item1 = ListFirst( list );
    item2 = ListLast( list );
    ASSERT_UINT16_EQUAL_MSG( "First item: ", 10, *item1 );
    ASSERT_UINT16_EQUAL_MSG( "Last item: ", 10, *item2 );

    ListTakeOut( list, item2 );
    ASSERT( ListSize( list ) == 0 );
    ASSERT( ListFirst( list ) == NULL );
    ASSERT( ListLast( list ) == NULL );

    MemPtrFree( item2 );
}



void InitializeListSuite( void )
{
    TestGroup* group;

    /* List Tester */
    group = add_test_group( "List", ListSuiteSetUp, ListSuiteTearDown,
                ListInitTest, ListCleanupTest );
    add_test_case( group, "Append", TestAppendItem );
    add_test_case( group, "Get", TestGetItem );
    add_test_case( group, "Traverse", TestTraverse );
    add_test_case( group, "Remove", TestRemove );
}



void TerminateListSuite( void )
{
}

