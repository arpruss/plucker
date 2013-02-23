/*
 * $Id: ImageParser.cpp,v 1.5 2005/04/27 22:19:09 chrish Exp $
 *
 * Cpluck - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 2005 Christopher R. Hawks (chawks@dls.net) 
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

#include "ImageParser.h"

#define simpleImageMaxSize (60 * 1024)

ImageParser::ImageParser(Url* url, Resolver* resolver)
{
    m_resolver = resolver;
    m_url = url;

    int newId = url->m_id;
    m_currentDocument = new PluckerImageDocument(newId, url->getUrlString());
    m_currentDocumentSize = 0;
    m_documents.push_back(m_currentDocument);
}

ImageParser::~ImageParser()
{
}

void ImageParser::processData(Config* config, const unsigned char* data, unsigned long size)
{
    unsigned char* bigData;
    long bigSize;

    m_currentDocument->processImage(config, data, size);

    if(m_currentDocument->getBigImage(&bigData, &bigSize))
    {
        PluckerImageDocument* newDoc;
        int newId;

        if(bigSize <= simpleImageMaxSize)  // Make a big version
        {
            std::string bigName = m_url->getUrlString() + "#bigimage";
            m_resolver->getOrAdd(bigName, &newId);
            newDoc = new PluckerImageDocument(newId, bigName);
            newDoc->setData(bigData, bigSize);
            m_documents.push_back(newDoc);
        }
        else                               // Make a Multi-image
        {
            unsigned int wide, high, cols, rows;
            unsigned int count = 0, Y = 0, X = 0;
            PluckerMultiImageDocument* multiDoc;
            int height, width;

            std::string multiName = m_url->getUrlString() + "#multi";
            m_resolver->getOrAdd(multiName, &newId);
            multiDoc = new PluckerMultiImageDocument(newId, multiName);
            m_documents.push_back(multiDoc);
            m_currentDocument->getBigImageSize(&width, &height);

            switch(config->m_bpp)
            {
                case 1:
                    wide = 800;
                    high = 600;
                    break;
                case 2:
                    wide = 600;
                    high = 400;
                    break;
                case 4:
                    wide = 400;
                    high = 300;
                    break;
                case 8:
                    wide = 300;
                    high = 200;
                    break;
                default:
                    wide = 300;
                    high = 100;
                    break;
            }

            cols = width / wide;
            if(width % wide)
                cols += 1;

            rows = height / high;
            if(height % high)
                rows += 1;

            multiDoc->setSize(cols, rows);

            while(Y < height)
            {
                while(X < width)
                {
                    unsigned int W, H;

                    W = std::min(wide, width - X);
                    H = std::min(high, height - Y);

                    std::stringstream ss;
                    ss << m_url->getUrlString() + "#multi";
                    ss << X;
                    ss <<"x";
                    ss << Y;

                    m_resolver->getOrAdd(ss.str(), &newId);
                    newDoc = new PluckerImageDocument(newId, ss.str());
                    newDoc->setData(bigData, bigSize);
                    newDoc->cropImage(X, Y, W, H);
                    m_documents.push_back(newDoc);
                    multiDoc->addDocId(newId);

                    X = X + wide;
                }
                X = 0;
                Y = Y + high;
            }
        }
        delete [] bigData;
    }
}

