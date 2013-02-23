// condinst.cpp : Defines the entry point for the application.
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <string>
#include <sstream>
#include <condmgr.h>
#include <userdata.h>
#include <hsapi.h>
#include <InstAide.H>

void ShowUsage()
{
}

std::string FindCLParameter(std::string commandLine, std::string command, std::string def)
{
    int startCmd = commandLine.find(command);
    if (startCmd == std::string::npos)
        return def;

    int endCmd = startCmd + command.size();

    int startParam = endCmd + 1;
    if (startParam > commandLine.size() || commandLine[startParam] == '-')
        return command;

    int nextCmd;
    if (commandLine[startParam] == '"') {
        ++startParam;
        nextCmd = commandLine.find('"', startParam);
        if (nextCmd == std::string::npos)
            nextCmd = commandLine.size();
    }
    else {
        nextCmd = commandLine.find('-', startParam);
        if (nextCmd == std::string::npos)
            nextCmd = commandLine.size();
        else
            --nextCmd;
    }

    if (nextCmd > startParam)
        return commandLine.substr(startParam, nextCmd - startParam);
    else
        return def;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    int retval = 0;
    bool restart = false;
    bool hotsync_stop = false;
    if (FindCLParameter(lpCmdLine, "-r", "") == "-r") {
        restart = true;
    }
    bool silent = false;
    if (FindCLParameter(lpCmdLine, "-s", "") == "-s") {
        silent = true;
    }

    if (FindCLParameter(lpCmdLine, "-u", "") == "-u") {
        std::string creator = FindCLParameter(lpCmdLine, "-e", "");
        if (creator.size() > 0) {
		    HsSetAppStatus(HsCloseApp, HSFLAG_NONE);
            hotsync_stop = true;
            if (CmRemoveConduitByCreatorID(creator.c_str()) != 1) {
                MessageBox(NULL, "Error removing conduit.", "Conduit Installer", 0);
                retval = 1;
            }
            else {
                if (!silent) {
                    MessageBox(NULL, "Conduit uninstalled.", "Conduit Installer", 0);
                }
                retval = 0;
            }
        }
        else {
            MessageBox(NULL, "Uninstall failed, no creator specified.", "Conduit Installer", 0);
            retval = 1;
        }
    }
    else if (FindCLParameter(lpCmdLine, "-i", "-i") == "-i") {
        std::string creator = FindCLParameter(lpCmdLine, "-e", "");
        std::string classPath = FindCLParameter(lpCmdLine, "-p", "");
        std::string className = FindCLParameter(lpCmdLine, "-l", "");
        std::string conduit = FindCLParameter(lpCmdLine, "-c", "");
        std::string user = FindCLParameter(lpCmdLine, "-u", "");
        std::string info = FindCLParameter(lpCmdLine, "-i", "");
        std::string condName = FindCLParameter(lpCmdLine, "-n", "Conduit");
        std::string file = FindCLParameter(lpCmdLine, "-f", "file");
        std::string title = FindCLParameter(lpCmdLine, "-t", "Conduit");
        std::string directory = FindCLParameter(lpCmdLine, "-d", "dir");
        std::string remoteDB = FindCLParameter(lpCmdLine, "-b", "remoteDB");

        int priority = 2;

        if ((className.size() > 0 || classPath.size() > 0) && conduit.size() == 0)
            conduit = "jsync13.dll";

        if (creator.size() > 0 && conduit.size() > 0) {    
			HsSetAppStatus(HsCloseApp, HSFLAG_NONE);
            hotsync_stop = true;
		    int err = CmInstallCreator(creator.c_str(), CONDUIT_APPLICATION);
		    if (err != 0) {
			    if (err == ERR_CREATORID_ALREADY_IN_USE) {
				    err = 0;
			    }
			    else
			    {
                    MessageBox(NULL, "Error installing conduit.", "Conduit Installer", 0);
				    retval = 1;
			    }
		    }

		    if (err == 0) {
			    err = CmSetCreatorName(creator.c_str(), conduit.c_str());
                if (directory.size() > 0)
			        err = CmSetCreatorDirectory(creator.c_str(), directory.c_str());
                if (file.size() > 0)
			        err = CmSetCreatorFile(creator.c_str(), file.c_str());
                if (remoteDB.size() > 0)
			        err = CmSetCreatorRemote(creator.c_str(), remoteDB.c_str());
                if (title.size() > 0)
			        err = CmSetCreatorTitle(creator.c_str(), title.c_str());
                if (user.size() > 0)
                    err = CmSetCreatorUser(creator.c_str(), user.c_str());
                if (priority >= 0 && priority <= 4)
                    err = CmSetCreatorPriority(creator.c_str(), priority);
                if (info.size() > 0)
			        err = CmSetCreatorInfo(creator.c_str(), info.c_str());

                if (className.size() > 0)
                    err = CmSetCreatorValueString(creator.c_str(), "ClassName", (char*) className.c_str());
                if (classPath.size() > 0)
                    err = CmSetCreatorValueString(creator.c_str(), "ClassPath13", (char*) classPath.c_str());

			    int numUsers = UmGetUserCount();
			    unsigned long creatorID;
			    CmConvertStringToCreatorID(creator.c_str(), &creatorID);
			    for (int i = 0; i < numUsers; i++) {
				    unsigned long userID;
				    UmGetUserID(i, &userID);

				    UmSetUserPermSyncPreferences(userID, creatorID, PCToHH);
				    UmSetUserTempSyncPreferences(userID, creatorID, PCToHH);
			    }
		    }
            if (!silent) {
                MessageBox(NULL, "Conduit installed.", "Conduit Installer", 0);
            }
        }
        else {
            MessageBox(NULL, "Error installing conduit. No creator or conduit supplied.", "Conduit Installer", 0);
            retval = 1;
        }
    }

    if (hotsync_stop && restart) {
        HsSetAppStatus(HsStartApp, HSFLAG_NONE);
    }

	return retval;
}
