/*
 * $Id: PluckerImageDocument.cpp,v 1.11 2007/06/11 02:37:57 chrish Exp $
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

#include "GlobalFunctions.h"
#include "PluckerImageDocument.h"
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <Magick++.h>

PluckerImageDocument::PluckerImageDocument(int id, std::string urlString) : PluckerDocument()
{
    m_id = id;
    m_urlString = urlString;
    m_compressedSize = 0;
    m_compressedContent = 0;
    m_contentSize = 0;
    m_content = 0;
    m_bigContentSize = 0;
    m_bigContent = 0;
    m_compression = 0;
    m_width = 0;
    m_height = 0;
    m_bigWidth = 0;
    m_bigHeight = 0;
}

PluckerImageDocument::~PluckerImageDocument()
{
    delete [] m_content;
    delete [] m_compressedContent;
}

void PluckerImageDocument::ZlibCompressFunction()
{
	m_compressedSize = m_contentSize * 2;
    delete [] m_compressedContent;
	m_compressedContent = new Byte[m_compressedSize];

    ::compress(m_compressedContent, &m_compressedSize, m_content, m_contentSize);
}

void PluckerImageDocument::compress(int compression)
{
    m_compression = compression;

    if(m_compression == 2)
        ZlibCompressFunction();
}

int PluckerImageDocument::getDocumentSize()
{
    int size = 0;

    if (m_contentSize <= m_compressedSize || m_compressedSize == 0) 
        size = m_contentSize;
    else
        size = m_compressedSize;

    return 8 + size;
}

bool PluckerImageDocument::getBigImage(unsigned char** data, long* size)
{
    if(m_bigContentSize > 0)
    {
        *data = new Byte[m_bigContentSize];
        memcpy(*data, m_bigContent, m_bigContentSize);
        *size = m_bigContentSize;
        return true;
    }

    return false;
}

void PluckerImageDocument::setData(unsigned char* data, long size)
{
    delete [] m_content;
    m_contentSize = size;
    m_content = new Byte[size];
    memcpy(m_content, data, size);
}

void PluckerImageDocument::processImage(Config* config, const unsigned char* data, long size)
{
    delete [] m_content;
    m_content = new Byte[size];
    memcpy(m_content, data, size);
    m_contentSize = size;

    if(config->m_altMaxWidth >= 0 && config->m_altMaxHeight >= 0)
    {
        convertImage(config->m_bpp, config->m_altMaxWidth, config->m_altMaxHeight);
        m_bigContentSize = m_contentSize;
        m_bigContent = new Byte[m_bigContentSize];
        memcpy(m_bigContent, m_content, m_bigContentSize);
        m_bigWidth = m_width;
        m_bigHeight = m_height;

        delete [] m_content;
        m_content = new Byte[size];
        memcpy(m_content, data, size);
        m_contentSize = size;
  }

    convertImage(config->m_bpp, config->m_maxWidth, config->m_maxHeight);

	if(m_bigContentSize <= m_contentSize)
	{
        delete [] m_bigContent;
        m_bigContent = NULL;
        m_bigContentSize = 0;
        m_bigWidth = 0;
        m_bigHeight = 0;
	}
}

void PluckerImageDocument::writeContents(Writer* writer)
{
    // write id
    writer->writeInt(m_id);

    // write number of paragraphs
    writer->writeInt(0);

    // write total length
    writer->writeInt(m_contentSize);

    // write type
    if (m_contentSize <= m_compressedSize || m_compressedSize == 0)
    {
        writer->writeByte(0x02);
        writer->writeByte(0);
        for (int i = 0; i < m_contentSize; i++)
            writer->writeByte(m_content[i]);
    }
    else
    {
        writer->writeByte(0x03);
        writer->writeByte(0);
        for (int i = 0; i < m_compressedSize; i++)
            writer->writeByte(m_compressedContent[i]);
    }
}

long PluckerImageDocument::convertImage(int depth, int width, int height)
{
    Magick::Geometry geometry(width, height);
    Magick::Blob blob(m_content, m_contentSize);
    Magick::Image image(blob);

    delete [] m_content;

    if (width == 0)
        width = image.columns();
    if (height == 0)
        height = image.rows();
    geometry.greater(true);
    image.sample(geometry);

    if(depth < 8)
        image.quantizeColorSpace(Magick::GRAYColorspace);
    image.quantizeColors(1 << depth);
    image.quantize(); 

    image.magick("PALM");
    image.write(&blob);

    m_contentSize = blob.length();
    m_content = new Byte[m_contentSize];
    memcpy(m_content, blob.data(), m_contentSize);
    m_width = image.columns();
    m_height = image.rows();
}

void PluckerImageDocument::getBigImageSize(int* width, int* height)
{
    *width = m_bigWidth;
    *height = m_bigHeight;
}

long PluckerImageDocument::cropImage(int x, int y, int width, int height)
{
    Magick::Geometry geometry(width, height, x, y);
    Magick::Geometry oldGeometry(m_width, m_height, 0, 0);
    Magick::Blob blob(m_content, m_contentSize);
    Magick::Image image(blob, oldGeometry, "PALM");

    delete [] m_content;

    image.crop(geometry);
    image.write(&blob);

    m_contentSize = blob.length();
    m_content = new Byte[m_contentSize];
    memcpy(m_content, blob.data(), m_contentSize);
    m_width = image.columns();
    m_height = image.rows();
}
