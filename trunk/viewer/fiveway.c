/*
 * $Id: fiveway.c,v 1.19 2008/01/21 16:44:41 prussar Exp $
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

#include "os.h"
#include "control.h"
#include "debug.h"
#include "prefsdata.h"
#include "util.h"
#include "rotate.h"
#include "libraryform.h"

#include "fiveway.h"





/* Handler to process any FiveWay requests in the library form */
Boolean FiveWayLibraryHandler
    (
    EventType* event  /* pointer to an EventType structure */
    )
{
    Boolean keyDown;

    keyDown = ( event->eType == keyDownEvent );

    if ( ! HaveFiveWay() )
        return false;
    if ( FiveWayCenterPressed( event ) )
        return LibrarySelectorHandler( librarySelectorGo, 0 );
    if ( keyDown && FiveWayDirectionPressed( event, Up ) ) {
        return LibrarySelectorHandler( librarySelectorUp, 0 );
    }
    if ( keyDown && FiveWayDirectionPressed( event, Down ) )
        return LibrarySelectorHandler( librarySelectorDown, 0 );
    if ( keyDown && FiveWayDirectionPressed( event, Left ) ) {
        return LibrarySelectorHandler( librarySelectorLeft, 0 );
    }
    if ( keyDown && FiveWayDirectionPressed( event, Right ) )
        return LibrarySelectorHandler( librarySelectorRight, 0 );

    return false; 
}



/* Translate fiveway event to *_ARROW */
Int16 FiveWayTranslate
    (
    EventType* event,
    Boolean    ignoreRotate
    )
{
    Boolean handled;
    Boolean didLeft;
    Boolean didRight;
    Boolean didUp;
    Boolean didDown;
    Boolean keyDown;

    handled = false;

    keyDown = ( event->eType == keyDownEvent );

    if ( ! HaveFiveWay() || ! IsFiveWayEvent( event ))
    {
        return NOT_FIVEWAY;
    }

    didLeft  = keyDown && FiveWayKeyPressed( event, Left );
    didRight = keyDown && FiveWayKeyPressed( event, Right );
    didUp    = keyDown && FiveWayKeyPressed( event, Up );
    didDown  = keyDown && FiveWayKeyPressed( event, Down );
    
    if ( ignoreRotate ) {
        if ( didLeft ) 
            return LEFT_ARROW;
        else if ( didRight )
            return RIGHT_ARROW;
        else if ( didUp )
            return UP_ARROW;
        else if ( didDown )
            return DOWN_ARROW;
    }
    else {
        if ( RotSelect( didLeft, didDown, didUp ) ) {
            return LEFT_ARROW;
        }
        else if ( RotSelect( didRight, didUp, didDown ) ) {
            return RIGHT_ARROW;
        }
        else if ( RotSelect( didUp, didLeft, didRight ) ) {
            return UP_ARROW;
        }
        else if ( RotSelect( didDown, didRight, didLeft ) ) {
            return DOWN_ARROW;
        }
    }

    if ( FiveWayCenterPressed( event ) ) {
        return SELECT_ARROW;
    }

    return NOT_ARROW;
}



/* Handler to process any FiveWay requests in the main form */
Boolean FiveWayMainHandler
    (
    EventType* event
    )
{
    Int16 arrow;

    arrow = FiveWayTranslate( event, false );

    if ( arrow == NOT_FIVEWAY ) {
        return false;
    }
    else if ( arrow < 0 ) {
        return true;
    }
    else if ( Prefs()->arrowMode[ arrow ] != SELECT_NONE ) {
        DoSelectTypeAction( Prefs()->arrowMode[ arrow ] );
        return true;
    }
    else {
        return false;
    }
}
