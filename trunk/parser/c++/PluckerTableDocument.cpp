/*
 * $Id: PluckerTableDocument.cpp,v 1.10 2005/05/12 03:04:23 chrish Exp $
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

#pragma warning( disable : 4786 )

#include "PluckerTableDocument.h"

#define CURR_CELL m_rows[m_currRow - 1][m_currCol - 1]

PluckerTableDocument::PluckerTableDocument(int id, std::string urlString, int border, int borderColor, int linkColor)
{
    m_id = id;
    m_urlString = urlString;
    m_border = border;
    m_borderColor = borderColor;
    m_linkColor = linkColor;
    m_compressedSize = 0;
    m_compressedContent = 0;
    m_contentSize = 0;
    m_compression = 0;
    m_currRow = 0;
    m_currCol = 0;
    m_maxCol = 0;
}

PluckerTableDocument::~PluckerTableDocument()
{
    delete [] m_compressedContent;
}

void PluckerTableDocument::ZlibCompressFunction()
{
	m_compressedSize = m_content.length() * 2 * sizeof(Byte);
    delete [] m_compressedContent;
	m_compressedContent = new Byte[m_compressedSize];

    ::compress(m_compressedContent, &m_compressedSize, (const unsigned char*) m_content.c_str(), m_content.length());
}

void PluckerTableDocument::compress(int compression)
{
    m_compression = compression;

    if (compression == 2) {
        ZlibCompressFunction();
    }
}

void PluckerTableDocument::finishDocument(Resolver* resolver, Config* config)
{
    int col, row;
    std::string body;

    // calc_table
    for(row = 0; row < m_rows.size(); row++) {
        unsigned int rowcols = 0;
        for(col = 0; col < m_rows[row].size(); col++) {
            rowcols += m_rows[row][col].colSpan;
            if (rowcols > m_maxCol)
                m_maxCol = rowcols;
        }
    }

    for(row = 0; row < m_rows.size(); row++) {
        for(col = 0; col < m_rows[row].size(); col++) {
            if (m_rows[row][col].text.length()) {
                int size = m_rows[row][col].text.length();
                if (size > 5) {
                    for (int i = 0; i < size - 5; i++) {
                        // go through to find images that may
                        // have alternate big images too
                        if (m_rows[row][col].text[i] == 0x00
                            && m_rows[row][col].text[i + 1] == 0x5c) {
                            // need to set byte at i + 2, i + 3 to
                            // id of alternate image if it exists
                            int id = (m_rows[row][col].text[i + 2] << 8)
                                      + m_rows[row][col].text[i + 3];
                            std::string url = resolver->findLinkById(id);
                            std::string big_url = url + "#bigimage";
                            std::string multi_url = url + "#multi";
                            if (resolver->hasResolved(big_url)) {
                                int big_id;
                                resolver->getOrAdd(big_url, &big_id);
                                unsigned char hi = (big_id & 0xff00) >> 8;
                                unsigned char lo = big_id & 0x00ff;
                                m_rows[row][col].text[i + 2] = hi;
                                m_rows[row][col].text[i + 3] = lo;
                            }
                            else if (resolver->hasResolved(multi_url)) {
                                int multi_id;
                                resolver->getOrAdd(multi_url, &multi_id);
                                unsigned char hi = (multi_id & 0xff00) >> 8;
                                unsigned char lo = multi_id & 0x00ff;
                                m_rows[row][col].text[i + 2] = hi;
                                m_rows[row][col].text[i + 3] = lo;
                            }
                            else {
                                // Change it to a single image
                                m_rows[row][col].text[i + 1] = 0x1a;
                                m_rows[row][col].text.erase(i + 2, 2);
                                size -= 2;
                            }
                        }
                    }
                }
                m_rows[row][col].text = splitLine(m_rows[row][col].text, m_rows[row][col].colSpan);
            }
            else
                m_rows[row][col].text += (char)0xa0;
        }
    }

    for(row = 0; row < m_rows.size(); row++) {
        while (m_rows[row].size() < m_maxCol) {
            struct TableCell newCell;
            newCell.align = 0;
            newCell.colSpan = 1;
            newCell.rowSpan = 1;
            newCell.imageRef = 0;
            m_rows[row].push_back(newCell);
        }
    }

    // dump_record
    for(row = 0; row < m_rows.size(); row++) {
        body += (char)0x00;
        body += (char)0x90;
        for(col = 0; col < m_rows[row].size(); col++) {
            body += (char)0x00;
            body += (char)0x97;
            body += (char)m_rows[row][col].align & 0xff;
            body += (char)(m_rows[row][col].imageRef >> 8) & 0xff;
            body += (char)m_rows[row][col].imageRef & 0xff;
            body += (char)m_rows[row][col].colSpan & 0xff;
            body += (char)m_rows[row][col].rowSpan & 0xff;
            body += (char)(m_rows[row][col].text.length() >> 8) & 0xff;
            body += (char)m_rows[row][col].text.length() & 0xff;
            for (int i = 0; i < m_rows[row][col].text.length(); i++)
                body += (char)m_rows[row][col].text[i];
        }
    }
    m_content += (char)(body.length() >> 8) & 0xff;    // Content size
    m_content += (char)body.length() & 0xff;
    m_content += (char)m_maxCol >> 8;
    m_content += (char)m_maxCol & 0xff;
    m_content += (char)m_rows.size() >> 8;
    m_content += (char)m_rows.size() & 0xff;
    m_content += (char)config->m_bpp & 0xff;
    m_content += (char)m_border;
    m_content += (char)(m_borderColor >> 24) & 0xff;
    m_content += (char)(m_borderColor >> 16) & 0xff;
    m_content += (char)(m_borderColor >> 8) & 0xff;
    m_content += (char)m_borderColor & 0xff;
    m_content += (char)(m_linkColor >> 24) & 0xff;
    m_content += (char)(m_linkColor >> 16) & 0xff;
    m_content += (char)(m_linkColor >> 8) & 0xff;
    m_content += (char)m_linkColor & 0xff;

    m_content.append(body);
    m_contentSize = m_content.length();
}

void PluckerTableDocument::writeContents(Writer* writer)
{
    // write id
    writer->writeInt(m_id);

    // write number of paragraphs
    writer->writeInt(0);

    // write total length
    writer->writeInt(m_contentSize);

    // write type
    if (m_contentSize <= m_compressedSize || m_compressedSize == 0)
        writer->writeByte(0x0d);
    else
        writer->writeByte(0x0e);
    writer->writeByte(0);

    if (m_contentSize <= m_compressedSize || m_compressedSize == 0) {
        for (int i = 0; i < m_content.length(); i++) {
            writer->writeByte(m_content[i]);
        } 
    }
    else {
        for (int i = 0; i < m_compressedSize; i++) {
            writer->writeByte(m_compressedContent[i]);
        } 
    }
}

int PluckerTableDocument::getDocumentSize()
{
    int size = 0;

    if (m_contentSize <= m_compressedSize || m_compressedSize == 0) 
        size = m_contentSize;
    else
        size = m_compressedSize;

    size += 8;

    return size;
}

void PluckerTableDocument::addRow()
{
    if (m_currCol > m_maxCol)
        m_maxCol = m_currCol;
    std::vector<struct TableCell> newRow;
    m_rows.push_back(newRow);
    m_currCol = 0;
    m_currRow++;
}

void PluckerTableDocument::addCell(int align, int colSpan, int rowSpan)
{
    struct TableCell newCell;
    newCell.align = align;
    newCell.colSpan = colSpan;
    newCell.rowSpan = rowSpan;
    newCell.imageRef = 0;
    if (0 == m_currRow)
        addRow();
    m_rows[m_currRow - 1].push_back(newCell);
    m_currCol++;
}

void PluckerTableDocument::addText(char* text)
{
    if (m_currCol && m_currRow)
        CURR_CELL.text += text;
}

void PluckerTableDocument::addChar(char data)
{
    if (m_currCol && m_currRow)
        CURR_CELL.text += data;
}

void PluckerTableDocument::addNewLine()
{
    addChar(0x00);
    addChar(0x38);
}

void PluckerTableDocument::startStyle(int style)
{
    addChar(0x00);
    addChar(0x11);
    addChar(style);
}

void PluckerTableDocument::stopStyle(int oldStyle)
{
    addChar(0x00);
    addChar(0x11);
    addChar(oldStyle);
}

void PluckerTableDocument::startItalics()
{
    addChar(0x00);
    addChar(0x40);
}

void PluckerTableDocument::stopItalics()
{
    addChar(0x00);
    addChar(0x48);
}

void PluckerTableDocument::startUnderline()
{
    addChar(0x00);
    addChar(0x60);
}

void PluckerTableDocument::stopUnderline()
{
    addChar(0x00);
    addChar(0x68);
}

void PluckerTableDocument::startStrike()
{
    addChar(0x00);
    addChar(0x70);
}

void PluckerTableDocument::stopStrike()
{
    addChar(0x00);
    addChar(0x78);
}

void PluckerTableDocument::startAnchor(int id)
{
    // add anchor to know id
    addChar(0x00);
    addChar(0x0A);
    addChar(id >> 8);
    addChar(id & 0xff);
}


void PluckerTableDocument::startAnchor(std::string href, int maxDepth, int stayOnHost, Url* url, Resolver* resolver, bool tables)
{
    Url* newUrl = new Url(href, url);

    int id;
    bool isNew = resolver->getOrAdd(newUrl->getUrlString(), &id);
    newUrl->m_id = id;
    if (!newUrl->m_tables)
        newUrl->m_tables = tables;
    if (maxDepth != -1) {
        newUrl->m_maxDepth = maxDepth;
    }
    if (stayOnHost != -1) {
        newUrl->setStayOnHost(stayOnHost);
    }

    addChar(0x00);
    addChar(0x0A);
    addChar(id >> 8);
    addChar(id & 0xff);

    if (newUrl->shouldFetch() && isNew) {
        // add to list of those to fetch, will be deleted by parser
        m_links.push_back(newUrl);
    }
    else {
        delete newUrl;
    }
}

void PluckerTableDocument::stopAnchor()
{
    addChar(0x00);
    addChar(0x08);
}

void PluckerTableDocument::startLI(std::string text)
{
    for (int i = 0; i < text.length(); i++)
        addChar(text[i]);
}

void PluckerTableDocument::startImage(std::string href, int maxDepth, std::string alt, Url* url, Resolver* resolver)
{
    Url* newUrl = new Url(href, url);

    int id;
    bool isNew = resolver->getOrAdd(newUrl->getUrlString(), &id);
    newUrl->m_id = id;
    if (maxDepth != -1) {
        newUrl->m_maxDepth = maxDepth;
    }
    else {
        newUrl->m_maxDepth = url->m_maxDepth; // inline image
    }

    if (newUrl->shouldFetch() && newUrl->m_bpp > 0) {
        addChar(0x00); // Will be changed when resolved
        addChar(0x5c); // if no BigId document is found
        addChar(id >> 8);
        addChar(id & 0xff);
        addChar(id >> 8);
        addChar(id & 0xff);

        // add to list of those to fetch, will be deleted by parser
        if(isNew)
            m_links.push_back(newUrl);
    }
    else {
        // just add a marker
        if (alt.empty()) {
            addText("[img]");
        }
        else {
            addText((char*)alt.c_str());
        }

        delete newUrl;
    }
}

std::string PluckerTableDocument::splitLine(std::string is, int cols)
{
    std::string os;
    int maxlen = 25;  // Not bold
    while (is.length()) {
        std::string data;
        int pos = is.find("\000\070", 0, 2);
        int offset = 0;
        int count = 0;
        int mark = 0;
        if (-1 == pos)
            pos = is.length();
        else
            pos += 2;
        data = is.substr(0, pos);
        is.erase(0, pos);
        // Split data
        while (offset < data.length()) {
            count = 0;
            while (offset < data.length()  && count < (maxlen * cols)) {
                if (data[offset] > 0) {
                    offset++;
                    count++;
                }
                else {
                    if (data[offset + 1] == 0x11)
                        if (data[offset + 2] == 7)
                            maxlen = 20; // Not Bold
                        else
                            maxlen = 25; // Bold
                    offset = offset + 2 + (data[offset + 1] & 7);
                }
            }
            while (offset < data.length() && data[offset] > 32) // word
                offset++;
            if (offset < data.length() && data[offset] == 0) // function
                offset = offset + 2 + (data[offset + 1] & 7);
            os += data.substr(mark, offset - mark);
            mark = offset;
            if (data.length() - offset > 0) {
                os += (char)0x00;
                os += (char)0x38;
            }
        }
    }
    return os;
}

void PluckerTableDocument::addUnicodeChar(int num, char* text)
{
    addChar(0x00);
    if (num > 0xffff)
        addChar(0x85);
    else
        addChar(0x83);
    addChar(strlen(text));
    if (num > 0xffff) {
        addChar(num >> 24);
        addChar(num >> 16);
    }
    addChar(num >> 8);
    addChar(num);
    addText(text);
}

void PluckerTableDocument::addColor(int color)
{
    addChar(0x00);
    addChar(0x53);
    addChar(color >> 16);
    addChar(color >> 8);
    addChar(color);
}
