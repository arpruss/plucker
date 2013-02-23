/*
 * $Id: HttpRetriever.cpp,v 1.4 2006/06/21 01:16:05 chrish Exp $
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
#include <wininet.h>
#endif

#include "GlobalFunctions.h"
#include "HtmlParser.h"
#include "HttpRetriever.h"
#if !defined(WIN32)
#include <http.h>
#endif

#include <sstream>

HttpRetriever::HttpRetriever()
{
    m_parser = 0;
}

HttpRetriever::~HttpRetriever()
{
    delete m_parser;
    m_parser = 0;
}

bool HttpRetriever::retrieve(Url* url, Config* config, Message* message, Resolver* resolver)
{
#if !defined(WIN32)
    HTTP_Response hResponse;
    HTTP_Extra hExtra;
    char* name;
    char* ptr;
    char buff[256];
    int id = 0;


    name = strdup((char *)url->getUrlString().c_str());
    ptr = strchr(name, '#');
    if(ptr != NULL)
    	*ptr = '\0';

//    hResponse = http_request((char *)url->getUrlString().c_str(),
    hResponse = http_request(name,
    						(HTTP_Extra *)NULL, kHMethodGet,
    						HFLAG_RETURN_HEADER);

    if(ptr != NULL)
    	*ptr = '#';
    free(name);

    if(hResponse.iError != 0)
    {
        message->addMessage("Error fetching " + url->getUrlString(), 1);

        if(hResponse.pHdr != NULL) free(hResponse.pHdr);
        if(hResponse.pData != NULL) free(hResponse.pData);
        return false;
    }

    // gets unique id associated with this link
    resolver->getOrAdd(url->getUrlString(), &id);

    find_header(hResponse.pHdr, -1, "Content-Type:", buff, 256);
    url->m_contentType = buff;
    if(strcmp(hResponse.szHCode, "200"))
        url->m_statusText = "Failed";
    else
        url->m_statusText = "OK";

    // create the parser associated with this type of content
    m_parser = Parser::getParserByContent(url, resolver, message, url->m_contentType);

    if (m_parser != 0)
        m_parser->processData(config, (const unsigned char *)hResponse.pData, hResponse.lSize);

    if(hResponse.pHdr != NULL) free(hResponse.pHdr);
    if(hResponse.pData != NULL) free(hResponse.pData);
    return true;

#else
    HINTERNET io = InternetOpen(config->m_userAgent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (io == NULL) {
        message->addMessage("InternetOpen failed for " + url->getUrlString(), 1);
        return false;
    }

	char userName[1025];
	DWORD n_size = 1024;
    strcpy(userName, "");
    if (url->m_userName.empty()) {
        GetUserName(userName, &n_size);
    }
    else {
        strcpy(userName, url->m_userName.c_str());
    }
   
	HINTERNET ic = InternetConnect(io, url->m_hostName.c_str(), url->m_port, userName, url->m_password.c_str(), INTERNET_SERVICE_HTTP, 0, 0);
	if (ic == NULL) {
        message->addMessage("InternetConnect failed for " + url->getUrlString(), 1);
        InternetCloseHandle(io);
		return false;
	}

	HINTERNET hor = HttpOpenRequest(ic, NULL, url->m_path.c_str(), NULL, NULL, NULL, INTERNET_FLAG_CACHE_IF_NET_FAIL | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_RESYNCHRONIZE | INTERNET_FLAG_NO_UI, 0);
	if (hor == 0) {
        message->addMessage("HttpOpenRequest failed for " + url->getUrlString(), 1);
        InternetCloseHandle(ic);
        InternetCloseHandle(io);
		return false;
	}

	BOOL err = HttpSendRequest(hor, NULL, 0, NULL, 0);
	if (err == FALSE) {
        message->addMessage("HttpSendRequest failed for " + url->getUrlString(), 1);
        InternetCloseHandle(hor);
        InternetCloseHandle(ic);
        InternetCloseHandle(io);
		return false;
	}

	INTERNET_BUFFERS httpBuffer;
	httpBuffer.dwStructSize = sizeof(INTERNET_BUFFERS);
	httpBuffer.Next = NULL;
	err = HttpEndRequest(hor, &httpBuffer, 0, 0);

	char headerbuf[1024];
	DWORD headerbufsize = 1024;
	DWORD headerindex = 0;
    headerbuf[0] = '\0';
	err = HttpQueryInfo(hor, HTTP_QUERY_CONTENT_TYPE, headerbuf, &headerbufsize, &headerindex);
    url->m_contentType = headerbuf;
	//int atSemi = contentType.find(';');
	//if (atSemi != std::string::npos)
	//	contentType = ToLower(contentType.substr(0, atSemi));

	headerbufsize = 1024;
	headerindex = 0;
	//char headerbuf2[1024];
    headerbuf[0] = '\0';
	err = HttpQueryInfo(hor, HTTP_QUERY_STATUS_CODE, headerbuf, &headerbufsize, &headerindex);
    url->m_statusCode = headerbuf;

	headerbufsize = 1024;
	headerindex = 0;
	//char headerbuf3[1024];
    headerbuf[0] = '\0';
	err = HttpQueryInfo(hor, HTTP_QUERY_STATUS_TEXT, headerbuf, &headerbufsize, &headerindex);
    url->m_statusText = headerbuf;
/*    if (statusCode[0] == '4' || statusCode[0] == '5') {
        // 400's bad, 500's bad
        if (m_configuration->GetVerbosity() > 0) {
            std::cout << "Failed to retrieve item for " << m_url->GetLink() << " reason was " << statusCode << "\n" << statusText << std::endl;
        }
        CloseConnection();
        return;
    }
*/
    // gets unique id associated with this link
    int id = 0;
    resolver->getOrAdd(url->getUrlString(), &id);

    // create the parser associated with this type of content
    m_parser = Parser::getParserByContent(url, resolver, message, url->m_contentType);

    if (m_parser != 0) {
        DWORD size = 1;   
	    while (size > 0) {
		    // this will block if no data available
		    if (!InternetQueryDataAvailable(hor, &size, 0, 0)) {
                InternetCloseHandle(hor);
                InternetCloseHandle(ic);
                InternetCloseHandle(io);
                delete m_parser;
                m_parser = 0;
			    return false;
		    }

            if (size > 0) {
		        unsigned char* buffer = new unsigned char[size];
		        DWORD read = 0;
		        if (!InternetReadFile(hor, buffer, size, &read)) {
                    InternetCloseHandle(hor);
                    InternetCloseHandle(ic);
                    InternetCloseHandle(io);
			        delete [] buffer;
                    delete m_parser;
                    m_parser = 0;
			        return false;
		        }
                
                m_parser->processData(config, buffer, read);
//                for (DWORD i = 0; i < read; i++) {
//                    url->m_content.Append(buffer[i], 1);
//                }
                //url->m_content.Append(buffer);

		        delete [] buffer;
            }
	    }
    }

    InternetCloseHandle(hor);
    InternetCloseHandle(ic); 
    InternetCloseHandle(io);
#endif

    return true;
}

