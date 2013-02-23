/*
 * $Id: prefsdata.c,v 1.97 2006/11/12 16:23:44 prussar Exp $
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

#include "const.h"
#include "debug.h"
#include "genericfile.h"
#include "jogdial.h"
#include "libraryform.h"
#include "os.h"
#include "util.h"
#include "DIA.h"

#include "prefsdata.h"


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void RemovePref( UInt16 prefID ) PREFSDATA_SECTION;
static void ReadSearchPatterns( void ) PREFSDATA_SECTION;
static void WriteSearchPatterns( void ) PREFSDATA_SECTION;


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static Preferences  pref;
static Char*        patterns;
static UInt16       patternsSize;



/* Remove preferences from the Preferences database */
static void RemovePref
    (
    UInt16 prefID   /* preference to remove */
    )
{
    DmOpenRef           ref;
    DmSearchStateType   state;
    LocalID             dbID;
    UInt16              cardNo;
    Int16               index;

    DmGetNextDatabaseByTypeCreator( true, &state, 'sprf', 'psys', false,
        &cardNo, &dbID );
    ref = DmOpenDatabase( cardNo, dbID, dmModeReadWrite );
    if ( ref == NULL )
        return;

    index = DmFindResource( ref, ViewerAppID, prefID, NULL );
    if ( index != -1 )
        DmRemoveResource( ref, index );

    DmCloseDatabase( ref );
}



