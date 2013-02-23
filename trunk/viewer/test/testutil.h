/*
 * $Id: testutil.h,v 1.6 2004/01/11 17:33:32 nordstrom Exp $
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

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include "viewer.h"
#include "genericfile.h"


extern Err OpenTestDocument( Char* name, DocumentInfo* docInfo ) UNIT_TEST_SECTION;
extern void CloseTestDocument( Char* name ) UNIT_TEST_SECTION;
extern Boolean CheckRecordData( UInt8* inData, UInt8* knownData,
                UInt16 knownSize ) UNIT_TEST_SECTION;

#endif

