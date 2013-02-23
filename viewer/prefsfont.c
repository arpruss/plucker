/*
 * $Id: prefsfont.c,v 1.3 2004/07/25 19:42:56 nordstrom Exp $
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
#include "font.h"
#include "hires.h"

#include "prefsfont.h"


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define FONT_BUTTONS        1

static const Char letter  = 'A';
#define BUTTON_COUNT         6
#define BUTTON_COUNT_OS2     2
#define BUTTON_COUNT_HANDERA 5

#define NO_USER_FONT_BUTTON  -1

/***********************************************************************
 *
 *      Internal Types
 *
 ***********************************************************************/
typedef enum {
    LASTFORM_MAIN = 0,
    LASTFORM_LIBRARY
} LastFormType;

typedef struct {
    FontModeType fontMode;
    FontID       fontID;
    UInt16       objID;
    ControlType* control;
} ButtonType;


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static Boolean          showFirst = false;
static ButtonType*      buttonList;
static UInt8            buttonCount;
static UInt8            selected;
static FontModeType     fontModePref[ 2 ];
static Int16            mainUserFontSelection;
static Int16            libraryUserFontSelection;
static Int16            userFontButtonNumber;
static Int16            oldUserFontNumber;
static Char*            mainUserFontName;
static Char*            libraryUserFontName;
static ModeType         prevControlMode;
static ModeType         controlMode;
static UInt16           objectList[ 2 ][ MAX_OBJECTS ];
static Boolean          cameFromLibrary;


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void LoadFontPrefs( void ) PREFSFORM_SECTION;



/* Draw or redraw the font selection buttons */
static void DrawButtons
    (
    Boolean redraw   /* is this a first drawing or a redrawing? */
    )
{
    FontID  prevFont;
    UInt16  i;
    FormType* form;

    form = FrmGetActiveForm();

    if ( redraw && userFontButtonNumber == NO_USER_FONT_BUTTON )
        return;

    prevFont = FntGetFont();

    if ( userFontButtonNumber != NO_USER_FONT_BUTTON ) {
        if ( controlMode == MODE1 ) {
            LoadUserFont( mainUserFontSelection );
        }
        else {
            LoadUserFont( libraryUserFontSelection );
        }
        RefreshCustomFonts();
    }

    /* Display the letters in the middle of each pushbutton */
    for ( i = 0; i < buttonCount; i++ ) {
        buttonList[ i ].control = GetObjectPtr( buttonList[ i ].objID );
        if ( redraw ) {
            CtlSetValue( buttonList[ i ].control, false );
            CtlEraseControl( buttonList[ i ].control );
            CtlDrawControl( buttonList[ i ].control );
        }
        DisplayChar( buttonList[ i ].fontID, letter, form,
            buttonList[ i ].objID );
    }

    if ( userFontButtonNumber != NO_USER_FONT_BUTTON ) {
        LoadUserFont( oldUserFontNumber );
        RefreshCustomFonts();
    }

    FntSetFont( prevFont );
    FrmSetControlGroupSelection( form, FONT_BUTTONS,
        buttonList[ selected ].objID );
}