/* Retrieve preferences from the Preferences database */
void ReadPrefs( void )
{
    Int16   version;
    Int16   oldVersion;
    UInt16  prefSize;
    UInt16  oldPrefSize;

    ASSERT( ViewerPrefID != ViewerPrefSearchStringID );

    /* if not a test version remove preferences with ID 0 */
    if ( ViewerPrefID != 0 )
        RemovePref( 0 );

    MemSet( &pref, sizeof( Preferences ), 0 );

    /* if no preference found or the found preference has a "bad" size
       and there is no preference using an old (incompatible) format,
       then initialize a new preference structure */
    prefSize    = 0;
    version     = PrefGetAppPreferences( (UInt32) ViewerAppID,
                    (UInt16) ViewerPrefID, NULL, &prefSize, true);
    if ( version == noPreferenceFound || sizeof( Preferences ) < prefSize ) {
        MSG( _( "Version = %d, prefSize = %u\n", version, prefSize ) );
        oldPrefSize = 0;
        oldVersion  = PrefGetAppPreferences( (UInt32) ViewerAppID,
                        (UInt16) ViewerOldPrefID, NULL, &oldPrefSize, true);
        if ( oldVersion == noPreferenceFound ) {
            MSG( "No old preferences found\n" );

            pref.column.date                = SHOW;
            pref.column.size                = SHOW;
            pref.column.type                = SHOW;
            pref.strikethrough              = false;
            pref.defaultStrikethrough       = false;
            pref.multipleSelect             = true;
            pref.hardKeys                   = false;
            pref.arrowKeys                  = false;
            pref.searchFlags                = 0;
            pref.toolbar                    = TOOLBAR_TOP;
            pref.lastForm                   = frmLibrary;
            pref.scrollbar                  = SCROLLBAR_RIGHT;
            pref.controlMode                = MODE1;
            pref.screenDepth                = 0;
            pref.searchEntries              = 0;
            pref.categories                 = dmAllCategoriesAdvanced;
            pref.cardNo                     = 0;
            pref.toolbarButton              = 0;
            pref.filterMode                 = FILTER_OR;
            pref.fontModeMain               = FONT_DEFAULT;
            pref.underlineMode              = false;
            pref.defaultUnderlineMode       = false;
            pref.sortType                   = SORT_NAME;
            pref.sortOrder                  = SORTORDER_ASC;
            pref.autoscrollEnabled          = false;
            pref.autoscrollInterval         = 0;
            pref.autoscrollLastScrollTime   = 0;
            pref.autoscrollJump             = 1;
            pref.autoscrollMode             = AUTOSCROLL_PIXELS;
            pref.autoscrollDir              = AUTOSCROLL_DOWN;
            pref.autoscrollStayOn           = false;
            pref.location                   = RAM;
            pref.gestures                   = true;
            pref.gestMode[ GESTURES_UP ]    = SELECT_GO_TO_TOP;
            pref.gestMode[ GESTURES_RIGHT ] = SELECT_GO_FORWARD;
            pref.gestMode[ GESTURES_DOWN ]  = SELECT_GO_TO_BOTTOM;
            pref.gestMode[ GESTURES_LEFT ]  = SELECT_GO_BACK;
            pref.gestMode[ GESTURES_TAP ]   = SELECT_GO_HOME;
            pref.syncPolicy                 = SYNC_AUTOMATIC;
            pref.fontModeLibrary            = FONT_DEFAULT;
            pref.categoryStyle              = CATEGORY_ADVANCED;
            pref.hardcopyAction             = HARDCOPY_DIALOG;
            pref.hardcopyRange              = HARDCOPY_INVIEW;
            pref.hardcopyLink               = HARDCOPY_ATBOTTOM;
            pref.lineSpacing                = -MINIMAL_LINE_SPACING;
            pref.paragraphSpacing           = DEFAULT_PARAGRAPH_SPACING;
            pref.searchMode                 = SEARCH_IN_ONE_PAGE;
            pref.enableSoftHyphens          = true;
            pref.defaultEnableSoftHyphens   = true;
            pref.indicateOpened             = false;
            pref.individualDocumentLayout   = false;
            pref.individualDocumentFonts    = false;
            pref.individualDocumentColor    = false;
            pref.individualDocumentLookup   = false;
            pref.forceDefaultColors         = false;
            pref.defaultForceDefaultColors  = false;
            pref.savedToolbar               = TOOLBAR_TOP;
            pref.savedScrollbar             = SCROLLBAR_RIGHT;
            pref.savedSilkscreen            = DIA_STATE_MIN;
            pref.pageControlsLink           = false;
            pref.defaultPageControlsLink    = false;
            pref.forceAlign                 = FORCE_ALIGN_NONE;
            pref.defaultForceAlign          = FORCE_ALIGN_NONE;
            pref.joinUpAllRecords           = false;
            pref.rotate                     = ROTATE_ZERO;
            pref.defaultRotate              = ROTATE_ZERO;
            pref.defaultLineSpacing         = pref.lineSpacing;
            pref.defaultParagraphSpacing    = pref.paragraphSpacing;
            pref.defaultFontModeMain        = pref.fontModeMain;
            pref.selectedWordAction         = SELECT_WORD_SEARCH_FORM;
            pref.defaultSelectedWordAction  = SELECT_WORD_SEARCH_FORM;
            pref.selectWordTap              = SELECT_WORD_TAP_NONE;
            pref.defaultSelectWordTap       = SELECT_WORD_TAP_NONE;
            pref.useDateTime                = false;
            pref.searchXlit                 = 0;
            pref.rtl                        = false;
            pref.defaultRTL                 = false;
            pref.enableMargins              = true;
            pref.defaultEnableMargins       = true;
            pref.noAnnotationsInBookmarkList  = false;
            pref.alwaysDeleteAnnotations      = false;

            StrCopy( pref.docName, "" );
            StrCopy( pref.mainUserFontName, "" );
            StrCopy( pref.libraryUserFontName, "" );
            StrCopy( pref.defaultMainUserFontName, pref.mainUserFontName );

            MemSet( &pref.select, sizeof( pref.select ), SELECT_NONE );
            MemSet( &pref.hwMode, sizeof( pref.hwMode ), SELECT_NONE );

            pref.arrowKeys                         = true;
            pref.arrowMode[ UP_ARROW ]             = SELECT_FULL_PAGE_UP;
            pref.arrowMode[ DOWN_ARROW ]           = SELECT_FULL_PAGE_DOWN;
            pref.arrowMode[ LEFT_ARROW ]           = SELECT_GO_BACK;
            pref.arrowMode[ RIGHT_ARROW ]          = SELECT_GO_FORWARD;
            pref.arrowMode[ SELECT_ARROW ]         = SELECT_GO_HOME;

            MemSet( &pref.jogMode, sizeof( pref.jogMode ), SELECT_NONE );

            if ( JogdialType() != noJogdial ) {
                pref.jogEnabled                    = true;
                pref.jogMode[ JOGEVENTS_UP ]       = SELECT_HALF_PAGE_UP;
                pref.jogMode[ JOGEVENTS_DOWN ]     = SELECT_HALF_PAGE_DOWN;
                pref.jogMode[ JOGEVENTS_PUSH ]     = SELECT_GO_FORWARD;
                pref.jogMode[ JOGEVENTS_PUSHUP ]   = SELECT_FULL_PAGE_UP;
                pref.jogMode[ JOGEVENTS_PUSHDOWN ] = SELECT_FULL_PAGE_DOWN;
                pref.jogMode[ JOGEVENTS_BACK ]     = SELECT_GO_BACK;
            }

            RemovePref( ViewerPrefSearchStringID );
            RemovePref( ViewerPrefID );
        }
        else {
            /* we found a preference structure using an old (incompatible) 
               format; convert it to the new format */
            OldPreferences old;

            MemSet( &old, sizeof( OldPreferences ), 0 );

            oldVersion = PrefGetAppPreferences( (UInt32) ViewerAppID,
                            (UInt16) ViewerOldPrefID, &old, &oldPrefSize,
                            true);
            MSG( _( "Old version = %d, oldPrefSize = %u\n", oldVersion,
                    oldPrefSize ) );

            /* if we found an older version of the old preference
               format we add default values for the new items */
            if ( oldVersion < 2 ) {
                old.column.type                = SHOW;
                old.fontModeLibrary            = FONT_DEFAULT;
                old.categoryStyle              = CATEGORY_ADVANCED;
                old.hardcopyAction             = HARDCOPY_DIALOG;
                old.hardcopyRange              = HARDCOPY_INVIEW;
                old.hardcopyLink               = HARDCOPY_ATBOTTOM;
                old.searchMode                 = SEARCH_IN_ONE_PAGE;
                old.enableSoftHyphens          = true;
                old.lineSpacing                = -MINIMAL_LINE_SPACING;
                old.paragraphSpacing           = DEFAULT_PARAGRAPH_SPACING;

                MemSet( &old.jogMode, sizeof( old.jogMode ), SELECT_NONE );

                if ( JogdialType() != noJogdial ) {
                    old.jogEnabled = true;
                    old.jogMode[ JOGEVENTS_UP ]       = SELECT_HALF_PAGE_UP;
                    old.jogMode[ JOGEVENTS_DOWN ]     = SELECT_HALF_PAGE_DOWN;
                    old.jogMode[ JOGEVENTS_PUSH ]     = SELECT_GO_FORWARD;
                    old.jogMode[ JOGEVENTS_PUSHUP ]   = SELECT_FULL_PAGE_UP;
                    old.jogMode[ JOGEVENTS_PUSHDOWN ] = SELECT_FULL_PAGE_DOWN;
                    old.jogMode[ JOGEVENTS_BACK ]     = SELECT_GO_BACK;
                }
            }
            if ( version < 3 ) {
                old.indicateOpened             = false;
                old.individualDocumentFonts    = false;
                old.forceDefaultColors         = false;
                old.savedToolbar               = TOOLBAR_TOP;
                old.savedScrollbar             = SCROLLBAR_RIGHT;
                old.savedSilkscreen            = DIA_STATE_MIN;
                old.pageControlsLink           = false;
                old.forceAlign                 = FORCE_ALIGN_NONE;
                old.joinUpAllRecords           = false;
                old.rotate                     = ROTATE_ZERO;
                old.defaultRotate              = ROTATE_ZERO;
                old.defaultLineSpacing         = old.lineSpacing;
                old.defaultParagraphSpacing    = old.paragraphSpacing;
                old.defaultFontModeMain        = old.fontModeMain;
                old.selectedWordAction         = SELECT_WORD_SEARCH_FORM;
                old.selectWordTap              = SELECT_WORD_TAP_NONE;
                old.useDateTime                = false;
                old.searchXlit                 = 0;

                StrCopy( old.mainUserFontName, "" );
                StrCopy( old.libraryUserFontName, "" );
                StrCopy( old.defaultMainUserFontName, old.mainUserFontName );
            }

            pref.column                         = old.column;
            pref.strikethrough                  = old.strikethrough;
            pref.multipleSelect                 = old.multipleSelect;
            pref.searchFlags                    = old.searchFlags;
            pref.toolbar                        = old.toolbar;
            pref.lastForm                       = old.lastForm;
            pref.scrollbar                      = old.scrollbar;
            pref.controlMode                    = old.controlMode;
            pref.screenDepth                    = old.screenDepth;
            pref.searchEntries                  = old.searchEntries;
            pref.categories                     = old.categories;
            pref.cardNo                         = old.cardNo;
            pref.toolbarButton                  = old.toolbarButton;
            pref.filterMode                     = old.filterMode;
            pref.fontModeMain                   = old.fontModeMain;
            pref.underlineMode                  = old.underlineMode;
            pref.sortType                       = old.sortType;
            pref.sortOrder                      = old.sortOrder;
            pref.autoscrollEnabled              = old.autoscrollEnabled;
            pref.autoscrollInterval             = old.autoscrollInterval;
            pref.autoscrollLastScrollTime       = old.autoscrollLastScrollTime;
            pref.autoscrollJump                 = old.autoscrollJump;
            pref.autoscrollMode                 = old.autoscrollMode;
            pref.autoscrollDir                  = old.autoscrollDir;
            pref.autoscrollStayOn               = old.autoscrollStayOn;
            pref.location                       = old.location;
            pref.syncPolicy                     = old.syncPolicy;
            pref.fontModeLibrary                = old.fontModeLibrary;
            pref.categoryStyle                  = old.categoryStyle;
            pref.hardcopyAction                 = old.hardcopyAction;
            pref.hardcopyRange                  = old.hardcopyRange;
            pref.hardcopyLink                   = old.hardcopyLink;
            pref.searchMode                     = old.searchMode;
            pref.enableSoftHyphens              = old.enableSoftHyphens;
            pref.lineSpacing                    = old.lineSpacing;
            pref.paragraphSpacing               = old.paragraphSpacing;
            pref.indicateOpened                 = old.indicateOpened;
            pref.individualDocumentFonts        = old.individualDocumentFonts;
            pref.forceDefaultColors             = old.forceDefaultColors;
            pref.savedToolbar                   = old.savedToolbar;
            pref.savedScrollbar                 = old.savedScrollbar;
            pref.savedSilkscreen                = old.savedSilkscreen;
            pref.pageControlsLink               = old.pageControlsLink;
            pref.forceAlign                     = old.forceAlign;
            pref.joinUpAllRecords               = old.joinUpAllRecords;
            pref.rotate                         = old.rotate;
            pref.defaultRotate                  = old.defaultRotate;
            pref.defaultLineSpacing             = old.defaultLineSpacing;
            pref.defaultParagraphSpacing        = old.defaultParagraphSpacing;
            pref.defaultFontModeMain            = old.defaultFontModeMain;
            pref.selectedWordAction             = old.selectedWordAction;
            pref.selectWordTap                  = old.selectWordTap;
            pref.useDateTime                    = old.useDateTime;
            pref.searchXlit                     = old.searchXlit;
            pref.hardKeys                       = old.hardKeys;
            pref.arrowKeys                      = old.arrowKeys;
            pref.gestures                       = old.gestures;
            pref.arrowKeys                      = old.arrowKeys;
            pref.jogEnabled                     = old.jogEnabled;

            StrCopy( pref.docName, old.docName );
            StrCopy( pref.mainUserFontName, old.mainUserFontName );
            StrCopy( pref.libraryUserFontName, old.libraryUserFontName );
            StrCopy( pref.defaultMainUserFontName, old.defaultMainUserFontName );

            if ( sizeof( pref.select ) == sizeof( old.select ) ) {
                MemMove( &pref.select, &old.select, sizeof( pref.select ) );
            }
            else {
                MemSet( &pref.select, sizeof( pref.select ), SELECT_NONE );
            }
            if ( sizeof( pref.jogMode ) == sizeof( old.jogMode ) ) {
                MemMove( &pref.jogMode, &old.select, sizeof( pref.jogMode ) );
            }
            else {
                MemSet( &pref.jogMode, sizeof( pref.jogMode ), SELECT_NONE );
            }
            if ( sizeof( pref.hwMode ) == sizeof( old.hwMode ) ) {
                MemMove( &pref.hwMode, &old.select, sizeof( pref.hwMode ) );
            }
            else {
                MemSet( &pref.hwMode, sizeof( pref.hwMode ), SELECT_NONE );
            }
            if ( sizeof( pref.gestMode ) == sizeof( old.gestMode ) ) {
                MemMove( &pref.gestMode, &old.select, sizeof( pref.gestMode ) );
            }
            else {
                MemSet( &pref.gestMode, sizeof( pref.gestMode ), SELECT_NONE );
            }
            if ( sizeof( pref.arrowMode ) == sizeof( old.arrowMode ) ) {
                MemMove( &pref.arrowMode, &old.select, sizeof( pref.arrowMode ) );
            }
            else {
                MemSet( &pref.arrowMode, sizeof( pref.arrowMode ), SELECT_NONE );
            }

            /* items not included in old format */
            pref.rtl                        = false;
            pref.defaultRTL                 = false;
            pref.defaultUnderlineMode       = false;
            pref.defaultEnableSoftHyphens   = true;
            pref.defaultPageControlsLink    = false;
            pref.defaultForceAlign          = FORCE_ALIGN_NONE;
            pref.defaultStrikethrough       = false;
            pref.defaultForceDefaultColors  = false;
            pref.defaultSelectWordTap       = SELECT_WORD_TAP_NONE;
            pref.defaultSelectedWordAction  = SELECT_WORD_SEARCH_FORM;
            pref.individualDocumentLayout   = false;
            pref.individualDocumentColor    = false;
            pref.individualDocumentLookup   = false;
            pref.enableMargins              = true;
            pref.defaultEnableMargins       = true;
            pref.noAnnotationsInBookmarkList  = false;
            pref.alwaysDeleteAnnotations      = false;

            RemovePref( ViewerOldPrefID );
        }
    }
    else {
        version = PrefGetAppPreferences( (UInt32) ViewerAppID,
                    (UInt16) ViewerPrefID, &pref, &prefSize, true);
        MSG( _( "Version = %d\n", version ) );
    }
}



