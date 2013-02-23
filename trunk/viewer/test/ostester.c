/*
 * $Id: ostester.c,v 1.5 2004/01/04 12:12:28 nordstrom Exp $
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

#include "TestDB.h"
#include "mock_Device.h"
#include "os.h"

#include "ostester.h"


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



int OSSuiteSetUp( void )
{
    return 0;
}



int OSSuiteTearDown( void )
{
    /* Make sure we don't leave the device in a "fake" state */
    ReleaseDevice();

    return 0;
}



int OSInitTest( void )
{
    return 0;
}



int OSCleanupTest( void )
{
    return 0;
}



void TestOSInit( void )
{
    UInt32 romVersion;

    FtrGet( sysFtrCreator, sysFtrNumROMVersion, &romVersion );

    /* We can only "fake" the device to be less than it really is */
    if ( RomVersion33 <= romVersion ) {
        InitOS33Device();

        OS_Init();
        ASSERT( RomVersionCompatible( RomVersion33 ) == errNone );
        ASSERT( ! SupportNotification());
        ASSERT( GetCharEncoding() == charEncodingPalmLatin );
        ASSERT( Support20() );
        ASSERT( Support30() );
        ASSERT( Support31() );
        ASSERT( Support33() );
        ASSERT( ! Support35() );
        ASSERT( ! Support40() );
        ASSERT( ! Support50() );
        OS_Release();

        ReleaseDevice();
    }
    if ( RomVersion40 <= romVersion ) {
        InitOS4Device();

        OS_Init();
        ASSERT( RomVersionCompatible( RomVersion40 ) == errNone );
        ASSERT( SupportNotification());
        ASSERT( GetCharEncoding() == charEncodingPalmLatin );
        ASSERT( Support20() );
        ASSERT( Support30() );
        ASSERT( Support31() );
        ASSERT( Support33() );
        ASSERT( Support35() );
        ASSERT( Support40() );
        ASSERT( ! Support50() );
        OS_Release();

        ReleaseDevice();
    }
}



void InitializeOSSuite( void )
{
    TestGroup* group;

    /* OS Tester */
    group = add_test_group( "OS", OSSuiteSetUp, OSSuiteTearDown,
                OSInitTest, OSCleanupTest );
    add_test_case( group, "Init", TestOSInit );
}



void TerminateOSSuite( void )
{
}

