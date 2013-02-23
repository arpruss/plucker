/*
 * $Id: cpuspeed.c,v 1.2 2005/04/04 22:18:32 prussar Exp $
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
#include "cpuspeed.h"

#ifdef HAVE_PALMCUNIT
#include "mock_Header.h"
#endif


/***********************************************************************
 *
 *      Internal types
 *
 ***********************************************************************/
typedef enum {
    SPEED_NORMAL = 0,
    SPEED_SLOW,
    SPEED_FAST,
    SPEED_DEFAULT
} SpeedType;



/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void InitializeLightspeed( void ) CPUSPEED_SECTION;
static void SetSpeed( SpeedType speed ) CPUSPEED_SECTION;



/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static UInt32  speedupLevels = 0;
static LocalID lightspeedDB;
static Boolean lightspeedInitialized = false;



static void InitializeLightspeed( void )
{
    if ( ! lightspeedInitialized ) {
        UInt32 unused;

	    if ( FtrGet( 'LISP', 1, &unused ) == errNone ) {
            lightspeedDB          = DmFindDatabase( 0, "Lightspeed" );
            lightspeedInitialized = true;
        }
        else {
            lightspeedDB          = 0;
            lightspeedInitialized = true;
        }

    }
}



static void SetSpeed
    (
    SpeedType speed
    )
{
    InitializeLightspeed();

    if ( lightspeedDB != 0 ) {
        UInt32 result;

        SysAppLaunch( 0, lightspeedDB, 0, sysAppLaunchCmdCustomBase + speed,
            NULL, &result );
    }
}



void SpeedFaster( void )
{
    if ( speedupLevels == 0 ) {
        SetSpeed( SPEED_FAST );
    }

    speedupLevels++;
}



void SpeedRestore( void )
{
    if ( speedupLevels != 0 ) {
        speedupLevels--;

        if ( speedupLevels == 0 ) {
            SetSpeed( SPEED_DEFAULT );
        }
    }
}

