/*
 * $Id: prefslookup.c,v 1.6 2007/03/31 23:34:15 prussar Exp $
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
#include "os.h"
#include "prefsdata.h"
#include "resourceids.h"
#include "util.h"
#include "metadocument.h"

#include "prefslookup.h"


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static SelectedWordActionType  lookupAction;
static Boolean                 showFirst = false;
static Boolean                 cameFromLibrary;


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void LoadLookupPrefs( void ) PREFSFORM_SECTION;



/* Handle the Lookup preferences */
Boolean PrefsLookupPreferenceEvent
    (
    ActionType action
    )
{
    Boolean handled;

    handled = false;

    switch ( action ) {
        case AVAILABLE:
#ifdef SUPPORT_WORD_LOOKUP
            if ( IsFormMain( Prefs()->lastForm ) ) {
                cameFromLibrary = false;
                handled = true;
            }
            else {
                /* We can only access this form from the library if the "vary
                   with documents" setting is unchecked */
                cameFromLibrary = true;
                if ( Prefs()->individualDocumentLookup ) {
                    handled = false;
                }
                else {
                    handled = true;
                }
            }
#else
            handled = false;
#endif
            break;

        case SHOWFIRST:
            handled = showFirst;
            showFirst = false;
            break;

        case LOAD:
            LoadLookupPrefs();
            handled = true;
            break;

        case SAVE:
            if ( cameFromLibrary ) {
                Prefs()->individualDocumentLookup = false;
            }
            else {
                Prefs()->individualDocumentLookup = CtlGetValue( GetObjectPtr( frmPrefsLookupIndividual ));
            }

            if ( Prefs()->individualDocumentLookup ) {
                GetHistoryPtr()->selectedWordAction = lookupAction;
                if ( CtlGetValue( GetObjectPtr( frmPrefsLookupAlwaysActive ))) {
                    GetHistoryPtr()->selectWordTap  = SELECT_WORD_TAP_ONCE;
                }
                else {
                    GetHistoryPtr()->selectWordTap  = SELECT_WORD_TAP_NONE;
                }
            }
            else {
                Prefs()->selectedWordAction         = lookupAction;
                Prefs()->defaultSelectedWordAction  = lookupAction;

                if ( CtlGetValue( GetObjectPtr( frmPrefsLookupAlwaysActive ))) {
                    Prefs()->selectWordTap          = SELECT_WORD_TAP_ONCE;
                    Prefs()->defaultSelectWordTap   = SELECT_WORD_TAP_ONCE;
                }
                else {
                    Prefs()->selectWordTap          = SELECT_WORD_TAP_NONE;
                    Prefs()->defaultSelectWordTap   = SELECT_WORD_TAP_NONE;
                }
            }
            if ( Prefs()->individualDocumentLookup &&
                 CtlGetValue( GetObjectPtr( frmPrefsLookupAsDefault ))) {
                Prefs()->defaultSelectedWordAction  = GetHistoryPtr()->selectedWordAction;
                Prefs()->defaultSelectWordTap       = GetHistoryPtr()->selectWordTap;
            }
            ReRenderAllIfNeeded();
            handled = true;
            break;

        case DISPLAY:
            if ( cameFromLibrary ) {
                HidePrefsFormObject( frmPrefsLookupIndividual, true );
                HidePrefsFormObject( frmPrefsLookupAsDefault, true );
            }
            else {
                Boolean individualDocumentLookup;

                individualDocumentLookup = Prefs()->individualDocumentLookup;
                CtlSetValue( GetObjectPtr( frmPrefsLookupIndividual ), individualDocumentLookup );
                CtlSetValue( GetObjectPtr( frmPrefsLookupAsDefault ), false );
                if ( ! individualDocumentLookup )
                    HidePrefsFormObject( frmPrefsLookupAsDefault, true );
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



/* Load the Lookup section into memory */
static void LoadLookupPrefs( void )
{
    CtlSetValue( GetObjectPtr( frmPrefsLookupAlwaysActive ),
        ( Prefs()->selectWordTap != SELECT_WORD_TAP_NONE ) );
    lookupAction = Prefs()->selectedWordAction;
    SetListToSelection( frmPrefsLookupActionList,
        frmPrefsLookupActionPopup, lookupAction );
}



/* Nominate the Lookup section to be shown first when the prefsform loads */
void PrefsLookupShowFirst( void )
{
    showFirst = true;
}



/* Event handler for the Lookup preferences */
Boolean PrefsLookupPalmEvent
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
                    case frmPrefsLookupActionPopup:
                        lookupAction = (SelectedWordActionType) selection;
                        handled = true;
                        break;

                    default:
                        break;
                }
            }
            break;
        }

        case ctlSelectEvent:
            switch ( event->data.ctlEnter.controlID ) {
                case frmPrefsLookupIndividual:
                    if ( ! CtlGetValue( GetObjectPtr( frmPrefsLookupIndividual ))) {
                        HidePrefsFormObject( frmPrefsLookupAsDefault, true );
                    }
                    else {
                        ShowPrefsFormObject( frmPrefsLookupAsDefault, true );
                    }
                    handled = true;
                    break;

                case frmPrefsLookupAsDefault:
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

