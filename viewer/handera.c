/*
 * $Id: handera.c,v 1.2 2004/05/22 19:42:43 prussar Exp $
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

#include <PalmOS.h>
#include <Silk.h>
#include <BmpGlue.h>

#include "debug.h"
#include "prefsdata.h"
#include "util.h"
#include "mainform.h"
#include "libraryform.h"
#include "DIA.h"

#include "handera.h"


/***********************************************************************
 *
 *      Internal Constants
 *
 ***********************************************************************/
#define TOTAL_ICONS           11
#define TOOLBAR_START_X       43
#define TOOLBAR_START_Y        3

#define NUM_HANDERA_SILKSCREENS 2

/***********************************************************************
 *
 *      Internal Types
 *
 ***********************************************************************/

typedef struct {
    UInt16        resourceId;
    WChar         asciiCode;
    RectangleType bounds[ NUM_HANDERA_SILKSCREENS ];
} SilkIconType;


typedef enum {
    HANDERA_SILK_UP   = 1,
    HANDERA_SILK_DOWN = 0
} HanderaSilkscreenState;


/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static void GetButtons( void ) HANDERA_SECTION;
static void DrawButtons( void ) HANDERA_SECTION;
static void SetButtons( PenBtnListType* buttonList ) HANDERA_SECTION;



/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/

static MemHandle        defaultButtonsHandle[ NUM_HANDERA_SILKSCREENS ];
static SilkIconType     iconList[ TOTAL_ICONS ];

static HanderaSilkscreenState           currentSilkStatus;





/* Reset the toolbar (both minimized and maximized) to its default values */
void HanderaResetSilkScreen( void )
{
    PenBtnListType* defaultButtons;
    UInt16          i;

    /* currently only handeraSilkScreen supported */
    if ( GetDIAHardware() != DIA_HARDWARE_HANDERA )
        return;

    SilkRestoreDefaultTemplates();
    for ( i = 0; i < NUM_HANDERA_SILKSCREENS; i++ ) {
        if ( defaultButtonsHandle[ i ] != NULL ) {
            defaultButtons = MemHandleLock( defaultButtonsHandle[ i ] );
            SilkSetButtonList( defaultButtons, (Boolean) i );
            SafeMemPtrFree( defaultButtons );
            defaultButtonsHandle[ i ] = NULL;
        }
    }
}



void HanderaUpdateSilkVerticalOffset
    (
    const Char* offText
    )
{
    WinHandle silkWindow, origWindow;
    FontID    origFont;

    silkWindow = SilkGetWindow();
    origWindow = WinSetDrawWindow( silkWindow );
    origFont   = FntSetFont( stdFont );

    WinEraseRectangle( &( iconList[ 1 ].bounds[ currentSilkStatus ] ), 0 );
    WinDrawChars( offText, StrLen( offText ),
        iconList[ 1 ].bounds[ currentSilkStatus ].topLeft.x,
        iconList[ 1 ].bounds[ currentSilkStatus ].topLeft.y );

    FntSetFont( origFont );
    WinSetDrawWindow( origWindow );
}



/* function to populate the iconList with valid data */
static void GetButtons( void )
{
    iconList[ 0 ].resourceId = bmpDbase;
    iconList[ 0 ].asciiCode  = vchrPluckerDbase;

    iconList[ 1 ].resourceId = 0;
    iconList[ 1 ].asciiCode  = vchrPluckerOffset;

    iconList[ 2 ].resourceId = bmpBookmark;
    iconList[ 2 ].asciiCode  = vchrPluckerBookmark;

    iconList[ 3 ].resourceId = bmpFind;
    iconList[ 3 ].asciiCode  = vchrPluckerFind;

    iconList[ 4 ].resourceId = bmpAgain;
    iconList[ 4 ].asciiCode  = vchrPluckerAgain;

    iconList[ 5 ].resourceId = bmpAutoscrollDecr;
    iconList[ 5 ].asciiCode  = vchrPluckerAutoscrollDecr;

    iconList[ 6 ].resourceId = bmpAutoscrollStop;
    iconList[ 6 ].asciiCode  = vchrPluckerAutoscrollStop;

    iconList[ 7 ].resourceId = bmpAutoscrollIncr;
    iconList[ 7 ].asciiCode  = vchrPluckerAutoscrollIncr;

    iconList[ 8 ].resourceId = bmpLeft;
    iconList[ 8 ].asciiCode  = vchrPluckerLeft;

    iconList[ 9 ].resourceId = bmpHome;
    iconList[ 9 ].asciiCode  = vchrPluckerHome;

    iconList[ 10 ].resourceId = bmpRight;
    iconList[ 10 ].asciiCode  = vchrPluckerRight;

/* FIXME: Maybe used later, still have to figure out how to :) */
#if 0
    iconList[ 11 ].resourceId = bmpAutoscrollStart;
    iconList[ 11 ].asciiCode  = vchrPluckerAutoscrollStart;

    iconList[ 12 ].resourceId = bmpWait;
    iconList[ 12 ].asciiCode  = vchrPluckerWait;
#endif
}



