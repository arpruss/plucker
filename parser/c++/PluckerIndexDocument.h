/*
 * $Id: PluckerIndexDocument.h,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#if !defined(PLUCKERINDEXDOCUMENT_H)
#define PLUCKERINDEXDOCUMENT_H

#include "PluckerDocument.h"

// index document record
class PluckerIndexDocument : public PluckerDocument  
{
public:
	PluckerIndexDocument(bool hasLinkIndex, bool hasCategoryInfo);
	virtual ~PluckerIndexDocument();

    void compress(int compression);
    int getDocumentSize();
    void writeContents(Writer* writer);
    void finishDocument(Resolver* resolver, Config* config) {}

private:
    int m_numReserved;
    bool m_hasLinkIndex;
    bool m_hasCategoryInfo;
};

#endif // !defined(PLUCKERINDEXDOCUMENT_H)