/* Handle the Font preferences */
Boolean PrefsFontPreferenceEvent
    (
    ActionType action
    )
{
    Boolean handled;

    handled = false;

    switch ( action ) {
        case AVAILABLE:
            /* Always available */
            handled = true;
            break;

        case SHOWFIRST:
            handled = showFirst;
            showFirst = false;
            break;

        case LOAD:
            LoadFontPrefs();
            handled = true;
            break;

        case SAVE:
        {
            Boolean updateDoc;
            Boolean changeMainUserFont;
            Boolean changeLibraryUserFont;

            updateDoc               = false;
            changeMainUserFont      = false;
            changeLibraryUserFont   = false;

            if ( cameFromLibrary ) {
                Prefs()->individualDocumentFonts = false;
            }
            else {
                Prefs()->individualDocumentFonts = CtlGetValue( GetObjectPtr( frmPrefsFontIndividual ));
            }

            if ( userFontButtonNumber != NO_USER_FONT_BUTTON && 
                 GetNumberOfUserFonts() != 0 ) {
                if ( mainUserFontSelection != GetUserFontNumber( mainUserFontName, true, fontCacheOff )) {
                    changeMainUserFont = true;
                }
                if ( libraryUserFontSelection != GetUserFontNumber( libraryUserFontName, true, fontCacheOff )) {
                    changeLibraryUserFont = true;
                }
            }

            if (( fontModePref[ MODE1 ] != Prefs()->fontModeMain ) || changeMainUserFont ) {
                if ( Prefs()->individualDocumentFonts ) {
                    GetHistoryPtr()->font           = fontModePref[ MODE1 ];
                }
                else {
                    Prefs()->fontModeMain           = fontModePref[ MODE1 ];

                    Prefs()->defaultFontModeMain    = fontModePref[ MODE1 ];
                }
                if ( changeMainUserFont ) {
                    LoadUserFont( mainUserFontSelection );
                    RefreshCustomFonts();
                }
                SetFontStyles();
                if ( changeMainUserFont ) {
                    if ( Prefs()->individualDocumentFonts ) {
                        StrCopy( GetHistoryPtr()->userFontName, GetUserFontName( mainUserFontSelection ));
                    }
                    else {
                        StrCopy( Prefs()->mainUserFontName, GetUserFontName( mainUserFontSelection ));
                        StrCopy( Prefs()->defaultMainUserFontName, GetUserFontName( mainUserFontSelection ));
                    }
                }
                if ( Prefs()->individualDocumentFonts &&
                     CtlGetValue( GetObjectPtr( frmPrefsFontAsDefault ))) {
                    Prefs()->defaultFontModeMain    = GetHistoryPtr()->font;
                    StrCopy( Prefs()->defaultMainUserFontName, GetHistoryPtr()->userFontName );
                }
                updateDoc = true;
            }

            if (( fontModePref[ MODE2 ] != Prefs()->fontModeLibrary ) || changeLibraryUserFont ) {
                Prefs()->fontModeLibrary        = fontModePref[ MODE2 ];
                if ( changeLibraryUserFont ) {
                    LoadUserFont( libraryUserFontSelection );
                    RefreshCustomFonts();
                }
                SetFontStyles();
                if ( changeLibraryUserFont ) {
                    StrCopy( Prefs()->libraryUserFontName, GetUserFontName( libraryUserFontSelection ));
                }
            }

            if ( updateDoc && ! cameFromLibrary ) {
                ReRenderAll();
            }

            handled = true;
            break;
        }

        case DISPLAY:
        {
            UInt16 i;

            CtlSetValue( GetObjectPtr( frmPrefsFontMainMode ),
                controlMode == MODE1 );
            CtlSetValue( GetObjectPtr( frmPrefsFontLibraryMode ),
                controlMode == MODE2 );

            PrefsShowSection( objectList, controlMode );
            prevControlMode = controlMode;

            /* Highlight the pushbutton that reflects the currently set font */
            for ( i = 0; i < buttonCount; i++ ) {
                if ( fontModePref[ controlMode ] == buttonList[ i ].fontMode ) {
                    selected = i;
                    break;
                }
            }

            if ( userFontButtonNumber != NO_USER_FONT_BUTTON ) {
                if ( controlMode == MODE1 ) {
                    mainUserFontSelection = GetUserFontNumber( mainUserFontName, true, 
                                                fontCacheOff );
                    if ( mainUserFontSelection == NO_SUCH_USER_FONT )
                        mainUserFontSelection = 0;
                    SetListToSelection( frmPrefsFontUserFontList, frmPrefsFontUserFontPopup,
                        mainUserFontSelection );
                }
                else {
                    libraryUserFontSelection = GetUserFontNumber( mainUserFontName, true, 
                                                fontCacheOff );
                    if ( libraryUserFontSelection == NO_SUCH_USER_FONT )
                        libraryUserFontSelection = 0;
                    SetListToSelection( frmPrefsFontUserFontList, frmPrefsFontUserFontPopup,
                        libraryUserFontSelection );
                }
            }
            if ( cameFromLibrary ) {
                HidePrefsFormObject( frmPrefsFontIndividual, true );
                HidePrefsFormObject( frmPrefsFontAsDefault, true );
            }
            else {
                Boolean individualDocumentFonts;

                individualDocumentFonts = Prefs()->individualDocumentFonts;
                CtlSetValue( GetObjectPtr( frmPrefsFontIndividual ), individualDocumentFonts );
                CtlSetValue( GetObjectPtr( frmPrefsFontAsDefault ), false );
                if ( ! individualDocumentFonts )
                    HidePrefsFormObject( frmPrefsFontAsDefault, true );
            }

            DrawButtons( false );

            handled = true;
            break;
        }

        case CLEAR:
            CtlSetValue( buttonList[ selected ].control, false );
            PrefsHideSection( objectList, prevControlMode );
            handled = true;
            break;

        case RELEASE:
            SafeMemPtrFree( buttonList );
            buttonList = NULL;
            handled = true;
            break;

        default:
            handled = false;
            break;
    }
    return handled;
}

