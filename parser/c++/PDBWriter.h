/*
 * $Id: PDBWriter.h,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#if !defined(PDBWRITER_H)
#define PDBWRITER_H

#include <fstream>

#include "Writer.h"

// writes pdb file to disk
class PDBWriter : public Writer  
{
public:
    PDBWriter(Config* config, Message* message, Resolver* resolver);
	virtual ~PDBWriter();

    void writeByte(unsigned char value);
    void writeInt(unsigned int value);
    bool endWrite();
    bool startWrite(int numberDocuments);
    void writeOffset(int documentSize);
    void endOffset();

protected:

private:
    void writeLong(unsigned long value);

    std::ofstream* m_file;
    unsigned long m_offset;
};

#endif // !defined(PDBWRITER_H)
