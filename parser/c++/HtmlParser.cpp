/*
 * $Id: HtmlParser.cpp,v 1.22 2007/10/17 01:27:51 chrish Exp $
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

#include <sstream>

#include "PluckerColors.h"
#include "GlobalFunctions.h"
#include "HtmlParser.h"

static char* uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char* lowercase = "abcdefghijklmnopqrstuvwxyz";

typedef struct
{
  char *tag;
  char *conversion;
} EntityRef;

static EntityRef entity_ref[] =
{
  { "&amp;",	"&"},
  { "&bull;",	"*" },
  { "&copy;",	"\xA9" },
  { "&emdash;	","-" },
  { "&emsp;",	" " },
  { "&endash;	","-" },
  { "&ensp;",	" " },
  { "&gt;",		">" },
  { "&hellip;",	"..." },
  { "&lt;",		"<" },
  { "&nbsp;",	" " },
  { "&quot;",	"\"" },
  { "&reg;",	"\xAE" },
  { "&trade;",	"(tm)" },
  { "&AElig;",	"\xc6" },	/* capital AE diphthong (ligature) */
  { "&Aacute;",	"\xc1" },	/* capital A, acute accent */
  { "&Acirc;",	"\xc2" },	/* capital A, circumflex accent */
  { "&Agrave;",	"\xc0" },	/* capital A, grave accent */
  { "&Aring;",	"\xc5" },	/* capital A, ring */
  { "&Atilde;",	"\xc3" },	/* capital A, tilde */
  { "&Auml;",	"\xc4" },	/* capital A, dieresis or umlaut mark*/
  { "&Ccedil;",	"\xc7" },	/* capital C, cedilla */
  { "&ETH;",	"\xd0" },	/* capital Eth, Icelandic */
  { "&Eacute;",	"\xc9" },	/* capital E, acute accent */
  { "&Ecirc;",	"\xca" },	/* capital E, circumflex accent */
  { "&Egrave;",	"\xc8" },	/* capital E, grave accent */
  { "&Euml;",	"\xcb" },	/* capital E, dieresis or umlaut mark*/
  { "&Iacute;",	"\xcd" },	/* capital I, acute accent */
  { "&Icirc;",	"\xce" },	/* capital I, circumflex accent */
  { "&Igrave;",	"\xcc" },	/* capital I, grave accent */
  { "&Iuml;",	"\xcf" },	/* capital I, dieresis or umlaut mark*/
  { "&Ntilde;",	"\xd1" },	/* capital N, tilde */
  { "&Oacute;",	"\xd3" },	/* capital O, acute accent */
  { "&Ocirc;",	"\xd4" },	/* capital O, circumflex accent */
  { "&Ograve;",	"\xd2" },	/* capital O, grave accent */
  { "&Oslash;",	"\xd8" },	/* capital O, slash */
  { "&Otilde;",	"\xd5" },	/* capital O, tilde */
  { "&Ouml;",	"\xd6" },	/* capital O, dieresis or umlaut mark*/
  { "&THORN;",	"\xdd" },	/* capital THORN, Icelandic */
  { "&Uacute;",	"\xda" },	/* capital U, acute accent */
  { "&Ucirc;",	"\xdb" },	/* capital U, circumflex accent */
  { "&Ugrave;",	"\xd9" },	/* capital U, grave accent */
  { "&Uuml;",	"\xdc" },	/* capital U, dieresis or umlaut mark*/
  { "&Yacute;",	"\xdd" },	/* capital Y, acute accent */
  { "&aacute;",	"\xe1" },	/* small a, acute accent */
  { "&acirc;",	"\xe2" },	/* small a, circumflex accent */
  { "&aelig;",	"\xe6" },	/* small ae diphthong (ligature) */
  { "&agrave;",	"\xe0" },	/* small a, grave accent */
  { "&aring;",	"\xe5" },	/* small a, ring */
  { "&atilde;",	"\xe3" },	/* small a, tilde */
  { "&auml;",	"\xe4" },	/* small a, dieresis or umlaut mark */
  { "&ccedil;",	"\xe7" },	/* small c, cedilla */
  { "&eacute;",	"\xe9" },	/* small e, acute accent */
  { "&ecirc;",	"\xea" },	/* small e, circumflex accent */
  { "&egrave;",	"\xe8" },	/* small e, grave accent */
  { "&eth;",	"\xf0" },	/* small eth, Icelandic */
  { "&euml;",	"\xeb" },	/* small e, dieresis or umlaut mark */
  { "&iacute;",	"\xed" },	/* small i, acute accent */
  { "&icirc;",	"\xee" },	/* small i, circumflex accent */
  { "&igrave;",	"\xec" },	/* small i, grave accent */
  { "&iuml;",	"\xef" },	/* small i, dieresis or umlaut mark */
  { "&ntilde;",	"\xf1" },	/* small n, tilde */
  { "&oacute;",	"\xf3" },	/* small o, acute accent */
  { "&ocirc;",	"\xf4" },	/* small o, circumflex accent */
  { "&ograve;",	"\xf2" },	/* small o, grave accent */
  { "&oslash;",	"\xf8" },	/* small o, slash */
  { "&otilde;",	"\xf5" },	/* small o, tilde */
  { "&ouml;",	"\xf6" },	/* small o, dieresis or umlaut mark */
  { "&szlig;",	"\xdf" },	/* small sharp s, German (szligature)-> */
  { "&thorn;",	"\xfe" },	/* small thorn, Icelandic */
  { "&uacute;",	"\xfa" },	/* small u, acute accent */
  { "&ucirc;",	"\xfb" },	/* small u, circumflex accent */
  { "&ugrave;",	"\xf9" },	/* small u, grave accent */
  { "&uuml;",	"\xfc" },	/* small u, dieresis or umlaut mark */
  { "&yacute;",	"\xfd" },	/* small y, acute accent */
  { "&yuml;",	"\xff" },	/* small y, dieresis or umlaut mark */
  { "&","&" },				/* must be last entry in table */
  { NULL,NULL }
};


HtmlParser::HtmlParser(Url* url, Resolver* resolver)
{
    m_resolver = resolver;
    m_url = url;

    int newId = url->m_id;
    m_currentDocument = new PluckerTextDocument(newId, url->getUrlString());
    m_currentDocumentSize = 0;
    m_documents.push_back(m_currentDocument);

    m_ulListDepth = 0;
    m_olListDepth = 0;
    m_inLi = false;
    m_firstTd = false;

    m_visible = 1;
    m_currentAlignment = 0;
    m_italics = false;
    m_inAnchor = false;
    m_underline = false;
    m_strike = false;

    m_currentTagSize = 255;
    m_currentParamValueSize = 1024;
    m_inTag = false;
    m_inScript = false;
    m_inComment = false;
    m_inEntity = false;

    m_cleanWhitespace.push(true);

    m_inTable = false;
    m_tableNumber = 0;
    m_currentTable = NULL;
    m_doTables = m_url->m_tables;
    m_color.push(0x000000);
}

