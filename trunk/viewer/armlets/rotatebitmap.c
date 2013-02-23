/*
 * $Id: rotatebitmap.c,v 1.4 2003/08/01 11:49:44 prussar Exp $
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
#include "rotatebitmap.h"
#define ARM_RESOURCE
#include "../rotatebitmap.h"

/* This is the bitmap rotation armlet */
/* Only works for 1-bit deep bitmaps */


/* Required definition to build our armlet as a Stand-alone code resource */
STANDALONE_CODE_RESOURCE_ID( armRotateBitmap );



/* 'Main()' entrypoint for the searching armlet */
UInt32 NativeFunction
    (
    const void*      emulStateP,
    void*            userData68KP,
    Call68KFuncType* call68KFuncP
    )
{
    ArmRotateType*  passedData;

    passedData = ( ArmRotateType* ) userData68KP;

    RotateBitmap(
            ( Int16 ) SWAP16( passedData->width ),
            ( Int16 ) SWAP16( passedData->height ),
            ( UInt8* ) READUNALIGNED32( &(passedData->dest) ),
            ( UInt16 ) SWAP16( passedData->destRowBytes ),
            ( UInt8* ) READUNALIGNED32( &(passedData->src) ),
            ( UInt16 ) SWAP16( passedData->srcRowBytes ),
            passedData->clockwise,
            passedData->bitDepth );

    return 0;
}


#include "../rotatebitmap.c"