/* Store preferences in the Preferences database */
void WritePrefs( void )
{
    PrefSetAppPreferences( (UInt32) ViewerAppID, (UInt16) ViewerPrefID,
        (Int16) ViewerVersion, &pref, sizeof( Preferences ), true );
}



/* Retrieve search patterns */
static void ReadSearchPatterns( void )
{
    patternsSize    = 0;

    if ( PrefGetAppPreferences( (UInt32) ViewerAppID,
            (UInt16) ViewerPrefSearchStringID, NULL, &patternsSize, true ) !=
         noPreferenceFound ) {
        Char* entry;
        Int16 i;
        Int16 len;
        SafeMemPtrFree( patterns );
        patterns = SafeMemPtrNew( patternsSize );
        PrefGetAppPreferences( (UInt32) ViewerAppID,
            (UInt16) ViewerPrefSearchStringID, patterns, &patternsSize, true );
        /* do a sanity check */
        entry = patterns;
        for ( i = 0 ; i < pref.searchEntries ; i++ ) {
            len = StrLen( entry );
            if ( MAX_PATTERN_LEN < len ||
                patterns + patternsSize < entry + len + 1
              ) {
                /* error in search patterns! */
                SafeMemPtrFree( patterns );
                pref.searchEntries = 0;
                break;
            }
            entry += len + 1;
        }
    }
}



