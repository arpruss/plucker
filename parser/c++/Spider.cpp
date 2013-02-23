/*
 * $Id: Spider.cpp,v 1.7 2007/06/11 19:22:55 chrish Exp $
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

#if defined(WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <sstream>

#include "FileRetriever.h"
#include "GlobalFunctions.h"
#include "HttpRetriever.h"
#include "Parser.h"
#include "Spider.h"

#include "PluckerCategoryDocument.h"
#include "PluckerIndexDocument.h"
#include "PluckerLinkIndexDocument.h"
#include "PluckerMailtoDocument.h"
#include "PluckerMetaDocument.h"

Spider::Spider(Message *message, Config *config, Resolver* resolver)
{
    m_message = message;
    m_config = config;
    m_resolver = resolver;
    m_urlInfo = false;
    m_liDoc = 0;
}

Spider::~Spider()
{
    for (int i = 0; i < m_pending.size(); i++) {
        Url* url = m_pending[i];
        delete url;
    }

    for (int i = 0; i < m_retrieved.size(); i++) {
        Url* url = m_retrieved[i];
        delete url;
    }

    for (int i = 0; i < m_documents.size(); i++) {
        PluckerDocument* doc = m_documents[i];
        delete doc;
    }
}

void Spider::startup()
{
    // get starting info
    showRunParameters();

    Url* startUrl = new Url(m_config->m_homeUrl);
    startUrl->m_maxDepth = m_config->m_maxDepth;
    startUrl->m_bpp = m_config->m_bpp;
    startUrl->m_id = 2;
    if (m_config->m_homeStayOnDomain)
        startUrl->setStayOnHost(2);
    if (m_config->m_homeStayOnHost)
        startUrl->setStayOnHost(1);
    m_resolver->add(startUrl->getUrlString(), startUrl->m_id);

    m_urlInfo = true;
    if (m_config->m_noUrlInfo != 0) {
        m_urlInfo = false;
    }
    bool categoryInfo = false;
    if (!(m_config->m_category.empty())) {
        categoryInfo = true;
    }

    // always first record
    PluckerIndexDocument *doc = new PluckerIndexDocument(m_urlInfo, categoryInfo);
    addPluckerDocument(doc);

    addUrlToPending(startUrl);

    m_liDoc = 0;
    if (m_urlInfo) {
        m_liDoc = new PluckerLinkIndexDocument();
        addPluckerDocument(m_liDoc);
    }

    if (categoryInfo) {
        PluckerCategoryDocument *catDoc = new PluckerCategoryDocument(m_config->m_category);
        addPluckerDocument(catDoc);
    }

/*    PluckerMetaDocument* metaDoc = new PluckerMetaDocument();
    if (!(m_config->m_title.empty())) {
        metaDoc->addTitle(m_config->m_title);
    }
    if (!(m_config->m_author.empty())) {
        metaDoc->addAuthor(m_config->m_author);
    }
    addPluckerDocument(metaDoc); */
}

bool Spider::runOne()
{
    if (haveUrlPending()) {
        int linksTogo = numLinksToDo();
        int linksDone = numLinksDone();
        m_message->updateProgress(linksDone, linksTogo);
        std::stringstream ss;
        ss << "---- " << linksDone << " collected, " << linksTogo << " to do ----";
        m_message->addMessage(ss.str(), 1);

        Url* url = removeUrlFromPending();
        m_message->addMessage("Retrieving: " + url->getUrlString(), 1);

        retrieveAndParse(url);
    }

    return true;
}

bool Spider::runAll()
{
    // retrieve all links
    while (haveUrlPending()) {
        runOne();
    }

    int links_togo = numLinksToDo();
    int links_done = numLinksDone();
    m_message->updateProgress(links_done, links_togo);

    return true;
}

void Spider::finish(Writer* writer)
{
    // create link documents
    if (m_liDoc != 0 && m_urlInfo) {
        m_liDoc->createLinkDocuments(m_resolver);
        int numLinks = m_liDoc->getNumLinkDocuments();
        for (int i = 0; i < numLinks; i++) {
            addPluckerDocument(m_liDoc->getLinkDocument(i));
        }
    }

    int linksDone = numLinksDone();
    std::stringstream ss;
    ss << "---- all " << linksDone << " pages retrieved and parsed ----";
    m_message->addMessage(ss.str(), 1);

    m_message->addMessage("", 1);
    m_message->addMessage("Writing out collected data...", 1);

    // find highest doc
    int numberDocs = getNumberDocuments();
    int highestId = 0;
    for (int i = 0; i < numberDocs; i++) {
        PluckerDocument* doc = getDocument(i);
        doc->finishDocument(m_resolver, m_config);

        if (doc->getID() > highestId)
            highestId = doc->getID();
    }

    // sort the documents
    m_sortedDocuments.resize(highestId + 1);
    for (int i = 0; i < numberDocs; i++) {
        PluckerDocument* doc = getDocument(i);
        doc->compress(m_config->m_compressionType);
        m_sortedDocuments[doc->getID()] = doc;
    }

    // initialize writing
    if (!writer->startWrite(numberDocs)) {
        return;
    }

    // write record offsets
    for (int i = 0; i < highestId + 1; i++) {
        PluckerDocument* document = m_sortedDocuments[i];	    
        if (document != 0) {
            writer->writeOffset(document->getDocumentSize());
        }
    }
    writer->endOffset();

    // write each document
    for (int i = 0; i < highestId + 1; i++) {
        PluckerDocument* doc = m_sortedDocuments[i];
        if (doc != 0) {
            std::stringstream ss;
            ss << "Wrote " << doc->getID() << " <= " << doc->getUrlString();
            m_message->addMessage(ss.str(), 1);

            doc->writeContents(writer);
        }
    }

    // give subclasses chance to cleanup writing
    writer->endWrite();

    m_message->addMessage("Done!", 1); 
}