HtmlParser::~HtmlParser()
{
}

void HtmlParser::processData(Config* config, const unsigned char* data, unsigned long size)
{
    if (!m_doTables)
        m_doTables = config->m_doTables;

    // process the data given, which isn't necessarily all the data
    // for this document
    int i = 0;
    while (i < size) {
        if (m_inEntity) {
            m_currentEntity[m_currentEntityPlace++] = data[i];
            if(data[i] == ';' || isspace(data[i])
                || !(isalnum(data[i]) || data[i] == '#')) {
                if(data[i] != ';')    // is it an un-escaped '&' ??
                    m_currentEntityPlace--;
                m_currentEntity[m_currentEntityPlace] = '\0';
                parseEntity();
                m_inEntity = false;
                if(data[i] != ';')
                    continue;
            }
            i++;
            continue;
        }
        if (m_inComment) {
            switch (m_commentState) {
                case 0:
                    while (i < size && data[i] != '-')
                        i++;
                    if (i < size && data[i] == '-')
                        m_commentState = 1;
                    break;
                case 1:
                    if (data[i] == '-')
                        m_commentState = 2;
                    else
                        m_commentState = 0;
                    break;
                case 2:
                    while (i < size && data[i] == '-')
                        i++;
                    if (i < size && data[i] == '>')
                        m_inComment = false;
                    break;
            }
            i++;
            continue;
        }
        if (m_inScript) {
            // need to skip everything and find the end of the script
            if (!m_inTag) {
                while (i < size && data[i] != '<')
                    ++i;
                if (i < size && data[i] == '<') {
                    m_currentTagPlace = 0;
                    m_inTag = true; // possible found beginning of tag
                    ++i;    // move past '<' 
                }
            }
            if (m_inTag) {
                while (i < size && (data[i] == '\n' || data[i] == '\r'))
                    ++i;
                if (i < size) {
                    if ((data[i] == '/' && m_currentTagPlace == 0) || m_currentTagPlace > 0) {
                        // found possible end
                        if (data[i] == '/')
                            ++i;
                        int t = i;
                        // peek ahead here to see if it's the real end
                        while (t < size && data[t] != '>') {
                            if (m_currentTagPlace < (m_currentTagSize - 1)) {
                                m_currentTag[m_currentTagPlace] = toupper(data[t]);
                                ++m_currentTagPlace;
                            }
                            ++t;
                        }
                        if (t < size) {
                            // found an ending tag
                            m_currentTag[m_currentTagPlace] = '\0';
                            if (strcmp(m_currentTag, "SCRIPT") == 0 || strcmp(m_currentTag, "JAVASCRIPT") == 0) {
                                endTag();
                                i = t + 1;
                                m_inTag = false;
                            }
                            else {
                                // not the right tag and need to go back to i
                                m_inTag = false;
                            }
                        }
                    }
                    else {
                        // not an ending tag so move on
                        m_inTag = false;
                    }
                }
            }
        }
        else {
            if (data[i] == '<' || m_inTag) {
                // start of tag, or already processing a tag
                if (data[i] == '<') {
                    // this is a new start of a tag
                    m_inTag = true;
                    m_inParamName = false;
                    m_inParamValue = false;
                    m_tagDone = false;
                    m_currentTagPlace = 0;
                    m_tagParameters.clear();
                    m_startTag = true;
                    // move past <
                    ++i;
                }
                if (i < size && data[i] == '/' && m_currentTagPlace == 0) {
                    // this is an ending tag
                    m_startTag = false;
                    // move past /
                    ++i;
                }

                if (i < size) {
                    while (i < size && !isspace(data[i]) && data[i] != '>' && !m_tagDone) {
                        // find the tag name
                        if (data[i] != '\n' && data[i] != '\r' && m_currentTagPlace < (m_currentTagSize - 1) ) {
                            m_currentTag[m_currentTagPlace] = toupper(data[i]);
                            ++m_currentTagPlace;
                        }
                        ++i;
                        if (m_startTag && m_currentTagPlace == 3 && !strncmp(m_currentTag, "!--", 3)) {
                            // Comment starts
                            m_inTag = false;
                            m_currentTagPlace = 0;
                            m_inComment = true;
                            m_commentState = 0;
                            break;
                        }
                    }

                    if (m_inComment)
                        continue;

                    while (i < size && isspace(data[i]) || data[i] == '\n' || data[i] == '\r') {
                        // need to move past this to the start of the first parameter
                        ++i;
                    }

                    if (i < size) {
                        // got to the end of the tag
                        m_currentTag[m_currentTagPlace] = '\0';
                        m_tagDone = true;

                        // find parameters
                        while (i < size && data[i] != '>') {
                            while (i < size && ((data[i] != '=' && data[i] != '>' && !m_inParamQuote) || m_inParamQuote)) {
                                if (isspace(data[i]) && !m_inParamQuote) {
                                    if (m_inParamValue && m_currentParamNamePos > 1)
                                        break;

                                    i++;
                                    continue;
                                }
                                if (data[i] != '\n' && data[i] != '\r') {
                                    if (!m_inParamName && !m_inParamValue) {
                                        m_inParamName = true;
                                        m_inParamQuote = false;
                                        m_currentParamNamePos = 0;
                                        m_currentParamValuePos = 0;
                                        m_currentParamName[0] = '\0';
                                        m_currentParamValue[0] = '\0';
                                    }
                                    if (!m_inParamQuote && (data[i] == '"' || data[i] == '\'')) {
                                        m_inParamQuote = true;
                                    }
                                    else if (m_inParamQuote && (data[i] == '"' || data[i] == '\'')) {
                                        m_inParamQuote = false;
                                    }
                                    else {
                                        if (m_inParamName && m_currentParamNamePos < (m_currentTagSize - 1)) {
                                            m_currentParamName[m_currentParamNamePos] = toupper(data[i]);
                                            ++m_currentParamNamePos;
                                        }
                                        if (m_inParamValue && m_currentParamValuePos < (m_currentParamValueSize -1)) {
                                            m_currentParamValue[m_currentParamValuePos] = data[i];
                                            ++m_currentParamValuePos;
                                        }
                                    }
                                }
                                ++i;
                            }
                            if (i < size) {
                                if (data[i] == '=') {
                                    if (m_inParamName) {
                                        m_inParamName = false;
                                        m_inParamValue = true;
                                    }
                                    ++i;
                                    while(isspace(data[i]) && i < size) i++;
                                }
                                if (data[i] == '>' || isspace(data[i]) || ((data[i] == '\n' || data[i] == '\r') && m_inParamValue && m_currentParamNamePos > 1)) {
                                    // ending parameter single or pair, need to save
                                    m_currentParamName[m_currentParamNamePos] = '\0';
                                    m_currentParamValue[m_currentParamValuePos] = '\0';
                                    if (m_currentParamName[0] != '\0')
                                        m_tagParameters[m_currentParamName] = m_currentParamValue;
                                    m_inParamName = false;
                                    m_inParamValue = false;
                                    if (isspace(data[i]))
                                        ++i;
                                }
                            }
                        }

                        if (i < size) {
                            // got to the end of the parameters
                            m_inTag = false;

                            // do something with the tag found
                            if (m_startTag) {
                                startTag();
                            }
                            else {
                                endTag();
                            }

                            ++i;
                        }
                    }
                }
            }
            else if (isspace(data[i])) {
                while(i < size && isspace(data[i])) {
                    if (m_cleanWhitespace.top() == false) {
                        if (data[i] == '\n')
                            addVSpace(0);
                        if (data[i] == '\t' || data[i] == ' ') {
                            if (!m_inTable) {
                                addDocEstSize(1, !m_inAnchor);
                                m_currentDocument->addChar(' ');
                            }
                            else {
                                m_currentTable->addChar(' ');
                            }
                        }
                    }
                    i++;
                }
                if (m_cleanWhitespace.top() == true) {
                    if (!m_inTable) {
                        addDocEstSize(1, !m_inAnchor);
                        m_currentDocument->addChar(' ');
                    }
                    else {
                        m_currentTable->addChar(' ');
                    }
                }
            }
            else if (data[i] == '&') {
                m_inEntity = true;
                m_currentEntityPlace = 0;
                m_currentEntity[m_currentEntityPlace++] = data[i];
                i++;
                continue;
            }
            else {
                if (m_visible && !m_inScript) {
                    if (m_inTable) {
                        m_currentTable->addChar(data[i]);
                    }
                    else {
                        addDocEstSize(1, !m_inAnchor);
                        m_currentDocument->addChar(data[i]);
                    }
                }
                ++i;
            }
        }
    }
}

