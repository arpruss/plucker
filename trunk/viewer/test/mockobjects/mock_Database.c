/*
 * $Id: mock_Database.c,v 1.1 2004/01/11 17:33:32 nordstrom Exp $
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

#include "CUnit.h"
#include "util.h"
#include "resourceids.h"

#include "mock_Database.h"


static const Char fakeMemoDBName[] = "UntT-FakeMemoDB";


static DmOpenRef palmDmOpenDatabaseByTypeCreator( UInt32 type,
                    UInt32 creator, UInt16 mode ) SYS_TRAP(sysTrapDmOpenDatabaseByTypeCreator);

static Err InitFakeMemoDBAppInfo( DmOpenRef docRef ) MOCK_SECTION;



/* Create an app info chunk if missing */
static Err InitFakeMemoDBAppInfo
    (
    DmOpenRef docRef    /* reference to document */
    )
{
    UInt16          cardNo;
    MemHandle       handle;
    LocalID         dbID;
    LocalID         appInfoID;
    AppInfoType*    appInfoP;
    Err             err;

    appInfoID   = 0;
    err         = DmOpenDatabaseInfo( docRef, &dbID, NULL, NULL, &cardNo, NULL );
    if ( err == errNone ) {
        err = DmDatabaseInfo( cardNo, dbID, NULL, NULL, NULL, NULL, NULL,
                NULL, NULL, &appInfoID, NULL, NULL, NULL );
    }
    if ( err == errNone && appInfoID == 0 ) {
        handle = DmNewHandle( docRef, sizeof *appInfoP );
        if ( handle != NULL ) {
            appInfoID = MemHandleToLocalID( handle );
            DmSetDatabaseInfo( cardNo, dbID, NULL, NULL, NULL, NULL,
                NULL, NULL, NULL, &appInfoID, NULL, NULL, NULL );
            appInfoP = MemLocalIDToLockedPtr( appInfoID, cardNo );
            DmSet( appInfoP, 0, sizeof *appInfoP, 0 );
            CategoryInitialize( ( AppInfoType* ) appInfoP, strCatDefault );
            MemPtrUnlock( appInfoP );
        }
    }
    return err;
}




Err CreateFakeMemoDB( void )
{
    Err err;

    err = DmCreateDatabase( 0, fakeMemoDBName, 'UntT', 'Memo', false );
    if ( err == errNone ) {
        DmOpenRef   dbRef;
        LocalID     dbId;

        dbId    = DmFindDatabase( 0, fakeMemoDBName );
        dbRef   = DmOpenDatabase( 0, dbId, dmModeReadWrite );
        if ( dbRef != NULL ) {
            err = InitFakeMemoDBAppInfo( dbRef );
            DmCloseDatabase( dbRef );
        }
    }
    return err;
}



void DeleteFakeMemoDB( void )
{
    LocalID dbId;

    dbId = DmFindDatabase( 0, fakeMemoDBName );
    if ( dbId != 0 ) {
        DmDeleteDatabase( 0, dbId );
    }
}



UInt16 mock_CheckNumOfRecordsInFakeMemoDB( void )
{
    LocalID dbId;
    UInt16  numOfRecords;

    numOfRecords    = 0;
    dbId            = DmFindDatabase( 0, fakeMemoDBName );
    if ( dbId != 0 ) {
        DmOpenRef dbRef;

        dbRef = DmOpenDatabase( 0, dbId, dmModeReadOnly );
        if ( dbRef != NULL ) {
            numOfRecords = DmNumRecords( dbRef );
            DmCloseDatabase( dbRef );
        }
    }
    return numOfRecords;
}



DmOpenRef OpenFakeMemoDB( void )
{
    LocalID dbId;

    dbId = DmFindDatabase( 0, fakeMemoDBName );
    if ( dbId != 0 ) {
        return DmOpenDatabase( 0, dbId, dmModeReadOnly );
    }
    else {
        return NULL;
    }
}



DmOpenRef mock_DmOpenDatabaseByTypeCreator
    (
    UInt32 type,
    UInt32 creator,
    UInt16 mode
    )
{
    if ( creator == 'memo' && type == 'DATA' ) {
        creator = 'UntT';
        type    = 'Memo';
    }
    return palmDmOpenDatabaseByTypeCreator( type, creator, mode );
}

