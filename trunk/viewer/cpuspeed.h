/*
 * $Id: cpuspeed.h,v 1.2 2005/04/04 22:18:32 prussar Exp $
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

#ifndef PLUCKER_CPUSPEED_H
#define PLUCKER_CPUSPEED_H

#include "viewer.h"

#ifdef SUPPORT_CPUSPEED

/* Put SpeedFaster(); ... SpeedRestore(); around any code that needs to run
   at a higher CPU speed.  SpeedRestore() restores the speed to that which
   was operative before the SpeedFaster() call.  Currently, only two speeds
   are available: normal and fast.  Moreover, only the Lightspeed CPU speed setter
   is currently supported.  SpeedFaster() and SpeedRestore() pairs must be
   matched, but can be nested to an arbitrary depth (OK, depth is UInt32). Note
   that calling SpeedFaster() multiple times does not increase the speed beyond
   a single call. However, the nesting lets you do SpeedFaster(); FuncA();
   FuncB(); SpeedRestore(); even if FuncA() itself calls some function that
   does SpeedFaster(); ... SpeedRestore();.

   The cost of SpeedFaster() is two-fold.  First, there is a small delay when
   switching speeds (eyeballing, this seems to be on the order of 0.1 seconds
   but it has not been benchmarked).  Second, power usage is significantly
   increased.  Thus, the faster speed should NOT be enabled while waiting for
   user input, but only when doing computationally intensive stuff, and only
   when the stuff takes long enough that the speed-switching delay is 
   worthwhile.  The current Plucker code increases speed for searching, page
   rendering and auto-scrolling if SUPPORT_CPUSPEED is set. */

extern void SpeedFaster( void ) CPUSPEED_SECTION;
extern void SpeedRestore( void ) CPUSPEED_SECTION;
#else
#define SpeedFaster()
#define SpeedRestore()
#endif

#endif

