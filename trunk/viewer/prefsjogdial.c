/*
 * $Id: prefsjogdial.c,v 1.7 2003/07/03 14:13:43 prussar Exp $
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

#include "jogdial.h"
#include "prefsdata.h"
#include "resourceids.h"
#include "util.h"

#include "prefsjogdial.h"


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define HANDERA_ADJUST_JOGDIAL 24


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static Boolean     showFirst = false;
static SelectType  jogdialMode[ NUM_OF_JOGEVENTS ];
static JogEvents   jogdialSelected;
static UInt16      jogdialObjectList[][ MAX_OBJECTS ] = {
    /* sonyJogdialType1 */
    { frmPrefsJogdialUp, bmpJogdialUp, frmPrefsJogdialPushUp, bmpJogdialPushUp,
      frmPrefsJogdialPush, bmpJogdialPush, frmPrefsJogdialDown, bmpJogdialDown,
      frmPrefsJogdialPushDown, bmpJogdialPushDown, EOF },
    /* sonyJogdialType2 */
    { frmPrefsJogdialUp, bmpJogdialUp, frmPrefsJogdialPushUp, bmpJogdialPushUp,
      frmPrefsJogdialPush, bmpJogdialPush, frmPrefsJogdialDown, bmpJogdialDown,
      frmPrefsJogdialPushDown, bmpJogdialPushDown, frmPrefsJogdialBack,
      bmpJogdialBack, EOF },
    /* handeraJogdial */
    { frmPrefsJogdialUp, bmpJogdialUp, frmPrefsJogdialPush, bmpJogdialPush,
      frmPrefsJogdialDown, bmpJogdialDown, frmPrefsJogdialBack, bmpJogdialBack,
      EOF },
    /* handspringJogdial */
    { frmPrefsJogdialUp, bmpJogdialUp, frmPrefsJogdialPush, bmpJogdialPush,
      frmPrefsJogdialDown, bmpJogdialDown, EOF },
};



/* Handle the Jogdial preferences */
Boolean PrefsJogdialPreferenceEvent
    (
    ActionType action
    )
{
    Boolean       handled;
    Jogdial       jogType;
    static UInt16 layout = 0;

    handled = false;

    switch ( action ) {
        case AVAILABLE:
            /* Only available if the device supports it */
            handled = ( JogdialType() != noJogdial );
            break;

        case SHOWFIRST:
            handled = showFirst;
            showFirst = false;
            break;

        case LOAD:
            jogdialSelected = JOGEVENTS_UP;
            MemMove( jogdialMode, Prefs()->jogMode, sizeof( jogdialMode ) );
            CtlSetValue( GetObjectPtr( frmPrefsJogdialJogEnabled ),
                Prefs()->jogEnabled );

            InitializeActionList( frmPrefsJogdialActionList );

            jogType = JogdialType();
            layout = 0;
            while ( 1 < jogType ) {
                jogType >>= 1;
                layout++;
            }
            jogType = 1 << layout;

            if ( jogType == handeraJogdial ) {
                FormType* form;
                Coord     x;

                form = FrmGetActiveForm();
                x = HANDERA_ADJUST_JOGDIAL;
                MoveObjectRelatively( form, frmPrefsJogdialUp, x, 0 );
                MoveObjectRelatively( form, frmPrefsJogdialDown, x, 0 );
                MoveObjectRelatively( form, frmPrefsJogdialPush, -x, 0 );
                MoveObjectRelatively( form, frmPrefsJogdialBack, -x, 0 );
                MoveObjectRelatively( form, bmpJogdialUp, x, 0 );
                MoveObjectRelatively( form, bmpJogdialDown, x, 0 );
                MoveObjectRelatively( form, bmpJogdialPush, -x, 0 );
                MoveObjectRelatively( form, bmpJogdialBack, -x, 0 );
            }

            SetListToSelection( frmPrefsJogdialActionList,
                frmPrefsJogdialSelectAction, jogdialMode[ jogdialSelected ] );
            handled = true;
            break;

        case DISPLAY:
            PrefsShowSection( jogdialObjectList, layout );
            AffirmControlImage( jogdialSelected, frmPrefsJogdialUp,
                bmpJogdialUp );
            handled = true;
            break;

        case CLEAR:
            PrefsHideSection( jogdialObjectList, layout );
            handled = true;
            break;

        case SAVE:
            Prefs()->jogEnabled = CtlGetValue( GetObjectPtr(
                frmPrefsJogdialJogEnabled ) );
            MemMove( Prefs()->jogMode, jogdialMode, sizeof( jogdialMode ) );
            handled = true;
            break;

        default:
            break;
    }
    return handled;
}



/* Nominate the Jogdial section to be shown first when the prefsform loads */
void PrefsJogdialShowFirst( void )
{
    showFirst = true;
}



/* Event handler for the Jogdial preference */
Boolean PrefsJogdialPalmEvent
    (
    EventType* event  /* pointer to an EventType structure */
    )
{
    Boolean handled;

    handled     = false;

    switch ( event->eType ) {
        case ctlSelectEvent:
            switch ( event->data.ctlEnter.controlID ) {
                case frmPrefsJogdialUp:
                case frmPrefsJogdialDown:
                case frmPrefsJogdialPush:
                case frmPrefsJogdialPushUp:
                case frmPrefsJogdialPushDown:
                case frmPrefsJogdialBack:
                    jogdialSelected = event->data.ctlEnter.controlID -
                        frmPrefsJogdialUp;
                    SetListToSelection( frmPrefsJogdialActionList,
                        frmPrefsJogdialSelectAction,
                        jogdialMode[ jogdialSelected ] );
                    handled = true;
                    break;

                case frmPrefsJogdialSelectAction:
                    jogdialMode[ jogdialSelected ] =
                         SetActionForSelectedButton( frmPrefsJogdialActionList,
                             event->data.ctlEnter.controlID );
                    handled = true;
                    break;

                default:
                    break;
            }
            break;

        default:
            handled = false;
    }

    return handled;
}

