/*
 * $Id: form.c,v 1.2 2003/12/13 03:21:28 prussar Exp $
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

#include "data.h"

#include "form.h"


/***********************************************************************
 *
 *      Internal Types
 *
 ***********************************************************************/
typedef enum {
    VALID = 0,
    NOT_FOUND,
    CLEARED
} StatusType;

typedef struct {
    StatusType status;
    UInt32     creator;
    UInt32     type;
    UInt16     recordIndex;
    UInt16     sizeIndex;
    UInt16     notFoundIndex;
    UInt16     clearedIndex;
    Char*      recordText;
    Char*      sizeText;
} ButtonType;


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define NUM_OF_BUTTONS 4
#define PREF           0
#define TEMP           1
#define DOCLIST        2
#define FONTCACHE      3

#define NUM_OF_FIELDS  6
#define BASE_INDEX frmPrefsRecords

#define PreferencesID         'psys'
#define SavedPreferencesType  'sprf'


/***********************************************************************
 *
 *      Private Variables
 *
 ***********************************************************************/
static FormType*   form;
static ButtonType  buttonList[ NUM_OF_BUTTONS ] = {
    { VALID, PreferencesID, SavedPreferencesType,
      frmPrefsRecords, frmPrefsSize,
      frmPrefsNotFound, frmPrefsCleared, NULL, NULL },
    { VALID, ViewerAppID, MetaDocumentType,
      frmTempRecords, frmTempSize,
      frmTempNotFound, frmTempCleared, NULL, NULL },
    { VALID, ViewerAppID, PlkrDocListType,
      frmDocListRecords, frmDocListSize,
      frmDocListNotFound, frmDocListCleared, NULL, NULL },
    { VALID, ViewerAppID, PlkrVFSFontCacheType,
      frmFontCacheRecords, frmFontCacheSize,
      frmFontCacheNotFound, frmFontCacheCleared, NULL, NULL },
};


/***********************************************************************
 *
 *      Local Functions
 *
 ***********************************************************************/
static void FormInit( void );
static void HideNShow( UInt16 hideThis, UInt16 andThis, UInt16 butShowThis );
static void HandleButton( ButtonType* button );
static void FreeMemory( void );



/* Initialize the main form */
static void FormInit( void )
{
    UInt16 i;

    form = FrmGetActiveForm();

    for ( i = 0; i < NUM_OF_BUTTONS; i++ ) {
        ButtonType* button;
        UInt32      records;
        UInt32      size;

        button = &buttonList[ i ];

        EnumerateData( button->creator, button->type, &records, &size );

        if ( records == 0 ) {
            HideNShow( button->recordIndex, button->sizeIndex,
                button->notFoundIndex );
            button->status = NOT_FOUND;
        } else {
            button->recordText = MemPtrNew( 255 );
            button->sizeText   = MemPtrNew( 255 );

            StrPrintF( button->recordText, "%ld", records );
            StrPrintF( button->sizeText, "%ld", size );

            FldSetTextPtr( FrmGetObjectPtr( form,
                FrmGetObjectIndex( form, button->recordIndex ) ),
                button->recordText );
            FldSetTextPtr( FrmGetObjectPtr( form,
                FrmGetObjectIndex( form, button->sizeIndex ) ),
                button->sizeText );

            button->status = VALID;
        }
    }

    FrmDrawForm( form );
}



/* A game of Hide and go Seek er, Show? */
static void HideNShow
    (
    UInt16 hideThis,
    UInt16 andThis,
    UInt16 butShowThis
    )
{
    FrmHideObject( form, FrmGetObjectIndex( form, hideThis ) );
    FrmHideObject( form, FrmGetObjectIndex( form, andThis ) );
    FrmShowObject( form, FrmGetObjectIndex( form, butShowThis ) );
}



static void HandleButton
    (
    ButtonType* button
    )
{
    Err err;

    err = ClearData( button->creator, button->type );
    if ( err == errNone ) {
        HideNShow( button->recordIndex, button->sizeIndex,
            button->clearedIndex );
        button->status = CLEARED;
    } else {
        Char errCode[ 10 ];

        StrPrintF( errCode, "%d", err );
        FrmCustomAlert( errCannotDelete, errCode, NULL, NULL );
    }
}



/* Free the allocated memory */
static void FreeMemory( void )
{
    UInt16 i;

    for ( i = 0; i < NUM_OF_BUTTONS; i++ ) {
        if ( buttonList[ i ].recordText != NULL )
            MemPtrFree( buttonList[ i ].recordText );
        if ( buttonList[ i ].sizeText != NULL )
            MemPtrFree( buttonList[ i ].sizeText );
    }

    /* Free the linked list generated by EnumerateData() */
    FreeDataList();
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
                case frmPrefsButton:
                    if ( buttonList[ PREF ].status == VALID &&
                         FrmAlert( confirmPrefs ) == 0 )
                        HandleButton( &buttonList[ PREF ] );
                    handled = true;
                    break;

                case frmTempButton:
                    if ( buttonList[ TEMP ].status == VALID &&
                         FrmAlert( confirmTemp ) == 0 )
                        HandleButton( &buttonList[ TEMP ] );
                    handled = true;
                    break;

                case frmDocListButton:
                    if ( buttonList[ DOCLIST ].status == VALID &&
                         FrmAlert( confirmDocList ) == 0 )
                        HandleButton( &buttonList[ DOCLIST ] );
                    handled = true;
                    break;

                case frmFontCacheButton:
                    if ( buttonList[ FONTCACHE ].status == VALID &&
                         FrmAlert( confirmFontCache ) == 0 )
                        HandleButton( &buttonList[ FONTCACHE ] );
                    handled = true;
                    break;

                case frmDone:
                {
                    EventType stopEvent;

                    stopEvent.eType = appStopEvent;
                    EvtAddEventToQueue( &stopEvent );
                    handled = true;
                    break;
                }

                case frmWhatIsThis:
                    FrmHelp( strWhatIsThis );
                    handled = true;
                    break;

                default:
                    break;
            }
            break;

        case menuEvent:
            switch ( event->data.menu.itemID ) {
                case menuHelpWhatIsThis:
                    FrmHelp( strWhatIsThis );
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

        case frmOpenEvent:
            FormInit();
            handled = true;
            break;

        case frmCloseEvent:
            FreeMemory();
            handled = false;
            break;

        default:
            handled = false;
            break;
    }

    return handled;
}
