/*
 * $Id: prefsdatatester.h,v 1.4 2004/07/25 16:41:28 nordstrom Exp $
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

#ifndef PREFS_DATA_TESTER_H
#define PREFS_DATA_TESTER_H

#include "viewer.h"

extern int PrefsDataSuiteSetUp( void ) UNIT_TEST_SECTION;
extern int PrefsDataSuiteTearDown( void ) UNIT_TEST_SECTION;
extern int PrefsDataInitTest( void ) UNIT_TEST_SECTION;
extern int PrefsDataCleanupTest( void ) UNIT_TEST_SECTION;

extern void TestPrefsDataInit( void ) UNIT_TEST_SECTION;
extern void TestPrefsDataRead( void ) UNIT_TEST_SECTION;
extern void TestPrefsDataReadOld( void ) UNIT_TEST_SECTION;
extern void TestPrefsDataWrite( void ) UNIT_TEST_SECTION;

extern void InitializePrefsDataSuite( void ) UNIT_TEST_SECTION;
extern void TerminatePrefsDataSuite( void ) UNIT_TEST_SECTION;

#endif

