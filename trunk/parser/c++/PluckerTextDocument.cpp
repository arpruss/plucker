/*
 * $Id: PluckerTextDocument.cpp,v 1.12 2007/06/19 21:47:59 chrish Exp $
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

static const long maxParaSize = 3000;

#include "PluckerTextDocument.h"

PluckerTextDocument::PluckerTextDocument(int id, std::string urlString)
{
    m_id = id;
    m_urlString = urlString;
    m_compressedSize = 0;
    m_compressedContent = 0;
    m_contentSize = 0;
    m_compression = 0;

    addParagraph();
}

PluckerTextDocument::~PluckerTextDocument()
{
    for (size_t i = 0; i < m_paragraphs.size(); i++) {
        PluckerTextParagraph* paragraph = m_paragraphs[i];
        delete paragraph;
    }

    delete [] m_compressedContent;
}

void PluckerTextDocument::addChar(const char data)
{
    if (m_estimatedParaSize + 1 > 3000) {
        addParagraph();
    }

    m_currentParagraph->addChar(data);
    ++m_estimatedParaSize;
}

void PluckerTextDocument::addText(const char *temp)
{
    int length = strlen(temp);
    if (m_estimatedParaSize + length > maxParaSize) {
        addParagraph();
    }

    m_currentParagraph->add(temp);
    m_estimatedParaSize += length;
}

void PluckerTextDocument::ZlibCompressFunction()
{
	m_compressedSize = m_content.length() * 2 * sizeof(Byte);
    delete [] m_compressedContent;
	m_compressedContent = new Byte[m_compressedSize];

    ::compress(m_compressedContent, &m_compressedSize, (const unsigned char*) m_content.c_str(), m_content.length());
}

void PluckerTextDocument::compress(int compression)
{
    m_compression = compression;

    // collect paragraph data
    m_contentSize = 0;
    for (int i = 0; i < m_paragraphs.size(); i++) {
        PluckerTextParagraph* paragraph = m_paragraphs[i];
        m_contentSize += paragraph->getUncompressedSize();
        m_content.append(paragraph->getContent());
    }

    if (compression == 2) {
        ZlibCompressFunction();
    }
}

void PluckerTextDocument::finishDocument(Resolver* resolver, Config* config)
{
    for (int i = 0; i < m_paragraphs.size(); i++) {
        PluckerTextParagraph* paragraph = m_paragraphs[i];
        paragraph->finishDocument(resolver, config);
    }
}

void PluckerTextDocument::writeContents(Writer* writer)
{
    // write id
    writer->writeInt(m_id);

    // write number of paragraphs
    writer->writeInt(m_paragraphs.size());

    // write total length
    writer->writeInt(m_contentSize);

    // write type
    if (m_contentSize <= m_compressedSize || m_compressedSize == 0)
        writer->writeByte(0x00);
    else
        writer->writeByte(0x01);
    writer->writeByte(1);
    
    // write paragraph headers
    for (int i = 0; i < m_paragraphs.size(); i++) {
        PluckerTextParagraph* paragraph = m_paragraphs[i];

        // size
        writer->writeInt(paragraph->getUncompressedSize());
        // attributes
        writer->writeInt(paragraph->getAttributes());
    }

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

int PluckerTextDocument::getDocumentSize()
{
    int size = 0;

    if (m_contentSize <= m_compressedSize || m_compressedSize == 0) 
        size = m_contentSize;
    else
        size = m_compressedSize;

    size += 8 + m_paragraphs.size() * 4;

    return size;
}


void PluckerTextDocument::addParagraph()
{
    m_currentParagraph = new PluckerTextParagraph();
    m_paragraphs.push_back(m_currentParagraph);
    m_estimatedParaSize = 0;
}

void PluckerTextDocument::startStyle(int style)
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x11);
    m_currentParagraph->addChar(style);
    m_estimatedParaSize += 3;
}

void PluckerTextDocument::stopStyle(int oldStyle)
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x11);
    m_currentParagraph->addChar(oldStyle);
    m_estimatedParaSize += 3;
}

void PluckerTextDocument::addVSpace(int space)
{
    // code from Plucker - TextParser.py
    bool additional = false;

    if (space > 7)
        space = 7;

    if (space == 0 && !m_currentParagraph->isNew()) {
        if (!m_currentParagraph->isNewLine()) {
            // special case: use <NL> code
            m_currentParagraph->addChar(0x00);
            m_currentParagraph->addChar(0x38);
            m_currentParagraph->setNewLine(true);
            m_estimatedParaSize += 2;
            return;
        }
        else {
            // we are on a new line, so we need to add 4 units to get an additional new line
            space = 4;
        }
    }

    if (m_currentParagraph->isNew()) {
        if (space == 0 && additional) 
            space = 4;
        int oldSpacing = m_currentParagraph->getExtraSpacing();
        if (!additional && oldSpacing >= space) {
            // already enough space
            return;
        }
        int newSpacing = space;
        if (additional)
            newSpacing = oldSpacing + space;
        else if (oldSpacing > space)
            newSpacing = oldSpacing;                       
        if (newSpacing <= 7)
            m_currentParagraph->setExtraSpacing(newSpacing);
        else {
            while (newSpacing > 7) {
                m_currentParagraph->setExtraSpacing(7);
                newSpacing -= 7;
                m_currentParagraph->addChar(' ');
                m_currentParagraph->addChar(' ');
                m_estimatedParaSize += 2;
                addParagraph();
            }
            m_currentParagraph->setExtraSpacing(newSpacing);
        }
    }
    else {
        addParagraph();
        m_currentParagraph->setExtraSpacing(space);
    }
}

void PluckerTextDocument::startAlignment(int alignment)
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x29);
    m_currentParagraph->addChar(alignment);
    m_estimatedParaSize += 3;
}

void PluckerTextDocument::stopAlignment(int oldAlignment)
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x29);
    m_currentParagraph->addChar(oldAlignment);
    m_estimatedParaSize += 3;
}

void PluckerTextDocument::startItalics()
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x40);
    m_estimatedParaSize += 2;
}

void PluckerTextDocument::stopItalics()
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x48);
    m_estimatedParaSize += 2;
}

void PluckerTextDocument::startAnchor(int id)
{
    // add anchor to know id
    m_currentParagraph->setNew(false);
    m_currentParagraph->setNewLine(false);
    m_estimatedParaSize += 6;

    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x0A);
    m_currentParagraph->addInt(id);
}

void PluckerTextDocument::startAnchor(std::string href, int maxDepth, int stayOnHost, Url* url, Resolver* resolver, bool tables)
{
    m_currentParagraph->setNew(false);
    m_currentParagraph->setNewLine(false);
    m_estimatedParaSize += 6;

    if(href.substr(0, 1) == "#") {
        // Add Paragraph link
        int idx = resolver->addAnchor(href.substr(1));
        m_currentParagraph->addChar(0x00);
        m_currentParagraph->addChar(0x0C);
        m_currentParagraph->addInt(0);
        m_currentParagraph->addInt(idx);
    }
    else {
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

        m_currentParagraph->addChar(0x00);
        m_currentParagraph->addChar(0x0A);
        m_currentParagraph->addInt(id);

        if (newUrl->shouldFetch() && isNew) {
            // add to list of those to fetch, will be deleted by parser
            m_links.push_back(newUrl);
        }
        else {
            delete newUrl;
        }
    }
}

void PluckerTextDocument::stopAnchor()
{
    m_currentParagraph->setNew(false);
    m_currentParagraph->setNewLine(false);
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x08);
    m_estimatedParaSize += 2;
}

void PluckerTextDocument::startUnderline()
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x60);
    m_estimatedParaSize += 2;
}

void PluckerTextDocument::stopUnderline()
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x68);
    m_estimatedParaSize += 2;
}

void PluckerTextDocument::indent(int left, int right)
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x22);
    m_currentParagraph->addChar(left);
    m_currentParagraph->addChar(right);
    m_estimatedParaSize += 4;
}

void PluckerTextDocument::startLI(std::string text)
{
    for (int i = 0; i < text.length(); i++) {
        m_currentParagraph->addChar(text[i]);
    }
    m_estimatedParaSize += text.length();
}

void PluckerTextDocument::addHR(int height, int width, int percWidth)
{
    m_currentParagraph->setNew(false);
    m_currentParagraph->setNewLine(false);
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x33);
    m_currentParagraph->addChar(height);
    m_currentParagraph->addChar(width);
    m_currentParagraph->addChar(percWidth);
    m_estimatedParaSize += 5;
}

void PluckerTextDocument::startStrike()
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x70);
    m_estimatedParaSize += 2;
}

void PluckerTextDocument::stopStrike()
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x78);
    m_estimatedParaSize += 2;
}

void PluckerTextDocument::addNewLine()
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x38);
    m_estimatedParaSize += 2;
}

void PluckerTextDocument::startImage(std::string href, int maxDepth, std::string alt, Url* url, Resolver* resolver)
{
    m_currentParagraph->setNew(false);
    m_currentParagraph->setNewLine(false);
    
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
        m_currentParagraph->addChar(0x00); // Will be changed when resolved
        m_currentParagraph->addChar(0x5c); // if no BigId document is found
        m_currentParagraph->addInt(id);
        m_currentParagraph->addInt(id);
        m_estimatedParaSize += 6;

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
            addText(alt.c_str());
        }

        delete newUrl;
    }
}

void PluckerTextDocument::addTable(int id)
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x92);
    m_currentParagraph->addInt(id);
    m_estimatedParaSize += 4;
}

void PluckerTextDocument::addUnicodeChar(int num, char* text)
{
    m_currentParagraph->addChar(0x00);
    if (num > 0xffff)
        m_currentParagraph->addChar(0x85);
    else
        m_currentParagraph->addChar(0x83);
    m_currentParagraph->addChar(strlen(text));
    if (num > 0xffff)
        m_currentParagraph->addInt(num >> 16);
    m_currentParagraph->addInt(num);
    m_currentParagraph->add(text);
    m_estimatedParaSize += (num > 0xffff ? 7 : 5) + strlen(text);
}


void PluckerTextDocument::addColor(int color)
{
    m_currentParagraph->addChar(0x00);
    m_currentParagraph->addChar(0x53);
    m_currentParagraph->addChar(color >> 16);
    m_currentParagraph->addChar(color >> 8);
    m_currentParagraph->addChar(color);
    m_estimatedParaSize += 5;
}

void PluckerTextDocument::addName(Resolver* resolver, std::string name)
{
    // Start a new one.
    if(m_estimatedParaSize)
        addParagraph();
    // Document and paragraph this name points to
    resolver->addName(name, m_id, m_paragraphs.size() - 1);
}

/*
    def add_name (self, name):
        """Give name to the current paragraph"""
        self._paragraph.add_name (name)


    def get_forecolor (self):
        """Get current forecolor value. value should be an rgb"""
        return self._attributes.get_forecolor ()


    def set_forecolor (self, value):
        """Set current forecolor to 'value' """
        # assess whether a hex color (eg #C0C0C0) or a text colorname (eg 'silver')
        rgb = None
        if value in Colornames.keys():
            rgb = Colornames[value]
        elif value[0] == '#':
            try:
                val = string.atoi(value[1:], 16)
                rgb = '%06x' % val
            except:
                pass
        # May need to do error checking, at this last bit, if people using non
        #  standard color text names    
        else:
            try:
                # possible hex value without leading hash mark...
                val = string.atoi(value, 16)
                rgb = '%06x' % val
            except:
                pass
            
        if not rgb:
            return

        # Don't want any white text on PDA's white form since would be invisible,
        # so if white, just darken it a bit to silver.
        if string.atoi(rgb, 16) == 0xFFFFFF:
            rgb = "C0C0C0"

        if self._attributes.push_forecolor (rgb):
            # forecolor has changed
            self._add_forecolor_change () 
            
    def unset_forecolor (self, value):
        """Un-set an alignment change by a previous 'set_forecolor'.
        Make sure it previously set 'value' (unless 'value' is None)."""
        if self._attributes.pop_forecolor (value):
            # forecolor has changed
            self._add_forecolor_change ()            
            
    def _add_forecolor_change (self):
        """Add info about a new forecolor to take effect."""
        self._paragraph.add_set_forecolor (self._attributes.get_forecolor ())        
        

    def _ship_paragraph (self):
        """Finish the current paragraph and start a fresh one"""

        # finish off the old paragraph
        the_anchor_dict = None
        if self._within_anchor ():
            the_anchor_dict = {}
            the_anchor_dict.update (self._anchor_dict)
            self.add_document_link_end ()
        if self._attributes.change_italics (0):
            self._paragraph.add_italics_end ()
        if self._attributes.change_underline (0):
            self._paragraph.add_underline_end ()
        if self._attributes.change_strike (0):
            self._paragraph.add_strike_end ()
     
            
        # now start new paragraph
        self._doc.add_paragraph (self._paragraph)
        self._paragraph = PluckerDocs.PluckerTextParagraph ()
        self._is_new_paragraph = 1
        self._is_new_line = 1
        self._approximate_size = 0

        if self._attributes.get_style ():
            # we are in non-default style
            self._add_style_change ()

        if self._attributes.get_alignment ():
            # we are in non-default alignment
            self._add_alignment_change ()
        
        if self._attributes.get_forecolor () or self._color_paragraphs:
            # we are in non-default forecolor
            self._paragraph.add_set_forecolor (self._attributes.get_forecolor ())    

        (l, r) = self._attributes.indent ()
        if l != 0 or r != 0:
            self._paragraph.add_set_margin (l, r)

        if self._attributes.change_italics (0):
            self._paragraph.add_italics_start ()

        if self._attributes.change_underline (0):
            self._paragraph.add_underline_start ()

        if self._attributes.change_strike (0):
            self._paragraph.add_strike_start ()
            
        # re-start the link if there was one
        if the_anchor_dict is not None:
            self.add_document_link_start (the_anchor_dict)
            

    def _find_text_split (self, line, size):
        """Find a split of line so that one part is approx. size bytes
        long.  Return (first_part, rest)."""
        first = line[:size]
        rest = line[size:]

        # We try to split at a space:
        if " " in rest:
            f = string.split(rest, None, 1)
            if len (f) > 0:
                # Shouldn't this always be the case?  Mike reports that it can happen...
                first = first + f[0]
                if len (f) > 1:
                    rest = f[1]
                else:
                    rest = ""
            else:
                # Strange... how does this happen?
                first = first + rest
                rest = ""
        else:
            # No decent split found: just don't split it...
            first = first + rest
            rest = ""

        return (first, rest)
        

    def add_text (self, text):
        """Add some text, maybe even many lines."""
        lines = string.split (text, "\n")
        for i in range (len (lines)):
            line = lines[i]
            while 1:
                new_size = self._approximate_size + len (line)
                if self._within_anchor ():
                    max_size = self._max_para_size+self._max_para_size_stretch
                else:
                    max_size = self._max_para_size
                if new_size < max_size:
                    break
                rest_size = self._max_para_size - self._approximate_size
                if rest_size < 0:
                    rest_size = 0
                (first, rest) = self._find_text_split (line, rest_size)
                self._paragraph.add_text (first)
                self._approximate_size = self._approximate_size + len (first)
                self._is_new_paragraph = 0
                self._is_new_line = 0
                line = rest
                self._ship_paragraph ()
                if not line:
                    break
            
            if line:
                self._paragraph.add_text (line)
                self._approximate_size = self._approximate_size + len (line)
                self._is_new_paragraph = 0
                self._is_new_line = 0

            if i != len (lines)-1:
                # add the newline that was left out
                self.add_vspace (n_units=0, additional=1)

    def add_unicode_char (self, char_code, text_alternative):
        """Add a Unicode character, along with a non-Unicode text alternative."""
        self._paragraph.add_unicode_char (char_code, text_alternative)
        self._is_new_line = 0
        self._is_new_paragraph = 0
        self._approximate_size = self._approximate_size + 7 + len(text_alternative)

*/

