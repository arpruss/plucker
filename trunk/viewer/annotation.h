/*
 * $Id: annotation.h,v 1.12 2008/01/20 22:21:33 prussar Exp $
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

#ifndef PLUCKER_ANNOTATION_H
#define PLUCKER_ANNOTATION_H

#include "doclist.h"
#include "viewer.h"
#include "const.h"

#define NO_ANNOTATION   -1
#define NEW_ANNOTATION  -1

/* flags */
#define ANNOTATION_BOOKMARK 1
#define ANNOTATION_HIDDEN   2

typedef struct {
   UInt16 uid;
   UInt16 paragraphNum;
   Int16  indexInParagraph;
} AnnotationIdentifier;

/* Code assumes that id field is at the very top */
typedef struct {
   AnnotationIdentifier id;
   UInt16 headerLength;  /* sizeof( AnnotationEntry ) */
   UInt16 triggerStart;  /* relative to paragraph start */
   UInt16 triggerStop;   /* relative to paragraph start */
   UInt16 dataOffset;    /* offset of data from beginning of this record */
   UInt16 dataLength;
   YOffset yOffset;      /* currently unused */
   UInt32 flags;
   UInt32 createDate;
   UInt32 modificationDate;
} AnnotationEntry;

/* Leave some spare room for future expansion */
#define MAX_ANNOTATION_ENTRY_SIZE  ( sizeof( AnnotationEntry ) + 32 )

extern void InitDocAnnotations( Char* docName, UInt16 location, UInt32 docDate )
                ANNOTATION_SECTION;

extern Boolean GetAnnotationHeader( AnnotationEntry* outP, UInt16 uid,
                UInt16 paragraphNum, UInt16 index ) ANNOTATION_SECTION;

extern MemHandle GetAnnotation( UInt16 uid, UInt16 paragraph, UInt16 index,
                UInt16* recordNum ) ANNOTATION_SECTION;

extern Boolean AddAnnotation( AnnotationEntry* header, Char* text )
                ANNOTATION_SECTION;

extern Boolean DeleteAnnotation( AnnotationIdentifier* id ) ANNOTATION_SECTION;

extern Boolean FindAnnotation( AnnotationEntry* annotationHeader, UInt16 uid,
                UInt16 paragraph, UInt16 start, UInt16 stop )
                ANNOTATION_SECTION;

extern void CloseDocAnnotations( void ) ANNOTATION_SECTION;

extern MemHandle GetAnnotationByRecordNum( UInt16 recordNum ) 
                ANNOTATION_SECTION; 

extern Boolean GetAnnotationHeaderByRecordNum( AnnotationEntry* e,
                UInt16 recordNum ) ANNOTATION_SECTION;

extern UInt16 GetNumberOfAnnotations( void ) ANNOTATION_SECTION;

void DeleteAnnotationByRecordNum( UInt16 recordNum ) ANNOTATION_SECTION;

void CheckAnnotationsDate( void ) ANNOTATION_SECTION;

void DeleteAnnotations( char* docName, UInt16 location ) ANNOTATION_SECTION;

void RenameAnnotations ( Char* newName, UInt16 oldLocation, 
    DocumentInfo* docInfo ) ANNOTATION_SECTION;

#endif /* PLUCKER_ANNOTATION_H */

