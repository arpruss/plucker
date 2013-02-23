/*
 * $Id: cleaner.c,v 1.1 2003/01/07 23:17:21 adamm Exp $
 *
 * Cleaner - a part of Plucker, the free off-line HTML viewer for PalmOS
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
 *
 * Originally based on code by Andrew Howlett from his Tex2Hex demo. (Thanks!)
 */


#include "cleaner.h"


/***********************************************************************
 *
 *      Local Functions
 *
 ***********************************************************************/
static Err StartApplication( MemPtr cmdPBP );
static void HandleFormLoad( EventType* event );
static void EventLoop( void );
static void StopApplication( void );



/* Simply goto the main form */
static Err StartApplication
    (
    MemPtr cmdPBP
    )
{
    FrmGotoForm( frmMain );

    return errNone;
}



/* This routine handles the loading of forms */
static void HandleFormLoad
    (
    EventType* event
    )
{
    FormType*  form;
    UInt16     formID;

    formID  = event->data.frmLoad.formID;
    form    = FrmInitForm( formID );
    FrmSetActiveForm( form );

    switch( formID ) {
        case frmMain:
            FrmSetEventHandler( form, FormHandleEvent );
            break;

        default:
            ErrFatalDisplay( "Unknown form ID" );
            break;
    }
}



/* This routine is the event loop */
static void EventLoop( void )
{
    EventType event;
    UInt16    err;

    do {
        EvtGetEvent( &event, evtWaitForever );

        if ( SysHandleEvent( &event ) )
            continue;

        if ( MenuHandleEvent( NULL, &event, &err ) )
            continue;

        if ( event.eType == frmLoadEvent )
            HandleFormLoad( &event );

        FrmDispatchEvent( &event );
    } while ( event.eType != appStopEvent );
}



/* Close and save any open forms */
static void StopApplication( void )
{
    FrmSaveAllForms();
    FrmCloseAllForms();
}



/* This is the main entry point for the cleaner application */
UInt32 PilotMain
    (
    UInt16 cmd,         /* SysAppLaunch Command */
    MemPtr cmdPBP,      /* pointer to command parameter block */
    UInt16 launchFlags  /* launch flag ( see SystemMgr.h for details ) */
    )
{
    Err err;

    err = errNone;
    if ( cmd == sysAppLaunchCmdNormalLaunch ) {
        err = StartApplication( NULL );
        if ( err != errNone )
            return err;

        EventLoop();
        StopApplication();
    }

    return err;
}

