/*
 * $Id: Resolver.cpp,v 1.4 2007/10/10 01:17:26 chrish Exp $
 *
 * Cpluck - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 2003 Bill Nalen (bill@nalens.com) 
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

#pragma warning( disable : 4786 )

#include "Resolver.h"

Resolver::Resolver()
{
    m_lastId = 10;
}

Resolver::~Resolver()
{
}

void Resolver::add(std::string link, int id)
{
    m_linkmap[link] = id;
}

bool Resolver::hasResolved(std::string link)
{
    if (m_linkmap.find(link) == m_linkmap.end()) {
        return false;
    }
    else {
        return true;
    } 
}

bool Resolver::getOrAdd(std::string link, int *id)
{
    bool isNew = false;
    *id = 0;

    std::map<std::string, int>::iterator it = m_linkmap.find(link);
    if (it == m_linkmap.end()) {
        // not found so add
        m_linkmap[link] = m_lastId;
        *id = m_lastId;
        ++m_lastId;
        isNew = true;
    }
    else {
        *id = it->second;
    }

    return isNew;
}

std::string Resolver::findLinkById(int id)
{
    std::map<std::string, int>::iterator it;
    for(it = m_linkmap.begin(); it != m_linkmap.end(); ++it) {
        std::string url = it->first;
        int resolved_id = it->second;

        if (id == resolved_id) {
            return url;
        }
    }

    return "";
}

void Resolver::addName(std::string name, int doc, int para)
{
    struct AnchorLocation loc;

    loc.document = doc;
    loc.paragraph = para;
	m_nameMap[name] = loc;
}

int Resolver::addAnchor(std::string name)
{
    m_anchorMap.push_back(name);
    return m_anchorMap.size() - 1;
}

struct AnchorLocation Resolver::findName(unsigned int idx)
{
    std::string name = m_anchorMap[idx];
	return m_nameMap[name];
}


