/*
 * $Id: metadocument.h,v 1.20 2008/01/21 00:47:01 prussar Exp $
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

#ifndef PLUCKER_METADOCUMENT_H
#define PLUCKER_METADOCUMENT_H

#include "viewer.h"
#include "genericfile.h"

/* Internal record "names" */
#define MAX_INTERNAL        5

#define _HOME_PAGE          0
#define _INTERNAL_BOOKMARKS 1
#define _SHOW_IMAGES        2
#define _SESSION_DATA       3
#define _LINK_TABLE         4

#define _LAST_INTERNAL      _LINK_TABLE

typedef struct {
    UInt16 uid;
    UInt16 records;
    UInt16 internal[ 0 ];
} MetaIndexRecord;


/* Return the internal record's unique ID */
extern UInt16 GetInternalID( UInt16 name ) METADOCUMENT_SECTION;

/* Get the status for a specific bit in the given record */
extern Boolean GetBitStatus( const UInt16 record, UInt16 reference ) METADOCUMENT_SECTION;

/* Add bookmark record to document */
/* Note: You must release the record when done, e.g., by using
   CloseRecord() */
extern void AddBookmarkRecord( MemHandle* hand ) METADOCUMENT_SECTION;

/* Remove bookmark record from document */
extern void RemoveBookmarkRecord( void ) METADOCUMENT_SECTION;

/* Get a handle to a meta record */
/* Note: You must release the record when done, e.g., by using 
   CloseRecord() */
extern MemHandle ReturnMetaHandle( UInt16 id, const UInt16 num ) METADOCUMENT_SECTION;

/* Remove meta record */
extern void RemoveMetaRecord( const UInt16 id ) METADOCUMENT_SECTION;

/* Resize if record is too small */
extern void MetaCheckByteSize( const UInt16 id, UInt16 size,
                MemHandle handle ) METADOCUMENT_SECTION;

/* Resize record */
/* Note: You must release the record when done, e.g., by using 
   CloseRecord() */
extern void ResizeMetaRecord( const UInt16 id, const UInt16 size,
                MemHandle* handle ) METADOCUMENT_SECTION;

/* Open meta document */
extern void OpenMetaDocument( Char* name, UInt16 location, UInt32 date ) 
    METADOCUMENT_SECTION;

/* Close document */
extern void CloseMetaDocument( void ) METADOCUMENT_SECTION;

/* Delete meta document */
extern void DeleteMetaDocument( Char* name, UInt16 location )
    METADOCUMENT_SECTION;

/* Tag meta-data for re-rendering if fonts or spacing or the like has changed */
extern Boolean ReRenderAllIfNeeded( void ) METADOCUMENT_SECTION;

extern void RenameMetaDocument( Char* newName, UInt16 oldLocation,
                DocumentInfo* docInfo ) METADOCUMENT_SECTION;

/* Set the status of a bit in the specified record */
extern void SetBitStatus( const UInt16 record, UInt16 reference,
                Boolean setStatus ) METADOCUMENT_SECTION;

/* Force re-rendering of all records */
extern void ReRenderAll( void ) METADOCUMENT_SECTION;

/* Activate/deactive auto re-render */
extern void SetReRenderCheck( Boolean check ) METADOCUMENT_SECTION;

/* Note: You must release the record when done, e.g., by using 
   CloseRecord() */
extern MemHandle FindMetaRecord( UInt16 item,
                    Int16 *index ) METADOCUMENT_SECTION;

/* Delete a record by unique ID */
extern void DeleteMetaRecord ( const UInt16 id ) METADOCUMENT_SECTION;

/* Get the creation date of the document */
UInt32 GetDocumentDate( void ) METADOCUMENT_SECTION;

/* Fix name incorrectly truncated by older Plucker versions */
void LegacyFixMetaOrAnnotation ( Char* newMetaName, Char* docName ) 
    METADOCUMENT_SECTION;

/* Make a metadocument name */
void MakeMetaName ( Char* metaName, const Char*  prefix,
    UInt16 location, Char* docName ) METADOCUMENT_SECTION;



#define INTERNAL_BOOKMARKS_ID   GetInternalID( _INTERNAL_BOOKMARKS )
#define SHOW_IMAGES_ID          GetInternalID( _SHOW_IMAGES )
#define SESSION_DATA_ID         GetInternalID( _SESSION_DATA )
#define LINK_TABLE_ID           GetInternalID( _LINK_TABLE )

/* Prefixes for metadocument and annotation dbnames */
#define META_PREFIX             "Plkr"
#define ANN_PREFIX              "PlkA"

#endif

