/*
 * $Id: MODULEtester.c,v 1.1 2004/01/01 14:42:51 nordstrom Exp $
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

/******************************************************************
 * Replace <MODULE> with name of the module that should be tested *
 ******************************************************************/

#include "TestDB.h"

#include "<MODULE>tester.h"



/* Add any actions that must be run before the test cases can be run. This
   function will be callled once for each group of test cases; for any actions
   that should be run before each individual test case see the InitTest function
   below.
   
   Return 0 if no errors. */
int <MODULE>SuiteSetUp( void )
{
    return 0;
}



/* Add any actions that must be run after the test cases have been run (e.g. to
   release resources). This function will be callled once for each group of test
   cases; for any actions that should be run after each individual test case see
   the CleanupTest function below.

   Return 0 if no errors. */
int <MODULE>SuiteTearDown( void )
{
    return 0;
}



/* Add any actions that must be run before each test case.

   Return 0 if no errors. */
int <MODULE>InitTest( void )
{
    return 0;
}



/* Add any actions that must be run after each test case.

   Return 0 if no errors. */
int <MODULE>CleanupTest( void )
{
    return 0;
}



void TestCase( void )
{
}



void Initialize<MODULE>Suite( void )
{
    TestGroup* group;

    /* <MODULE> Tester */
    group = add_test_group( "<MODULE>", <MODULE>SuiteSetUp,
                <MODULE>SuiteTearDown, <MODULE>InitTest,
                <MODULE>CleanupTest );
    add_test_case( group, "Init", TestCase );
}



void Terminate<MODULE>Suite( void )
{
}

