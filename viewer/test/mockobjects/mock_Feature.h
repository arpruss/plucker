/*
 * $Id: mock_Feature.h,v 1.4 2004/02/19 17:00:13 chrish Exp $
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

#ifndef MOCK_FEATURE_H
#define MOCK_FEATURE_H

#include "viewer.h"
#include <VFSMgr.h>

#define FtrGet mock_FtrGet

extern UInt32 mock_GetRealVFSVersion( void ) MOCK_SECTION;

extern Err mock_FtrGet( UInt32 creator, UInt16 featureNum,
            UInt32* value ) MOCK_SECTION;

#endif

