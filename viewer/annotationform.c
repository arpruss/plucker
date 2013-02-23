/*
 * $Id: annotationform.c,v 1.9 2006/11/12 02:00:54 prussar Exp $
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
#include "metadocument.h"
#include "annotation.h"
#include "ramfile.h"
#include "DIA.h"

#include "annotationform.h"


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define MAX_FIELD   4000


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void AnnotationFormInit(void) ANNOTATIONFORM_SECTION;
static void AnnotationFormClose(void) ANNOTATIONFORM_SECTION;



/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static AnnotationEntry*     entryP = NULL;
static Char*                data = NULL;
static ScrollBarType*       scrollBar;
static FieldType*           field;


static void AnnotationFormInit( void )
{
    FormType*   annotationForm;

    annotationForm = FrmGetFormPtr( frmAnnotation );
    
    if ( ( entryP->flags & ANNOTATION_BOOKMARK ) &&
         entryP->id.indexInParagraph == NEW_ANNOTATION ) {
        SetFormTitle( annotationForm, strAddBookmarkTitle );
    }
    else {
        SetFormTitle( annotationForm, strAnnotationTitle );
    }

    scrollBar      = GetObjectPtr( frmAnnotationScrollBar );
    if ( Prefs()->scrollbar == SCROLLBAR_LEFT ) {
        SetObjectPosition( annotationForm, frmAnnotationField, false );
        SetObjectPosition( annotationForm, frmAnnotationScrollBar, true );
    }
    field = GetObjectPtr( frmAnnotationField );
    CtlSetUsable( GetObjectPtr( frmAnnotationDelete ),
        entryP->id.indexInParagraph != NEW_ANNOTATION );

    FrmDrawForm( annotationForm );
    InsertText( field, data );
    if ( entryP->id.indexInParagraph == NEW_ANNOTATION )
        FldSetSelection( field, 0, StrLen( data ) );
    else
        FldSetInsertionPoint( field, 0 );

    UpdateFieldScrollbar( field, scrollBar );

    FrmSetFocus( annotationForm, FrmGetObjectIndex( annotationForm,
                                     frmAnnotationField ) );
}



static void SaveAnnotation( void )
{
    FieldType* field;
    Char*      note;
    UInt16     length;

    field  = GetObjectPtr( frmAnnotationField );
    note   = FldGetTextPtr( field );
    if ( note == NULL ) {
        length = 0;
        data[ 0 ] = 0;
    }
    else {
        length = StrLen( note );
        StrCopy( data, note );
    }

    AddAnnotation( entryP, data );
}



static void AnnotationFormClose( void )
{
    SafeMemPtrFree( entryP );
    entryP = NULL;
    SafeMemPtrFree( data );
    data = NULL;
}



Boolean AnnotationFormHandleEvent
    (
    EventType* event  /* pointer to an EventType structure */
    )
{
    Boolean     handled;

    handled = ResizeHandleEvent( event );

    if ( handled )
        return handled;

    switch ( event->eType ) {
        case frmOpenEvent:
            AnnotationFormInit();
            handled = true;
            break;
            
        case keyDownEvent:
            switch ( event->data.keyDown.chr ) {
                case pageUpChr:
                    if ( FldScrollable( field, winUp ) )
                        FldScrollField( field, 1, winUp );
                    UpdateFieldScrollbar( field, scrollBar );
                    break;

                case pageDownChr:
                    if ( FldScrollable( field, winDown ) )
                        FldScrollField( field, 1, winDown );
                    UpdateFieldScrollbar( field, scrollBar );
                    break;

                default: {
                    UInt16 scrollPos;
                    UInt16 textHeight;
                    UInt16 fieldHeight;

                    FldGetScrollValues( field, &scrollPos, &textHeight, &fieldHeight );

                    if ( FldHandleEvent( field, event ) ) {
                        UInt16 newScrollPos;
                        UInt16 newTextHeight;
                        UInt16 newFieldHeight;

                        FldGetScrollValues( field, &newScrollPos, 
                            &newTextHeight, &newFieldHeight );

                        if ( scrollPos != newScrollPos || textHeight != newTextHeight ||
                             fieldHeight != newFieldHeight )
                            UpdateFieldScrollbar( field, scrollBar );
                            
                        handled = true;
                    }
                    break;
                }
            }
            break;

        case sclRepeatEvent:
            ScrollMessage( field, event->data.sclRepeat.newValue -
                                         event->data.sclRepeat.value );
            break;

        case ctlSelectEvent:
            switch ( event->data.ctlSelect.controlID ){
                case frmAnnotationCancel:
                	AnnotationFormClose();
                    FrmReturnToForm( Prefs()->lastForm );
                    handled = true;
                    break;

                case frmAnnotationOK:
                	SaveAnnotation();
                	AnnotationFormClose();
                    FrmReturnToForm( Prefs()->lastForm );
                    if ( IsFormMain( Prefs()->lastForm ) )
                        FrmUpdateForm( GetMainFormId(), frmRedrawUpdateCode );
  	            handled = true;
                    break;

                case frmAnnotationDelete:
                    if ( entryP->id.indexInParagraph != NEW_ANNOTATION ) {
                        DeleteAnnotation( &( entryP->id ) );
                    }
                	AnnotationFormClose();
                    if ( IsFormMain( Prefs()->lastForm ) )
                        FrmUpdateForm( GetMainFormId(), frmRedrawUpdateCode );
                    FrmReturnToForm( Prefs()->lastForm );
                    handled = true;
                    break;

                default:
                    handled = true;
                    break;
            }
            break;

        case frmCloseEvent:
            AnnotationFormClose();
            handled = false;
            break;

        default:
            handled = false;
    }
    return handled;
}



