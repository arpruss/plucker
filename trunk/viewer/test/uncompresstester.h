/*
 * $Id: uncompresstester.h,v 1.2 2004/01/01 21:07:44 nordstrom Exp $
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

#ifndef UNCOMPRESS_TESTER_H
#define UNCOMPRESS_TESTER_H

#include "viewer.h"

extern int UncompressSuiteSetUp( void ) UNIT_TEST_SECTION;
extern int UncompressSuiteTearDown( void ) UNIT_TEST_SECTION;
extern int UncompressInitTest( void ) UNIT_TEST_SECTION;
extern int UncompressCleanupTest( void ) UNIT_TEST_SECTION;

extern void TestCreateBuffer( void ) UNIT_TEST_SECTION;
extern void TestDeleteBuffer( void ) UNIT_TEST_SECTION;
extern void TestUnDoc( void ) UNIT_TEST_SECTION;
extern void TestUnZip( void ) UNIT_TEST_SECTION;

extern void InitializeUncompressSuite( void ) UNIT_TEST_SECTION;
extern void TerminateUncompressSuite( void ) UNIT_TEST_SECTION;

#endif

