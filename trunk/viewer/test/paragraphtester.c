/*
 * $Id: paragraphtester.c,v 1.2 2004/01/04 12:03:18 nordstrom Exp $
 *
 * Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 1998-2003, Mark Ian Lillywhite and Michael Nordstrom
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

#include "TestDB.h"
#include "const.h"
#include "util.h"
#include "testutil.h"
#include "uncompress.h"
#include "paragraph.h"
#include "rotate.h"
#include "font.h"
#include "mock_Device.h"
#include "mock_Window.h"

#include "paragraphtester.h"


#define TEST_DOCUMENT       "UnitTest"


static DocumentInfo docInfo;

/* Known data from the records in UnitTest.pdb */
/* Record 2 */
/* First paragraph includes two lines of text */
static Char* para1line1 = "This is a test document used for unit";
static Char* para1line2 = "test. It has one link of depth 2. ";
/* Second paragraph includes only one line */
static Char* para2line1 = "Link A1";



int ParagraphSuiteSetUp( void )
{
    /* Check that we have access to the test documents we require */
    if ( DmFindDatabase( 0, TEST_DOCUMENT ) == 0 )
        return 1;

    SetFontFunctions();
    InitializeViewportBoundaries();

    return OpenTestDocument( TEST_DOCUMENT, &docInfo );
}



int ParagraphSuiteTearDown( void )
{
    CloseTestDocument( TEST_DOCUMENT );

    return 0;
}



int ParagraphInitTest( void )
{
    CreateUncompressBuffer();

    return 0;
}



int ParagraphCleanupTest( void )
{
    DeleteUncompressBuffer();

    return 0;
}



void TestDraw( void )
{
    MemHandle   handle      = NULL;
    UInt8*      data        = NULL;
    Paragraph*  paragraph   = NULL;
    Header*     header      = NULL;
    Char*       line        = NULL;
    TextContext textContext;

    /* Get handle for home page and lock it */
    handle      = GetRecordHandle( 2 );
    header      = MemHandleLock( handle );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong record ID: ", 2, header->uid );
    ASSERT_UINT16_EQUAL_MSG_GOTOERR( "Wrong number of paragraphs: ", 2,
        header->paragraphs );

    /* Uncompress the data and initialize the text context */
    UnZip( header );
    textContext.cursorX                 = RotTopLeftX();
    textContext.cursorY                 = RotTopLeftY();
    textContext.writeMode               = WRITEMODE_DRAW_CHAR;
    textContext.activeAnchor            = false;
    textContext.findCharacterPosition   = -1;

    /* The first paragraph is located directly after the document header */
    paragraph = (Paragraph*) (header + 1 );

    /* "Draw" the first paragraph */
    mock_CreateLineCache();
    DrawParagraph( &textContext, paragraph, header );
    line = mock_GetLine( 1 );
    ASSERT_MSG_GOTOERR( "No line found", line != NULL );
    ASSERT_STR_EQUAL_MSG_GOTOERR( "Wrong line: ", para1line1, line );
    line = mock_GetLine( 2 );
    ASSERT_MSG_GOTOERR( "No line found", line != NULL );
    ASSERT_STR_EQUAL_MSG_GOTOERR( "Wrong line: ", para1line2, line );

ERR:
    mock_ReleaseLineCache();
    MemHandleUnlock( handle );
}



void InitializeParagraphSuite( void )
{
    TestGroup* group;

    /* Paragraph Tester */
    group = add_test_group( "Paragraph", ParagraphSuiteSetUp,
                ParagraphSuiteTearDown, ParagraphInitTest,
                ParagraphCleanupTest );
    add_test_case( group, "Draw", TestDraw );
}



void TerminateParagraphSuite( void )
{
}

