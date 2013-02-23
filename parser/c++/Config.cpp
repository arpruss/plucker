/*
 * $Id: Config.cpp,v 1.14 2007/06/13 00:20:57 chrish Exp $
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

#if defined(WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define DWORD long int
#endif

#include "Config.h"
#include "GlobalFunctions.h"

static const OptionDescription options[] =
{
/* 0*/    {0, "s", "extra-section",    "Add <str> to the list of searched section in the config files", 0}, 
/* 1*/    {1, "h", "help",             "\tShow help message", 0},
/* 2*/    {0, "P", "pluckerhome",      "Use <str> as plucker home instead of the default", 0},
/* 3*/    {0, "V", "verbosity",        "Set verbosity level", 1},
/* 4*/    {0, "",  "user-agent",       "Send <str> as the indentification of browser when fetching remote pages", 0},
/* 5*/    {0, "f", "doc-file",         "Specify <str> as the name of the output file", 0},
/* 6*/    {0, "N", "doc-name",         "Specify <str> as the name of the document (NOT the filename)", 0},
/* 7*/    {0, "M", "maxdepth",         "Use <n> for max depth on the home document", 1},
/* 8*/    {1, "q", "quiet",            "\tBe quiet, i.e. set verbosity level 0", 0},
/* 9*/    {1, "v", "",                 "\tSet verbosity level 1", 0},
/*10*/    {0, "H", "home-url",         "Use <str> as the root document", 0},
/*11*/    {1, "",  "no-urlinfo",       "Do not include info about the URLs", 0},
/*12*/    {0, "",  "category",         "Put <str> in the database as the default category", 0},
/*13*/    {1, "",  "backup",           "\tSet the backup bit in the output file", 0},
/*14*/    {1, "",  "no-backup",        "Clear the backup bit in the output file", 0},
/*15*/    {1, "",  "beamable",         "Set the beamable bit in the output file", 0},
/*16*/    {1, "",  "not-beamable",     "Clear the beamable bit in the output file", 0},
/*17*/    {1, "",  "launchable",       "Set the launchable bit in the output file", 0},
/*18*/    {1, "",  "not-launchable",   "Clear the launchable bit in the output file", 0},
/*19*/    {1, "",  "version",          "\tPrint out the version of Plucker-build and then exit", 0},
/*20*/    {1, "",  "zlib-compression", "Use Zlib compression", 0},
/*21*/    {0, "",  "compression",      "Use <str> as compression format (allowable formats: zlib)", 0},
/*22*/    {0, "",  "bpp",              "\tBits per pixel for images, defaults to 1 (0, 1, 2, 4, 8, 16)", 1},
/*23*/    {1, "",  "noimages",         "Do not include images (same as --bpp=0)", 0},
/*24*/    {1, "",  "stayonhost",       "Do not follow external URLs", 0},
/*25*/    {0, "",  "title",            "\tUse <str> as the title of the documnet", 0},
/*26*/    {0, "",  "author",           "\tUse <str> as the author of the documnet", 0},
/*27*/    {0, "",  "maxheight",        "Set maximum height of images to <n> pixels.", 1},
/*28*/    {0, "",  "maxwidth",         "Set maximum width of images to <n> pixels.", 1},
/*29*/    {0, "",  "alt-maxheight",    "Set maximum height of big images to <n> pixels.", 1},
/*30*/    {0, "",  "alt-maxwidth",     "Set maximum width of big images to <n> pixels.", 1},
/*31*/    {0, "E",  "exclusion-list",  "Add <str> to exclusion lists.", 1},
/*32*/    {0, "",   "tables",          "\tGenerate real tables instead of vertical lists of cells.", 1},
/*33*/    {0, "p", "pluckerdir",       "Use <str> as plucker directory instead of the default", 0},
/*34*/    {1, "",  "stayondomain",     "Do not follow URLs off of this domain", 0},
/*35*/    {1, "W",  "wait-time",       "Wait this many milli-seconds between http requests.", 0},
          {-1, "", "", "", 0}
};

