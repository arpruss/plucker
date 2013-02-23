/*
 * $Id: FileRetriever.cpp,v 1.5 2005/05/02 03:17:31 chrish Exp $
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

#include <fstream>

#include "FileRetriever.h"
#include "GlobalFunctions.h"

FileRetriever::FileRetriever()
{
    m_parser = 0;
}

FileRetriever::~FileRetriever()
{
}

bool FileRetriever::retrieve(Url* url, Config* config, Message* message, Resolver* resolver)
{
    guessType(url);

    int id = 0;
    url->m_statusText = "KO";
    resolver->getOrAdd(url->getUrlString(), &id);

    // create the parser associated with this type of content
    m_parser = Parser::getParserByContent(url, resolver, message, url->m_contentType);

    if (m_parser != 0) {
        if (url->m_contentType.substr(0, 6) == "image/") {
            std::ifstream fis(url->m_urlPath.c_str());
            if (fis != 0) {
                char* buffer;
                size_t size;
                fis.seekg (0, std::ios_base::end);
                size = fis.tellg();
                fis.seekg (0, std::ios_base::beg);
                buffer = new char [size];
                fis.read(buffer, size);
                fis.close();
                m_parser->processData(config, (const unsigned char*) buffer, size);
                delete [] buffer;
                url->m_statusText = "OK";
            }
        }
        else {
            std::ifstream fis(url->m_urlPath.c_str());
            if (fis != 0) {
                while (!fis.eof()) {
                    std::string test;
                    std::getline(fis, test);
                    m_parser->processData(config, (const unsigned char*) test.c_str(), test.length());
                }
                url->m_statusText = "OK";
            }
        }

        if (url->m_statusText != "OK") {
            message->addMessage("Could not open file " + url->m_urlPath, 1);
            url->m_statusText = "Not Found";
            return false;
        }
    }
    else {
        // so message of no handler gets printed
        url->m_statusText = "OK";
    }

    return true;
}

void FileRetriever::guessType(Url* url)
{
    // find extension
    int at = url->m_urlPath.find_last_of('.');

    if (at != std::string::npos) {
        std::string ext = toLower(url->m_urlPath.substr(at));

        if (ext == ".gif") 
            url->m_contentType = "image/gif";
        else if (ext == ".png")
            url->m_contentType = "image/png";
        else if (ext == ".jpg")
            url->m_contentType = "image/jpeg";
        else if (ext == ".jpe")
            url->m_contentType = "image/jpeg";
        else if (ext == ".jpeg")
            url->m_contentType = "image/jpeg";
        else if (ext == ".html")
            url->m_contentType = "text/html";
        else if (ext == ".htm")
            url->m_contentType = "text/html";
        else if (ext == ".txt")
            url->m_contentType = "text/plain";
        else if (ext == ".asc")
            url->m_contentType = "text/plain";
        else if (ext == ".doc")
            url->m_contentType = "application/msword";
        else if (ext == ".bmp") 
            url->m_contentType = "image/bmp";
    }
    else {
        url->m_contentType = "";
    }
}
