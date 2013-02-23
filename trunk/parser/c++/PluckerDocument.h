/*
 * $Id: PluckerDocument.h,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#if !defined(PLUCKERDOCUMENT_H)
#define PLUCKERDOCUMENT_H

#include <string>
#include <vector>

#include "Config.h"
#include "Resolver.h"
#include "Url.h"
#include "Writer.h"

#define MAX_DOCUMENT_SIZE 30000

// Base class for plucker document (record) types
class PluckerDocument  
{
public:
	PluckerDocument();
	virtual ~PluckerDocument();

    int getID() {return m_id;}
    std::string getUrlString() {return m_urlString;}

    int getNumLinksToFetch() {return m_links.size();}
    Url* getLinkToFetch(int index) {return m_links[index];}

    virtual void compress(int compression) = 0;
    virtual int getDocumentSize() = 0;
    virtual void writeContents(Writer* writer) = 0;
    virtual void finishDocument(Resolver* resolver, Config* config) = 0;

protected:
    int m_id;
    int m_compression;
    std::string m_urlString;
    std::vector<Url*> m_links;
};

#endif // PLUCKERDOCUMENT_H