#if !defined(WIN32) // We'll supply one
int GetPrivateProfileString(const char* section, const char* entry, char* ignore, char* buffer, int bufSize, const char* filename)
{
    int file;
    int last = 0;
    int next = 0;
    char* text;
    char* start;
    char* end;
    char* ptr;


    if(-1 == (file = open(filename, O_RDONLY)))
        return 0;

    last = lseek(file, 0, SEEK_END);
    if(last == 0)
    {
        close(file);
        return 0;
    }
    lseek(file, 0, SEEK_SET);

    text = (char*)malloc(last);
    last = read(file, text, last);
    start = text;
    end = text + last;
    close(file);

    while(start < end)
    {
        while(*start != '[')
        {
            start = strchr(start, '\n');
            if(!start) return 0;
            while(*start == '\n' || *start == '\r')
                start++;
            if(start >= end) return 0;
        }

        if(start > end) return 0;

        start++;
        if(!strncasecmp(start, section, strlen(section))) // Found Section
        {
            while(strncasecmp(start, entry, strlen(entry)))
            {
                if(*start == '[') return 0; // Not in Section
                start = strchr(start, '\n');
                if(!start) return 0;
                while(*start == '\n' || *start == '\r')
                    start++;
                if(start >= end) return 0;
            }
            start = strchr(start, '=');
            if(!start) return 0;
            start++;  // Found it
            if(start >= end) return 0;
            while(isblank(*start)) start++;
            if(start >= end) return 0;
            ptr = start;
            while(!isspace(*ptr)) ptr++;
            memcpy(buffer, start, ptr - start);
            buffer[ptr - start] = '\0';
            return(ptr - start);
        }
    }
	
}
#endif


Config::Config(Message* message)
{
    // defaults if no option or parameter given
    m_verbosity = 1;
    m_maxDepth = 2;
    m_userAgent = "Plucker";
    m_homeUrl = "/home.html";
    m_noUrlInfo = 0;
    m_backupBit = false;
    m_copyPreventionBit = false;
    m_launchableBit = false;
    m_compressionType = 0;
    m_closeOnExit = false;
    m_maxThreads = 2;
    m_threadSleep = 100;
    m_bpp = 1;
    m_tryReduceBpp = 1;
    m_homeStayOnHost = false;
    m_homeStayOnDomain = false;
    m_maxWidth = 0;
    m_maxHeight = 0;
    m_altMaxWidth = 0;
    m_altMaxHeight = 0;
    m_statusLineLength = 60;
    m_doTables = false;

    m_message = message;

    m_exclusion = new ExclusionList();

    m_waitTime = 0;
}

Config::~Config()
{
}

void Config::showVersion()
{
    std::stringstream ss;
    ss << "Plucker-build C++ version " << BUILD_MAJOR << "." << BUILD_MINOR << " by Bill Nalen (bill@nalens.com) (c) 2003";
    m_message->addMessage(ss.str(), 0);
}

void Config::showOptions()
{
    showVersion();
    m_message->addMessage("A Plucker retriever and parser", 0);
    m_message->addMessage("", 0);

    int i = 0;
    OptionDescription optDescrip = options[i];
    while (optDescrip.type != -1) {
        std::stringstream ss;
        ss << optDescrip.singleName << "\t" << optDescrip.longName << "\t" << optDescrip.description;
        m_message->addMessage(ss.str(), 0);
        ++i;
        optDescrip = options[i];
    }
}

bool Config::hasOption(const OptionDescription optDescrip)
{
    if (strlen(optDescrip.singleName) > 0 && m_commandLineOptions.find(optDescrip.singleName) != m_commandLineOptions.end()) {
        return true;
    }
    if (strlen(optDescrip.longName) > 0 && m_commandLineOptions.find(optDescrip.longName) != m_commandLineOptions.end()) {
        return true;
    }

    return false;
}

bool Config::hasOption(const OptionDescription optDescrip, std::string* value)
{
    if (strlen(optDescrip.singleName) > 0 && m_commandLineOptions.find(optDescrip.singleName) != m_commandLineOptions.end()) {
        *value = m_commandLineOptions[optDescrip.singleName];
        return true;
    }
    if (strlen(optDescrip.longName) > 0 && m_commandLineOptions.find(optDescrip.longName) != m_commandLineOptions.end()) {
        *value = m_commandLineOptions[optDescrip.longName];
        return true;
    }

    return false;
}