// handle the starting of thecurrent tag
void HtmlParser::startTag()
{
    if (m_inScript) {
        // don't do anything till out of script
        return;
    }
    if (strcmp(m_currentTag, "HEAD") == 0) {
        pushVisibility(0);
        return;
    }
    if (strcmp(m_currentTag, "TITLE") == 0) {
        pushVisibility(0);
        return;
    }
    if (strcmp(m_currentTag, "SCRIPT") == 0) {
        m_inScript = true;
        pushVisibility(0);
        return;
    }
    if (strcmp(m_currentTag, "JAVASCRIPT") == 0) {
        m_inScript = true;
        pushVisibility(0);
        return;
    }
    if (strcmp(m_currentTag, "B") == 0 || strcmp(m_currentTag, "STRONG") == 0) {
        startStyle(7);
        return;
    }
    if (strcmp(m_currentTag, "I") == 0 || strcmp(m_currentTag, "ADDRESS") == 0 || strcmp(m_currentTag, "EM") == 0 || strcmp(m_currentTag, "CITE") == 0) {
        if (!m_italics) {
            if (!m_inTable) {
                addDocEstSize(1, true);
                m_currentDocument->startItalics();
            }
            else {
                m_currentTable->startItalics();
            }
            m_italics = true;
        }
        return;
    }
    if (strcmp(m_currentTag, "P") == 0) {
        int align = m_currentAlignment;
        std::string alignment = findTagParameter("ALIGN");
        if (alignment == "left")
            align = 0;
        else if (alignment == "center")
            align = 2;
        else if (alignment == "right")
            align = 1;
        else if (alignment == "justify")
            align = 3;
        addVSpace(0);
        startAlignment(align);
//        if self._needs_newpara ():
//            if self._indent_paragraphs:
//                self._add_text('\xa0\xa0\xa0\xa0\xa0\xa0')
//            else:
//                self._add_vspace (2)
        return;
    }
    if (strcmp(m_currentTag, "A") == 0) {
    // need to pick up attributes from link as well
    // maxdepth, etc.

/*    def start_a (self, attributes):
        attributes = _list_to_dict (attributes)
        if attributes.has_key ("href"):
            if self._visible:
                if attributes.has_key ('href'):
                    attributes['href'] = Url.CleanURL (attributes['href'], self._base or self._url)
                    attributes['_plucker_id_tag'] = PluckerDocs.obtain_fresh_id()
                    attributes['_plucker_from_url'] = self._url
                    the_url = Url.URL (attributes['href'])
                    #if the_url.get_protocol () == 'mailto':
                    #    # Mailtos are not yet handled...
                    #    attributes['href'] = "plucker:/mailto.html"
                    
                    # Color the anchor according to anchor_forecolor
                    # (set with 'link' attribute of body tag). Can override
                    # with a <font color=""> inside the anchor
                    if self._anchor_forecolor:
                        self._doc.set_forecolor(self._anchor_forecolor)
                    
                    self._anchors.append ((attributes['href'], attributes))
                    self._doc.add_document_link_start (attributes)
                    self._push_element ("a")
        if attributes.has_key ("name"):
            self._doc.add_name (attributes['name'])
*/
        std::string name = findTagParameter("NAME");
        if (!name.empty()) {
            m_currentDocument->addName(m_resolver, name);
        }
        else {
            int maxDepth = -1;
            int stayOnHost = -1;
            bool tables = false;
            std::string tag = findTagParameter("MAXDEPTH");
            if (!tag.empty()) {
                maxDepth = atol(tag.c_str());
            }
            tag = findTagParameter("STAYONDOMAIN");
            if (!tag.empty()) {
                stayOnHost = 2;
            }
            tag = findTagParameter("STAYONHOST");
            if (!tag.empty()) {
                stayOnHost = 1;
            }
            tag = findTagParameter("TABLES");
            if (!tag.empty()) {
                tables = true;
            }
            tag = findTagParameter("HREF");
            if (!tag.empty()) {
                startAnchor(tag, maxDepth, stayOnHost, tables);
            }
        }
        m_linkStarted = true;
    }
    if (strcmp(m_currentTag, "BR") == 0) {
        addVSpace(0);
        return;
    }
    if (strcmp(m_currentTag, "U") == 0) {
        if (!m_underline) {
            if (!m_inTable) {
                addDocEstSize(2, true);
                m_currentDocument->startUnderline();
            }
            else {
                m_currentTable->startUnderline();
            }
            m_underline = true;
        }
        return;
    }
    if (strcmp(m_currentTag, "H1") == 0) {
        addVSpace(3);
        startStyle(1);
        startAlignment(2);
        return;
    }
    if (strcmp(m_currentTag, "H2") == 0) {
        addVSpace(3);
        startStyle(2);
        return;
    }
    if (strcmp(m_currentTag, "H3") == 0) {
        addVSpace(2);
        startStyle(3);
        return;
    }
    if (strcmp(m_currentTag, "H4") == 0) {
        addVSpace(2);
        startStyle(4);
        return;
    }
    if (strcmp(m_currentTag, "H5") == 0) {
        addVSpace(2);
        startStyle(5);
        return;
    }
    if (strcmp(m_currentTag, "H6") == 0) {
        addVSpace(2);
        startStyle(6);
        return;
    } 
    if (strcmp(m_currentTag, "UL") == 0 || strcmp(m_currentTag, "MENU") == 0) {
        ++m_ulListDepth;
        m_listTypes.push_back(1);
        return;
    }
    if (strcmp(m_currentTag, "OL") == 0) {
        ++m_olListDepth;
        m_olLevels.push_back(1);
        m_listTypes.push_back(0);
        return;
    }
    if (strcmp(m_currentTag, "LI") == 0) {
        if (m_inLi) {
            stopLI();
        }
        addVSpace(0);

        std::string text;
        int ind;
        int listType = 0;
        if (!m_listTypes.empty()) {
            listType = m_listTypes[m_listTypes.size() - 1];
        }

        if (listType == 0) {
            ind = 12;
            int number = 0;
            if (!m_olLevels.empty()) {
                number = m_olLevels[m_olLevels.size() - 1];
                m_olLevels[m_olLevels.size() - 1] = number + 1;
            }
            if (m_olListDepth % 3 == 1) {
                std::stringstream ss;
                ss << number << ") ";
                text = ss.str();
            }
            else if (m_olListDepth % 3 == 2) {
                if (number < 26) {
                    std::stringstream ss;
                    ss << uppercase[number-1] << ") ";
                    text = ss.str();
                }
                else
                    text = "?) ";
            }
            else {
                if (number < 26) {
                    std::stringstream ss;
                    ss << lowercase[number-1] << ") ";
                    text = ss.str();
                }
                else
                    text = "?) ";
            }
        }
        else {
            if (m_ulListDepth == 1) {
                text.append(1, (unsigned char) 0x0);
                text.append(1, (unsigned char) 0x83);
                text.append(1, (unsigned char) 0x01);
                text.append(1, (unsigned char) 0x20);
                text.append(1, (unsigned char) 0x22);
                text.append("o ");
                ind = 7;
            }
            else if (m_ulListDepth == 2) {
                text.append(1, (unsigned char) 0xbb);
                text.append(" ");
                ind = 6;
            }
            else if (m_ulListDepth == 3) {
                text = "+ ";
                ind = 8;
            }
            else {
                text = "> ";
                ind = 4;
            }
        }
        startStyle(0);

        if (!m_inTable) {
            addDocEstSize(text.size(), true);
            m_currentDocument->startLI(text);
        }
        else {
            m_currentTable->startLI(text);
        }

        stopStyle();
        indent(ind, 0);

        m_inLi = true;
        return;
    }
    if (strcmp(m_currentTag, "TR") == 0) {
        if (m_inTable) {
            m_currentTable->addRow();
        }
        else
        if (m_visible == 1) {
            addVSpace(1);
    /*
        def start_tr (self, attributes):
            if self._get_element_info ('table'):
                if self._tableborder_forecolor == "default":
                    self._doc.set_forecolor ("#999999")
                else:
                    self._doc.set_forecolor (self._tableborder_forecolor)
                self._doc.add_hr (height=2)
                self._doc.unset_forecolor (self._doc.get_forecolor ()) 
                self._add_vspace (1)
    */
            m_firstTd = true;
        }
        return;
    }
    if (strcmp(m_currentTag, "TD") == 0
       || strcmp(m_currentTag, "TH") == 0) {
        int alignment = 0;
        std::string align = findTagParameter("ALIGN");
        if (align == "left")
            alignment = 0;
        else if (align == "center")
            alignment = 2;
        else if (align == "right")
            alignment = 1;
        else if (align == "justify")
            alignment = 3;
        int colSpan = 1;
        int rowSpan = 1;
        std::string tag = findTagParameter("COLSPAN");
        if (!tag.empty())
            colSpan = atol(tag.c_str());
        tag = findTagParameter("ROWSPAN");
        if (!tag.empty())
            rowSpan = atol(tag.c_str());
        //elif align == 'top' or align == 'bottom' or align == 'middle':
        //    # vertical alignment not handled...
        //    pass

        startTD(alignment, colSpan, rowSpan);
        if (strcmp(m_currentTag, "TH") == 0)
            startStyle(7);
        return;
    }
    if (strcmp(m_currentTag, "HR") == 0 && !m_inTable) {
        int height = 2;
        long width = 0;
        long percWidth = 0;
        bool needsAlignClean = false;

        std::string sizeParam = findTagParameter("SIZE");
        if (!sizeParam.empty()) {
             height = atol(sizeParam.c_str());
        }
            //if attributes.has_key ('color'):
            //    rgb = string.lower (attributes['color'])
            //else:
            //    rgb = "#808080" # Default to gray for hr if color not specified
        std::string widthParam = findTagParameter("WIDTH");
        if (!widthParam.empty()) {
            if (widthParam != "100%") {
                if (widthParam[widthParam.length() - 1] == '%') {
                    percWidth = atol(widthParam.substr(0, widthParam.length() - 1).c_str());
                }
                else {
                    width = atol(widthParam.c_str());
                    if (width > 150)
                        width = 150;
                }
                std::string align = findTagParameter("ALIGN");
                if (!align.empty()) {
                    int alignment = 2;
                    if (align == "left")
                        alignment = 0;
                    else if (align == "center")
                        alignment = 2;
                    else if (align == "right")
                        alignment = 1;
            
                    startAlignment(alignment);
                    needsAlignClean = true;
                }
                else {
                    startAlignment(2);
                    needsAlignClean = true;
                }
            }
        }

        addVSpace(4);
            //self._doc.set_forecolor (rgb)
        if (m_visible == 1) {
            addDocEstSize(5, true);
            m_currentDocument->addHR(height, width, percWidth);
        }
            //forecolor = self._doc.get_forecolor ()
            //self._doc.unset_forecolor (forecolor) 
        addVSpace(4);
        if (needsAlignClean) {
            stopAlignment();
        }
        return;
    }
    if (strcmp(m_currentTag, "SMALL") == 0) {
        startStyle(9);
        return;
    }
    if (strcmp(m_currentTag, "CENTER") == 0) {
        addVSpace(0);
        startAlignment(2);
        return;
    } 
    if (strcmp(m_currentTag, "S") == 0 || strcmp(m_currentTag, "STRIKE") == 0) {
        if (!m_strike) {
            if (!m_inTable) {
                addDocEstSize(2, true);
                m_currentDocument->startStrike();
            }
            else {
                m_currentTable->startStrike();
            }
            m_strike = true;
        }
        return;
    }
    if (strcmp(m_currentTag, "STYLE") == 0) {
        pushVisibility(0);
        return;
    }
    if (strcmp(m_currentTag, "DT") == 0 || strcmp(m_currentTag, "DD") == 0) {
        addVSpace(1);
        return;
    }
    if (strcmp(m_currentTag, "BODY") == 0) {
//        if attributes.has_key ('link'):
//            rgb = string.lower (attributes['link'])
//            self._anchor_forecolor = rgb
//        if attributes.has_key ('text'):
//            rgb = string.lower (attributes['text'])
//            self._doc.set_forecolor (rgb)                        
        pushVisibility(1);
        return;
    }
    if (strcmp(m_currentTag, "DIV") == 0) {
        int alignment = m_currentAlignment;
        std::string align = findTagParameter("ALIGN");
        if (align == "left")
            alignment = 0;
        else if (align == "center")
            alignment = 2;
        else if (align == "right")
            alignment = 1;
        else if (align == "justify")
            alignment = 3;
        addVSpace(0);
        startAlignment(alignment);
        return;
    } 
    if (strcmp(m_currentTag, "BLOCKQUOTE") == 0) {
        addVSpace(2);
        indent(12, 12);
        return;
    }
    if (strcmp(m_currentTag, "FONT") == 0) {
        std::string colorName = findTagParameter("COLOR");
        if (!colorName.empty()) {
            int color = parseColor(colorName.c_str());
            m_color.push(color);
            if (m_inTable)
                m_currentTable->addColor(color);
            else
                m_currentDocument->addColor(color);
        }
/*    def start_font (self, attributes):
        attributes = _list_to_dict (attributes)
        if attributes.has_key ('color'): 
            rgb = string.lower (attributes['color'])
            self._doc.set_forecolor (rgb)
        # Following keeps the forecolor stack intact if it is a <font>
        # without a 'color' attribute, such as <font size="+1">
        else:    
            self._doc.set_forecolor (self._doc.get_forecolor ())
*/
        return;
    }
    if (strcmp(m_currentTag, "PRE") == 0 || strcmp(m_currentTag, "TT") == 0) {
//        self._doc.set_style ("pre")
        m_cleanWhitespace.push(false);
        return;
    }
    if (strcmp(m_currentTag, "Q") == 0) {
        if (m_inTable) {
            m_currentTable->addText(" ''");
        }
        else {
            addVSpace(2);
            indent(5, 5);
            m_currentDocument->addText(" ''");
            indent(7, 7);
        }
        return;
    }
    if (strcmp(m_currentTag, "IMG") == 0) {
        if (m_visible) {
            int maxDepth = -1;
            std::string alt = findTagParameter("ALT");
            std::string tag = findTagParameter("MAXDEPTH");
            if (!tag.empty()) {
                maxDepth = atol(tag.c_str());
            }
            tag = findTagParameter("SRC");
            if (!tag.empty()) {
                startImage(tag, maxDepth, alt);
            }
        }
        return;
    }   
    if (strcmp(m_currentTag, "TABLE") == 0 && m_doTables) {
        int border = 0;
        int borderColor = 0x00000000;
        int linkColor = 0x000000ff;
        std::string tag = findTagParameter("BORDER");
        if (!tag.empty())
            border = atol(tag.c_str());
        tag = findTagParameter("BORDERCOLOR");
        if (!tag.empty())
            borderColor = parseColor(tag.c_str());
        tag = findTagParameter("LINKCOLOR");
        if (!tag.empty())
            linkColor = parseColor(tag.c_str());
        std::string oldhref = m_currentDocument->getUrlString();
        std::stringstream ss;
        ss << m_url->getUrlString() << "#Table" << m_tableNumber;
        std::string newhref = ss.str();
        int newId;
        m_resolver->getOrAdd(newhref, &newId);
        addDocEstSize(4, true);
        if (m_currentTable == NULL) {
            m_currentDocument->addTable(newId);
        }
        else {
            m_currentTable->addChar(0x00);
            m_currentTable->addChar(0x92);
            m_currentTable->addChar((newId >> 8) & 0xff);
            m_currentTable->addChar(newId & 0xff);
        }

        // create new document
        m_currentTable = new PluckerTableDocument(newId, newhref, border, borderColor, linkColor);
        m_tableList.push_back(m_currentTable);
        m_documents.push_back(m_currentTable);
        m_inTable = true;
        m_tableNumber++;
/*    def start_table (self, attributes):
        border = 0
        attr = _list_to_dict (attributes)
        if attr.has_key ('border') and attr['border']!="0":
            border = 1
        # Robert: Colored nested tables aren't supported, but could be added.
        if attr.has_key ('bordercolor'):
            rgb = string.lower (attr['bordercolor'])
            self._tableborder_forecolor = rgb
        else:
            self._tableborder_forecolor = "default"   
        self._push_element ("table", border)
*/
        return;
    }
/*
    def do_meta (self, data):
        # if the charset is not already assigned (from the HTTP headers, presumably)
        # and it's available here, then use it
        if not self._charset and string.lower(data[0][0]) == 'http-equiv' and string.lower(data[0][1]) == 'content-type':
            from PyPlucker.Retriever import parse_http_header_value
            ctype, parameters = parse_http_header_value(data[1][1])
            for parameter in parameters:
                if parameter[0] == 'charset':
                    self._set_charset(parameter[1])
        

    def handle_charref (self, name):
        try:
            n = int(name)
        except ValueError:
            self.unknown_entityref(name)
            return
        if not 0 <= n <= 255:
            self.unknown_charref(name)
            return
        self.handle_data(chr(n))


    def handle_data (self, data):
        if self._clean_whitespace[-1]:
            data = _RE_WHITESPACE.sub(" ", data, 0)   # data = re.sub("[\n\r\f \t]+", " ", data)
        else:
            # we should retain the whitespace layout.
            # However Plucker cannot handle tabs, form-feeds, so we need to
            # do something useful with that
            data = string.translate (data, _CLEANUP_TRANSTABLE)
            data = string.replace (data, "\t", "  ")


        #stripped_data = string.strip(data)
        if data:
            # not just blank or empty text (e.g. from comments), so we
            # add it...
            self._add_text (data)


    def do_base(self, attributes):
        # set self._base even if not in visible section
        attributes = _list_to_dict (attributes)
        if attributes.has_key ('href'):
            # clean up cruft
            self._base = Url.CleanURL (attributes['href'], self._url)
*/
}

