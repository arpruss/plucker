/*
 * $Id: PluckerMailtoDocument.cpp,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#include "GlobalFunctions.h"
#include "PluckerMailtoDocument.h"

PluckerMailtoDocument::PluckerMailtoDocument(int id) : PluckerDocument()
{
    m_id = id;
}

PluckerMailtoDocument::~PluckerMailtoDocument()
{
}

void PluckerMailtoDocument::compress(int compression)
{
    m_compression = compression;
}

int PluckerMailtoDocument::getDocumentSize()
{
    int size = 8;
    size += 8;
    size += m_to.length() + 1;
    size += m_cc.length() + 1;
    size += m_subject.length() + 1;
    size += m_body.length() + 1;

    return size;
}

void PluckerMailtoDocument::writeContents(Writer* writer)
{
    // write id
    writer->writeInt(m_id);

    // write number of paragraphs
    writer->writeInt(0);

    int size = 0;
    size += m_to.length() + 1;
    size += m_cc.length() + 1;
    size += m_subject.length() + 1;
    size += m_body.length() + 1;

    // write total length
    writer->writeInt(size);

    // write type
    writer->writeByte(0x04);
    writer->writeByte(0);

    // write to offsets
    int offset = 8;
    writer->writeInt(offset);
    offset += m_to.length() + 1;
    writer->writeInt(offset);
    offset += m_cc.length() + 1;
    writer->writeInt(offset);
    offset += m_subject.length() + 1;
    writer->writeInt(offset);

    // write to
    for (size_t i = 0; i < m_to.length(); i++)
        writer->writeByte(m_to[i]);
    writer->writeByte('\0');

    // write cc
    for (size_t i = 0; i < m_cc.length(); i++)
        writer->writeByte(m_cc[i]);
    writer->writeByte('\0');

    // write subject
    for (size_t i = 0; i < m_subject.length(); i++)
        writer->writeByte(m_subject[i]);
    writer->writeByte('\0');

    // write body
    for (size_t i = 0; i < m_body.length(); i++)
        writer->writeByte(m_body[i]);
    writer->writeByte('\0');
}

std::string PluckerMailtoDocument::findPart(std::string startTag, std::string endTag)
{
    int atStart = m_lowerUrlString.find(startTag);
    if (atStart == std::string::npos)
        return "";

    atStart += startTag.length();

    int atStop = m_lowerUrlString.find(endTag, atStart);
    if (atStop == std::string::npos)
        atStop = m_lowerUrlString.length();

    if (atStart >= atStop) 
        return "";

    std::string part = m_urlString.substr(atStart, atStop - atStart);

    return part;
}

void PluckerMailtoDocument::parseMailUrl(std::string url)
{
    m_urlString = url;
    m_lowerUrlString = toLower(url);
    m_to = findPart("mailto:", "?");
    m_cc = findPart("cc=", "&");
    m_subject = findPart("subject=", "&");
    m_body = findPart("body=", "&");    
}
