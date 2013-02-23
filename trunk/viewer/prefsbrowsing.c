/*
 * $Id: prefsbrowsing.c,v 1.14 2005/08/25 21:03:30 prussar Exp $
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
#include "debug.h"
#include "fullscreenform.h"

#include "prefsbrowsing.h"


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static Boolean        showFirst   = false;
static ForceAlignType forceAlign;
static Int16          lineSpacing;
static Int16          paragraphSpacing;
static Boolean        underlineMode;
static Boolean        pageControlsLink;
static Boolean        enableSoftHyphens;
static Boolean        enableMargins;
static Boolean        cameFromLibrary;
#ifdef HAVE_ROTATE
static RotateType     rotate;
#endif
#ifdef HAVE_RTL
static Boolean        rtl;
#endif



/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void LoadLayoutPrefs( void ) PREFSFORM_SECTION;



/* Handle the Layout preferences */
Boolean PrefsLayoutPreferenceEvent
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
                if ( Prefs()->individualDocumentLayout ) {
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
            LoadLayoutPrefs();
            handled = true;
            break;

        case SAVE:
            lineSpacing       += MINIMAL_LINE_SPACING;
            underlineMode      = CtlGetValue( GetObjectPtr( frmPrefsLayoutUnderline ));
            pageControlsLink   = CtlGetValue( GetObjectPtr( frmPrefsLayoutPageControlsLink ));
            enableSoftHyphens  = CtlGetValue( GetObjectPtr( frmPrefsLayoutEnableSoftHyphens ));
            enableMargins      = CtlGetValue( GetObjectPtr( frmPrefsLayoutEnableMargins ));
#ifdef HAVE_RTL
            rtl = CtlGetValue( GetObjectPtr( frmPrefsLayoutRTL ) );
#endif

            if ( cameFromLibrary ) {
                Prefs()->individualDocumentLayout = false;
            }
            else {
                Prefs()->individualDocumentLayout = CtlGetValue( GetObjectPtr( frmPrefsLayoutIndividual ));
            }

            if (( lineSpacing      != Prefs()->lineSpacing       ) ||
                ( paragraphSpacing != Prefs()->paragraphSpacing  ) ||
                ( underlineMode    != Prefs()->underlineMode     ) ||
                ( pageControlsLink != Prefs()->pageControlsLink  ) ||
                ( enableSoftHyphens!= Prefs()->enableSoftHyphens ) ||
                ( enableMargins    != Prefs()->enableMargins     ) ||
#ifdef HAVE_ROTATE
                ( rotate           != Prefs()->rotate            ) ||
#endif
#ifdef HAVE_RTL
                ( rtl              != Prefs()->rtl               ) ||
#endif
                ( forceAlign       != Prefs()->forceAlign        )) {
                if ( Prefs()->individualDocumentLayout ) {
                    GetHistoryPtr()->underlineMode      = underlineMode;
                    GetHistoryPtr()->pageControlsLink   = pageControlsLink;
                    GetHistoryPtr()->enableSoftHyphens  = enableSoftHyphens;
                    GetHistoryPtr()->enableMargins      = enableMargins;
                    GetHistoryPtr()->forceAlign         = forceAlign;
                    GetHistoryPtr()->lineSpacing        = lineSpacing;
                    GetHistoryPtr()->paragraphSpacing   = paragraphSpacing;
#ifdef HAVE_ROTATE
                    GetHistoryPtr()->rotate             = rotate;
#endif
#ifdef HAVE_RTL
                    GetHistoryPtr()->rtl                = rtl;
#endif
                }
                else {
                    Prefs()->underlineMode              = underlineMode;
                    Prefs()->pageControlsLink           = pageControlsLink;
                    Prefs()->enableSoftHyphens          = enableSoftHyphens;
                    Prefs()->enableMargins              = enableMargins;
                    Prefs()->forceAlign                 = forceAlign;
                    Prefs()->lineSpacing                = lineSpacing;
                    Prefs()->paragraphSpacing           = paragraphSpacing;
#ifdef HAVE_ROTATE
                    Prefs()->rotate                     = rotate;
#endif
#ifdef HAVE_RTL
                    Prefs()->rtl                        = rtl;
#endif

                    Prefs()->defaultUnderlineMode       = underlineMode;
                    Prefs()->defaultPageControlsLink    = pageControlsLink;
                    Prefs()->defaultEnableSoftHyphens   = enableSoftHyphens;
                    Prefs()->defaultEnableMargins       = enableMargins;
                    Prefs()->defaultForceAlign          = forceAlign;
                    Prefs()->defaultLineSpacing         = lineSpacing;
                    Prefs()->defaultParagraphSpacing    = paragraphSpacing;
#ifdef HAVE_ROTATE
                    Prefs()->defaultRotate              = rotate;
#endif
#ifdef HAVE_RTL
                    Prefs()->defaultRTL                 = rtl;
#endif
                }
                if ( Prefs()->individualDocumentLayout &&
                     CtlGetValue( GetObjectPtr( frmPrefsLayoutAsDefault ))) {

                    Prefs()->defaultUnderlineMode       = GetHistoryPtr()->underlineMode;
                    Prefs()->defaultPageControlsLink    = GetHistoryPtr()->pageControlsLink;
                    Prefs()->defaultEnableSoftHyphens   = GetHistoryPtr()->enableSoftHyphens;
                    Prefs()->defaultEnableMargins       = GetHistoryPtr()->enableMargins;
                    Prefs()->defaultForceAlign          = GetHistoryPtr()->forceAlign;
                    Prefs()->defaultLineSpacing         = GetHistoryPtr()->lineSpacing;
                    Prefs()->defaultParagraphSpacing    = GetHistoryPtr()->paragraphSpacing;
#ifdef HAVE_ROTATE
                    Prefs()->defaultRotate              = GetHistoryPtr()->rotate;
#endif
#ifdef HAVE_RTL
                    Prefs()->defaultRTL                 = GetHistoryPtr()->rtl;
#endif
                }
                if ( ! cameFromLibrary ) {
                    ReRenderAll();
                }
            }
            handled = true;
            break;

        case DISPLAY:
            if ( cameFromLibrary ) {
                HidePrefsFormObject( frmPrefsLayoutIndividual, true );
                HidePrefsFormObject( frmPrefsLayoutAsDefault, true );
            }
            else {
                Boolean individualDocumentLayout;

                individualDocumentLayout = Prefs()->individualDocumentLayout;
                CtlSetValue( GetObjectPtr( frmPrefsLayoutIndividual ), individualDocumentLayout );
                CtlSetValue( GetObjectPtr( frmPrefsLayoutAsDefault ), false );
                if ( ! individualDocumentLayout )
                    HidePrefsFormObject( frmPrefsLayoutAsDefault, true );
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



/* Load the layout section into memory */
static void LoadLayoutPrefs( void )
{
    CtlSetValue( GetObjectPtr( frmPrefsLayoutUnderline ),
        Prefs()->underlineMode );
    CtlSetValue( GetObjectPtr( frmPrefsLayoutPageControlsLink ),
        Prefs()->pageControlsLink );
    CtlSetValue( GetObjectPtr( frmPrefsLayoutEnableSoftHyphens ),
        Prefs()->enableSoftHyphens );
    CtlSetValue( GetObjectPtr( frmPrefsLayoutEnableMargins ),
        Prefs()->enableMargins );

    forceAlign = Prefs()->forceAlign;
    SetListToSelection( frmPrefsLayoutAlignList,
        frmPrefsLayoutAlignPopup, forceAlign );

    lineSpacing = Prefs()->lineSpacing - MINIMAL_LINE_SPACING;
    SetListToSelection( frmPrefsLayoutLineSpacingList,
        frmPrefsLayoutLineSpacingPopup, lineSpacing );

    paragraphSpacing = Prefs()->paragraphSpacing;
    SetListToSelection( frmPrefsLayoutParagraphSpacingList,
        frmPrefsLayoutParagraphSpacingPopup, paragraphSpacing );
#ifdef HAVE_ROTATE
    rotate = Prefs()->rotate;
    SetListToSelection( frmPrefsLayoutRotateList, frmPrefsLayoutRotatePopup, rotate );
#endif
#ifdef HAVE_RTL
    CtlSetValue( GetObjectPtr( frmPrefsLayoutRTL ), Prefs()->rtl );
#endif

}



/* Nominate the Layout section to be shown first when the prefsform loads */
void PrefsLayoutShowFirst( void )
{
    showFirst = true;
}



/* Event handler for the Layout preferences */
Boolean PrefsLayoutPalmEvent
    (
    EventType* event  /* pointer to an EventType structure */
    )
{
    Boolean handled;

    handled = false;

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
                    case frmPrefsLayoutAlignPopup:
                        forceAlign = (ForceAlignType) selection;
                        handled = true;
                        break;

                    case frmPrefsLayoutParagraphSpacingPopup:
                        paragraphSpacing = selection;
                        handled = true;
                        break;

                    case frmPrefsLayoutLineSpacingPopup:
                        lineSpacing = selection;
                        handled = true;
                        break;

#ifdef HAVE_ROTATE
                    case frmPrefsLayoutRotatePopup:
                        rotate = (RotateType) selection;
                        handled = true;
                        break;
#endif
#ifdef HAVE_RTL
                    case frmPrefsLayoutRTL:
                        handled = true;
                        break;
#endif

                    default:
                        break;
                }
            }
            break;
        }

        case ctlSelectEvent:
            switch ( event->data.ctlEnter.controlID ) {
                case frmPrefsLayoutIndividual:
                    if ( ! CtlGetValue( GetObjectPtr( frmPrefsLayoutIndividual ))) {
                        HidePrefsFormObject( frmPrefsLayoutAsDefault, true );
                    }
                    else {
                        ShowPrefsFormObject( frmPrefsLayoutAsDefault, true );
                    }
                    handled = true;
                    break;

                case frmPrefsLayoutAsDefault:
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

