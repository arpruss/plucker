/*
 * $Id: pluckerbuild.cpp,v 1.4 2007/10/08 15:25:27 chrish Exp $
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

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <time.h>

#include "Config.h"
#include "Message.h"
#include "Spider.h"
#include "PDBWriter.h"
#include "Resolver.h"

int main(int argc, char* argv[])
{
    clock_t start = clock();

    Message* message = new Message(2, 1000);

    Config* config = new Config(message);
    if (config->loadOptions(argc, argv)) { 
        if (!config->m_beforeCommand.empty()) {
#if defined(WIN32)
//            ::wxExecute(m_config->m_before_command, wxEXEC_SYNC, NULL);
#else
            system(config->m_beforeCommand.c_str());
#endif
        }

        message->setVerbosity(config->m_verbosity);
        message->setStatusLineLength(config->m_statusLineLength);

        Resolver* resolver = new Resolver();
        Writer* writer = new PDBWriter(config, message, resolver);
        Spider* spider = new Spider(message, config, resolver);

        spider->startup();
        spider->runAll();
        if (spider->hasStartUrl()) {
            spider->finish(writer);

            if (!config->m_afterCommand.empty()) {
#if defined(WIN32)
//                ::wxExecute(m_config->m_after_command, wxEXEC_SYNC, NULL);
#else
                system(config->m_afterCommand.c_str());
#endif
            }
        }
        else {
            message->addMessage("No home document retrieved!", 0);
        }

        delete resolver;
        delete writer;
        delete spider;
    }

    clock_t finish = clock();

    double duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::stringstream ss;
    ss << duration << " seconds elapsed.";
    message->addMessage(ss.str(), 1);

    delete config;
    delete message;

	return 0;
}

