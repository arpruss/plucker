/*
 * $Id: metadocumenttester.h,v 1.3 2004/01/03 13:32:02 nordstrom Exp $
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

#ifndef META_DOCUMENT_TESTER_H
#define META_DOCUMENT_TESTER_H

#include "viewer.h"

extern int MetaDocumentSuiteSetUp( void ) UNIT_TEST_SECTION;
extern int MetaDocumentSuiteTearDown( void ) UNIT_TEST_SECTION;
extern int MetaDocumentInitTest( void ) UNIT_TEST_SECTION;
extern int MetaDocumentCleanupTest( void ) UNIT_TEST_SECTION;

extern void TestOpenMetaDocument( void ) UNIT_TEST_SECTION;
extern void TestRemoveMetaDocument( void ) UNIT_TEST_SECTION;

extern void InitializeMetaDocumentSuite( void ) UNIT_TEST_SECTION;
extern void TerminateMetaDocumentSuite( void ) UNIT_TEST_SECTION;

#endif

