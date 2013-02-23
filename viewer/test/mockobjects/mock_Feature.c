/*
 * $Id: mock_Feature.c,v 1.4 2004/01/04 12:03:18 nordstrom Exp $
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
#include "mock_Device.h"

#include "mock_Feature.h"


Err palmFtrGet( UInt32 creator, UInt16 featureNum,
        UInt32 *valuePtr ) SYS_TRAP(sysTrapFtrGet);


UInt32 mock_GetRealVFSVersion( void )
{
    Err     err;
    UInt32  vfsMgrVersion;

    err = palmFtrGet( sysFileCVFSMgr, vfsFtrIDVersion, &vfsMgrVersion );
    if ( err == errNone ) {
        return vfsMgrVersion;
    }
    else {
        return 0;
    }
}



Err mock_FtrGet
    (
    UInt32  creator,
    UInt16  featureNum,
    UInt32* value
    )
{
    Err status = ftrErrNoSuchFeature;

    ASSERT_GOTOERR( value != NULL );

    if ( Device()->fake ) {
        if ( creator == sysFtrCreator ) {
            switch ( featureNum ) {
                case sysFtrNumROMVersion:
                    *value = Device()->romVersion;
                    status = errNone;
                    break;

                case sysFtrNumNotifyMgrVersion:
                    *value = Device()->notifySupport;
                    status = errNone;
                    break;

                case sysFtrNumEncoding:
                    *value = Device()->charEncoding;
                    status = errNone;
                    break;

                default:
                    status = ftrErrNoSuchFeature;
                    break;
            }
        }
        else if ( creator == sysFileCVFSMgr ) {
            switch( featureNum ) {
                case vfsFtrIDVersion:
                    *value = Device()->vfsSupport;
                    if ( *value != 0 )
                        status = errNone;
                    break;

                default:
                    status = ftrErrNoSuchFeature;
                    break;
            }
        }
    }
    else {
        status = palmFtrGet( creator, featureNum, value );
    }

ERR:
    return status;
}

