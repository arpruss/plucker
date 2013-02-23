/*
 * $Id: PluckerTextDocument.h,v 1.7 2007/06/14 22:36:46 chrish Exp $
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

#if !defined(PLUCKERTEXTDOCUMENT_H)
#define PLUCKERTEXTDOCUMENT_H

#include <vector>
#include <zlib.h>

#include "PluckerDocument.h"
#include "PluckerTextParagraph.h"
#include "Url.h"

class PluckerTextDocument : public PluckerDocument
{
public:
    PluckerTextDocument(int id, std::string urlString);
	virtual ~PluckerTextDocument();

    // overriden methods
    void compress(int compression);
    int getDocumentSize();
    void writeContents(Writer* writer);
    void finishDocument(Resolver* resolver, Config* config);

    // content modifiers
    void addText(const char *temp);
    void addChar(const char data);
    void addVSpace(int space);
    void addHR(int height, int width, int percWidth);
    void addNewLine();
    void indent(int left, int right);

    void startStyle(int style);
    void stopStyle(int oldStyle);
    void startAlignment(int alignment);
    void stopAlignment(int oldAlignment);
    void startItalics();
    void stopItalics();
    void startAnchor(std::string href, int maxDepth, int stayOnHost, Url* url, Resolver* resolver, bool tables);
    void startAnchor(int id);
    void stopAnchor();
    void startUnderline();
    void stopUnderline();
    void startStrike();
    void stopStrike();

    void startLI(std::string text);
    void startImage(std::string href, int maxDepth, std::string alt, Url* url, Resolver* resolver);
    void addTable(int id);
    void addUnicodeChar(int num, char* text);
    void addColor(int color);
    void addName(Resolver* resolver, std::string name);

private:
    void addParagraph();
    void ZlibCompressFunction();

    PluckerTextParagraph* m_currentParagraph;
    std::vector<PluckerTextParagraph*> m_paragraphs;
    int m_estimatedParaSize;

    std::string m_content;
    unsigned long m_contentSize;
    Byte* m_compressedContent;
    unsigned long m_compressedSize;
};

#endif // !defined(PLUCKERTEXTDOCUMENT_H)
