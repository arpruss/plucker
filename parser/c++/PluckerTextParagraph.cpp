/*
 * $Id: PluckerTextParagraph.cpp,v 1.7 2007/10/10 01:17:26 chrish Exp $
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

#include "PluckerTextParagraph.h"

PluckerTextParagraph::PluckerTextParagraph()
{
    m_attributes = 0;
    m_empty = true;
    m_newline = true;
    m_spacing = 0;
}

PluckerTextParagraph::~PluckerTextParagraph()
{
}

void PluckerTextParagraph::finishDocument(Resolver* resolver, Config* config)
{
    int size = m_content.length();
    if (size > 6) {
        for (int i = 0; i < size - 6; i++) {
            // go through to find images that may have alternate big images too
            if (m_content[i] == 0x00) {
                if(m_content[i + 1] == 0x5c) {
                    // need to set byte at i + 2, i + 3 to
                    // id of alternate image if it exists
                    int id = (m_content[i + 2] << 8) + m_content[i + 3];
                    std::string url = resolver->findLinkById(id);
                    std::string big_url = url + "#bigimage";
                    std::string multi_url = url + "#multi";
                    if (resolver->hasResolved(big_url)) {
                        int big_id;
                        resolver->getOrAdd(big_url, &big_id);
                        unsigned char hi = (big_id & 0xff00) >> 8;
                        unsigned char lo = big_id & 0x00ff;
                        m_content[i + 2] = hi;
                        m_content[i + 3] = lo;
                    }
                    else if (resolver->hasResolved(multi_url)) {
                        int multi_id;
                        resolver->getOrAdd(multi_url, &multi_id);
                        unsigned char hi = (multi_id & 0xff00) >> 8;
                        unsigned char lo = multi_id & 0x00ff;
                        m_content[i + 2] = hi;
                        m_content[i + 3] = lo;
                    }
                    else {
                        // Change it to a single image
                        m_content[i + 1] = 0x1a;
                        m_content.erase(i + 2, 2);
                        size -= 2;
                    }
                }
                else if (m_content[i + 1] == 0x0C) {
                    struct AnchorLocation loc;
                    unsigned int idx;
                    // Is Paragraph Anchor
                    // Set paragraph number
                    unsigned char hi = m_content[i + 4];
                    unsigned char lo = m_content[i + 5];
                    idx = (hi << 8) | lo;
                    loc = resolver->findName(idx);
                    m_content[i + 2] = loc.document >> 8;
                    m_content[i + 3] = loc.document;
                    m_content[i + 4] = loc.paragraph >> 8;
                    m_content[i + 5] = loc.paragraph;
                }
                // Skip rest of tag
                i += (m_content[i + 1] & 0x07) + 1;
            }
        }

    }
}

void PluckerTextParagraph::add(const char* txt)
{
    m_empty = false;
    m_newline = false;
    m_content.append(txt);
}

void PluckerTextParagraph::addChar(const char data)
{
    m_empty = false;
    m_newline = false;
    m_content.append(1, data);
}

void PluckerTextParagraph::addInt(int value)
{
    unsigned char hi = (value & 0xff00) >> 8;
    unsigned char lo = value & 0x00ff;

    addChar(hi);
    addChar(lo);
}

int PluckerTextParagraph::getUncompressedSize()
{
    return m_content.length();
}
