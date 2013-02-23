/*
 * $Id: Parser.h,v 1.3 2005/04/06 20:35:05 chrish Exp $
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

#if !defined(PARSER_H)
#define PARSER_H

#include <string>
#include <vector>

#include "Message.h"
#include "PluckerDocument.h"
#include "Resolver.h"
#include "Url.h"

class Parser  
{
public:
	Parser();
	virtual ~Parser();

    virtual void processData(Config* config, const unsigned char* buffer, unsigned long read) = 0;
    static Parser* getParserByContent(Url* url, Resolver* resolver, Message* message, std::string contentType);

    int getNumPluckerDocuments() {return m_documents.size();}
    PluckerDocument* getPluckerDocument(int index) {return m_documents[index];}

protected:
    std::vector<PluckerDocument*> m_documents;
};

#endif // !defined(PARSER_H)
