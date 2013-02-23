/*
 * $Id: TextParser.cpp,v 1.3 2005/04/06 20:35:06 chrish Exp $
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

#include <sstream>

#include "TextParser.h"

TextParser::TextParser(Url* url, Resolver* resolver)
{
    m_resolver = resolver;
    m_url = url;

    int newId = url->m_id;
    m_currentDocument = new PluckerTextDocument(newId, url->getUrlString());
    m_currentDocumentSize = 0;
    m_documents.push_back(m_currentDocument);
}

TextParser::~TextParser()
{
}

void TextParser::addDocEstSize(int size, bool canStart)
{
    // keep track of estimated size and possibly
    // create new document if needed
    if (m_currentDocumentSize + size > MAX_DOCUMENT_SIZE && canStart) {
        int oldId = m_currentDocument->getID();
        std::string oldhref = m_currentDocument->getUrlString();
        std::stringstream ss;
        ss << "plucker:/~parts~/" << m_url->getUrlString() << "/" << m_documents.size();
        std::string newhref = ss.str();
        int newId;
        m_resolver->getOrAdd(newhref, &newId);

        // link to new part
        startAnchor(newId);
        m_currentDocument->addText("Click here for the next part");
        stopAnchor();

        // create new document
        m_currentDocument = new PluckerTextDocument(newId, newhref);
        m_currentDocumentSize = 40;
        m_documents.push_back(m_currentDocument);

        // link back to old part
        startAnchor(oldId);
        m_currentDocument->addText("Click here for the previous part");
        stopAnchor();
    }

    m_currentDocumentSize += size;
}

void TextParser::processData(Config* config, const unsigned char* data, unsigned long size)
{
    for (int i = 0; i < size; i++) {
        if (data[i] == '\n') {
            m_currentDocument->addVSpace(0);
        }
        
        if (data[i] != '\n' && data[i] != '\t' && data[i] != '\r') {
            addDocEstSize(1, true);
            m_currentDocument->addChar(data[i]);
        }
    }
}

void TextParser::startAnchor(int id)
{
    addDocEstSize(6, false);
    m_currentDocument->startAnchor(id);
}

void TextParser::stopAnchor()
{
    addDocEstSize(2, false);
    m_currentDocument->stopAnchor();
}

