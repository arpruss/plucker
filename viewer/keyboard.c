/*
 * $Id: keyboard.c,v 1.9 2005/08/08 02:30:59 prussar Exp $
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

#include "keyboard.h"

#include "ramfile.h"
#include "const.h"
#include "prefsdata.h"


/***********************************************************************
 *
 *      Internal Types
 *
 ***********************************************************************/




/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static DmOpenRef dbKeyboard;
static Char keyboardMapName[] = "PlkrKeyboardMap";
static KeyMapType defaultMainformMap[] = {
    { chrHorizontalTabulation, 0, SELECT_COMMAND_STROKE },
    { chrDownArrow,            0, SELECT_NEXT_ANCHOR },
    { chrUpArrow,              0, SELECT_PREV_ANCHOR },
    { chrLeftArrow,            0, SELECT_GO_BACK },
    { chrRightArrow,           0, SELECT_GO_FORWARD },
    { chrLineFeed,             0, SELECT_GO_TO_LINK }
};



/* Load in the keyboard control database */
void OpenKeyboardMap( void )
{
    LocalID keyboardID;
    UInt16  cardNo;

    dbKeyboard = NULL;
    cardNo     = 0;
    keyboardID = DmFindDatabase( cardNo, keyboardMapName );
    if ( keyboardID == 0 ) {
        Err         err;
        UInt16      index;
        MemHandle   handle;
        KeyMapType* map;

        err    = DmCreateDatabase( cardNo, keyboardMapName, ViewerAppID,
                     PlkrKeyboardMapType, false );
        if ( err != errNone )
            return;
        keyboardID = DmFindDatabase( cardNo, keyboardMapName );
        if ( keyboardID == 0 )
            return;
        dbKeyboard = DmOpenDatabase( cardNo, keyboardID, dmModeReadWrite );
        if ( dbKeyboard == NULL )
            return;

        index  = 0;
        handle = NewRecord( dbKeyboard, &index, sizeof( defaultMainformMap ) );
        if ( handle == NULL ) {
            CloseDatabase( dbKeyboard );
            DmDeleteDatabase( cardNo, keyboardID );
            dbKeyboard = NULL;
            return;
        }
        map = MemHandleLock( handle );

        DmWrite( map, 0, defaultMainformMap, sizeof( defaultMainformMap ) );

        MemHandleUnlock( handle );
        CloseRecord( handle, true );
    }
    else {
        dbKeyboard = DmOpenDatabase( cardNo, keyboardID, dmModeReadWrite );
    }
}



/* Close the keyboard control database */
void CloseKeyboardMap( void )
{
    if ( dbKeyboard != NULL ) {
        CloseDatabase( dbKeyboard );
        dbKeyboard = NULL;
    }
}



/* look up the key and get the action */
KeyActionType GetKeyboardAction
    (
    WChar  key,         /* key to look up */
    UInt16 modifier     /* modifier: currently ignored */
    )
{
    MemHandle     handle;
    UInt16        length;
    UInt16        i;
    KeyMapType*   keyboardMap;
    KeyActionType action;

    if ( dbKeyboard == NULL )
        return -1;

    handle = OpenRecord( dbKeyboard, 0 );

    if ( handle == NULL )
        return KEYBOARD_ACTION_NONE;

    /* currently modifiers are ignored */
    modifier    = 0;

    length      = MemHandleSize( handle ) / sizeof( KeyMapType );
    keyboardMap = MemHandleLock( handle );

    action = KEYBOARD_ACTION_NONE;

    for ( i = 0 ; i < length ; i ++ ) {
        if ( keyboardMap[ i ].key == key &&
             keyboardMap[ i ].modifier == modifier ) {
            action = keyboardMap[ i ].action;
            break;
        }
    }
    MemHandleUnlock( handle );
    CloseRecord( handle, false );

    return action;
}



/* Get length of keyboard data */
UInt16 GetKeyboardMapLength( void )
{
    MemHandle   handle;
    UInt16      length;

    if ( dbKeyboard == NULL )
        return 0;

    handle = OpenRecord( dbKeyboard, 0 );

    if ( handle == NULL )
        return 0;

    length = MemHandleSize( handle ) / sizeof( KeyMapType );

    CloseRecord( handle, false );

    return length;
}



/* Get all keyboard map data */
void GetKeyboardMap
    (
    KeyMapType*   dest    /* where to put keyboard map data */
    )
{
    MemHandle   handle;
    KeyMapType* keyboardMap;
    UInt16      length;

    if ( dbKeyboard == NULL )
        return;

    handle = OpenRecord( dbKeyboard, 0 );

    if ( handle == NULL )
        return;

    length = GetKeyboardMapLength();

    if ( length == 0 )
        return;

    keyboardMap = MemHandleLock( handle );

    MemMove( dest, keyboardMap, length * sizeof( KeyMapType ) );

    MemHandleUnlock( handle );

    CloseRecord( handle, false );
}





/* Set all keyboard map data */
void SetKeyboardMap
    (
    KeyMapType*   src,    /* where to get keyboard map data from */
    UInt16        length/* size of table */
    )
{
    MemHandle   handle;
    KeyMapType* keyboardMap;

    if ( dbKeyboard == NULL )
        return;

    handle = ResizeRecord( dbKeyboard, 0, length * sizeof( KeyMapType ) );

    if ( handle == NULL || length == 0 )
        return;

    keyboardMap = MemHandleLock( handle );

    DmWrite( keyboardMap, 0, src, length * sizeof( KeyMapType ) );

    MemHandleUnlock( handle );
    CloseRecord( handle, true );
}





/* Get default keyboard map length */
UInt16 GetDefaultKeyboardMapLength( void )
{
    return sizeof( defaultMainformMap ) / sizeof( KeyMapType );
}



/* Get default keyboard map */
void GetDefaultKeyboardMap
    (
    KeyMapType*   dest   /* where to put keyboard map data */
    )
{
    MemMove( dest, defaultMainformMap, sizeof( defaultMainformMap ) );
}

