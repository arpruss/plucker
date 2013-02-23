/*
 * $Id: search.c,v 1.6 2003/08/17 21:33:54 prussar Exp $
 *
 * Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 1998-2002, Mark Ian Lillywhite and Michael Nordstrom
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

#include "armlets.h"
#define ARM_RESOURCE
#include "search.h"
#include "../search8.h"

/* This is the search armlet */
/* It only works for 8-bits wide encodings.  Caller must check the encoding. */


/* Required definition to build our armlet as a Stand-alone code resource */
STANDALONE_CODE_RESOURCE_ID( armDoSearch );



/* 'Main()' entrypoint for the searching armlet */
UInt32 NativeFunction
    (
    const void*      emulStateP,
    void*            userData68KP,
    Call68KFuncType* call68KFuncP
    )
{
    ArmSearchType*  passedData;
    UInt16          i;
    Int16           j;
    Int16           top;
    Char            ch;
    Boolean         haveDepth;

    passedData = (ArmSearchType*) userData68KP;

    i          = SWAP16( passedData->i );
    j          = SWAP16( passedData->j );
    top        = SWAP16( passedData->top );
    if( passedData->depth )
        haveDepth = true;
    else
        haveDepth = false;

    ch = DoSearch8BitText(
                 (Char*) READUNALIGNED32( &( passedData->text ) ),
                 SWAP16( passedData->size ),
                 (WChar*) READUNALIGNED32( &( passedData->wpattern ) ),
                 SWAP16( passedData->wplen ),
                 (Char*) READUNALIGNED32( &( passedData->xlat ) ),
                 &i,
                 &j,
                 &top,
                 haveDepth
            );

    passedData->i   = SWAP16( i );
    passedData->j   = SWAP16( j );
    passedData->top = SWAP16( top );

    return (unsigned long)(unsigned char)ch;
}


#include "../search8.c"
