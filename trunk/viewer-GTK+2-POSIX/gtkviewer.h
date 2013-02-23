/* -*- mode: c; indent-tabs-mode: nil; c-indent-level: 4; -*-

 * $Id: gtkviewer.h,v 1.2 2003/12/28 20:58:20 chrish Exp $
 *
 * Viewer - a part of Plucker, the open-source, open-format ebook system
 * Copyright (c) 2002, Bill Janssen
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
 */

/***********************************************************************/
/***********************************************************************/
/***                                                                 ***/
/***   This struct connects the viewer code with the library code.   ***/
/***   There is an instance of it for each document in the library.  ***/
/***                                                                 ***/
/***********************************************************************/
/***********************************************************************/

typedef struct {
    char*    name;
    char*    path;
    int      nrecords;
    char*    title;
    char*    author;
    time_t   pubtime;
    int      charset;
    GSList*  categories;
} DocInfo;
