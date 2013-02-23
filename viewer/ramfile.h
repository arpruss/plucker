/*
 * $Id: ramfile.h,v 1.13 2005/08/08 02:30:59 prussar Exp $
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

#ifndef PLUCKER_RAMFILE_H
#define PLUCKER_RAMFILE_H

#include "viewer.h"
#include "genericfile.h"


/* return UID for last record in document, i.e. the max UID */
extern UInt16 GetRAMMaxUID( void ) RAMFILE_SECTION;

/* Open document */
extern Err OpenRAMDocument( DocumentInfo* docInfo ) RAMFILE_SECTION;

/* Close document */
extern void CloseRAMDocument( void ) RAMFILE_SECTION;

/* update document list with documents in RAM */
extern void EnumerateRAM( void ) RAMFILE_SECTION;

/* Delete document */
extern void DeleteRAMDocument( DocumentInfo* docInfo ) RAMFILE_SECTION;

/* Beam Plucker document in RAM, return error code, or 0 if there is no error */
extern void BeamRAMDocument( DocumentInfo* docInfo ) RAMFILE_SECTION;

/* Rename document and its associated meta document */
extern void RenameRAMDocument( Char* newName,
                DocumentInfo* docInfo ) RAMFILE_SECTION;

/* Find record in document, returns handle to record or NULL if not found */
/* Note: You must release the record when done by using CloseRecord() */
extern MemHandle SearchRAMDocument( DmOpenRef docRef, CompareFunc* compare,
                    void* item, UInt16 numRecords,
                    Int16* index ) RAMFILE_SECTION;

/* Find record with given uid in document */
/* Note: You must release the record when done by using CloseRecord() */
extern MemHandle FindRAMRecord( UInt16 uid, Int16* index ) RAMFILE_SECTION;

/* Return a handle to a record */
/* Note: You must release the record when done by using CloseRecord() */
extern MemHandle ReturnRAMRecordHandle( UInt16 uid ) RAMFILE_SECTION;

/* Return a handle to a record */
/* Note: You must release the record when done, e.g., by using 
   CloseRecord() */
extern MemHandle ReturnRAMRecordHandleByIndex( const UInt16 index )
    RAMFILE_SECTION;

/* Return a handle to next record */
/* Note: You must release the record when done, e.g., by using 
   CloseRecord() */
extern MemHandle ReturnNextRAMRecordHandle( UInt16* index ) RAMFILE_SECTION;

/* Return number of records in document */
extern UInt16 GetRAMNumOfRecords( void ) RAMFILE_SECTION;

/* Free allocated handle */
extern void FreeRAMRecordHandle( MemHandle* handle ) RAMFILE_SECTION;

/* Receive Plucker document, return error code, or 0 if there is no error */
extern Err ReceiveRAMDocument( ExgSocketType* s, Boolean globalsAvailable );

/* Release a record without having to know which database it is in */
extern void CloseRecord( MemHandle handle, Boolean dirty )
    RAMFILE_SECTION;
    
extern MemHandle NewRecord( DmOpenRef dbRef, UInt16* indexP, UInt32 size )
    RAMFILE_SECTION;

extern MemHandle ResizeRecord( DmOpenRef dbRef, UInt16 index, UInt32 size )
    RAMFILE_SECTION;

extern MemHandle OpenRecord( DmOpenRef dbRef, UInt16 index ) RAMFILE_SECTION;

extern void CloseRecord( MemHandle handle, Boolean dirty ) RAMFILE_SECTION;

extern void DestroyOpenRecordsList( void ) RAMFILE_SECTION;

extern void MarkRecordDirty( MemHandle handle ) RAMFILE_SECTION;

extern Err CloseDatabase( DmOpenRef dbRef ) RAMFILE_SECTION;

extern Err RemoveRecord ( DmOpenRef dbRef, UInt16 index ) RAMFILE_SECTION;

#endif