/* Set the silkscreen to contain the toolbar buttons */
void HanderaSetSilkScreen( void )
{
    UInt16          defaultSize;
    PenBtnListType* defaultButtons;
    UInt16          numNewButtons;
    UInt16          newSize;
    PenBtnListType* newButtons;

    /* currently only handeraSilkScreen supported */
    if ( GetDIAHardware() != DIA_HARDWARE_HANDERA )
        return;

    currentSilkStatus = ( GetDIAState() == DIA_STATE_MAX ? 
                                         HANDERA_SILK_UP : HANDERA_SILK_DOWN );

    /* if we already have defaultButtonsHandle stored in memory, there
       is no point in doing everything again, since its already done */
    if ( defaultButtonsHandle[ currentSilkStatus ] != NULL )
        return;

    defaultSize = SilkGetButtonListSize( (Boolean) currentSilkStatus );

    defaultButtonsHandle[ currentSilkStatus ] = MemHandleNew( defaultSize );
    defaultButtons = MemHandleLock( defaultButtonsHandle[ currentSilkStatus ] );
    SilkGetButtonList( defaultButtons, (Boolean) currentSilkStatus );

    numNewButtons = defaultButtons->numButtons + TOTAL_ICONS;
    newSize = sizeof( PenBtnListType ) +
        ( sizeof( PenBtnInfoType ) * ( numNewButtons - 1 ) );
    newButtons = SafeMemPtrNew( newSize );
    MemMove( newButtons, defaultButtons, defaultSize );

    GetButtons();
    DrawButtons();
    SetButtons( newButtons );
    SilkSetButtonList( newButtons, (Boolean) currentSilkStatus );

    MemPtrUnlock( defaultButtons );
    SafeMemPtrFree( newButtons );
}



/* Draw the actual buttons onto blank bitmaps, and set them as valid
   templates on the silkscreen */
