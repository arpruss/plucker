/*
 * $Id: mock_Window.c,v 1.1 2004/01/02 15:38:51 nordstrom Exp $
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

#include "list.h"
#include "util.h"

#include "mock_Window.h"


#define MAX_LINE_LEN 256



static LinkedList lineCache;



void mock_WinDrawChars
    (
    const Char* chars,
    Int16       len,
    Coord       x,
    Coord       y
    )
{
    if ( lineCache != NULL ) {
        Char* line;

        line = SafeMemPtrNew( len + 1 );
        StrNCopy( line, chars, len );
        line[ len ] = '\0';
        ListAppend( lineCache, line );
    }
}



void mock_CreateLineCache( void )
{
    if ( lineCache == NULL ) {
        lineCache = ListCreate();
    }
}



void mock_ReleaseLineCache( void )
{
    ListRelease( lineCache );
}



Char* mock_GetLine
    (
    UInt16 lineNum  /* 1-based index */
    )
{
    Char* line = NULL;

    if ( lineCache != NULL ) {
        line = ListGet( lineCache, lineNum );
    }
    return line;
}