/* Store search patterns */
static void WriteSearchPatterns( void )
{
    if ( patterns != NULL ) {
        PrefSetAppPreferences( (UInt32) ViewerAppID,
            (UInt16) ViewerPrefSearchStringID, (Int16) ViewerVersion, patterns,
            patternsSize, true );

        SafeMemPtrFree( patterns );
        patterns = NULL;
    }
}



/* Retrieve search string */
void GetSearchString
    (
    Char* string    /* string pointer to store result */
    )
{
    Int8    i;
    Char*   entry;

    MemSet( string, MAX_PATTERN_LEN + 1, 0 );

    ReadSearchPatterns();

    if ( patterns != NULL ) {
        entry = patterns;
        for ( i = 0; i < pref.searchEntries - 1; i++ )
            entry += StrLen( entry ) + 1;
        StrNCopy( string, entry, MAX_PATTERN_LEN );

        SafeMemPtrFree( patterns );
        patterns = NULL;
    }
    else {
        pref.searchEntries = 0;
        StrCopy( string, "" );
    }
}



/* Add new search string to patterns list */
void AddSearchString
    (
    Char* string    /* pattern string */
    )
{
    UInt8 item;
    UInt8 count;
    Char* temp1;
    Char* temp2;
    Char* newPatterns;

    ReadSearchPatterns();

    item        = 0;
    temp1       = NULL;
    temp2       = NULL;
    newPatterns = NULL;

    /* Add string to patterns list */
    if ( pref.searchEntries < 10 ) {
        patternsSize   += StrLen( string ) + 1;
        ErrTry {
            newPatterns     = SafeMemPtrNew( patternsSize );
            temp1           = patterns;
            temp2           = newPatterns;
            item            = 0;
        }
        ErrCatch( UNUSED_PARAM( err ) ) {
            return;
        } ErrEndCatch
    }
    /* Replace first string in patterns list with the new string */
    else {
        patternsSize   -= StrLen( patterns ) - StrLen( string );
        ErrTry {
            newPatterns     = SafeMemPtrNew( patternsSize );
            temp1           = patterns + StrLen( patterns ) + 1;
            temp2           = newPatterns;
            item            = 1;
        }
        ErrCatch( UNUSED_PARAM( err ) ) {
            return;
        } ErrEndCatch
    }

    count = 0;
    while ( item++ < pref.searchEntries ) {
        if ( ! STREQ( temp1, string ) ) {
            StrCopy( temp2, temp1 );
            temp2 += StrLen( temp2 ) + 1;
            count++;
        }
        else {
            patternsSize -= StrLen( string ) + 1;
        }

        temp1 += StrLen( temp1 ) + 1;
    }
    StrCopy( temp2, string );
    pref.searchEntries = count + 1;
    SafeMemPtrFree( patterns );
    patterns = newPatterns;

    WriteSearchPatterns();
}



/* Retrieve search patterns, call ReleaseSearchPatterns to release the memory
   allocated by this function */
Char* GetSearchPatterns( void )
{
    ReadSearchPatterns();
    return patterns;
}



/* Release memory allocated by GetSearchPatterns */
void ReleaseSearchPatterns( void )
{
    SafeMemPtrFree( patterns );
    patterns = NULL;
}



/* Retrieve the value of a preference ( through the pointer ) */
Preferences* Prefs( void )
{
    return &pref;
}
