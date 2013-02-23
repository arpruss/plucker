/*
 * $Id: rotatebitmap.h,v 1.4 2003/08/01 11:49:44 prussar Exp $
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

#ifndef PLUCKER_ARMLETS_ROTATEBITMAP_H
#define PLUCKER_ARMLETS_ROTATEBITMAP_H

typedef struct  {
    UInt8* dest;
    UInt8* src;
    Int16  width;
    Int16  height;
    UInt16 destRowBytes;
    UInt16 srcRowBytes;
    Boolean clockwise;
    UInt8  bitDepth;
} ArmRotateType;

#endif
