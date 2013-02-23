/*
 * $Id: jogdial.c,v 1.25 2007/02/15 15:16:43 prussar Exp $
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

#include "control.h"
#include "prefsfont.h"
#include "libraryform.h"
#include "prefsdata.h"
#include "prefsform.h"
#include "resourceids.h"
#include "util.h"
#include "rotate.h"
#include "libraryform.h"

#include "jogdial.h"


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static Boolean SonyJogdialSupported( Jogdial* jogType ) JOGDIAL_SECTION;
static Boolean HanderaJogdialSupported( void ) JOGDIAL_SECTION;
static Boolean HandspringJogdialSupported( void ) JOGDIAL_SECTION;


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static Jogdial jogdialType  = unknownJogdial;


/* return what type of jogdial support we have */
Jogdial JogdialType( void )
{
    if ( jogdialType != unknownJogdial )
        return jogdialType;

    /* jogdialType will be defined if SonyJogdialSupported is true */
    if ( ! SonyJogdialSupported( &jogdialType) ) {
        if ( HanderaJogdialSupported() )
            jogdialType = handeraJogdial;
        else {
            if ( HandspringJogdialSupported() )
                jogdialType = handspringJogdial;
            else
                jogdialType = noJogdial;
      }
    }

    return jogdialType;
}



#ifdef HAVE_SONY_SDK
/* Because JogAssist is inconvenient at best, we need to check
   if its enabled, back up the current mask and set our own to
   block any JogAssist-ing while plucker is running. */
void HandleJogAssistMask
    (
    Boolean block
    )
{
    static UInt16      pluckerMask[2] = { sonyJogAstMaskNone,
                                          sonyJogAstMaskNone
                                        };
    static UInt16*     backupMask       = NULL;
    static UInt16      backupCardNo     = 0;
    static LocalID     backupLocalID    = 0;

    UInt16**           maskPtr;
    UInt16*            cardNoPtr;
    LocalID*           localIDPtr;
    SonySysFtrSysInfoP sonySysInfo;

    /* Check to see if we're actually on a sony device with JogAssist enabled */
    FtrGet( sonySysFtrCreator,
            sonySysFtrNumSysInfoP,
            (UInt32 *) &sonySysInfo );

    if ( ! ( sonySysInfo && sonySysInfo->extn & sonySysFtrSysInfoExtnJogAst ) )
        return;

    /* Grab the pointer to modify the JogAssist mask */
    FtrGet( sonySysFtrCreator,
            sonySysFtrNumJogAstMaskP,
            (UInt32*) &maskPtr );

    if ( ! maskPtr )
        return;

    /* Grab what the JogAssist Mask thinks the current
       cardNo and localID are set to. */
    FtrGet( sonySysFtrCreator, 
            sonySysFtrNumJogAstMOCardNoP,
            (UInt32*) &cardNoPtr );
    FtrGet( sonySysFtrCreator,
            sonySysFtrNumJogAstMODbIDP,
            (UInt32*) &localIDPtr );

    if ( block ) {
        /* Allocate our mask in memory and set it to block everything.
           This really is a conservative way of doing things, later on
           once/if a decision is made to allow specific portions of
           JogAssist through to plucker, this is where it would be
           changed. Perhaps this can be user-definable, but I think
           that that would be too confusing to the average user, imo. */
        pluckerMask[0] = sonyJogAstMaskType2;
        pluckerMask[1] = sonyJogAstMaskUp | sonyJogAstMaskDown |
                         sonyJogAstMaskPushedUp | sonyJogAstMaskPushedDown |
                         sonyJogAstMaskPush | sonyJogAstMaskRelease |
                         sonyJogAstMaskPushRepeat | sonyJogAstMaskBack;

        /* backup the current mask */
        backupMask = *maskPtr;

        /* set plucker's mask */
        *maskPtr = pluckerMask;

        if ( cardNoPtr != NULL && localIDPtr != NULL ) {
            /* backup the current cardNo and localID */
            backupCardNo    = *cardNoPtr;
            backupLocalID   = *localIDPtr;

            /* set to plucker's cardNo and localID */
            SysCurAppDatabase( cardNoPtr, localIDPtr );
        }
    }
    else if ( pluckerMask[ 0 ] != sonyJogAstMaskNone ) {
        /* restore the backed-up mask */
        *maskPtr = backupMask;

        /* clear our mask from memory */
        pluckerMask[0] = sonyJogAstMaskNone;
        pluckerMask[1] = sonyJogAstMaskNone;

        if ( cardNoPtr != NULL && localIDPtr != NULL ) {
            /* restore the backed-up cardNo and localID */
            *cardNoPtr  = backupCardNo;
            *localIDPtr = backupLocalID;
        }
    }
}



