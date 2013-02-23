/*
 * $Id: PluckerTableDocument.h,v 1.6 2005/05/07 02:29:10 chrish Exp $
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

#if !defined(PLUCKERTABLEDOCUMENT_H)
#define PLUCKERTABLEDOCUMENT_H

#include <vector>
#include <zlib.h>

#include "PluckerDocument.h"
#include "Url.h"

struct TableCell {
    int align;
    int colSpan;
    int rowSpan;
    int imageRef;
    std::string text;
};


class PluckerTableDocument : public PluckerDocument
{
public:
    PluckerTableDocument(int id, std::string urlString, int border, int borderColor, int linkColor);
	virtual ~PluckerTableDocument();

    // overriden methods
    void compress(int compression);
    int getDocumentSize();
    void writeContents(Writer* writer);
    void finishDocument(Resolver* resolver, Config* config);

    void addRow();
    void addCell(int align, int colSpan, int rowSpan);
    void addText(char* text);
    void addChar(char data);
    void addNewLine();

    void startStyle(int style);
    void stopStyle(int oldStyle);
    void startItalics();
    void stopItalics();
    void startUnderline();
    void stopUnderline();
    void startStrike();
    void stopStrike();
    void startAnchor(std::string href, int maxDepth, int stayOnHost, Url* url, Resolver* resolver, bool tables);
    void startAnchor(int id);
    void stopAnchor();
    void startLI(std::string text);
    void startImage(std::string href, int maxDepth, std::string alt, Url* url, Resolver* resolver);
    void addUnicodeChar(int num, char* text);
    void addColor(int color);

private:
    void ZlibCompressFunction();
    std::string splitLine(std::string, int cols);

    std::vector<std::vector<struct TableCell> > m_rows;
    std::vector<struct TableCell> m_cols;
    int m_currRow;
    int m_currCol;
    int m_maxCol;

    std::string m_content;
    unsigned long m_contentSize;
    Byte* m_compressedContent;
    unsigned long m_compressedSize;

    int m_border;
    int m_borderColor;
    int m_linkColor;
};

#endif // !defined(PLUCKERTABLEDOCUMENT_H)