bool Config::hasOption(const OptionDescription optDescrip, long* value)
{
    if (strlen(optDescrip.singleName) > 0 && m_commandLineOptions.find(optDescrip.singleName) != m_commandLineOptions.end()) {
        *value = atol(m_commandLineOptions[optDescrip.singleName].c_str());
        return true;
    }
    if (strlen(optDescrip.longName) > 0 && m_commandLineOptions.find(optDescrip.longName) != m_commandLineOptions.end()) {
        *value = atol(m_commandLineOptions[optDescrip.longName].c_str());
        return true;
    }

    return false;
}

bool Config::hasOption(const OptionDescription optDescrip, bool* value)
{
    if (strlen(optDescrip.singleName) > 0 && m_commandLineOptions.find(optDescrip.singleName) != m_commandLineOptions.end()) {
        if (m_commandLineOptions[optDescrip.singleName] == "1" || toLower(m_commandLineOptions[optDescrip.singleName]) == "true" || toLower(m_commandLineOptions[optDescrip.singleName]) == "yes") {
            *value = true;
        }
        else {
            *value = false;
        }
        return true;
    }
    if (strlen(optDescrip.longName) > 0 && m_commandLineOptions.find(optDescrip.longName) != m_commandLineOptions.end()) {
        if (m_commandLineOptions[optDescrip.longName] == "1" || m_commandLineOptions[optDescrip.longName] == "true" || m_commandLineOptions[optDescrip.longName] == "yes") {
            *value = true;
        }
        else {
            *value = false;
        }
        return true;
    }

    return false;
}

bool Config::loadOptions(int argc, char** argv)
{
    // load options from command line
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            char* arg = argv[i];
            char* value = "";

            if (strlen(arg) > 1 && arg[0] == '-') {
                // found option
                arg++;
                if (strlen(arg) > 1 && arg[0] == '-') {
                    arg++;
                    if(value = strchr(arg, '=')) {
                        *value = '\0';
                        value++;
                        if(!strcmp(arg, "E") || !strcmp(arg, "exclusion-list")) {
                            m_exclusionFiles += value;
                            m_exclusionFiles += ":";
                        }
                        // store option & option value for later inspection
                        m_commandLineOptions[arg] = value;

                        continue;
                    }
                }
                if (strlen(arg) > 0) {
                    // find value if there is one
                    char* value = "";
                    if ((i+1) < argc && strlen(argv[i+1]) > 0 && argv[i+1][0] != '-') {
                        i++;
                        value = argv[i];

                        if(!strcmp(arg, "E") || !strcmp(arg, "exclusion-list")) {
                            m_exclusionFiles += value;
                            m_exclusionFiles += ":";
                        }
                    }

                    // store option & option value for later inspection
                    m_commandLineOptions[arg] = value;
                }
            }
            else if (i == (argc - 1)) // last arg
                m_commandLineOptions["home-url"] = arg;
        }
    }

    // Check for Help
    if (hasOption(options[1])) {
        showOptions();
        return false;
    }

    // need to find location of plucker home to read options from file
    if (!hasOption(options[2], &m_pluckerHome)) {
        // need to read from registry
#if defined(WIN32)
        HKEY key;
        LONG err = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\The Plucker Team\\Plucker", 0, KEY_READ, &key);
        if (err != ERROR_SUCCESS) {
            m_message->addMessage("Failed to open registry key", 0);
        }
        else {
            char buffer[2048];
            DWORD size;
            DWORD type = REG_SZ;
            err = RegQueryValueEx(key, "Path", NULL, &type, (unsigned char*) buffer, &size);
            if (err == ERROR_SUCCESS) {
                m_pluckerHome = buffer;
            }
            RegCloseKey(key);
        }
#else
    char buff[256];

	if (getenv("PLUCKERHOME")) {
	    m_pluckerHome = getenv("PLUCKERHOME");
	}
	else {
        sprintf(buff, "%s/.plucker/", getenv("HOME"));
    	m_pluckerHome = buff;
	}
#endif
    }
    hasOption(options[0], &m_section);

    // get options from file
    readConfigFile();

    // now override with command line options
    hasOption(options[3], &m_verbosity);
    hasOption(options[4], &m_userAgent);
    hasOption(options[5], &m_docFile);
    hasOption(options[6], &m_docName);
    hasOption(options[7], &m_maxDepth);
    hasOption(options[10], &m_homeUrl);
    if (hasOption(options[8])) {
        m_verbosity = 0;
    }
    if (hasOption(options[9])) {
        m_verbosity = 1;
    }
    if (hasOption(options[11])) {
        m_noUrlInfo = 1;
    }
    hasOption(options[12], &m_category);
    if (hasOption(options[13])) {
        m_backupBit = true;
    }
    if (hasOption(options[14])) {
        m_backupBit = false;
    }
    if (hasOption(options[15])) {
        m_copyPreventionBit = false;
    }
    if (hasOption(options[16])) {
        m_copyPreventionBit = true;
    }
    if (hasOption(options[17])) {
        m_launchableBit = true;
    }
    if (hasOption(options[18])) {
        m_launchableBit = false;
    }
    if (hasOption(options[19])) {
        showVersion();
        return false;
    }
    if (hasOption(options[20])) {
        m_compressionType = 2;
    }
    std::string compression;
    hasOption(options[21], &compression);
    compression = toLower(compression);
    if (compression == "zlib")
        m_compressionType = 2;
    hasOption(options[22], &m_bpp);
    if (hasOption(options[23])) {
        m_bpp = 0;
    }
    hasOption(options[24], &m_homeStayOnHost);
    hasOption(options[25], &m_title);
    hasOption(options[26], &m_author);
    hasOption(options[27], &m_maxHeight);
    hasOption(options[28], &m_maxWidth);
    hasOption(options[29], &m_altMaxHeight);
    hasOption(options[30], &m_altMaxWidth);
    if (hasOption(options[32])) {
        m_doTables = true;
    }
    hasOption(options[33], &m_pluckerDir);
    if (!m_pluckerDir.length())
        m_pluckerDir = m_pluckerHome;
    hasOption(options[34], &m_homeStayOnDomain);
    hasOption(options[35], &m_waitTime);

    // default adjustments