/* Load the general section into memory */
static void LoadFontPrefs( void )
{
    if ( IsFormMain( Prefs()->lastForm ) ) {
        controlMode     = MODE1;
        cameFromLibrary = false;
    }
    else {
        controlMode     = MODE2;
        cameFromLibrary = true;
    }

    fontModePref[ 0 ]   = Prefs()->fontModeMain;
    fontModePref[ 1 ]   = Prefs()->fontModeLibrary;
    mainUserFontName    = Prefs()->mainUserFontName;
    libraryUserFontName = Prefs()->libraryUserFontName;

#ifdef HAVE_HANDERA_SDK
    if ( IsHiResTypeHandera( HiResType())) {
        buttonCount = BUTTON_COUNT_HANDERA;
        objectList[ 0 ][ 0 ] = frmPrefsFontLabel;
        objectList[ 0 ][ 1 ] = frmPrefsFont1;
        objectList[ 0 ][ 2 ] = frmPrefsFont2;
        objectList[ 0 ][ 3 ] = frmPrefsFont3;
        objectList[ 0 ][ 4 ] = frmPrefsFont4;
        objectList[ 0 ][ 5 ] = frmPrefsFont5;
        objectList[ 0 ][ 6 ] = frmPrefsFontIndividual;
        objectList[ 0 ][ 7 ] = frmPrefsFontAsDefault;
        objectList[ 0 ][ 8 ] = EOL;

        objectList[ 1 ][ 0 ] = frmPrefsFontLabel;
        objectList[ 1 ][ 1 ] = frmPrefsFont1;
        objectList[ 1 ][ 2 ] = frmPrefsFont2;
        objectList[ 1 ][ 3 ] = frmPrefsFont3;
        objectList[ 1 ][ 4 ] = frmPrefsFont4;
        objectList[ 1 ][ 5 ] = frmPrefsFont5;
        objectList[ 1 ][ 6 ] = EOL;
    }
    else
#endif
    if ( Support30()) {
        buttonCount = BUTTON_COUNT;
        objectList[ 0 ][ 0 ] = frmPrefsFontLabel;
        objectList[ 0 ][ 1 ] = frmPrefsFont1;
        objectList[ 0 ][ 2 ] = frmPrefsFont2;
        objectList[ 0 ][ 3 ] = frmPrefsFont3;
        objectList[ 0 ][ 4 ] = frmPrefsFont4;
        objectList[ 0 ][ 5 ] = frmPrefsFont5;
        objectList[ 0 ][ 6 ] = frmPrefsFontUserLabel;
        objectList[ 0 ][ 7 ] = frmPrefsFontUser;
        objectList[ 0 ][ 8 ] = frmPrefsFontUserFontPopup;
        objectList[ 0 ][ 9 ] = frmPrefsFontIndividual;
        objectList[ 0 ][ 10 ] = frmPrefsFontAsDefault;
        objectList[ 0 ][ 11 ] = EOL;

        objectList[ 1 ][ 0 ] = frmPrefsFontLabel;
        objectList[ 1 ][ 1 ] = frmPrefsFont1;
        objectList[ 1 ][ 2 ] = frmPrefsFont2;
        objectList[ 1 ][ 3 ] = frmPrefsFont3;
        objectList[ 1 ][ 4 ] = frmPrefsFont4;
        objectList[ 1 ][ 5 ] = frmPrefsFont5;
        objectList[ 1 ][ 6 ] = frmPrefsFontUserLabel;
        objectList[ 1 ][ 7 ] = frmPrefsFontUser;
        objectList[ 1 ][ 8 ] = frmPrefsFontUserFontPopup;
        objectList[ 1 ][ 9 ] = EOL;
    }
    else {
        buttonCount = BUTTON_COUNT_OS2;
        objectList[ 0 ][ 0 ] = frmPrefsFontLabel;
        objectList[ 0 ][ 1 ] = frmPrefsFont1;
        objectList[ 0 ][ 2 ] = frmPrefsFont2;
        objectList[ 0 ][ 3 ] = frmPrefsFontIndividual;
        objectList[ 0 ][ 4 ] = frmPrefsFontAsDefault;
        objectList[ 0 ][ 5 ] = EOL;

        objectList[ 1 ][ 0 ] = frmPrefsFontLabel;
        objectList[ 1 ][ 1 ] = frmPrefsFont1;
        objectList[ 1 ][ 2 ] = frmPrefsFont2;
        objectList[ 1 ][ 3 ] = EOL;
    }

    buttonList  = SafeMemPtrNew( buttonCount * sizeof *buttonList );

#ifdef HAVE_HANDERA_SDK
    if ( IsHiResTypeHandera( HiResType())) {
        buttonList[0].fontMode = FONT_NARROW;
        buttonList[1].fontMode = FONT_TINY;
        buttonList[2].fontMode = FONT_SMALL;
        buttonList[3].fontMode = FONT_DEFAULT;
        buttonList[4].fontMode = FONT_LARGE;
        buttonList[0].fontID   = narrowFont;
        buttonList[1].fontID   = stdFont;
        buttonList[2].fontID   = largeFont;
        buttonList[3].fontID   = VgaBaseToVgaFont( stdFont );
        buttonList[4].fontID   = VgaBaseToVgaFont( largeFont );
        buttonList[0].objID    = frmPrefsFont1;
        buttonList[1].objID    = frmPrefsFont2;
        buttonList[2].objID    = frmPrefsFont3;
        buttonList[3].objID    = frmPrefsFont4;
        buttonList[4].objID    = frmPrefsFont5;
        userFontButtonNumber   = NO_USER_FONT_BUTTON;
    }
    else
#endif
    if ( Support30()) {
#ifdef HAVE_HIRES
        if ( IsHiResTypePalm( HiResType())) {
            buttonList[0].fontMode = FONT_TINY;
            buttonList[1].fontMode = FONT_SMALL;
            buttonList[2].fontMode = FONT_NARROW;
            buttonList[3].fontMode = FONT_DEFAULT;
            buttonList[4].fontMode = FONT_LARGE;
            buttonList[5].fontMode = FONT_USER;
            buttonList[0].fontID   = tinyFont_palm;
            buttonList[1].fontID   = smallFont_palm;
            buttonList[2].fontID   = narrowFont;
            buttonList[3].fontID   = stdFont;
            buttonList[4].fontID   = largeFont;
            buttonList[5].fontID   = userStdFont_palm;
            buttonList[0].objID    = frmPrefsFont1;
            buttonList[1].objID    = frmPrefsFont2;
            buttonList[2].objID    = frmPrefsFont3;
            buttonList[3].objID    = frmPrefsFont4;
            buttonList[4].objID    = frmPrefsFont5;
            buttonList[5].objID    = frmPrefsFontUser;
            userFontButtonNumber   = 5;
        }
        else
#endif
#ifdef HAVE_SONY_SDK
        if ( IsHiResTypeSony( HiResType())) {
            buttonList[0].fontMode = FONT_NARROW;
            buttonList[1].fontMode = FONT_TINY;
            buttonList[2].fontMode = FONT_SMALL;
            buttonList[3].fontMode = FONT_DEFAULT;
            buttonList[4].fontMode = FONT_LARGE;
            buttonList[5].fontMode = FONT_USER;
            buttonList[0].fontID   = narrowFont;
            buttonList[1].fontID   = hrTinyFont;
            buttonList[2].fontID   = hrSmallFont;
            buttonList[3].fontID   = hrStdFont;
            buttonList[4].fontID   = hrLargeFont;
            buttonList[5].fontID   = userStdFont_sony;
            buttonList[0].objID    = frmPrefsFont1;
            buttonList[1].objID    = frmPrefsFont2;
            buttonList[2].objID    = frmPrefsFont3;
            buttonList[3].objID    = frmPrefsFont4;
            buttonList[4].objID    = frmPrefsFont5;
            buttonList[5].objID    = frmPrefsFontUser;
            userFontButtonNumber   = 5;
        }
        else
#endif
        {
            buttonList[0].fontMode = FONT_NARROW;
            buttonList[1].fontMode = FONT_DEFAULT;
            buttonList[2].fontMode = FONT_BOLD;
            buttonList[3].fontMode = FONT_LARGE;
            buttonList[4].fontMode = FONT_LARGEBOLD;
            buttonList[5].fontMode = FONT_USER;
            buttonList[0].fontID   = narrowFont;
            buttonList[1].fontID   = stdFont;
            buttonList[2].fontID   = boldFont;
            buttonList[3].fontID   = largeFont;
            buttonList[4].fontID   = largeBoldFont;
            buttonList[5].fontID   = userStdFont;
            buttonList[0].objID    = frmPrefsFont1;
            buttonList[1].objID    = frmPrefsFont2;
            buttonList[2].objID    = frmPrefsFont3;
            buttonList[3].objID    = frmPrefsFont4;
            buttonList[4].objID    = frmPrefsFont5;
            buttonList[5].objID    = frmPrefsFontUser;
            userFontButtonNumber   = 5;
        }
    }
    else {
        buttonList[0].fontMode = FONT_DEFAULT;
        buttonList[1].fontMode = FONT_BOLD;
        buttonList[0].fontID   = stdFont;
        buttonList[1].fontID   = boldFont;
        buttonList[0].objID    = frmPrefsFont1;
        buttonList[1].objID    = frmPrefsFont2;
        userFontButtonNumber   = NO_USER_FONT_BUTTON;
    }

#ifdef SUPPORT_VFS_FONTS
    ScanVFSFonts();
#endif

    if ( userFontButtonNumber != NO_USER_FONT_BUTTON ) {
        InitializeUserFontList( frmPrefsFontUserFontList );
        oldUserFontNumber = GetCurrentUserFontNumber();
    }
}



