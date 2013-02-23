/*
 * $Id: PluckerMultiImageDocument.cpp,v 1.1 2005/04/09 20:16:03 chrish Exp $
 *
 * Cpluck - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 2005 Christopher R. Hawks (chawks@dls.net)
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

#include "PluckerMultiImageDocument.h"

PluckerMultiImageDocument::PluckerMultiImageDocument(int id, std::string urlString)
{
    m_id = id;
    m_urlString = urlString;
    m_cols = 0;
    m_rows = 0;

}

PluckerMultiImageDocument::~PluckerMultiImageDocument()
{
}

int PluckerMultiImageDocument::getDocumentSize()
{
    return 8 + 4 + (m_docIds.size() * 2);
}

void PluckerMultiImageDocument::writeContents(Writer* writer)
{
    // write id
    writer->writeInt(m_id);

    // write number of paragraphs
    writer->writeInt(0);

    // write total length
    writer->writeInt(4 + (m_docIds.size() * 2));

    // write type
    writer->writeByte(0x0f);
    writer->writeByte(0);


    // write data
    writer->writeInt(m_cols);
    writer->writeInt(m_rows);
    for (int i = 0; i < m_docIds.size(); i++)
        writer->writeInt(m_docIds[i]);
}

void PluckerMultiImageDocument::setSize(int cols, int rows)
{
    m_cols = cols;
    m_rows = rows;
}

void PluckerMultiImageDocument::addDocId(int id)
{
    m_docIds.push_back(id);
}
