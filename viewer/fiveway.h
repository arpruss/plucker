/*
 * $Id: fiveway.h,v 1.13 2008/01/21 16:44:41 prussar Exp $
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

#ifndef PLUCKER_FIVEWAY_H
#define PLUCKER_FIVEWAY_H

#include "viewer.h"

#ifdef HAVE_PALMONE_SDK

#define OrCheckUp( eventP )      || ( ( eventP )->eType == keyDownEvent && \
            ( ( eventP )->data.keyDown.chr == pageUpChr && \
            ( ( eventP )->data.keyDown.modifiers & commandKeyMask ) ) )

#define OrCheckDown( eventP )    || ( ( eventP )->eType == keyDownEvent && \
            ( ( eventP )->data.keyDown.chr == pageDownChr && \
            ( ( eventP )->data.keyDown.modifiers & commandKeyMask ) ) )

#define OrCheckLeft( eventP )
#define OrCheckRight( eventP )

#define FiveWayCenterPressed( eventP )         NavSelectPressed( eventP )

#define FiveWayDirectionPressed( eventP, dir ) (   \
                NavDirectionPressed( eventP, dir ) \
                OrCheck ## dir( eventP ) )

#define FiveWayKeyPressed( eventP, dir )       NavKeyPressed( eventP, dir )
#define IsFiveWayEvent( eventP )               IsFiveWayNavEvent( eventP )

/* only to be used with Left, Right, Up, and Down; use
   FiveWayCenterPressed for Select/Center */

extern void FiveWayResetValues( void ) FIVEWAY_SECTION;
extern void FiveWaySetRow( Int16 row ) FIVEWAY_SECTION;
extern Int16 FiveWayGetRow( void ) FIVEWAY_SECTION;
extern void FiveWayHighlightRow( Boolean enable ) FIVEWAY_SECTION;
extern Boolean FiveWayLibraryHandler( EventType* event ) FIVEWAY_SECTION;
extern Boolean FiveWayMainHandler( EventType* event ) FIVEWAY_SECTION;
extern Int16 FiveWayTranslate ( EventType* event, Boolean ignoreRotate ) 
    FIVEWAY_SECTION;

#else /* HAVE_PALMONE_SDK */

#define IsFiveWayEvent(eventP)         false
#define FiveWayCenterPressed(eventP)   false
#define FiveWayDirectionPressed(eventP, nav) false
#define FiveWayKeyPressed(eventP, nav) false
#define FiveWayLibraryHandler( event ) false
#define FiveWayMainHandler( event )    false
#define FiveWayTranslate( event, i )      NOT_FIVEWAY

#endif /* HAVE_PALMONE_SDK */


/***********************************************************************
 *
 *      Private constants
 *
 ***********************************************************************/
#define NOT_FIVEWAY  -2
#define NOT_ARROW    -1

#endif
