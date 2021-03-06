/*
 * $Id: mock_Window.h,v 1.2 2004/01/11 17:33:32 nordstrom Exp $
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

#ifndef MOCK_WINDOW_H
#define MOCK_WINDOW_H

#include "viewer.h"

/* We don't care whether the text is inverted or not */
#undef WinDrawInvertedChars
#undef WinDrawChars
#define WinDrawInvertedChars    mock_WinDrawChars
#define WinDrawChars            mock_WinDrawChars

extern void mock_WinDrawChars( const Char* chars, Int16 len, Coord x,
                Coord y ) MOCK_SECTION;
extern void mock_CreateLineCache( void ) MOCK_SECTION;
extern void mock_ReleaseLineCache( void ) MOCK_SECTION;
extern Char* mock_GetLine( UInt16 lineNum ) MOCK_SECTION;

#endif