void DoAnnotate
     (
     AnnotationIdentifier* idP,
     UInt32  flags,
     UInt16  uid,
     Int16   paragraph,
     UInt16  start,
     UInt16  stop,
     Char*   word
     )
{
    if ( IsFullscreenformActive() ) {
        return;
    }

    entryP = SafeMemPtrNew( MAX_ANNOTATION_ENTRY_SIZE );
    MemSet( entryP, MAX_ANNOTATION_ENTRY_SIZE, 0 );
    entryP->flags = flags;

    data   = SafeMemPtrNew( MAX_FIELD + 1 );

    if ( idP == NULL ) {
        idP = &( entryP->id );

        if ( stop <= start ||
             ! FindAnnotation( entryP, uid, paragraph, start, stop ) ) {
            idP->indexInParagraph = NEW_ANNOTATION;
            idP->uid              = uid;
            idP->paragraphNum     = paragraph;
            entryP->triggerStart  = start;
            entryP->triggerStop   = stop;
        }
    }

    if ( idP->indexInParagraph == NEW_ANNOTATION ) {
        if ( word == NULL ) {
            *data = 0;
        }
        else {
            StrNCopy( data, word, MAX_FIELD - 2 );
            data[ MAX_FIELD - 2 ] = 0;
            if ( ! ( flags & ANNOTATION_BOOKMARK ) )
                StrCat( data, ": " );
        }
    }
    else {
        MemHandle h;
        AnnotationEntry* e;
        UInt16           size;

        h = GetAnnotation( idP->uid, idP->paragraphNum, idP->indexInParagraph, NULL );

        if ( h == NULL ) {
            SafeMemPtrFree( entryP );
            SafeMemPtrFree( data );
            return;
        }
        
        e = MemHandleLock( h );
        
        size = e->dataLength;
        
        if ( MAX_FIELD < size )
            size = MAX_FIELD;
            
        if ( 0 < size ) {
            MemMove( data, ( UInt8* )e + e->dataOffset, size );
            data[ size ] = 0;
        }
        
        if ( MAX_ANNOTATION_ENTRY_SIZE < e->headerLength ) {
            SafeMemPtrFree( entryP );
            SafeMemPtrFree( data );
            MemHandleUnlock( h );
            return;
        }

        MemMove( entryP, e, e->headerLength );

        MemHandleUnlock( h );
    }

    FrmPopupForm( frmAnnotation );
}

