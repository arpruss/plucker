/*
 * $Id: testsuite.c,v 1.7 2004/01/04 00:21:36 nordstrom Exp $
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

#include "listtester.h"
#include "historytester.h"
#include "externalformtester.h"
#include "ostester.h"
#include "prefsdatatester.h"
#include "metadocumenttester.h"
#include "genericfiletester.h"
#include "uncompresstester.h"
#include "paragraphtester.h"
#include "fonttester.h"
#include "vfsfiletester.h"

#include "TestDB.h"

void initialize_suite()
{
    InitializeListSuite();
    InitializeHistorySuite();
    InitializeExternalFormSuite();
    InitializeOSSuite();
    InitializePrefsDataSuite();
    InitializeMetaDocumentSuite();
    InitializeGenericFileSuite();
    InitializeUncompressSuite();
    InitializeParagraphSuite();
    InitializeFontSuite();
    InitializeVFSFileSuite();
}



void terminate_suite()
{
    TerminateListSuite();
    TerminateHistorySuite();
    TerminateOSSuite();
    TerminatePrefsDataSuite();
    TerminateMetaDocumentSuite();
    TerminateGenericFileSuite();
    TerminateUncompressSuite();
    TerminateParagraphSuite();
    TerminateFontSuite();
    TerminateVFSFileSuite();
}

