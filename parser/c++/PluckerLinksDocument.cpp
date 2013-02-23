/*
 * $Id: PluckerLinksDocument.cpp,v 1.3 2005/04/27 22:19:09 chrish Exp $
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

#include "PluckerLinksDocument.h"

PluckerLinksDocument::PluckerLinksDocument(int id, int linkNum) : PluckerDocument()
{
    m_linksSize = 0;
    m_id = id;
    m_compressedSize = 0;
    m_compressedLinks = 0;

    std::stringstream ss;
    ss << "plucker:/~special~/links" << linkNum;
    m_urlString = ss.str();
}

PluckerLinksDocument::~PluckerLinksDocument()
{
    delete [] m_compressedLinks;
}

void PluckerLinksDocument::ZlibCompressFunction()
{
	m_compressedSize = m_links.length() * 2 * sizeof(Byte);
    if (m_compressedLinks != 0) {
        delete [] m_compressedLinks;
        m_compressedLinks = 0;
    }
	m_compressedLinks = new Byte[m_compressedSize];

    ::compress(m_compressedLinks, &m_compressedSize, (const unsigned char*) m_links.c_str(), m_links.length());
}

void PluckerLinksDocument::compress(int compression)
{
    m_compression = compression;

    if (m_compression == 2) {
        ZlibCompressFunction();
    }
}

void PluckerLinksDocument::addLink(std::string link)
{
    m_linksSize += link.size() + 1;
    m_links.append(link);
    m_links.append(1, (char) 0x0);
}

int PluckerLinksDocument::getDocumentSize()
{
    if (m_linksSize < m_compressedSize || m_compressedSize == 0)
        return 8 + m_linksSize;
    else 
        return 8 + m_compressedSize;
}

void PluckerLinksDocument::writeContents(Writer* writer)
{
    // write id
    writer->writeInt(m_id);

    // write number of paragraphs
    writer->writeInt(0);

    // write size
    writer->writeInt(m_linksSize);

    // write type
    if (m_linksSize < m_compressedSize || m_compressedSize == 0) {
        writer->writeByte(0x06);
        writer->writeByte(0);
        for (unsigned long i = 0; i < m_linksSize; i++) {
            writer->writeByte(m_links[i]);
        }
    }
    else {
        writer->writeByte(0x07);
        writer->writeByte(0);
        for (unsigned long i = 0; i < m_compressedSize; i++) {
            writer->writeByte(m_compressedLinks[i]);
        }
    }  
}