/* Nominate the Font section to be shown first when the prefsform loads */
void PrefsFontShowFirst( void )
{
    showFirst = true;
}



/* Event handler for the Font preferences */
Boolean PrefsFontPalmEvent
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
                    case frmPrefsFontUserFontPopup:
                        if ( userFontButtonNumber != NO_USER_FONT_BUTTON ) {
                            selected = userFontButtonNumber;
                            if ( controlMode == MODE1 ) {
                                mainUserFontSelection       = selection;
                            }
                            else {
                                libraryUserFontSelection    = selection;
                            }
                            DrawButtons( true );
                        }
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
                case frmPrefsFontMainMode:
                    /* We can only access this form when coming here from the library
                       iff the "vary with documents" setting is unchecked */
                    if ( cameFromLibrary && Prefs()->individualDocumentFonts ) {
                        CtlSetValue( GetObjectPtr( frmPrefsFontMainMode ), false );
                        CtlSetValue( GetObjectPtr( frmPrefsFontLibraryMode ), true );
                    }
                    else {
                        controlMode = MODE1;
                        if ( controlMode != prevControlMode ) {
                            /* We need to hide then redraw our current section because
                               HandleControlPrefs() has the code to display the proper
                               objects to match our recently updated controlMode */
                            CtlSetValue( buttonList[ selected ].control, false );
                            PrefsHideSection( objectList, prevControlMode );
                            PrefsOpenSection( GetCurrentSection() );
                        }
                    }
                    handled = true;
                    break;

                case frmPrefsFontLibraryMode:
                    controlMode = MODE2;
                    if ( controlMode != prevControlMode ) {
                        /* We need to hide then redraw our current section because
                           HandleControlPrefs() has the code to display the proper
                           objects to match our recently updated controlMode */
                        CtlSetValue( buttonList[ selected ].control, false );
                        PrefsHideSection( objectList, prevControlMode );
                        PrefsOpenSection( GetCurrentSection() );
                    }
                    handled = true;
                    break;

                case frmPrefsFontIndividual:
                    if ( ! CtlGetValue( GetObjectPtr( frmPrefsFontIndividual ))) {
                        HidePrefsFormObject( frmPrefsFontAsDefault, true );
                    }
                    else {
                        ShowPrefsFormObject( frmPrefsFontAsDefault, true );
                    }
                    handled = true;
                    break;

                case frmPrefsFontAsDefault:
                    handled = true;
                    break;
        
                case frmPrefsFont1:
                case frmPrefsFont2:
                case frmPrefsFont3:
                case frmPrefsFont4:
                case frmPrefsFont5:
                case frmPrefsFontUser:
                {
                    UInt16  i;

                    for ( i = 0; i < buttonCount; i++ ) {
                        if ( buttonList[ i ].objID ==
                             event->data.ctlEnter.controlID ) {
                            selected = i;
                            fontModePref[ controlMode ] = buttonList[ i ].fontMode;
                            break;
                        }
                    }
                    handled = true;
                    break;
                }

                default:
                    break;
            }
            break;

        default:
            handled = false;
    }

    return handled;
}

