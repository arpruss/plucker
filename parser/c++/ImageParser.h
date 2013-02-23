/*
 * $Id: ImageParser.h,v 1.4 2005/04/09 20:16:03 chrish Exp $
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

#if !defined(IMAGEPARSER_H)
#define IMAGEPARSER_H

#include "Parser.h"
#include "PluckerImageDocument.h"
#include "PluckerMultiImageDocument.h"
#include "Resolver.h"
#include "Url.h"

class ImageParser : public Parser  
{
public:
	ImageParser(Url* url, Resolver* resolver);
	virtual ~ImageParser();

    void processData(Config* config, const unsigned char* data, unsigned long size);

private:
    PluckerImageDocument* m_currentDocument;
    long m_currentDocumentSize;
    Resolver* m_resolver;
    Url* m_url;
};

#endif // !defined(IMAGEPARSER_H)
