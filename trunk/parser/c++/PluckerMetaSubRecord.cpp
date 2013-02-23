/*
 * $Id: PluckerMetaSubRecord.cpp,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#include "PluckerMetaSubRecord.h"

PluckerMetaSubRecord::PluckerMetaSubRecord(int type)
{
    m_type = type;
    m_length = 0;
}

PluckerMetaSubRecord::~PluckerMetaSubRecord()
{
}

int PluckerMetaSubRecord::getDocumentSize()
{
    return 4 + 2 * m_length;
}

void PluckerMetaSubRecord::writeContents(Writer* writer)
{
    writer->writeInt(m_type);
    writer->writeInt(m_length);
    for (int i = 0; i < m_content.length(); i++) {
        writer->writeByte(m_content[i]);
    }
}

void PluckerMetaSubRecord::setString(std::string data)
{
    switch (m_type) {
    case 5:
        m_content = data;
        int numExtra = m_content.length() % 2;
        m_length = m_content.length() / 2 + numExtra;
        if (numExtra == 1) {
            m_content.append(1, (char) 0x0);
        }
        break;
    }
}
