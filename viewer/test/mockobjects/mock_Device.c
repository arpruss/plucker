/*
 * $Id: mock_Device.c,v 1.4 2004/01/04 12:03:18 nordstrom Exp $
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

#include "mock_Feature.h"

#include "mock_Device.h"


static const UInt32 RomVersion20 = sysMakeROMVersion( 2, 0, 0,
                                    sysROMStageRelease, 0 );
static const UInt32 RomVersion30 = sysMakeROMVersion( 3, 0, 0,
                                    sysROMStageRelease, 0 );
static const UInt32 RomVersion31 = sysMakeROMVersion( 3, 1, 0,
                                    sysROMStageRelease, 0 );
static const UInt32 RomVersion33 = sysMakeROMVersion( 3, 3, 0,
                                    sysROMStageRelease, 0 );
static const UInt32 RomVersion35 = sysMakeROMVersion( 3, 5, 0,
                                    sysROMStageRelease, 0 );
static const UInt32 RomVersion40 = sysMakeROMVersion( 4, 0, 0,
                                    sysROMStageRelease, 0 );
static const UInt32 RomVersion50 = sysMakeROMVersion( 5, 0, 0,
                                    sysROMStageDevelopment, 0 );


static DeviceType device;


DeviceType* Device( void )
{
    return &device;
}



void InitOS2Device( void )
{
    device.fake             = true;
    device.romVersion       = RomVersion20;
    device.notifySupport    = 0;
    device.charEncoding     = charEncodingPalmLatin;
    device.vfsSupport       = 0;
}



void InitOS30Device( void )
{
    device.fake             = true;
    device.romVersion       = RomVersion30;
    device.notifySupport    = 0;
    device.charEncoding     = charEncodingPalmLatin;
    device.vfsSupport       = mock_GetRealVFSVersion();
}



void InitOS31Device( void )
{
    device.fake             = true;
    device.romVersion       = RomVersion31;
    device.notifySupport    = 0;
    device.charEncoding     = charEncodingPalmLatin;
    device.vfsSupport       = mock_GetRealVFSVersion();
}



void InitOS33Device( void )
{
    device.fake             = true;
    device.romVersion       = RomVersion33;
    device.notifySupport    = 0;
    device.charEncoding     = charEncodingPalmLatin;
    device.vfsSupport       = mock_GetRealVFSVersion();
}



void InitOS35Device( void )
{
    device.fake             = true;
    device.romVersion       = RomVersion35;
    device.notifySupport    = 1;
    device.charEncoding     = charEncodingPalmLatin;
    device.vfsSupport       = mock_GetRealVFSVersion();
}



void InitOS4Device( void )
{
    device.fake             = true;
    device.romVersion       = RomVersion40;
    device.notifySupport    = 1;
    device.charEncoding     = charEncodingPalmLatin;
    device.vfsSupport       = mock_GetRealVFSVersion();
}



void InitOS5Device( void )
{
    device.fake             = true;
    device.romVersion       = RomVersion50;
    device.notifySupport    = 1;
    device.charEncoding     = charEncodingPalmLatin;
    device.vfsSupport       = mock_GetRealVFSVersion();
}



void ReleaseDevice( void )
{
    device.fake             = false;
}

