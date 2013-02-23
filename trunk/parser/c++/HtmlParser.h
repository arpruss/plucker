/*
 * $Id: HtmlParser.h,v 1.11 2005/05/07 02:29:10 chrish Exp $
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

#if !defined(HTMLPARSER_H)
#define HTMLPARSER_H

#include <map>
#include <stack>
#include <string>
#include <vector>

#include "Parser.h"
#include "PluckerTableDocument.h"
#include "PluckerTextDocument.h"
#include "Resolver.h"
#include "Url.h"

class HtmlParser : public Parser
{
public:
	HtmlParser(Url* url, Resolver* resolver);
	virtual ~HtmlParser();

    void processData(Config* config, const unsigned char* data, unsigned long size);

    void pushVisibility(int visibility);
    void popVisibility();
    void addVSpace(int space);
    void addNewLine();
    void indent(int left, int right);

    void startStyle(int style);
    void stopStyle();
    void startAlignment(int alignment);
    void stopAlignment();
    void startAnchor(std::string href, int maxDepth, int stayOnHost, bool tables);
    void startAnchor(int id);
    void stopAnchor();
    void stopLI();
    void startTD(int align, int colSpan, int rowSpan);
    void startImage(std::string href, int maxDepth, std::string alt);

private:
    void startTag();
    void endTag();
    void addDocEstSize(int size, bool canStart);
    std::string findTagParameter(std::string parameter);
    void parseEntity();
    void addUnicodeChar(char* text);
    int parseColor(const char* text);

    int m_currentAlignment;
    char m_currentParamName[255];
    int m_currentParamNamePos;
    char m_currentParamValue[1024];
    int m_currentParamValuePos;
    int m_currentParamValueSize;
    char m_currentTag[255];
    int m_currentTagPlace;
    int m_currentTagSize;
    bool m_inParamName;
    bool m_inParamQuote;
    bool m_inParamValue;
    bool m_inScript;
    bool m_inTag;
    bool m_inComment;
    int m_commentState;
    bool m_startTag;
    bool m_tagDone;
    std::map<std::string, std::string> m_tagParameters;

    int m_visible;
    std::vector<int> m_visibility;
    std::vector<int> m_styles;
    std::vector<int> m_alignment;
    bool m_italics;
    bool m_underline;
    bool m_inAnchor;
    bool m_linkStarted;
    bool m_strike;
    bool m_firstTd;
    std::vector<int> m_olLevels;
    std::vector<int> m_listTypes;
    int m_ulListDepth;
    int m_olListDepth;
    bool m_inLi;
    bool m_inOl;
    bool m_inEntity;
    char m_currentEntity[255];
    int m_currentEntityPlace;

    PluckerTableDocument* m_currentTable;
    std::vector<PluckerTableDocument*> m_tableList;
    bool m_inTable;
    int m_tableNumber;
    bool m_doTables;

    PluckerTextDocument* m_currentDocument;
    long m_currentDocumentSize;
    Resolver* m_resolver;
    Url* m_url;

    std::stack<bool> m_cleanWhitespace;
    std::stack<int> m_color;
};

#endif // !defined(HTMLPARSER_H)
