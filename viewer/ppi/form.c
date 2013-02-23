/*
 * $Id: form.c,v 1.3 2004/01/27 15:47:25 chrish Exp $
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
#include "form.h"
#include "resourceids.h"

/***********************************************************************
 *
 *      Private Variables
 *
 ***********************************************************************/
static FormType*             form;
static FieldType*            nameField;
static FieldType*            cmdField;



/***********************************************************************
 *
 *      Local Functions
 *
 ***********************************************************************/
static void FormInit( void );
static void HideShow( Boolean show );
static void* GetObjectPtr( const Int16 objectID );
static void ShowObject( const Int16 objectID );
static void HideObject( const Int16 objectID );




/* Initialize the main form */
static void FormInit( void )
{
    ListType*     list;
    ControlType*  ctl;
    Char          cmd[6];

    form   = FrmGetActiveForm();

    list      = GetObjectPtr( frmMainLookupList );
    ctl       = GetObjectPtr( frmMainLookupTrigger );
    nameField = GetObjectPtr( frmMainCustomName );
    cmdField  = GetObjectPtr( frmMainCustomCmd );
    LstSetSelection( list, Prefs()->lookupAction );
    CtlSetLabel( ctl, LstGetSelectionText( list, Prefs()->lookupAction ) );

    if ( FldGetTextPtr( nameField ) == NULL ) {
        FldInsert( nameField, Prefs()->name, StrLen( Prefs()->name ) );
    }

    StrIToA( cmd, Prefs()->launchCmd );

    if ( FldGetTextPtr( cmdField ) == NULL ) {
        FldInsert( cmdField, cmd, StrLen( cmd ) );
    }

    CtlSetValue( GetObjectPtr( frmMainCustomClipboard ), Prefs()->toClipboard );
    CtlSetValue( GetObjectPtr( frmMainCustomDA ), Prefs()->isDA );

    HideShow( Prefs()->lookupAction == SELECT_WORD_CUSTOM );

    FrmDrawForm( form );
}



/* Event handler for the main form */
Boolean FormHandleEvent
    (
    EventType* event
    )
{
    Boolean    handled;

    handled = false;

    switch ( event->eType ) {
        case ctlSelectEvent:
            switch ( event->data.ctlEnter.controlID ) {
                case frmMainSave:
                    if ( FldGetTextPtr( nameField ) != NULL )
                        StrCopy( Prefs()->name, FldGetTextPtr( nameField ) );
                    else
                        StrCopy( Prefs()->name, "" );

                    if ( FldGetTextPtr( cmdField ) != NULL )
                        Prefs()->launchCmd =
                                 StrAToI( FldGetTextPtr( cmdField ) );
                    else 
                        Prefs()->launchCmd = 0;

                    WritePrefs( Prefs() );
                    /* Fall through */

                case frmMainCancel:
                {
                    EventType stopEvent;

                    stopEvent.eType = appStopEvent;
                    EvtAddEventToQueue( &stopEvent );
                    handled = true;
                    break;
                }

                case frmMainTips:
                    FrmHelp( strTips );
                    handled = true;
                    break;

                default:
                    Prefs()->toClipboard = CtlGetValue(
                        GetObjectPtr( frmMainCustomClipboard ) );
                    Prefs()->isDA = CtlGetValue(
                        GetObjectPtr( frmMainCustomDA ) );
                    break;
            }
            break;

        case menuEvent:
            switch ( event->data.menu.itemID ) {
                case menuHelpTips:
                    FrmHelp( strTips );
                    handled = true;
                    break;

                case menuHelpAbout:
                {
                    FormType* form;

                    form = FrmInitForm( frmAbout );
                    FrmDoDialog( form );
                    FrmDeleteForm( form );
                    handled = true;
                    break;
                }
            }
            break;

        case popSelectEvent:
            switch ( event->data.popSelect.controlID ) {
                case frmMainLookupTrigger:
                {
                    Int16 selection;

                    selection = event->data.popSelect.selection;

                    if ( selection != noListSelection )
                        Prefs()->lookupAction = selection;
                    HideShow( Prefs()->lookupAction == SELECT_WORD_CUSTOM );
                    handled = true;
                    break;
                }
            break;
            }

        case frmOpenEvent:
            FormInit();
            handled = true;
            break;

        case frmCloseEvent:
            handled = false;
            break;

        default:
            handled = false;
            break;
    }

    return handled;
}



static void HideShow( Boolean show )
{
    if ( show ) {
        ShowObject( frmMainCustomClipboard );
        ShowObject( frmMainCustomDA );
        ShowObject( frmMainCustomNameLabel );
        ShowObject( frmMainCustomName );
        ShowObject( frmMainCustomCmdLabel );
        ShowObject( frmMainCustomCmd );
    }
    else {
        HideObject( frmMainCustomClipboard );
        HideObject( frmMainCustomDA );
        HideObject( frmMainCustomNameLabel );
        HideObject( frmMainCustomName );
        HideObject( frmMainCustomCmdLabel );
        HideObject( frmMainCustomCmd );
    }
}



/* Return a pointer to an object in a form */
void* GetObjectPtr
    (
    const Int16 objectID    /* ID of an object in the form */
    )
{
    FormType* frm;

    frm = FrmGetActiveForm();
    return FrmGetObjectPtr( frm, FrmGetObjectIndex( frm, objectID ) );
}



void ShowObject
    (
    const Int16 objectID    /* ID of an object in the form */
    )
{
    FormType* frm;

    frm = FrmGetActiveForm();
    return FrmShowObject( frm, FrmGetObjectIndex( frm, objectID ) );
}



void HideObject
    (
    const Int16 objectID    /* ID of an object in the form */
    )
{
    FormType* frm;

    frm = FrmGetActiveForm();
    return FrmHideObject( frm, FrmGetObjectIndex( frm, objectID ) );
}

