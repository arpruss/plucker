/*
 * $Id: Resolver.h,v 1.4 2007/10/10 01:17:26 chrish Exp $
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

#if !defined(RESOLVER_H)
#define RESOLVER_H

#include <map>
#include <string>
#include <vector>

struct AnchorLocation {
    int document;
    int paragraph;
};

class Resolver  
{
public:
	Resolver();
	virtual ~Resolver();

    void add(std::string link, int id);
    bool getOrAdd(std::string link, int *id);
    bool hasResolved(std::string link);
    int getLastId() {return m_lastId;}
    std::string findLinkById(int id);

    void addName(std::string name, int doc, int para);
    int addAnchor(std::string name);
    struct AnchorLocation findName(unsigned int idx);

    std::map<std::string, int>::iterator getLinkMapBegin() {return m_linkmap.begin();}
    std::map<std::string, int>::iterator getLinkMapEnd() {return m_linkmap.end();}

private:
    int m_lastId;
    std::map<std::string, int> m_linkmap;

    std::map<std::string, struct AnchorLocation> m_nameMap;
    std::vector<std::string> m_anchorMap;
};

#endif // !defined(RESOLVER_H)
