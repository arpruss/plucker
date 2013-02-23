/*
 * $Id: Spider.h,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#if !defined(SPIDER_H)
#define SPIDER_H

#include <vector>

#include "Config.h"
#include "Message.h"
#include "PluckerDocument.h"
#include "PluckerLinkIndexDocument.h"
#include "Resolver.h"
#include "Url.h"
#include "Writer.h"

class Spider
{
public:
	Spider(Message *message, Config *config, Resolver* resolver);
	virtual ~Spider();

    void startup();
    bool runAll();
    bool runOne();
    void finish(Writer* writer);
    bool haveUrlPending();
    
    int getNumberDocuments() {return m_documents.size();}
    PluckerDocument* getDocument(int index) {return m_documents[index];}
    int getNumberSortedDocuments() {return m_sortedDocuments.size();}
    PluckerDocument* getSortedDocument(int index) {return m_sortedDocuments[index];}
    void addPluckerDocument(PluckerDocument* doc);
    void addUrlToPending(Url* url);
    bool hasStartUrl() {return m_startDocOk;}

private:
    Url* removeUrlFromPending();
    bool retrieveAndParse(Url* url);
    void showRunParameters();
    int numLinksDone();
    int numLinksToDo();

    bool m_startDocOk;
    Config *m_config;
    Message *m_message;
    Resolver *m_resolver;
    std::vector<Url*> m_pending;
    std::vector<Url*> m_retrieved;
    std::vector<PluckerDocument*> m_documents;
    std::vector<PluckerDocument*> m_sortedDocuments;
    bool m_urlInfo;
    PluckerLinkIndexDocument* m_liDoc;
};

#endif // !defined(PARSER_H)
