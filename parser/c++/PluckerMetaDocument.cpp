/*
 * $Id: PluckerMetaDocument.cpp,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#include "PluckerMetaDocument.h"

PluckerMetaDocument::PluckerMetaDocument()
{
    m_id = 5;
    m_urlString = "plucker:/~special~/metadata";
    m_contentSize = 0;
}

PluckerMetaDocument::~PluckerMetaDocument()
{
    for (int i = 0; i < m_subRecords.size(); i++) {        
        delete m_subRecords[i];
    }
}

void PluckerMetaDocument::compress(int compression)
{
    m_compression = compression;
}

int PluckerMetaDocument::getDocumentSize()
{
    return 8 + m_contentSize;
}

void PluckerMetaDocument::writeContents(Writer* writer)
{
    // write id
    writer->writeInt(m_id);

    // write number of paragraphs
    writer->writeInt(0);

    // write size
    int size = m_contentSize;
    writer->writeInt(size);

    // write type
    writer->writeByte(0x0A);
    // write number of subrecords
    writer->writeByte(m_subRecords.size());

    // write links
    for (int i = 0; i < m_subRecords.size(); i++) {
        PluckerMetaSubRecord* subRecord = m_subRecords[i];
        subRecord->writeContents(writer);
    }
}

void PluckerMetaDocument::addTitle(std::string title)
{
    PluckerMetaSubRecord* subRecord = new PluckerMetaSubRecord(5);
    m_subRecords.push_back(subRecord);
    subRecord->setString(title);
    m_contentSize += subRecord->getDocumentSize();
}

void PluckerMetaDocument::addAuthor(std::string author)
{
    PluckerMetaSubRecord* subRecord = new PluckerMetaSubRecord(4);
    m_subRecords.push_back(subRecord);
    subRecord->setString(author);
    m_contentSize += subRecord->getDocumentSize();
}