static void DrawButtons( void )
{
    Err       err;
    BitmapPtr silkBitmap;
    BitmapPtr silkBitmapInv;
    WinHandle silkWindow;
    WinHandle silkWindowInv;
    WinHandle origWindow;
    UInt16    i;
    Coord     currentX;
    Coord     currentY;
    Coord     silkX;
    Coord     silkY;

    origWindow = WinGetDrawWindow();

    if ( currentSilkStatus == HANDERA_SILK_UP ) {
        SilkGetTemplateBitmaps( &silkBitmap, &silkBitmapInv, NULL, NULL );
    }
    else {
        SilkGetTemplateBitmaps( NULL, NULL, &silkBitmap, &silkBitmapInv );
    }
    BmpGlueGetDimensions( silkBitmap, &silkX, &silkY, NULL );
    silkWindow = WinCreateOffscreenWindow( silkX, silkY, screenFormat, &err );
    silkWindowInv = WinCreateOffscreenWindow( silkX, silkY, screenFormat, &err);
    WinSetDrawWindow( silkWindow );
    WinDrawBitmap( silkBitmap, 0, 0 );
    WinSetDrawWindow( silkWindowInv );
    WinDrawBitmap( silkBitmapInv, 0, 0 );

    /* We need to move down the existing silkscreen to make room for our
       own toolbar's buttons */
    if ( currentSilkStatus == HANDERA_SILK_UP ) {
        RectangleType area;
        UInt16        moveY;

        area.topLeft.x = 40;
        area.topLeft.y = 1;
        area.extent.x = 200;
        area.extent.y = 18;
        moveY         = 14;

        WinCopyRectangle( silkWindow, silkWindow, &area,
            area.topLeft.x, moveY, winPaint );
        WinCopyRectangle( silkWindowInv, silkWindowInv, &area,
            area.topLeft.x, moveY, winPaint );
        area.extent.y = moveY;
        WinSetDrawWindow( silkWindow );
        WinEraseRectangle( &area, 0 );
        WinSetDrawWindow( silkWindowInv );
        WinEraseRectangle( &area, 0 );
    }
    currentX = TOOLBAR_START_X;
    currentY = TOOLBAR_START_Y;

    for ( i = 0; i < TOTAL_ICONS; i++ ) {
        MemHandle bitmapH;
        BitmapPtr bitmap;
        Coord     width;
        Coord     height;

        if ( iconList[ i ].resourceId == 0 ) {
            /* This is just a placeholder for our '0' resourced offset image */
            width = 22;
            height = 13;
        }
        else {
            bitmapH = DmGetResource( bitmapRsc, iconList[ i ].resourceId );
            bitmap = MemHandleLock( bitmapH );
            BmpGlueGetDimensions( bitmap, &width, &height, NULL );

            WinSetDrawWindow( silkWindow );
            WinDrawBitmap( bitmap, currentX, currentY );
            WinSetDrawWindow( silkWindowInv );
            WinDrawBitmap( bitmap, currentX, currentY );

            MemPtrUnlock( bitmap );
            DmReleaseResource( bitmapH );
        }

        iconList[ i ].bounds[ currentSilkStatus ].topLeft.x = currentX;
        iconList[ i ].bounds[ currentSilkStatus ].topLeft.y = currentY;
        iconList[ i ].bounds[ currentSilkStatus ].extent.x = width;
        iconList[ i ].bounds[ currentSilkStatus ].extent.y = height;
        WinInvertRectangle( &( iconList[ i ].bounds[ currentSilkStatus ]), 0 );

        /* Because some icons are meant to appear right beside each other,
           they're defined here up top. Everything else is spaced so
           it fits nicely */
        switch ( iconList[ i ].resourceId ) {
            case bmpFind:
            case bmpAutoscrollDecr:
            case bmpAutoscrollStop:
            case bmpLeft:
            case bmpHome:
                currentX += width;
                break;

            default:
                currentX += width;
                if ( currentSilkStatus == HANDERA_SILK_UP )
                    currentX += 3;
                else if ( currentSilkStatus == HANDERA_SILK_DOWN )
                    currentX += 7;
                break;
        }
    }
    WinSetDrawWindow( origWindow );

    silkBitmap = WinGetBitmap( silkWindow );
    silkBitmapInv = WinGetBitmap( silkWindowInv );

    if ( currentSilkStatus == HANDERA_SILK_UP )
        SilkSetTemplateBitmaps( silkBitmap, silkBitmapInv, NULL, NULL );
    else
        SilkSetTemplateBitmaps( NULL, NULL, silkBitmap, silkBitmapInv );

    WinDeleteWindow( silkWindow, false );
    WinDeleteWindow( silkWindowInv, false );
}



/* Set the buttons on the silkscreen so that our new images are valid */
static void SetButtons
    (
    PenBtnListType* buttonList /* pointer to PenBtnListType structure */
    )
{
    UInt16 index, i;

    index = buttonList->numButtons;

    if ( currentSilkStatus == HANDERA_SILK_UP ) {
        RectangleType alpha, numeric;

        /* Set the numeric input area to null so our buttons are valid */
        SilkGetAreas( &alpha, &numeric );
        alpha.topLeft.y += 14;
        alpha.extent.y -= 14;
        numeric.topLeft.y += 14;
        numeric.extent.y -= 14;
        SilkSetAreas( &alpha, &numeric );
    }

    /* Because the some virtual silkscreen buttons will overlap the area
       that we want, we need to move the pen bounds around to the actual
       icon locations.

       FIXME: there should be a better way than just guessing 2, 6, 7 or 8
       as positions we want to modify in the array */
    if ( currentSilkStatus == HANDERA_SILK_UP ) {
        buttonList->buttons[ 6 ].boundsR.topLeft.y += 14;
        buttonList->buttons[ 7 ].boundsR.topLeft.y += 14;
        buttonList->buttons[ 8 ].boundsR.topLeft.y += 14;
    }
    else if ( currentSilkStatus == HANDERA_SILK_DOWN ) {
        buttonList->buttons[ 2 ].boundsR.topLeft.x = 220;
        buttonList->buttons[ 2 ].boundsR.extent.x = 20;
    }

    for ( i = 0; i < TOTAL_ICONS; i++ ) {
        RectangleType boundsR;

        boundsR = iconList[ i ].bounds[ currentSilkStatus ];
        if ( currentSilkStatus == HANDERA_SILK_UP )
            boundsR.topLeft.y = 3;
        else if ( currentSilkStatus == HANDERA_SILK_DOWN )
            boundsR.topLeft.y = 66;
        buttonList->buttons[ index + i ].boundsR = boundsR;
        buttonList->buttons[ index + i ].asciiCode = iconList[ i ].asciiCode;
        buttonList->buttons[ index + i ].keyCode = 0;
        buttonList->buttons[ index + i ].modifiers = 0;
    }
    buttonList->numButtons += TOTAL_ICONS;
}