// handle the ending of the current tag
void HtmlParser::endTag()
{
    if (strcmp(m_currentTag, "SCRIPT") == 0) {
        m_inScript = false;
        popVisibility();
        return;
    }
    if (strcmp(m_currentTag, "JAVASCRIPT") == 0) {
        m_inScript = false;
        popVisibility();
        return;
    }
    if (m_inScript) {
        // don't do anything until out of the script
        return;
    }
    if (strcmp(m_currentTag, "HEAD") == 0) {
        popVisibility();
        return;
    }
    if (strcmp(m_currentTag, "TITLE") == 0) {
        popVisibility();
        return;
    }    
    if (strcmp(m_currentTag, "B") == 0 || strcmp(m_currentTag, "STRONG") == 0) {
        stopStyle();
        return;
    }
    if (strcmp(m_currentTag, "BODY") == 0) {
        popVisibility();
        return;
    }
    if (strcmp(m_currentTag, "A") == 0) {
/*
    def end_a (self):
        # only set end anchor if this was a remote reference
        # (as compared to just a named anchor for later referencing)
        if self._get_top_element () == "a":
            if self._visible:
                self._pop_element ("a")
                self._doc.add_document_link_end ()
                if self._anchor_forecolor:
                    # Stop coloring the anchor
                    forecolor = self._doc.get_forecolor ()
                    self._doc.unset_forecolor (forecolor) 
*/
        if (m_linkStarted) {
            stopAnchor();
            m_linkStarted = false; 
        }
        return;
    }
    if (strcmp(m_currentTag, "SMALL") == 0) {
        stopStyle();
        return;
    }
    if (strcmp(m_currentTag, "I") == 0 || strcmp(m_currentTag, "ADDRESS") == 0 || strcmp(m_currentTag, "EM") == 0 || strcmp(m_currentTag, "CITE") == 0) {
        if (m_italics) {
            if (m_inTable) {
                m_currentTable->stopItalics();
            }
            else {
            addDocEstSize(2, false);
            m_currentDocument->stopItalics();
            }
            m_italics = false;
        }
        return;
    }
    if (strcmp(m_currentTag, "U") == 0) {
        if (m_underline) {
            if (m_inTable) {
                m_currentTable->stopUnderline();
            }
            else {
            addDocEstSize(2, false);
            m_currentDocument->stopUnderline();
            }
            m_underline = false;
        }
        return;
    }
    if (strcmp(m_currentTag, "S") == 0 || strcmp(m_currentTag, "STRIKE") == 0) {
        if (m_strike) {
            if (m_inTable) {
                m_currentTable->stopStrike();
            }
            else {
            addDocEstSize(2, false);
            m_currentDocument->stopStrike();
            }
            m_strike = false;
        }
        return;
    }
    if (strcmp(m_currentTag, "FONT") == 0) {
        m_color.pop();
        if (m_color.empty())
            m_color.push(0x00);
        if (m_inTable) {
            m_currentTable->addColor(m_color.top());
        }
        else {
            addDocEstSize(5, true);
            m_currentDocument->addColor(m_color.top());
        }
//    def end_font (self):
//        forecolor = self._doc.get_forecolor ()
//        self._doc.unset_forecolor (forecolor) 
        return;
    }
    if (strcmp(m_currentTag, "H1") == 0) {
        addVSpace(2);
        stopStyle();
        stopAlignment();
        return;
    }
    if (strcmp(m_currentTag, "H2") == 0) {
        addVSpace(2);
        stopStyle();
        return;
    }
    if (strcmp(m_currentTag, "H3") == 0) {
        addVSpace(1);
        stopStyle();
        return;
    }
    if (strcmp(m_currentTag, "H4") == 0) {
        addVSpace(1);
        stopStyle();
        return;
    }
    if (strcmp(m_currentTag, "H5") == 0) {
        addVSpace(1);
        stopStyle();
        return;
    }
    if (strcmp(m_currentTag, "H6") == 0) {
        addVSpace(1);
        stopStyle();
        return;
    }
    if (strcmp(m_currentTag, "UL") == 0 || strcmp(m_currentTag, "MENU") == 0) {
        if (m_ulListDepth < 0)
            m_ulListDepth = 0;
        if (m_inLi) 
            stopLI();
        --m_ulListDepth;

        if (!m_listTypes.empty()) {
            m_listTypes.erase(m_listTypes.end() - 1);
        }
        return;
    }
    if (strcmp(m_currentTag, "OL") == 0) {
        if (m_olListDepth < 0)
            m_olListDepth = 0;
        if (m_inLi) 
            stopLI();
        if (!m_olLevels.empty()) {
            m_olLevels.erase(m_olLevels.end() - 1);
        }
        --m_olListDepth;

        if (!m_listTypes.empty()) {
            m_listTypes.erase(m_listTypes.end() - 1);
        }
        return;
    }
    if (strcmp(m_currentTag, "LI") == 0) {
        stopLI();
        return;
    }
    if (strcmp(m_currentTag, "TD") == 0) {
        stopAlignment();
        return;
    }
    if (strcmp(m_currentTag, "TR") == 0) {
        if (!m_inTable)
            addVSpace(0);
        m_firstTd = false;
        return;
    }
    if (strcmp(m_currentTag, "TH") == 0) {
        stopAlignment();
        stopStyle();
        return;
    }
    if (strcmp(m_currentTag, "P") == 0) {
        addVSpace(0);
        stopAlignment();
        return;
    }
    if (strcmp(m_currentTag, "DIV") == 0) {
        addVSpace(0);
        stopAlignment();
        return;
    }
    if (strcmp(m_currentTag, "CENTER") == 0) {
        addVSpace(0);
        stopAlignment();
        return;
    }
    if (strcmp(m_currentTag, "STYLE") == 0) {
        popVisibility();
        return;
    }
    if (strcmp(m_currentTag, "BLOCKQUOTE") == 0) {
        indent(-12, -12);
        addVSpace(2);
        return;
    }
    if (strcmp(m_currentTag, "PRE") == 0 || strcmp(m_currentTag, "TT") == 0) {
//        self._doc.unset_style ("pre")
        m_cleanWhitespace.pop();
        if (m_cleanWhitespace.empty())
            m_cleanWhitespace.push(0x00);

        return;
    }
    if (strcmp(m_currentTag, "Q") == 0) {
        if (m_inTable) {
            m_currentTable->addText(" ''");
        }
        else {
            indent(0, -12);
            m_currentDocument->addText("''");
            indent(-12, 0);
            addVSpace(2);
        }
        return;
    }
    if (strcmp(m_currentTag, "TABLE") == 0 && m_inTable) {
        m_tableList.erase(m_tableList.end() - 1);
        if (m_tableList.empty()) {
            m_inTable = false;
            m_currentTable = NULL;
        }
        else {
            m_currentTable = m_tableList[m_tableList.size() - 1];
        }
/*
    def end_table (self):
        if self._get_element_info ('table'):
            self._add_vspace (1)
            if self._tableborder_forecolor == "default":
                self._doc.set_forecolor ("#999999")
            else:
                self._doc.set_forecolor (self._tableborder_forecolor)
            self._doc.add_hr (height=2)
            self._doc.unset_forecolor (self._doc.get_forecolor ()) 
            self._add_vspace (1)
        else:
            self._add_vspace (1)
        self._pop_element ("table")
        # Reset global for table colors
        self._tableborder_forecolor = "default"
*/
        return;
    }
}

