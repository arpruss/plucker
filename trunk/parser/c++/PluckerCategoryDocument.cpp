/*
 * $Id: PluckerCategoryDocument.cpp,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#include "PluckerCategoryDocument.h"

PluckerCategoryDocument::PluckerCategoryDocument(std::string category)
{
    m_id = 4;
    m_urlString = "plucker:/~special~/categories";
    m_category = category;
}

PluckerCategoryDocument::~PluckerCategoryDocument()
{

}

void PluckerCategoryDocument::compress(int compression)
{
    m_compression = compression;
}

int PluckerCategoryDocument::getDocumentSize()
{
    return 8 + m_category.size() + 1;
}

void PluckerCategoryDocument::writeContents(Writer* writer)
{
    // write id
    writer->writeInt(m_id);

    // write number of paragraphs
    writer->writeInt(0);

    // write size
    int size = m_category.size() + 1;
    writer->writeInt(size);

    // write type
    writer->writeByte(0x09);
    writer->writeByte(0);

    for (size_t i = 0; i < m_category.size(); i++) {
        writer->writeByte(m_category[i]);
    }
    writer->writeByte(0);
}