/* A bit of a hack to do this here, but there is no other way to get back to
   the native FrmHelp() call without conflicting with the FrmHelp re-definition
   in jogdial.h */
extern void FrmHelpSysTrap( UInt16 helpMsgID ) SYS_TRAP( sysTrapFrmHelp );



/* Re-enable JogAssist but only for the duration of a FrmHelp() call */
void JogAssistFrmHelp
    (
    UInt16 helpMsgID
    )
{
    HandleJogAssistMask( false );
    FrmHelpSysTrap( helpMsgID );
    HandleJogAssistMask( true );
}
#endif



/* Identify if we support sony jogdial */
static Boolean SonyJogdialSupported
    (
    Jogdial* jogType
    )
{
#ifdef HAVE_SONY_SDK
    Err                err;
    SonySysFtrSysInfoP sonySysFtrSysInfoP;

    err = FtrGet ( sonySysFtrCreator,
                   sonySysFtrNumSysInfoP,
                   (UInt32 *) &sonySysFtrSysInfoP );

    /* If we're on a sony system, and have access to the HRLib.. */
    if ( err == errNone ) {
        if ( sonySysFtrSysInfoP->jogType & sonySysFtrSysInfoJogType2 )
            *jogType = sonyJogdialType2;
        else if ( sonySysFtrSysInfoP->jogType & sonySysFtrSysInfoJogType1 )
            *jogType = sonyJogdialType1;
        else if ( sonySysFtrSysInfoP->jogType & sonySysFtrSysInfoJogTypeNone )
            *jogType = noJogdial;
        else
            /* The Sony S320 doesn't properly respond to the
               sonySysFtrSysInfoP->jogType variable so if none
               of the previous statements results in anything,
               assume sonySysFtrSysInfoJogType1 is valid */
            *jogType = sonyJogdialType1;

        return true;
    }
#endif
    return false;
}



/* Identify if we support handera jogdial */
static Boolean HanderaJogdialSupported( void )
{
#ifdef HAVE_HANDERA_SDK
    Err    err;
    UInt32 vgaVersion;

    /* Yes, this is the same method we used to find out if we had
       handera hires support. We have to make the assumetion that
       if they have hires support, they must have jogdial support.
       Bad I know, but there is no other specific function to check
       only for jogdial availability. */
    err = FtrGet ( TRGSysFtrID, TRGVgaFtrNum, &vgaVersion );

    /* Do something with vgaVersion? */

    if ( err == errNone )
        return true;
#endif
    return false;
}


/* Identify if we support handspring jogdial */
static Boolean HandspringJogdialSupported( void )
{
#ifdef HAVE_PALMONE_SDK 
    Err    err;
    UInt32 value;

    err = FtrGet ( hsFtrCreator, hsFtrIDTypeOfKeyboard, &value );

    if ( ( err == errNone ) && ( value & hsFtrValKeyboardJog ))

        return true;
#endif
    return false;
}