int Spider::numLinksDone()
{
    return m_retrieved.size();
}

int Spider::numLinksToDo()
{
    return m_pending.size();
}

bool Spider::haveUrlPending()
{
    return !m_pending.empty();
}

void Spider::addUrlToPending(Url* url)
{
#if !defined(WIN32)
    // libhttp isn't good about rejecting these.
    if(0 == url->m_urlPath.find("/./"))
        return;
    if(0 == url->m_urlPath.find("/../"))
        return;
#endif

    if(m_config->m_exclusion->Check(url))
        m_pending.push_back(url);
else
    m_message->addMessage("Excluding " + url->getUrlString(), 1);
}

Url* Spider::removeUrlFromPending()
{
    Url* url = m_pending[0];
    m_pending.erase(m_pending.begin());

    // need to add to already retrieved list
    m_retrieved.push_back(url);

    return url;
}

void Spider::addPluckerDocument(PluckerDocument* doc)
{
    m_documents.push_back(doc);
}

void Spider::showRunParameters()
{
    // show message detailing what info is being used by the parser
    m_message->addMessage("Plucker-build (c) Bill Nalen 2003", 1);
    std::stringstream ss;
    ss << "Plucker-build vesion " << BUILD_MAJOR << "." << BUILD_MINOR << " by Bill Nalen (bill@nalens.com)";
    m_message->addMessage(ss.str(), 1);
    m_message->addMessage("Plucker home is: " + m_config->m_pluckerHome, 1);
    m_message->addMessage("Plucker directory is: " + m_config->m_pluckerDir, 1);
    m_message->addMessage("Plucker home url is: " + m_config->m_homeUrl, 1);
    m_message->addMessage("Document name is: " + m_config->m_docName, 1);
    m_message->addMessage("Filename is: " + m_config->m_docFile, 1);
    std::stringstream ss2;
    ss2 << "Verbosity is: " << m_config->m_verbosity;
    m_message->addMessage(ss2.str(), 1);
}

bool Spider::retrieveAndParse(Url* url)
{
    // fetch document & parse it
    std::string scheme = toLower(url->m_scheme);
    Retriever* retriever = 0;

    if (scheme == "http" || scheme == "https") {
        if (m_config->m_waitTime)
#if defined(WIN32)
            Sleep(m_config->m_waitTime);
#else
            usleep(m_config->m_waitTime * 1000);
#endif
        retriever = new HttpRetriever();
        if (!retriever->retrieve(url, m_config, m_message, m_resolver)) {
            // retrieving had an unrecoverable error
            return false;
        }
    }
    else if (scheme == "file") {
        retriever = new FileRetriever();
        if (!retriever->retrieve(url, m_config, m_message, m_resolver)) {
            // retrieving had an unrecoverable error
            return false;
        }
    }
    else if (scheme == "mailto") {
        int newId = url->m_id;
        PluckerMailtoDocument* doc = new PluckerMailtoDocument(newId);
        addPluckerDocument(doc);
        doc->parseMailUrl(url->getUrlString());
        url->m_statusText = "OK";

        // nothing else to do here
        return true;
    } 
    else {
        m_message->addMessage("Unknown scheme: " + url->m_scheme, 1);
        return false;
    }

    m_message->addMessage("Retrieved URL with status: " + url->m_statusText, 1);

    // check for bad status codes
    if (toLower(url->m_statusText) != "ok") {
        return false;
    }

    if (url->m_id == 2) {
        m_startDocOk = true;
    }

    Parser* parser = retriever->getParser();
    if (parser != 0) {
        for (int i = 0; i < parser->getNumPluckerDocuments(); i++) {
            PluckerDocument* doc = parser->getPluckerDocument(i);
            addPluckerDocument(doc);

            // get links & images to retrieve
            for (int j = 0; j < doc->getNumLinksToFetch(); j++) {
                addUrlToPending(doc->getLinkToFetch(j));
            }
        }
                
        return true;
    }

    m_message->addMessage("No parser for content type: " + url->m_contentType, 1);

    return false;
}
