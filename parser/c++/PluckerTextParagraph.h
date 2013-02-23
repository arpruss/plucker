/*
 * $Id: PluckerTextParagraph.h,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#if !defined(PLUCKERTEXTPARAGRAPH_H)
#define PLUCKERTEXTPARAGRAPH_H

#include <string>

#include "Config.h"
#include "Resolver.h"
#include "Writer.h"

class PluckerTextParagraph  
{
public:
	PluckerTextParagraph();
	virtual ~PluckerTextParagraph();

    void add(const char* txt);
    void addChar(const char data);
    void addInt(int value);
    int getUncompressedSize();
    int getAttributes() {return m_attributes + m_spacing;}
    int getExtraSpacing() {return m_spacing;}
    bool isNew() {return m_empty;}
    bool isNewLine() {return m_newline;}
    void setExtraSpacing(int spacing) {m_spacing = spacing;}
    void setNewLine(bool newline) {m_newline = newline;}
    void setNew(bool newpara) {m_empty = newpara;}
    std::string& getContent() {return m_content;}
    void finishDocument(Resolver* resolver, Config* config);

private:
    int m_attributes;
    bool m_empty;
    bool m_newline;
    int m_spacing;
    std::string m_content;    
};

#endif // !defined(PLUCKERTEXTPARAGRAPH_H)