#if defined(WIN32)
    if (m_pluckerHome[m_pluckerHome.length() - 1] != '\\')
        m_pluckerHome += '\\';
    if (m_pluckerDir[m_pluckerDir.length() - 1] != '\\')
        m_pluckerDir += '\\';
    if (m_docFile.length() > 2 && m_docFile[1] != ':')
        m_docFile = m_pluckerDir + m_docFile + ".pdb";
#else
    if (m_pluckerHome[m_pluckerHome.length() - 1] != '/')
        m_pluckerHome += '/';
    if (m_pluckerDir[m_pluckerDir.length() - 1] != '/')
        m_pluckerDir += '/';
    if (m_docFile.length() > 1 && m_docFile[0] != '/')
        m_docFile = m_pluckerDir + m_docFile + ".pdb";
#endif


    // add exclusionlist.txt to m_exclusionFiles
    std::string s = m_pluckerHome + "exclusionlist.txt";
    m_exclusionFiles += s + ":";
    if (m_pluckerDir != m_pluckerHome) {
        std::string s = m_pluckerDir + "exclusionlist.txt";
        m_exclusionFiles += s + ":";
    }

    // check for valid config
    bool goodConfig = true;
    if (m_docName.empty()) {
        m_message->addMessage("No document name supplied", 0);
        goodConfig = false;
    }
    if (m_docFile.empty()) {
        m_message->addMessage("No doc file supplied", 0);
        goodConfig = false;
    }
    if (m_homeUrl.empty()) {
        m_message->addMessage("No home url supplied", 0);
        goodConfig = false;
    }
    if (!(m_bpp == 0 || m_bpp == 1 || m_bpp == 2 || m_bpp == 4 || m_bpp == 8 || m_bpp == 16)) {
        m_message->addMessage("Unsupported bpp option", 0);
        goodConfig = false;
    }

    if (!goodConfig) {
        showOptions();
    }

    if (m_docName.length() > 26) {
        m_docName = m_docName.substr(0, 26);
    }

    int startPos = 0;
    int endPos = 0;
    while(-1 != (endPos = m_exclusionFiles.find(':', startPos)))
    {
        std::string name = m_exclusionFiles.substr(startPos, endPos);
        if(name[name.size() - 1] == ':')
             name.erase(name.size() - 1);
        m_exclusion->LoadFile(name.c_str());
        startPos = endPos + 1;
    }

    return goodConfig;
}

