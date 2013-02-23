/*
 * $Id: Writer.h,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#if !defined(WRITER_H)
#define WRITER_H

#include "Config.h"
#include "Message.h"
#include "Resolver.h"

// base class for writing documents
class Writer  
{
public:
	Writer(Config* config, Message* message, Resolver* resolver);
	virtual ~Writer();

    virtual void writeByte(unsigned char value) = 0;
    virtual void writeInt(unsigned int value) = 0;
    virtual bool endWrite() = 0;
    virtual bool startWrite(int numberDocuments) = 0;
    virtual void writeOffset(int documentSize) = 0;
    virtual void endOffset() = 0;

protected:
    Config* m_config;
    Message* m_message;
    Resolver* m_resolver;
};

#endif // WRITER_H