std::string HtmlParser::findTagParameter(std::string parameter)
{
    std::map<std::string, std::string>::iterator it = m_tagParameters.find(parameter);
    if (it != m_tagParameters.end()) {
        // has align parameter
        if(parameter == "HREF" || parameter == "SRC" || parameter == "NAME")
            return it->second;
        else if(parameter == "STAYONDOMAIN" || parameter == "STAYONHOST" || parameter == "TABLES")
            return "true";
        else
            return toLower(it->second);
    }

    return "";
}

void HtmlParser::addDocEstSize(int size, bool canStart)
{
    // keep track of estimated size and possibly
    // create new document if needed
    if (m_currentDocumentSize + size > MAX_DOCUMENT_SIZE && canStart) {
        int oldId = m_currentDocument->getID();
        std::string oldhref = m_currentDocument->getUrlString();
        std::stringstream ss;
        ss << "plucker:/~parts~/" << m_url->getUrlString() << "/" << m_documents.size();
        std::string newhref = ss.str();
        int newId;
        m_resolver->getOrAdd(newhref, &newId);

        // link to new part
        int oldVisible = m_visible;
        m_visible = 1;
        m_currentDocument->addVSpace(0);
        m_currentDocument->startAnchor(newId);
        m_currentDocument->addText("Click here for the next part");
        m_currentDocument->stopAnchor();

        // create new document
        m_currentDocument = new PluckerTextDocument(newId, newhref);
        m_currentDocumentSize = 50;
        m_documents.push_back(m_currentDocument);

        // link back to old part
        m_currentDocument->startAnchor(oldId);
        m_currentDocument->addText("Click here for the previous part");
        m_currentDocument->stopAnchor();
        m_currentDocument->addVSpace(0);
        m_visible = oldVisible;

        // now set document to same italics, style, etc.
        if (m_visible == 1) {
            m_currentDocument->startAlignment(m_currentAlignment);
        }
        if (m_italics) {
            m_currentDocument->startItalics();
        }
        if (m_strike) {
            m_currentDocument->startStrike();
        }
        if (!m_styles.empty()) {
            m_currentDocument->startStyle(m_styles[m_styles.size() - 1]);
        }
        if (m_underline) {
            m_currentDocument->startUnderline();
        }
    }

    m_currentDocumentSize += size;
}

