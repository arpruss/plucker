/*
 * $Id: Parser.cpp,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#include "Parser.h"

#include "GlobalFunctions.h"
#include "HtmlParser.h"
#include "TextParser.h"
#include "ImageParser.h"

Parser::Parser()
{
}

Parser::~Parser()
{
}

Parser* Parser::getParserByContent(Url* url, Resolver* resolver, Message* message, std::string contentType)
{
    std::string lContentType = toLower(contentType);

    if (lContentType.substr(0, 9) == "text/html") {
        message->addMessage("Parsing HTML content", 2);
        return new HtmlParser(url, resolver);
    }
    else if (lContentType.substr(0, 10) == "text/plain") {
        message->addMessage("Parsing text content", 2);
        return new TextParser(url, resolver);
    }
    else if (lContentType.substr(0, 6) == "image/") {
        message->addMessage("Parsing image content", 2);
        return new ImageParser(url, resolver);
    }

/*    else if (content_type.StartsWith("application/msword")) {
        m_message->addMessage("Parsing MS Word content", 2);
        if (!m_config->m_worddoc_converter.IsEmpty()) {
            // save locally
            wxFile fos;
            wxString temp_name = wxFileName::CreateTempFileName(m_config->m_plucker_home, &fos);
            fos.Write(url->m_content, url->m_content.Len());
            fos.Close();
            url->m_content.Clear();

            m_message->addMessage("Changing MSWord file", 2);

            // convert to html
            wxArrayString output;
            wxArrayString errors;

            long err = ::wxExecute(wxString::Format("\"%s\" \"%s\"", m_config->m_worddoc_converter, temp_name), output, errors);
            if (err == 0) {
                for (size_t i = 0; i < output.Count(); i++) {
                    url->m_content.Append(output[i]);
                }
                ::wxRemoveFile(temp_name);

                // now parse html
                PluckerHtmlParser htmlParser;
                htmlParser.parseContent(url->m_content, url, m_resolver);

                for (int j = 0; j < htmlParser.getNumPluckerDocuments(); j++) {
                    PluckerTextDocument* doc = htmlParser.getPluckerDocument(j);
                    addPluckerDocument(doc);
                }
            }
            else {
                m_message->addMessage("Could not run MS Word converter " + m_config->m_worddoc_converter, 1);
            }
        }
        else {
            m_message->addMessage("No MS Word converter found.", 1);
        }

        url->m_content.erase();

        return true;
*/
/*    else if (content_type.StartsWith("image/")) {
        m_message->addMessage("Parsing image content", 2);
        PluckerImageDocument* doc = new PluckerImageDocument(url->m_id, m_message);
        addPluckerDocument(doc);
        doc->parseImage(url, m_config, m_resolver);

        // see if a big image was created
        PluckerImageDocument* big_doc = doc->getBigImage();
        if (big_doc != 0) {
            addPluckerDocument(big_doc);
        }

        url->m_content.erase();

        return true;
    }
*/

    return 0;
}
