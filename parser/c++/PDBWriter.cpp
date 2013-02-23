/*
 * $Id: PDBWriter.cpp,v 1.4 2007/06/11 19:22:54 chrish Exp $
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

#include <sstream>
#include <time.h>

#include "PDBWriter.h"

PDBWriter::PDBWriter(Config* config, Message* message, Resolver* resolver) : Writer(config, message, resolver)
{
    m_file = 0;
}

PDBWriter::~PDBWriter()
{
}

bool PDBWriter::endWrite()
{
    if (m_file != 0) {
        m_file->close();
        delete m_file;
        m_file = 0;
    }

    return true;
}

bool PDBWriter::startWrite(int numberDocuments)
{
    unsigned long timeAddition = 2082844800ul;

    m_message->addMessage("Writing document " + m_config->m_docName, 1);
    m_message->addMessage("  to file " + m_config->m_docFile, 1);
    m_message->addMessage("Converting", 1);

    m_file = new std::ofstream(m_config->m_docFile.c_str(), std::ios::out | std::ios::binary);
    if (!m_file->is_open()) {
        delete m_file;
        m_message->addMessage("Could not write to " + m_config->m_docFile, 0);
        return false;
    }

    // write pdb header
    // document name
    for (int i = 0; i < m_config->m_docName.length() && i < 32; i++)
        m_file->put(m_config->m_docName[i]);
    for (int i = 0; i < 32 - m_config->m_docName.length(); i++)
        writeByte(0);

    int flags = 0;
    if (m_config->m_backupBit) 
        flags |= 0x0008;
    if (m_config->m_copyPreventionBit)
        flags |= 0x0040;
    if (m_config->m_launchableBit)
        flags |= 0x0200;

    // write flags
    writeInt(flags);
    // write version
    writeInt(1);

    // write creation, modified, backup times
    time_t now;
    time(&now);
    writeLong(now + timeAddition);
    writeLong(now + timeAddition);
    writeLong(0);
    
    // write mod, appinfo, sortinfo 
    writeLong(0);
    writeLong(0);
    writeLong(0);

    // write type
    m_file->write("Data", 4);

    // write creator
    m_file->write("Plkr", 4);

    // write idseed, nextreclistid
    writeLong(0);
    writeLong(0);

    // write number of records
    writeInt(numberDocuments);

    m_offset = 78 + numberDocuments * 8 + 2;

    return true;
}

void PDBWriter::writeOffset(int documentSize)
{
    // write offset
	writeLong(m_offset);

    // write attributes
    writeByte(0);

    // write record id, letting the Palm assign
    writeByte(0);
    writeByte(0);
    writeByte(0);

    // update the offset
    m_offset += documentSize;
}

void PDBWriter::endOffset()
{
    // write filler
    writeInt(0);
}

void PDBWriter::writeByte(unsigned char value)
{
    m_file->put(value);
}

void PDBWriter::writeInt(unsigned int value)
{
    unsigned char hi = (value & 0xff00) >> 8;
    unsigned char lo = value & 0x00ff;

    writeByte(hi);
    writeByte(lo);
}

// write hi int then lo int
void PDBWriter::writeLong(unsigned long value)
{
    unsigned int hi = (value & 0xffff0000) >> 16;
    unsigned int lo = value & 0x0000ffff;

    writeInt(hi);
    writeInt(lo);
}