void HtmlParser::addVSpace(int space)
{
    if (m_visible == 1) {
        if (m_inTable) {
            m_currentTable->addNewLine();
        }
        else {
            addDocEstSize(2, false);
            m_currentDocument->addVSpace(space);
        }
    }       
}

void HtmlParser::pushVisibility(int visibility)
{
    m_visible = visibility;
    m_visibility.push_back(visibility);
}

void HtmlParser::popVisibility()
{
    if (!m_visibility.empty()) {
        m_visibility.erase(m_visibility.end() - 1);
        if (!m_visibility.empty()) 
            m_visible = m_visibility[m_visibility.size() - 1];
        else 
            m_visible = 1;
    }
    else {
        m_visible = 1;
    }
}

void HtmlParser::addNewLine()
{
    if (m_visible == 1) {
        if (m_inTable) {
            m_currentTable->addNewLine();
        }
        else {
            addDocEstSize(2, true);
            m_currentDocument->addNewLine();
        }
    }
}

void HtmlParser::indent(int left, int right)
{
    if (m_visible == 1 && !m_inTable) {
        addDocEstSize(4, false);
        m_currentDocument->indent(left, right);
    }
}

void HtmlParser::startStyle(int style)
{
    m_styles.push_back(style);
    if (m_visible == 1) {
        if (m_inTable) {
            m_currentTable->startStyle(style);
        }
        else {
            addDocEstSize(3, true);
            m_currentDocument->startStyle(style);
        }
    }
}

