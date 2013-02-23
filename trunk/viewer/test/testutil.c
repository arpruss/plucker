/*
 * $Id: testutil.c,v 1.8 2004/01/11 17:33:32 nordstrom Exp $
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
#include "mock_Device.h"
#include "metadocument.h"
#include "os.h"
#include "util.h"
#include "prefsdata.h"

#include "testutil.h"


typedef struct {
    Char*       name;
    UInt16      cardNo;
    UInt32      size;
    UInt32      created;
    UInt16      attributes;
    UInt16      categories;
    UInt16      location;
    Boolean     active;
    UInt32      timestamp;
    UInt16      volumeRef;
    UInt16      numRecords;
    Char*       filename;
} PluckerDocument;



/* This should be "real" data for documents in the data directory */
static PluckerDocument TestDocument[] = {
    { "UnitTest", 0, 550L, -1139389298L, 512, 1, 0, true, 0L, 0, 0, NULL },
    { "UnitTestDOC", 0, 509L, -1139140457, 512, 1, 0, true, 0L, 0, 0, NULL }
};



/* Open test document */
Err OpenTestDocument
    ( 
    Char*           name,
    DocumentInfo*   docInfo
    )
{
    Err     err = 1;
    UInt16  i;

    /* Find the data for the given document */
    for ( i = 0; i < sizeof TestDocument / sizeof TestDocument[ 0 ]; i++ ) {
        PluckerDocument* document;

        document = &TestDocument[ i ];

        if ( STREQ( name, document->name ) ) {
            OS_Init();
            ReadPrefs();
            SetStandardFunctions();
        
            DeleteMetaDocument( document->name, 0 );

            StrCopy( docInfo->name, document->name );
            docInfo->cardNo     = document->cardNo;
            docInfo->size       = document->size;
            docInfo->created    = document->created;
            docInfo->attributes = document->attributes;
            docInfo->categories = document->categories;
            docInfo->location   = document->location;
            docInfo->active     = document->active;
            docInfo->timestamp  = document->timestamp;
            docInfo->volumeRef  = document->volumeRef;
            docInfo->numRecords = document->numRecords;
            docInfo->filename   = document->filename;

            err = OpenDocument( docInfo );
        }
    }
    return err;
}



void CloseTestDocument
    (
    Char* name
    )
{
    DeleteMetaDocument( name, 0 );
    CloseDocument();
    OS_Release();
}



Boolean CheckRecordData
    (
    UInt8* inData,
    UInt8* knownData,
    UInt16 knownSize
    )
{
    UInt16 i;

    for ( i = 0; i < knownSize; i++ ) {
        if ( inData[ i ] != knownData[ i ] )
            return false;
    }
    return true;
}