bool Config::readConfigFileEntry(const std::string& iniFile, const std::string& section, const std::string& entry, std::string* var)
{
    char buffer[2048];
    DWORD err = GetPrivateProfileString(section.c_str(), entry.c_str(), "", buffer, 2048, iniFile.c_str());
    if (err > 0) {
        *var = buffer;
        return true;
    }

    return false;
}

bool Config::readConfigFileEntry(const std::string& iniFile, const std::string& section, const std::string& entry, long* var)
{
    char buffer[2048];
    DWORD err = GetPrivateProfileString(section.c_str(), entry.c_str(), "", buffer, 2048, iniFile.c_str());
    if (err > 0) {
        *var = atol(buffer);
        return true;
    }

    return false;
}

bool Config::readConfigFileEntry(const std::string& iniFile, const std::string& section, const std::string& entry, bool* var)
{
    char buffer[2048];
    DWORD err = GetPrivateProfileString(section.c_str(), entry.c_str(), "", buffer, 2048, iniFile.c_str());
    if (err > 0) {
        if (strcmp(buffer, "1") == 0 || strcmp(buffer, "true") == 0 || strcmp(buffer, "yes") == 0) {
            *var = true;
        }
        else {
            *var = false;
        }
        return true;
    }

    return false;
}

void Config::readConfigFileSection(const std::string& iniFile, const std::string& section)
{
    bool abool;
    std::string name;

    readConfigFileEntry(iniFile, section, "verbosity", &m_verbosity);
    readConfigFileEntry(iniFile, section, "pluckerhome", &m_pluckerHome);
    readConfigFileEntry(iniFile, section, "pluckerdir", &m_pluckerDir);
    readConfigFileEntry(iniFile, section, "doc_name", &m_docName);
    readConfigFileEntry(iniFile, section, "doc_file", &m_docFile);
    readConfigFileEntry(iniFile, section, "home_maxdepth", &m_maxDepth);
    readConfigFileEntry(iniFile, section, "home_url", &m_homeUrl);
    readConfigFileEntry(iniFile, section, "category", &m_category);
    readConfigFileEntry(iniFile, section, "backup_bit", &m_backupBit);
    readConfigFileEntry(iniFile, section, "copyprevention_bit", &m_copyPreventionBit);
    readConfigFileEntry(iniFile, section, "launchable_bit", &m_launchableBit);
    readConfigFileEntry(iniFile, section, "worddoc_converter", &m_wordDocConverter);
    std::string compression;
    readConfigFileEntry(iniFile, section, "compression", &compression);
    if (compression == "zlib")
        m_compressionType = 2;
    if(readConfigFileEntry(iniFile, section, "zlib_compression", &abool))
        m_compressionType = 2;
    readConfigFileEntry(iniFile, section, "bpp", &m_bpp);
    readConfigFileEntry(iniFile, section, "home_stayonhost", &m_homeStayOnHost);
    readConfigFileEntry(iniFile, section, "close_on_exit", &m_closeOnExit);
    readConfigFileEntry(iniFile, section, "try_reduce_bpp", &m_tryReduceBpp);
    readConfigFileEntry(iniFile, section, "user_agent", &m_userAgent);
    readConfigFileEntry(iniFile, section, "no_urlinfo", &m_noUrlInfo);
    readConfigFileEntry(iniFile, section, "title", &m_title);
    readConfigFileEntry(iniFile, section, "author", &m_author);
    readConfigFileEntry(iniFile, section, "maxheight", &m_maxHeight);
    readConfigFileEntry(iniFile, section, "maxwidth", &m_maxWidth);
    readConfigFileEntry(iniFile, section, "alt_maxheight", &m_altMaxHeight);
    readConfigFileEntry(iniFile, section, "alt_maxwidth", &m_altMaxWidth);
    readConfigFileEntry(iniFile, section, "status_line_length", &m_statusLineLength);
    readConfigFileEntry(iniFile, section, "before_command", &m_beforeCommand);
    readConfigFileEntry(iniFile, section, "after_command", &m_afterCommand);
    if(readConfigFileEntry(iniFile, section, "exclusion_lists", &name))
        m_exclusionFiles += name + ":";
    readConfigFileEntry(iniFile, section, "tables", &m_doTables);
    readConfigFileEntry(iniFile, section, "home_stayondomain", &m_homeStayOnDomain);
    readConfigFileEntry(iniFile, section, "wait_time", &m_waitTime);
}

