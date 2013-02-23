/*
 * $Id: annotationform.h,v 1.6 2006/11/12 02:00:55 prussar Exp $
 *
 * Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 1998-2001, Mark Ian Lillywhite and Michael Nordstrom
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

#ifndef PLUCKER_ANNOTATIONFORM_H
#define PLUCKER_ANNOTATIONFORM_H

#include "viewer.h"
#include "annotation.h"

extern Boolean AnnotationFormHandleEvent( EventType* event );
extern Boolean AnnotationPrefsFormHandleEvent( EventType* event );
extern void DoAnnotate( AnnotationIdentifier* id, UInt32 flags, UInt16 uid,
    Int16 paragraph, UInt16 start, UInt16 stop, Char* word ) 
    ANNOTATIONFORM_SECTION;

#endif

