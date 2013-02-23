/*
 * $Id: Message.h,v 1.2 2005/04/04 20:24:33 chrish Exp $
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

#if !defined(MESSAGE_H)
#define MESSAGE_H

#include <string>

class Message
{
public:
	Message(int verbosity, int statusLineLength);
	virtual ~Message();

    void addMessage(std::string info, int verbosity);
    void setVerbosity(int verbosity) {m_verbosity = verbosity;}
    void setStatusLineLength(int statusLineLength) {m_statusLineLength = statusLineLength;}
    void updateProgress(long numDone, long numTodo);

private:
    int m_verbosity;
    int m_statusLineLength;
};

#endif // !defined(MESSAGE_H)
