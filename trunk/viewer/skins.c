/*
 * $Id: skins.c,v 1.6 2004/04/18 15:34:48 prussar Exp $
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

#include "util.h"
#include "list.h"
#include "const.h"
#include "skins.h"
#include "resourceids.h"
#include "font.h"

static LinkedList resourceDBList     = NULL;

void OpenSkins( void )
{
    DmOpenRef           dbRef;
    DmSearchStateType   stateInfo;
    UInt16              cardNo;
    LocalID             dbID;
    Err                 err;

    resourceDBList = ListCreate();

    if ( resourceDBList == NULL )
        return;

    err = DmGetNextDatabaseByTypeCreator( true, &stateInfo,
            (UInt32) SkinResourceType, (UInt32) ViewerAppID,
            false, &cardNo, &dbID );

    while ( err == errNone ) {
        dbRef = DmOpenDatabase( cardNo, dbID, dmModeReadOnly );
        if ( dbRef != NULL )
            ListAppend( resourceDBList, dbRef );

        err = DmGetNextDatabaseByTypeCreator( false, &stateInfo,
                (UInt32) SkinResourceType, (UInt32) ViewerAppID, false,
                &cardNo, &dbID );
    }
}


void CloseSkins( void )
{
    DmOpenRef dbRef;

    if ( NULL == resourceDBList )
        return;

    dbRef = ListFirst( resourceDBList );
    while ( dbRef != NULL ) {
        DmCloseDatabase( dbRef );
        dbRef = ListNext( resourceDBList, dbRef );
    }

    ListDelete( resourceDBList );
}

