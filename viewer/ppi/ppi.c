/*
 * $Id: ppi.c,v 1.4 2004/12/09 20:27:41 prussar Exp $
 *
 * PPI - a part of Plucker, the free off-line HTML viewer for PalmOS
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


#include "ppi.h"


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
static Preferences prefs;


/***********************************************************************
 *
 *      Local Functions
 *
 ***********************************************************************/
static Err StartApplication( MemPtr cmdPBP );
static void HandleFormLoad( EventType* event );
static void EventLoop( void );
static void StopApplication( void );
static Err LookupWord( MemPtr cmdPBP );
static void ReadPrefs( Preferences* prefs );
static Err LaunchDa( LocalID db_id, Char* word, Boolean toClipboard );



static void ReadPrefs( Preferences* prefs )
{
    Int16         version;
    UInt16        prefSize;

    MemSet( prefs, sizeof( Preferences ), 0 );

    prefSize = 0;
    version = PrefGetAppPreferences( (UInt32) PpiAppID, (UInt16) PREFS_ID,
                                     NULL, &prefSize, true);

    if ( version == PREFS_VERSION && prefSize == sizeof( Preferences ) )
        version = PrefGetAppPreferences( (UInt32) PpiAppID,
                                         (UInt16) PREFS_ID, prefs,
                                         &prefSize, true);
}



void WritePrefs( Preferences* prefs )
{
    PrefSetAppPreferences( (UInt32) PpiAppID, (UInt16) PREFS_ID,
                           (Int16) PREFS_VERSION, prefs,
                           sizeof( Preferences ), true );
}



Preferences* Prefs ( void )
{
    return &prefs;
}



/* Simply goto the main form */
static Err StartApplication
    (
    MemPtr cmdPBP
    )
{
    ReadPrefs ( &prefs );
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



/* This is the main entry point for the PPI application */
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
    else if ( cmd == sysAppLaunchCmdCustomBase ) {
        err = LookupWord( cmdPBP );
    }

    return err;
}

/* Started with sysAppLaunchCmdCustomBase, pass word to Dictionary */
/* System Globals and Static Variables are not available */
static Err LookupWord( MemPtr cmdPBP )
{
    LocalID      dbID = 0;
    Err          err = dmErrCantFind;
    UInt32       outputValue;
    Preferences  prefs;
    LookupList   lookupList[] =  {
                                   {"None", 0, 0, false },
                                   {"Custom", TYPE_CUSTOM, 0, false },
                                   {"RoadLingua", TYPE_APP,
                                     sysAppLaunchCmdCustomBase, false },
                                   {"KDIC DA", TYPE_DA, 0, true },
                                   {"BDicty", TYPE_APP,
                                     sysAppLaunchCmdCustomBase + 212, true },
                                   {"BDicty", TYPE_APP,
                                     sysAppLaunchCmdCustomBase + 212, false },
                                   {"SlovoEd", TYPE_APP,
                                     sysAppLaunchCmdCustomBase + 0, false },
                                 };



    ReadPrefs ( &prefs );
    if ( prefs.lookupAction == ( LookupWordActionType ) 0 )
        return errNone;

    if ( prefs.lookupAction != SELECT_WORD_CUSTOM ) {
        dbID = DmFindDatabase( 0, lookupList[ prefs.lookupAction ].name );
        if ( dbID == 0 )
            return err;
    }

    switch ( lookupList[ prefs.lookupAction ].type ) {
        case TYPE_APP:
            err = SysAppLaunch( 0, dbID, 0,
                                lookupList[ prefs.lookupAction ].launchCmd,
                                cmdPBP, &outputValue );
            break;

        case TYPE_DA:
            err = LaunchDa( dbID, ( Char* ) cmdPBP,
                            lookupList[ prefs.lookupAction ].toClipboard );
            break;

        case TYPE_CUSTOM:
            if ( 0 < StrLen( prefs.name ) ) {
                Char* text = ( Char* ) cmdPBP;

                dbID = DmFindDatabase( 0, prefs.name );
                if ( dbID == 0 )
                    return err;
                if ( prefs.isDA )
                    return LaunchDa( dbID, text, prefs.toClipboard );
                if ( prefs.toClipboard )
                    ClipboardAddItem( clipboardText, text, StrLen( text ) );
                err = SysAppLaunch( 0, dbID, 0, sysAppLaunchCmdCustomBase
                                    + prefs.launchCmd, cmdPBP, &outputValue );
            }
            break;

    }
    return err;
}

/* System Globals and Static Variables are not available */
/* Save UI info */
static Err LaunchDa( LocalID dbID, Char* word, Boolean toClipboard )
{
    DmOpenRef  db;
    MemHandle  handle;
    void       ( *da_entry )();
    Err        err = dmErrCantFind;
    FormPtr    saved_form;
    UInt16     saved_form_id;
    WinHandle  saved_active_win;
    WinHandle  saved_draw_win;
    FontID     saved_font;

    db = DmOpenDatabase( 0, dbID, dmModeReadOnly );
    if ( db != NULL ) {
        handle = DmGet1Resource( DA_RSRC_TYPE, DA_RSRC_ID );
        if ( handle != NULL ) {
            da_entry = MemHandleLock( handle );
            if ( da_entry != NULL ) {
                if ( toClipboard )
                    ClipboardAddItem( clipboardText, word, StrLen( word ) );
                saved_form = FrmGetActiveForm();
                saved_form_id = FrmGetActiveFormID();
                saved_active_win = WinGetActiveWindow();
                saved_draw_win = WinGetDrawWindow();
                saved_font = FntSetFont(stdFont);
                ( *da_entry )();
                MemHandleUnlock( handle );
                if (saved_form_id != 0)
                    FrmSetActiveForm(saved_form);
                WinSetActiveWindow(saved_active_win);
                WinSetDrawWindow(saved_draw_win);
                FntSetFont(saved_font);
                err = errNone;
            }
            DmReleaseResource( handle );
        }
        DmCloseDatabase( db );
    }
    return err;
}


