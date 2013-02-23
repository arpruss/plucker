/*
 * $Id: ExclusionList.h,v 1.3 2005/04/22 19:42:25 chrish Exp $
 *
 * Cpluck - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 2005 Christopher R. Hawks (chawks@dls.net)
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

#if !defined(EXCLUSIONLIST_H)
#define EXCLUSIONLIST_H

#include <map>
#include <string>
#include "Url.h"
#include "Pattern.h"
#include "Matcher.h"

#define MYMAP std::map<int, std::map<std::string,bool >, std::greater<int> >

class ExclusionList
{
public:
    ExclusionList(void);
    ~ExclusionList(void);
    bool LoadFile(const char* filename);
    bool AddEntry(std::string expression);
    bool Check(Url* url);

private:
    MYMAP m_lists;
};
#endif
