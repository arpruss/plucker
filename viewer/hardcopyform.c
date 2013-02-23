/*
 * $Id: hardcopyform.c,v 1.23 2004/06/26 18:12:05 nordstrom Exp $
 *
 * Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 1998-2001, Mark Ian Lillywhite and Michael Nordstrom
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

#include <stdarg.h>

#include "debug.h"
#include "document.h"
#include "fullscreenform.h"
#include "hires.h"
#include "util.h"
#include "DIA.h"

#include "hardcopyform.h"


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void HardcopyImmediately() HARDCOPYFORM_SECTION;
static void HardcopyFormInit(void) HARDCOPYFORM_SECTION;
#ifdef HAVE_ROT13
static void DoRot13( void ) HARDCOPYFORM_SECTION;
#endif


/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static ScrollBarType*      scrollBar;
static FieldType*          messageField;



static void HardcopyFormInit( void )
{
    FormType*   hardcopyForm;
    FieldType*  field;
    Char*       msg;

    hardcopyForm = FrmGetFormPtr( frmHardcopy );
    messageField = GetObjectPtr( frmHardcopyField );
    scrollBar    = GetObjectPtr( frmHardcopyScrollBar );
    if ( Prefs()->scrollbar == SCROLLBAR_LEFT ) {
        SetObjectPosition( hardcopyForm, frmHardcopyField, false );
        SetObjectPosition( hardcopyForm, frmHardcopyScrollBar, true );
    }
    field = GetObjectPtr( frmHardcopyField );

    FrmDrawForm( hardcopyForm );

    msg = SafeMemPtrNew(3000);
    CopyRecord( msg, 3000 );
    InsertText( field, msg );
    SafeMemPtrFree( msg );

    UpdateFieldScrollbar( messageField, scrollBar );
}



#ifdef HAVE_ROT13
/* FIXME: This pretty much assumes single-byte ascii text -- this
 * assumption is, in general, false(?)  Since the conversion is
 * done in place and the string is always terminated with a NULL,
 * this will just generate gobbledygook on real multi-byte unicode
 * (rather than stomp over the stack).
 * Maybe.  I hope.  Should really fix this.
 *
 * All of the 'extra' work here is a hegde against my ignorance of what
 * the palmos might do to the field behind my back... See warning in 
 * palmos docs describing FldGetTextHandle() which directs me to follow
 * this procedure.
 */
static void DoRot13( void )
{
    FieldType*  field;
    MemHandle   textH;
    Char*       word;
    UInt16      fieldScrPos;

    field       = GetObjectPtr( frmHardcopyField );
    fieldScrPos = FldGetScrollPosition( field );

    /* manipulate the text */
    textH   = FldGetTextHandle( field );
    FldSetTextHandle( field, NULL );
    word    = MemHandleLock( textH );
    while( *word != NULL) {
        if( 'a' <= *word && *word <= 'z' )  {
            *word = 'a' + ( *word - 'a' + 13 ) % 26;
        }
        else if( 'A' <= *word && *word <= 'Z' )  {
            *word = 'A' + ( *word - 'A' + 13 ) % 26;
        }
        word++;
    }
    MemHandleUnlock( textH );
    FldSetTextHandle( field, textH );

    FldSetScrollPosition( field, fieldScrPos );
    FldDrawField( field );

    UpdateFieldScrollbar( field, GetObjectPtr( frmHardcopyScrollBar ));
}
#endif



Boolean HardcopyFormHandleEvent
    (
    EventType* event  /* pointer to an EventType structure */
    )
{
    Boolean handled;

    handled = false;
    
    switch ( event->eType ) {
        case winEnterEvent:
            handled = ResizeHandleWinEnterEvent();
            break;

        case winDisplayChangedEvent:
            handled = ResizeHandleWinDisplayChangedEvent();
            break;

        case winExitEvent:
            handled = ResizeHandleWinExitEvent();
            break;

        case frmOpenEvent:
#ifdef HAVE_SILKSCREEN
            ResizeHandleFrmOpenEvent();
#endif
            CheckMem(warnLowHardcopyMem);
            HardcopyFormInit();
            handled = true;
            break;

        case keyDownEvent:
            switch ( event->data.keyDown.chr ) {
                case pageUpChr:
                    if ( FldScrollable( messageField, winUp ) )
                        FldScrollField( messageField, 1, winUp );
                    UpdateFieldScrollbar( messageField, scrollBar );
                    break;
        
                case pageDownChr:
                    if ( FldScrollable( messageField, winDown ) )
                        FldScrollField( messageField, 1, winDown );
                    UpdateFieldScrollbar( messageField, scrollBar );
                    break;
        
                default:
                    break;
            }
            break;

        case sclRepeatEvent:
            ScrollMessage( messageField, event->data.sclRepeat.newValue -
                                         event->data.sclRepeat.value );
            break;

    case ctlSelectEvent:
        switch ( event->data.ctlSelect.controlID ){
            case frmHardcopyCancel:
                if ( IsFormMain( Prefs()->lastForm ) )
                    FrmUpdateForm( GetMainFormId(), frmUpdateAnchors );
                FrmReturnToForm( Prefs()->lastForm );
                handled = true;
                break;

            case frmHardcopyExport:
            {
                FieldType *field;
                UInt16 index;
                UInt32 offset;
                Char*   word;

                field = GetObjectPtr( frmHardcopyField );
                word = FldGetTextPtr(field);
                index = dmMaxRecordIndex;
                offset = 0;
                WriteMemoEntry( &index, &offset, word );
                if ( IsFormMain( Prefs()->lastForm ) )
                    FrmUpdateForm( GetMainFormId(), frmUpdateAnchors );
                FrmReturnToForm( Prefs()->lastForm );
                handled = true;
                break;
            }

#ifdef HAVE_ROT13
            case frmHardcopyRot13:
                DoRot13();
                handled = true;
                break;
#endif

            default:
                handled = true;
                break;
        }
        break;

        case frmCloseEvent:
#ifdef HAVE_SILKSCREEN
            ResizeHandleFrmCloseEvent();
#endif
            handled = false;
            break;

        default:
            handled = false;
    }
    return handled;
}



static void HardcopyImmediately( void )
{
    Char*     msg;
    UInt32    offset;
    UInt16    index;

    msg = SafeMemPtrNew(2900);
    CopyRecord( msg, 2900 );
    index = dmMaxRecordIndex;
    offset = 0;
    WriteMemoEntry( &index, &offset, msg );
    SafeMemPtrFree( msg );
}



void DoHardcopy( void )
{
    if ( ! IsFullscreenformActive() ) {
        if ( Prefs()->hardcopyAction == HARDCOPY_DIALOG ) {
            FrmPopupForm( frmHardcopy );
        }
        else { /*HARDCOPY_IMMEDIATE*/
            HardcopyImmediately();
        }
    }
}

