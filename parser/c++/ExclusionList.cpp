/*
 * $Id: ExclusionList.cpp,v 1.4 2005/04/22 19:42:25 chrish Exp $
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


#include "ExclusionList.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fstream>


ExclusionList::ExclusionList(void)
{
}

ExclusionList::~ExclusionList(void)
{
}

bool ExclusionList::LoadFile(const char* filename)
{
    std::ifstream fis(filename);
    std::string line;

    if (fis != 0) {
        while (!fis.eof()) {
            std::getline(fis, line);
            while (isspace(line[0]))
                line.erase(0, 1);
            if (line[0] == '#')
                continue;
            if (line.length())
                AddEntry(line);
        }
    }
}

bool ExclusionList::AddEntry(std::string expression)
{
    Pattern* pat;
    Matcher* mat;
    int prio = 0;
    bool action = false;
//    std::string str = "Priority \\1  Action \\2  Pattern \\3";


    pat = Pattern::compile("(\\d+):([\\+\\-]):(.*)");
    mat = pat->createMatcher("");

    mat->setString(expression);
    if(mat->matches()) {
//        printf("%s\n", mat->replaceWithGroups(str).c_str());

        prio = atoi(mat->getGroup(1).c_str());
        action = mat->getGroup(2) == "+";

        if(Pattern::compile(mat->getGroup(3)) == NULL)
            return false;

        MYMAP::iterator it;
        it = m_lists.find(prio);
        if(it == m_lists.end()) {
            std::map<std::string, bool> newMap;
            newMap[mat->getGroup(3)] = action;
            m_lists[prio] = newMap;
        }
        else {
            it->second[mat->getGroup(3)] = action;
        }

        return true;
    }
    return false;
}

bool ExclusionList::Check(Url* url)
{
    MYMAP::iterator it;

    for(it = m_lists.begin(); it != m_lists.end(); ++it)
    {
        std::map<std::string, bool> thisMap = it->second;
        std::map<std::string, bool>::iterator mIt;
        for(mIt = thisMap.begin(); mIt != thisMap.end(); ++mIt)
            if(Pattern::matches(mIt->first, url->getUrlString()))
                return mIt->second;
    }
    return true;
}


