/*
 * $Id: debug.c,v 1.30 2003/06/02 23:18:51 adamm Exp $
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

#include <stdarg.h>

#include "hires.h"
#include "resourceids.h"
#include "util.h"
#include "debug.h"


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define MAX_MSG_LEN     256
#define COUNT_TICKS     true



/* Create formatted debug message */
Char* _
    (
    Char* fmt,  /* format, see printf( 3 ) for details */
    ...         /* arguments */
    )
{
    static Char   msg[ MAX_MSG_LEN ];
    va_list       args;

    va_start( args, fmt );
    StrVPrintF( msg, fmt, args );
    va_end( args );

    return msg;
}



/* write debug message to log file */
void POSEDebugMsg
    (
    Char* fname,    /* file name */
    Int16 line,     /* line number */
    Char* msg       /* text message */
    )
{
    static Boolean  printFileLine = true;
    static Char     logBuf[ MAX_MSG_LEN ];
    static Char     logMsg[ MAX_MSG_LEN ];

    HostFILE*       fd;
    Char*           s;
    Char*           logfile;

    logfile = HostGetEnv( "PLUCKERLOG" );
    if ( logfile != NULL ) {
        fd = HostFOpen(logfile, "a" );
    }
    else {
        fd = HostLogFile();
    }

    if ( fd == NULL ) {
        return;
    }

    if ( printFileLine ) {
        StrPrintF( logBuf, "%s:%d:", fname, line );
        StrPrintF( logMsg, "%-25s", logBuf );
        HostFPutS( logMsg, fd );
        HostFFlush( fd );
        printFileLine = false;
    }

    if ( COUNT_TICKS ) {
        StrPrintF( logMsg, "[ ticks: %ld ] ", TimGetTicks() );
        HostFPutS( logMsg, fd );
        HostFFlush( fd );
    }
    StrPrintF( logMsg, "%s", msg );
    HostFPutS( logMsg, fd );
    HostFFlush( fd );
    HostFClose( fd );

    s = msg;
    if ( *s != '\0' ) {
        while ( *s != '\0' ) {
            ++s;
        }
        printFileLine = ( s[ -1 ] == '\n' );
    }
}



/* write debug message to FrmCustomAlert */
void FormDebugMsg
    (
    Char* fname,    /* file name */
    Int16 line,     /* line number */
    Char* msg       /* text message */
    )
{
    Char   location[ MAX_MSG_LEN ];
    UInt16 prevCoordSys;

    StrPrintF( location, "%s: %d", fname, line );
    prevCoordSys = PalmSetCoordinateSystem( STANDARD );
    FrmCustomAlert( infoDebug, location, msg, NULL );
    PalmSetCoordinateSystem( prevCoordSys );
}



/* write debug message to memo */
void MemoDebugMsg
    (
    Char* msg
    )
{
    static UInt16 index     = dmMaxRecordIndex;
    static UInt32 offset    = 0;

    WriteMemoEntry( &index, &offset, "%s", msg );
}



/* write debug message to log file */
void DebugMsg
    (
    Char* fname,    /* file name */
    Int16 line,     /* line number */
    Char* msg       /* text message */
    )
{
    HostIDType hostID;

    hostID = HostGetHostID();
    if ( hostID == hostIDPalmOSEmulator )
        POSEDebugMsg( fname, line, msg );
    else if ( hostID == hostIDPalmOS )
        MemoDebugMsg( msg );
}