/*
    def add_unicode_char (self, char, text_equivalent):
        self._add ((CMD_UNICODE_CHAR, (char, text_equivalent)))
        self._estimated_length = self._estimated_length + 3 + len(text_equivalent) + (((char > 0xFFFF) and 4) or 2)

    def add_set_forecolor (self, value):
        self._add ((CMD_SET_FORECOLOR, value))
        self._estimated_length = self._estimated_length + 5
          
*/

/*
            elif tag == CMD_SET_FORECOLOR:
                if allow_newstuff:
                    rgb = value
                    hexr = string.atoi (rgb[0:2], 16) # extract first 2 characters, then convert to hex
                    hexg = string.atoi (rgb[2:4], 16) # extract middle 2 characters, then convert to hex
                    hexb = string.atoi (rgb[4:6], 16) # extract last 2 characters,, then convert to hex
                    data.append (struct.pack (">BBBBB", 0, 0x53, hexr, hexg, hexb)) 
                else:
                    # ignored :-(   
                    pass
            elif tag == CMD_UNICODE_CHAR:
                if allow_newstuff:
                    if value[0] > 0xFFFF:
                        data.append(struct.pack (">BBBL", 0, 0x85, len(value[1]), value[0]))
                    else:
                        data.append(struct.pack (">BBBH", 0, 0x83, len(value[1]), value[0]))
                    data.append(value[1])
                else:
                    pass
*/