bool Config::readConfigFile()
{
    if (m_pluckerHome.empty()) {
        return false;
    }

    if (m_pluckerHome[m_pluckerHome.size() - 1] != '\\' && m_pluckerHome[m_pluckerHome.size() - 1] != '/') {
        m_pluckerHome += "/";
    }

    // read default values first
#if defined(WIN32)
    std::string iniLocation = m_pluckerHome + "plucker.ini";
#else
    char buff[256];

    sprintf(buff, "%s/.pluckerrc", getenv("HOME"));
    std::string iniLocation = buff;
#endif
    std::string defaultName = "DEFAULT";
    readConfigFileSection(iniLocation, defaultName);

    // read os values next
#if defined(WIN32)
    std::string osName = "WINDOWS";
#else
    std::string osName = "POSIX";
#endif
    readConfigFileSection(iniLocation, osName);

    // read section values if requested
    if (!m_section.empty()) {
        readConfigFileSection(iniLocation, m_section);
    }

    return true;
}

/*
    ;;pluckerdir=
    ;;cache_dir_name=cache
    ;;home_staybelow=
    ;;icon=false
    ;;big_icon=
    ;;small_icon=
    ;;http_proxy=
    ;;http_proxy_user=
    ;;http_proxy_pass=
    ;;exclusion_lists=
    ;;ready_sound=

    copy_to_dir=
    user=Bill Nalen
    depth_first=0
    referrer=
    home_stayondomain=1
    home_url_pattern=
    charset=
    indent_paragraphs=0
    anchor_color=#0000FF
    alt_text=1
    image_compression_limit=50
    owner_id_build=
    is_install_to_card=0
    directory_on_card=
    close_on_error=1
*/
/*
        message(0, "Usage: %s [OPTIONS] [HOMEURL]" % sys.argv[0])
        message(0, "  where HOMEURL is a 'file:' or 'http:' URL (which can alternatively")
        message(0, "  be specified with --home-url=<homeurl>) and OPTIONS are:")
        message(0, "    -c, --update-cache:")
        message(0, "                   Write a traditional cache directory in the <plucker dir>")
        message(0, "    -p<dir>, --pluckerdir=<dir>:")
        message(0, "                   Use <dir> as plucker dir instead of the default ")
        message(0, "                   Defaults to same as plucker home.")
        message(0, "    -E <filename>, --exclusion-list <filename>: ")
        message(0, "                   Add <filename> to list of files searched for exclusion lists")
        message(0, "    , --doc-compression:")
        message(0, "                   Specify which compression method to use. (For expert's use)")
        message(0, "    --compression=<compression-type>:")
        message(0, "                   Use <compression-type> as the compression format")
        message(0, "                   for the database.  Allowable options are 'doc', for")
        message(0, "                   Palm DOC compression, or 'zlib', for zlib compression.")
        message(0, "                   Zlib compression is typically better than DOC compression.")
        message(0, "    --depth-first:")
        message(0, "                   Do a depth-first pass through the web graph, rather than")
        message(0, "                   the default breadth-first traversal.")
        message(0, "    --stayondomain:")
        message(0, "                   Do not follow URLs off of this domain")
        message(0, "    --staybelow=<url-prefix>:")
        message(0, "                   Automatically exclude any URL that doesn't begin with <url-prefix>.")
        message(0, "    --charset=<name>:")
        message(0, "                   Set the default charset to that specified by <name>.")
        message(0, "    --owner-id=<name>:")
        message(0, "                   Set owner-id of the output document to <name>.")
        message(0, "    --url-pattern=<regexp-pattern>:")
        message(0, "                   Only fetch URLs if they match the regular expression specified")
        message(0, "                   by <regexp-pattern>.")
        message(0, "    --status-file=<filename>:")
        message(0, "                   Use <filename> as the status file.")
        message(0, "    --referrer=<referrer string>:")
        message(0, "                   Send the specified <referrer string> when fetching remote pages.")
        message(0, "")
        message(0, "Note that you must specify either -f or specify HOMEURL as an argument,")
        message(0, " or specify -c to update a cache.")
        message(0, "")

*/