void HtmlParser::stopStyle()
{
    int oldStyle = 0;
    if (!m_styles.empty()) {
        m_styles.erase(m_styles.end() - 1);
        if (!m_styles.empty()) 
            oldStyle = m_styles[m_styles.size() - 1];
    }
    if (m_visible == 1) {
        if (m_inTable) {
            m_currentTable->stopStyle(oldStyle);
        }
        else {
            addDocEstSize(3, false);
            m_currentDocument->stopStyle(oldStyle);
        }
    } 
}

void HtmlParser::startAlignment(int alignment)
{
    m_alignment.push_back(alignment);
    m_currentAlignment = alignment;
    if (m_visible == 1 && !m_inTable) {
        addDocEstSize(3, true);
        m_currentDocument->startAlignment(alignment);
    } 
}

void HtmlParser::stopAlignment()
{
    int oldAlignment = 0;
    if (!m_alignment.empty()) {
        m_alignment.erase(m_alignment.end() - 1);
        if (!m_alignment.empty()) 
            oldAlignment = m_alignment[m_alignment.size() - 1];
    }
    m_currentAlignment = oldAlignment;
    if (m_visible == 1 && !m_inTable) {
        addDocEstSize(3, false);
        m_currentDocument->stopAlignment(oldAlignment);
    } 
}

