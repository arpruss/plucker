/*
 * $Id: PluckerImageDocument.h,v 1.8 2007/06/11 02:37:57 chrish Exp $
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

#if !defined(PLUCKERIMAGEDOCUMENT_H)
#define PLUCKERIMAGEDOCUMENT_H

#include <zlib.h>

#include "PluckerDocument.h"

class PluckerImageDocument : public PluckerDocument  
{
public:
	PluckerImageDocument(int id, std::string urlString);
	virtual ~PluckerImageDocument();

    void compress(int compression);
    int getDocumentSize();
    void writeContents(Writer* writer);
    void finishDocument(Resolver* resolver, Config* config) {}
    bool getBigImage(unsigned char** data, long* size);
    void processImage(Config* config, const unsigned char* data, long size);
    void setData(unsigned char* data, long size);
    void getBigImageSize(int* width, int* height);
    long cropImage(int x, int y, int width, int height);

private:
    void ZlibCompressFunction();
    long convertImage(int depth, int width, int height);

    Byte* m_content;
    unsigned long m_contentSize;
    Byte* m_compressedContent;
    unsigned long m_compressedSize;
    Byte* m_bigContent;
    unsigned long m_bigContentSize;

    unsigned long m_width;
    unsigned long m_height;
    unsigned long m_bigWidth;
    unsigned long m_bigHeight;
};

#endif // !defined(PLUCKERIMAGEDOCUMENT_H)
