/*
 * $Id: prefscolor.c,v 1.2 2007/03/31 23:34:15 prussar Exp $
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

#include "history.h"
#include "metadocument.h"
#include "os.h"
#include "prefsdata.h"
#include "resourceids.h"
#include "DIA.h"
#include "util.h"

#include "prefscolor.h"


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static Boolean        showFirst = false;
static Boolean        strikethrough;
static Boolean        forceDefaultColors;
static Boolean        cameFromLibrary;


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void LoadColorPrefs( void ) PREFSFORM_SECTION;



/* Handle the Color preferences */
Boolean PrefsColorPreferenceEvent
    (
    ActionType action
    )
{
    Boolean handled;

    handled = false;

    switch ( action ) {
        case AVAILABLE:
            if ( IsFormMain( Prefs()->lastForm ) ) {
                cameFromLibrary = false;
                handled = true;
            }
            else {
                /* We can only access this form from the library if the "vary
                   with documents" setting is unchecked */
                cameFromLibrary = true;
                if ( Prefs()->individualDocumentColor ) {
                    handled = false;
                }
                else {
                    handled = true;
                }
            }
            break;

        case SHOWFIRST:
            handled = showFirst;
            showFirst = false;
            break;

        case LOAD:
            LoadColorPrefs();
            handled = true;
            break;

        case SAVE:
            strikethrough       = CtlGetValue( GetObjectPtr( frmPrefsColorStrikethrough ));
            forceDefaultColors  = CtlGetValue( GetObjectPtr( frmPrefsColorForceDefaultColors ));

            if ( cameFromLibrary ) {
                Prefs()->individualDocumentColor = false;
            }
            else {
                Prefs()->individualDocumentColor = CtlGetValue( GetObjectPtr( frmPrefsColorIndividual ));
            }

            if (( strikethrough      != Prefs()->strikethrough ) ||
                ( forceDefaultColors != Prefs()->forceDefaultColors )) {
                if ( Prefs()->individualDocumentColor) {
                    GetHistoryPtr()->strikethrough      = strikethrough;
                    GetHistoryPtr()->forceDefaultColors = forceDefaultColors;
                }
                else {
                    Prefs()->strikethrough              = strikethrough;
                    Prefs()->forceDefaultColors         = forceDefaultColors;

                    Prefs()->defaultStrikethrough       = strikethrough;
                    Prefs()->defaultForceDefaultColors  = forceDefaultColors;
                }
                if ( Prefs()->individualDocumentColor &&
                     CtlGetValue( GetObjectPtr( frmPrefsColorAsDefault ))) {
                    Prefs()->defaultStrikethrough       = GetHistoryPtr()->strikethrough;
                    Prefs()->defaultForceDefaultColors  = GetHistoryPtr()->forceDefaultColors;
                }
                if ( ! cameFromLibrary ) {
                    ReRenderAllIfNeeded();
                }
            }
            handled = true;
            break;

        case DISPLAY:
            if ( cameFromLibrary ) {
                HidePrefsFormObject( frmPrefsColorIndividual, true );
                HidePrefsFormObject( frmPrefsColorAsDefault, true );
            }
            else {
                Boolean individualDocumentColor;

                individualDocumentColor = Prefs()->individualDocumentColor;
                CtlSetValue( GetObjectPtr( frmPrefsColorIndividual ), individualDocumentColor );
                CtlSetValue( GetObjectPtr( frmPrefsColorAsDefault ), false );
                if ( ! individualDocumentColor )
                    HidePrefsFormObject( frmPrefsColorAsDefault, true );
            }
            handled = true;
            break;

        case RELEASE:
            handled = true;
            break;

        default:
            handled = false;
            break;
    }
    return handled;
}



/* Load the general section into memory */
static void LoadColorPrefs( void )
{
    CtlSetValue( GetObjectPtr( frmPrefsColorStrikethrough ),
        Prefs()->strikethrough );
    CtlSetValue( GetObjectPtr( frmPrefsColorForceDefaultColors ),
        Prefs()->forceDefaultColors );
}



/* Nominate the Color section to be shown first when the prefsform loads */
void PrefsColorShowFirst( void )
{
    showFirst = true;
}



/* Event handler for the Color preferences */
Boolean PrefsColorPalmEvent
    (
    EventType* event  /* pointer to an EventType structure */
    )
{
    Boolean handled;

    handled     = false;

    switch ( event->eType ) {
        case popSelectEvent:
        {
            Int16       selection;

            selection = event->data.popSelect.selection;
            if ( selection != noListSelection ) {
                ControlType*    ctl;
                ListType*       list;
                Char*           label;
                UInt16          controlID;

                list        = event->data.popSelect.listP;
                controlID   = event->data.popSelect.controlID;
                ctl         = GetObjectPtr( controlID );
                label       = LstGetSelectionText( list, selection );

                CtlSetLabel( ctl, label );
                LstSetSelection( list, selection );

                switch ( controlID ) {
                    default:
                        break;
                }
            }
            break;
        }

        case ctlSelectEvent:
            switch ( event->data.ctlEnter.controlID ) {
                case frmPrefsColorIndividual:
                    if ( ! CtlGetValue( GetObjectPtr( frmPrefsColorIndividual ))) {
                        HidePrefsFormObject( frmPrefsColorAsDefault, true );
                    }
                    else {
                        ShowPrefsFormObject( frmPrefsColorAsDefault, true );
                    }
                    handled = true;
                    break;

                case frmPrefsColorAsDefault:
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

