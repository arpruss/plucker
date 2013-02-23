/*
 * $Id: PluckerMultiImageDocument.h,v 1.1 2005/04/09 20:16:03 chrish Exp $
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

#if !defined(PLUCKERMULTIIMAGEDOCUMENT_H)
#define PLUCKERMULTIIMAGEDOCUMENT_H

#include <vector>

#include "PluckerDocument.h"

class PluckerMultiImageDocument : public PluckerDocument  
{
public:
	PluckerMultiImageDocument(int id, std::string urlString);
	virtual ~PluckerMultiImageDocument();

    int getDocumentSize();
    void writeContents(Writer* writer);
    void finishDocument(Resolver* resolver, Config* config) {}
    void setSize(int cols, int rows);
    void addDocId(int id);
    void compress(int compression) {}

private:
    std::vector<int> m_docIds;
    int m_cols;
    int m_rows;
};

#endif // !defined(PLUCKERMULTIIMAGEDOCUMENT_H)
