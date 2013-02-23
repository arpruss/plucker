/*
 * $Id: genericfiletester.h,v 1.2 2004/01/01 14:43:21 nordstrom Exp $
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

#ifndef GENERIC_FILE_TESTER_H
#define GENERIC_FILE_TESTER_H

#include "viewer.h"

extern int GenericFileSuiteSetUp( void ) UNIT_TEST_SECTION;
extern int GenericFileSuiteTearDown( void ) UNIT_TEST_SECTION;
extern int GenericFileInitTest( void ) UNIT_TEST_SECTION;
extern int GenericFileCleanupTest( void ) UNIT_TEST_SECTION;

extern void TestOpenDocument( void ) UNIT_TEST_SECTION;

extern void InitializeGenericFileSuite( void ) UNIT_TEST_SECTION;
extern void TerminateGenericFileSuite( void ) UNIT_TEST_SECTION;

#endif

