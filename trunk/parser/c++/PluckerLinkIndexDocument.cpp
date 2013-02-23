/*
 * $Id: PluckerLinkIndexDocument.cpp,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#include "PluckerLinkIndexDocument.h"

PluckerLinkIndexDocument::PluckerLinkIndexDocument() : PluckerDocument()
{
    m_id = 3;
    m_urlString = "plucker:/~special~/pluckerlinks";
}

PluckerLinkIndexDocument::~PluckerLinkIndexDocument()
{
}

void PluckerLinkIndexDocument::compress(int compression)
{
    m_compression = compression;
}

void PluckerLinkIndexDocument::addLinkDoc(int maxUrl, int id)
{
    m_lastUrlIds.push_back(maxUrl);
    m_recordIds.push_back(id);
}

int PluckerLinkIndexDocument::getDocumentSize()
{
    return 8 + 4 * m_lastUrlIds.size();
}

void PluckerLinkIndexDocument::writeContents(Writer* writer)
{
    // write id
    writer->writeInt(m_id);

    // write number of paragraphs
    writer->writeInt(0);

    // write size
    int size = 4 * m_lastUrlIds.size();
    writer->writeInt(size);

    // write type
    writer->writeByte(0x05);
    writer->writeByte(0);

    // write links
    for (int i = 0; i < m_lastUrlIds.size(); i++) {
        writer->writeInt(m_lastUrlIds[i]);
        writer->writeInt(m_recordIds[i]);
    }
}

void PluckerLinkIndexDocument::createLinkDocuments(Resolver* resolver)
{
    int lastId = resolver->getLastId() + 1;

    std::vector<std::string> sorted;
    sorted.resize(lastId);
    for (int i = 0; i < lastId; i++) {
        sorted[i] = "";
    }
    
    std::map<std::string, int>::iterator it;
    for(it = resolver->getLinkMapBegin(); it != resolver->getLinkMapEnd(); ++it) {
        std::string url = it->first;
        int id = it->second;

        if (id <= lastId && url.substr(0, 7) != "mailto:")
            sorted[id] = url;
    }
    
    int linkDocs = 1;
    PluckerLinksDocument* linkDoc = new PluckerLinksDocument(lastId++, linkDocs++);
    m_linkDocs.push_back(linkDoc);
    int linkDocNumLinks = 0;

    for (int i = 1; i < sorted.size(); i++) {
        std::string url = sorted[i];
        linkDoc->addLink(url);
        ++linkDocNumLinks;

        if (linkDocNumLinks > 200) {
            addLinkDoc(i, linkDoc->getID());

            linkDoc = new PluckerLinksDocument(lastId++, linkDocs++);
            m_linkDocs.push_back(linkDoc);
            linkDocNumLinks = 0;
        }
    }

    if (linkDocNumLinks > 0) {
        addLinkDoc(sorted.size() - 1, linkDoc->getID());
    }
}