void HtmlParser::startAnchor(int id)
{
    if (!m_inAnchor && m_visible == 1) {
        if (m_inTable) {
            m_currentTable->startAnchor(id);
        }
        else {
            addDocEstSize(6, false);
            m_currentDocument->startAnchor(id);
        }
        m_inAnchor = true;
    }
}

void HtmlParser::startAnchor(std::string href, int maxdepth, int stayonhost, bool tables)
{
    if (!m_inAnchor && m_visible == 1) {
        if (m_inTable) {
            m_currentTable->startAnchor(href, maxdepth, stayonhost, m_url, m_resolver, tables);
        }
        else {
            addDocEstSize(6, true);
            m_currentDocument->startAnchor(href, maxdepth, stayonhost, m_url, m_resolver, tables);
        }
        m_inAnchor = true;
    }
}

void HtmlParser::startImage(std::string href, int maxdepth, std::string alt)
{
    if (m_visible == 1) {
        if (m_inTable) {
            m_currentTable->startImage(href, maxdepth, alt, m_url, m_resolver);
        }
        else {
            addDocEstSize(6, true);
            m_currentDocument->startImage(href, maxdepth, alt, m_url, m_resolver);
        }
    }
}

void HtmlParser::stopAnchor()
{
    if (m_inAnchor && m_visible == 1) {
        if (m_inTable) {
            m_currentTable->stopAnchor();
        }
        else {
            addDocEstSize(2, false);
            m_currentDocument->stopAnchor();
            m_inAnchor = false;
        }
    }
}

void HtmlParser::startTD(int align, int colSpan, int rowSpan)
{
    if (m_inTable) {
        m_currentTable->addCell(align, colSpan, rowSpan);
    }
    else
    if (m_firstTd) {
        addVSpace(0);
        m_firstTd = false;
        startAlignment(align);
    }
    else {
        addVSpace(1);
        startAlignment(align);
/*    
            if self._get_element_info ('table'):
                if self._tableborder_forecolor == "default":
                    self._doc.set_forecolor ("#C0C0C0")
                else:
                    self._doc.set_forecolor (self._tableborder_forecolor)
                self._doc.add_hr (height=1)
                self._doc.unset_forecolor (self._doc.get_forecolor ()) 
                self._add_vspace (0)
*/
    }
}

void HtmlParser::stopLI()
{
    if (m_inLi) {
        int ind = 0;
        int listType = 0;
        if (!m_listTypes.empty()) {
            listType = m_listTypes[m_listTypes.size() - 1];
        }

        if (listType == 0) {
            ind = 12;
        }
        else {
            if (m_ulListDepth == 1) {
                ind = 7;
            }
            else if (m_ulListDepth == 2) {
                ind = 6;
            }
            else if (m_ulListDepth == 3) {
                ind = 8;
            }
            else {
                ind = 4;
            }
        }
        indent(-ind, 0);
        addVSpace(1);
        m_inLi = false;
    }
}

void HtmlParser::parseEntity()
{
    if (m_currentEntity[1] == '#') {
        int num = atoi(&m_currentEntity[2]);

        if (num >= 0x00 && num <= 0xff) {
            if (!m_inTable) {
                addDocEstSize(1, !m_inAnchor);
                m_currentDocument->addChar(num);
            }
            else {
                m_currentTable->addChar(num);
            }
        }
        else {
            if (!m_inTable) {
                addDocEstSize(m_currentEntityPlace, !m_inAnchor);
                m_currentDocument->addText(m_currentEntity);
            }
            else {
                m_currentTable->addText(m_currentEntity);
            }
        }
    }
    else {
        int j = 0;
        int len = 0;

        while(entity_ref[j].tag != NULL) {

            len = strlen(entity_ref[j].tag);
            if(!strncasecmp(m_currentEntity, entity_ref[j].tag, len)) {
                int length = strlen(entity_ref[j].conversion);
                if (!m_inTable) {
                    addDocEstSize(length, !m_inAnchor);
                    m_currentDocument->addText(entity_ref[j].conversion);
                }
                else {
                    m_currentTable->addText(entity_ref[j].conversion);
                }
                break;
            }
            j++;
        }
        if (m_currentEntityPlace - len > 0) {
            len = m_currentEntityPlace - len;
            if (!m_inTable) {
                addDocEstSize(len, !m_inAnchor);
                m_currentDocument->addText(&m_currentEntity[len]);
            }
            else {
                m_currentTable->addText(&m_currentEntity[len]);
            }
        }
    }
}

void HtmlParser::addUnicodeChar(char* text)
{
    long int num;
    int base = 10;
    char* alt;

    if (text[2] == 'X' || text[2] == 'x')
        base = 16;

    num = strtol(text + 2 + (base==16), NULL, base); 
    switch (num) {
        case 8211:
            alt = "-";
            break;
        case 8212:
            alt = "--";
            break;
        case 8216:
            alt = "`";
            break;
        case 8217:
            alt = "´";
            break;
        case 8220:
            alt = "\"";
            break;
        case 8230:
            alt = "...";
            break;
        case 8221:
            alt = "\"";
            break;
        case 8226:
            alt = " ";
            break;
        case 8482:
            alt = "(tm)";
            break;
        default:
            alt = text;
	}
    if (!m_inTable)
        m_currentDocument->addUnicodeChar(num, alt);
    else
        m_currentTable->addUnicodeChar(num, alt);
}

int HtmlParser::parseColor(const char* text)
{
    int color;
    int i = 0;

    while(color_ref[i].name != NULL) {
        if(!strcmp(text, color_ref[i].name))
            break;
        i++;
    }
    if (color_ref[i].name != NULL)
        return color_ref[i].color;

    if (text[0] == '#')
        text++;

    color = strtol(text, NULL, 16);
    if (color < 1 || color > 0xffffff)
        return 0x000000;
    else
        return color;
}

