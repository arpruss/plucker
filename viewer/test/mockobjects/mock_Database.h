/*
 * $Id: mock_Database.h,v 1.1 2004/01/11 17:33:32 nordstrom Exp $
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

#ifndef MOCK_DATABASE_H
#define MOCK_DATABASE_H

#include "viewer.h"

#define DmOpenDatabaseByTypeCreator mock_DmOpenDatabaseByTypeCreator

extern DmOpenRef mock_DmOpenDatabaseByTypeCreator( UInt32 type,
                    UInt32 creator, UInt16 mode ) MOCK_SECTION;

extern Err CreateFakeMemoDB( void ) MOCK_SECTION;
extern void DeleteFakeMemoDB( void ) MOCK_SECTION;
extern UInt16 mock_CheckNumOfRecordsInFakeMemoDB( void ) MOCK_SECTION;
extern DmOpenRef OpenFakeMemoDB( void ) MOCK_SECTION;

#endif

