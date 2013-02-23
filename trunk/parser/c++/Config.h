/*
 * $Id: Config.h,v 1.7 2005/05/22 23:20:21 chrish Exp $
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

#if !defined(PLUCKER_CONFIG_H)
#define PLUCKER_CONFIG_H

#define BUILD_MAJOR 0
#define BUILD_MINOR 2

#include <map>
#include <vector>
#include <string>

#include "Message.h"
#include "ExclusionList.h"

struct OptionDescription
{
    int type;
    const char* singleName;
    const char* longName;
    const char* description;
    int optionType;
};

class Config  
{
public:
	Config(Message* message);
	virtual ~Config();

    bool loadOptions(int argc, char** argv);
    void showVersion();
    void showOptions();

public:
    // options
    std::string m_pluckerHome;
    std::string m_section;
    long m_verbosity;
    std::string m_homeUrl;
    std::string m_docName;
    std::string m_docFile;
    std::string m_userAgent;
    long m_maxDepth;
    long m_noUrlInfo;
    std::string m_category;
    bool m_backupBit;
    bool m_copyPreventionBit;
    bool m_launchableBit;
    long m_compressionType;        // 0 = none, 1 = doc, 2 = zlib
    std::string m_wordDocConverter;
    bool m_closeOnExit;
    long m_maxThreads;
    long m_threadSleep;
    long m_bpp;
    long m_tryReduceBpp;
    bool m_homeStayOnHost;
    bool m_homeStayOnDomain;
    std::string m_title;
    std::string m_author;
    long m_maxHeight;
    long m_maxWidth;
    long m_altMaxHeight;
    long m_altMaxWidth;
    long m_statusLineLength;

    std::string m_beforeCommand;
    std::string m_afterCommand;

    ExclusionList* m_exclusion;
    bool m_doTables;
    std::string m_pluckerDir;

    long m_waitTime;
    
private:
    bool hasOption(const OptionDescription optDescrip);
    bool hasOption(const OptionDescription optDescrip, std::string* value);
    bool hasOption(const OptionDescription optDescrip, long* value);
    bool hasOption(const OptionDescription optDescrip, bool* value);
    bool readConfigFile();
    bool readConfigFileEntry(const std::string& iniFile, const std::string& section, const std::string& entry, std::string* var);
    bool readConfigFileEntry(const std::string& iniFile, const std::string& section, const std::string& entry, long* var);
    bool readConfigFileEntry(const std::string& iniFile, const std::string& section, const std::string& entry, bool* var);
    void readConfigFileSection(const std::string& iniFile, const std::string& section);

    Message* m_message;
    std::map<std::string, std::string> m_commandLineOptions;
    std::string m_exclusionFiles;
};

#endif // !defined(PLUCKER_CONFIG_H)
