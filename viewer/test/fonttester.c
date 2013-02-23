/*
 * $Id: fonttester.c,v 1.3 2004/02/21 16:31:33 chrish Exp $
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
#include "font.h"

#include "fonttester.h"



int FontSuiteSetUp( void )
{
    return 0;
}



int FontSuiteTearDown( void )
{
    return 0;
}



int FontInitTest( void )
{
    return 0;
}



int FontCleanupTest( void )
{
    return 0;
}



void TestPostprocessVFSFonts( void )
{
#ifdef SUPPORT_VFS_FONTS
    /* We start with an empty table */
   PostprocessVFSFonts(); 
#endif
}



void InitializeFontSuite( void )
{
    TestGroup* group;

    /* Font Tester */
    group = add_test_group( "Font", FontSuiteSetUp, FontSuiteTearDown,
                FontInitTest, FontCleanupTest );
    add_test_case( group, "Postprocess VFS fonts", TestPostprocessVFSFonts );
}



void TerminateFontSuite( void )
{
}