/* Handler to process any Jogdial requests */
Boolean JogdialLibraryHandler
    (
    EventType *event  /* pointer to an EventType structure */
    )
{
    static  Boolean jogPushed = false;
    static  Boolean jogRepeat = false;

    Boolean         handled;
    WChar           chr;
    Boolean         scroll;
    Int16           firstVisibleRow;
    Int16           currentRow;

    handled         = true;
    chr             = event->data.keyDown.chr;
    scroll          = false;
    firstVisibleRow = LibraryGetFirstVisibleRow();
    currentRow      = SelectorGetRow();

    if ( IsJogdialRelease( chr ) ) {
        if ( currentRow != NO_ROW ) {
            if ( ! jogRepeat && ! jogPushed ) {
                return LibrarySelectorHandler( librarySelectorGo, 0 );
            }
            else if ( jogRepeat && jogPushed ) {
                LibrarySetLastIndexForRow( currentRow );
                IconPopupList( currentRow - firstVisibleRow );
            }
        } else if ( ! jogRepeat && ! jogPushed ) {
            SelectNextCategory();
        }
        currentRow = NO_ROW;
        jogPushed = false;
        jogRepeat = false;

    } else if ( IsJogdialBack( chr ) ) {
        LibrarySelectorHandler( librarySelectorLeft, 0 );
    } else {
        if ( LibraryGetNumberOfRows() == 0 ) {
            handled = false;
            return handled;
        }

        if ( IsJogdialUp( chr ) ) {
            if ( currentRow == NO_ROW ) {
                LibrarySelectorHandler( librarySelectorInit, 0 );
                return handled;
            }
            if ( 0 < currentRow ) {
                return LibrarySelectorHandler( librarySelectorUp, 0 );
            } else {
                if ( jogdialType == handspringJogdial ) {
                    HighlightRectangle(
                        LibraryGetDisplayListBounds( currentRow ), 0, false,
                        TEXT);
                    SelectorSetRow( NO_ROW );
                }
                return handled;
            }
        }
        else if ( IsJogdialDown( chr ) ) {
            if ( currentRow == NO_ROW ) {
                LibrarySelectorHandler( librarySelectorInit, 0 );
                return handled;
            }
            if ( currentRow < LibraryGetNumberOfRows() - 1) {
                return LibrarySelectorHandler( librarySelectorDown, 0 );
            }
            else {
                if ( jogdialType == handspringJogdial ) {
                    HighlightRectangle(
                        LibraryGetDisplayListBounds( currentRow ), 0, false,
                        TEXT);
                    SelectorSetRow( NO_ROW );
                }
                return handled;
            }
        }
        else if ( IsJogdialPushRepeat( chr ) ) {
            jogPushed = true;
            jogRepeat = true;
        }
        else if ( IsJogdialPushUp( chr ) ) {
            jogPushed = true;
            jogRepeat = false;
            return LibrarySelectorHandler( librarySelectorPageUp, 0 );
        }
        else if ( IsJogdialPushDown( chr ) ) {
            jogPushed = true;
            jogRepeat = false;
            return LibrarySelectorHandler( librarySelectorPageDown, 0 );
        }
        else if ( IsJogdialPush( chr ) ) {
            handled = true;
        }
        else {
            handled = false;
        }
    }
    return handled;
}


/* Handler to process any Jogdial requests */
Boolean JogdialMainHandler
    (
    EventType *event
    )
{
    Boolean        handled   = false;
    static Boolean jogPushed = false;
    WChar          chr;

    if ( ! Prefs()->jogEnabled )
        return handled;

    handled = true;
    chr = event->data.keyDown.chr;
    if ( RotIsJogdialUp( chr ) &&
         Prefs()->jogMode[ JOGEVENTS_UP ] != SELECT_NONE ) {
        DoSelectTypeAction( Prefs()->jogMode[ JOGEVENTS_UP ] );
    }
    else if ( RotIsJogdialDown( chr ) &&
              Prefs()->jogMode[ JOGEVENTS_DOWN ] != SELECT_NONE ) {
        DoSelectTypeAction( Prefs()->jogMode[ JOGEVENTS_DOWN ] );
    }
    else if ( RotIsJogdialPushUp( chr ) &&
              Prefs()->jogMode[ JOGEVENTS_PUSHUP ] != SELECT_NONE ) {
        DoSelectTypeAction( Prefs()->jogMode[ JOGEVENTS_PUSHUP ] );
        jogPushed = true;
    }
    else if ( RotIsJogdialPushDown( chr ) &&
              Prefs()->jogMode[ JOGEVENTS_PUSHDOWN ] != SELECT_NONE ) {
        DoSelectTypeAction( Prefs()->jogMode[ JOGEVENTS_PUSHDOWN ] );
        jogPushed = true;
    }
    else if ( IsJogdialRelease( chr ) &&
              Prefs()->jogMode[ JOGEVENTS_PUSH ] != SELECT_NONE ) {
        if ( ! jogPushed )
            DoSelectTypeAction( Prefs()->jogMode[ JOGEVENTS_PUSH ] );
        jogPushed = false;
    }
    else if ( IsJogdialBack( chr ) &&
              Prefs()->jogMode[ JOGEVENTS_BACK ] != SELECT_NONE ) {
        DoSelectTypeAction( Prefs()->jogMode[ JOGEVENTS_BACK ] );
    }
    else {
        handled = false;
    }

    return handled;
}



/* Handler to process any Preference Form Jogdial requests */
Boolean JogdialPrefHandler
    (
    EventType* event  /* pointer to an EventType structure */
    )
{
    Boolean handled;
    WChar   chr;

    if ( event->eType != keyDownEvent )
        return false;

    chr     = event->data.keyDown.chr;
    handled = true;

    if ( IsJogdialUp( chr ) )
        PrefsPreviousSection();
    else if ( IsJogdialDown( chr ) )
        PrefsNextSection();
    else if ( IsJogdialRelease( chr ) )
        CtlHitControl( GetObjectPtr( frmPrefsOK ) );
    else if ( IsJogdialBack( chr ) )
        CtlHitControl( GetObjectPtr( frmPrefsCancel ) );
    else
        handled = false;

    return handled;
}



