/*
 * $Id: PluckerIndexDocument.cpp,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#include "PluckerIndexDocument.h"

PluckerIndexDocument::PluckerIndexDocument(bool hasLinkIndex, bool hasCategoryInfo)
{
    m_id = 1;   // always id 1
    m_numReserved = 1;
    m_urlString = "plucker:/~special~/index";
    m_hasLinkIndex = hasLinkIndex;
    if (m_hasLinkIndex) {
        ++m_numReserved;
    }
    m_hasCategoryInfo = hasCategoryInfo;
    if (m_hasCategoryInfo) {
        ++m_numReserved;
    }
    // meta data
//    ++m_numReserved;
}

PluckerIndexDocument::~PluckerIndexDocument()
{
}

void PluckerIndexDocument::compress(int compression)
{
    m_compression = compression;
}

int PluckerIndexDocument::getDocumentSize()
{
    return 6 + 4 * m_numReserved;
}

void PluckerIndexDocument::writeContents(Writer* writer)
{
    // write id, should always be 1
    writer->writeInt(m_id);

    // write version (compression)
    if (m_compression == 1)
        writer->writeInt(1);
    else
        writer->writeInt(2);

    // write number of reserved records
    writer->writeInt(m_numReserved);

    // home.html reserved record, always record 2
    writer->writeInt(0);
    writer->writeInt(2);

    // write reserved array
    // link index always record 3
    if (m_hasLinkIndex) {
        writer->writeInt(2);
        writer->writeInt(3);
    }

    // category info always record 4
    if (m_hasCategoryInfo) {
        writer->writeInt(3);
        writer->writeInt(4);
    }

    // meta data always record 5
//    writer->writeInt(4);
//    writer->writeInt(5);
}
