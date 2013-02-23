/* ReXX */

/* Create Plucker Object on Desktop */

/*
 * $Id:
 */

Call RxFuncAdd "SysLoadFuncs","RexxUtil","SysLoadFuncs"
Call SysLoadFuncs

Parse ARG installdir Port lang

'@Echo OFF'
'cls'
if installdir = '' then call help
if Port = '' then Port = 'COM1'
if lang = '' then lang = 'en'
env      = 'OS2ENVIRONMENT'
/* homedir  = VALUE('HOME', , env) */
/* Python can't handle driveletters, so home has to  reside below the
 * actual Python-Conduit. We assure this by setting HOME explicitly
 * via hotsync.cmd */

homedir = installdir || '\parser\python';


say "Welcome to Plucker!"
say "~~~~~~~~~~~~~~~~~~~"
say ""
say "   ---o-o---  This installation-routine will";
say "   ---o--o--  install Plucker for OS/2 distribution,";
say "   ---o-o---  that is:";
say "   ---o-----  - the scripts necessary to run Plucker";
say "   ---o-----  - the viewer to your Palm";
say "              - the files needed in your home";
say "              It will not install the programs Plucker";
say "              needs as system requirements.";
say "              Especially it will not install Perl and Pilot-Link.";
say "";
say "   Currently running OS: OS/2";
say "   Communicating via   : "||Port;
say "   Using as HOME       : "||homedir;
say "   Language of viewer  : "||lang;
say ""
'pause'
say ""
say "Now copying files..."
say ""

'xcopy ..\*.* '||installdir||'\*.* /S > nul 2> nul'
'md '||homedir||'\.plucker >nul 2>nul'
'md '||homedir||'\.plucker\cache >nul 2>nul'
'md '||homedir||'\.plucker\html >nul 2>nul'
'xcopy ..\parser\defaults\*.* '||homedir||'\.plucker\*.* /S /E > nul 2> nul'

say ""
say "All files copied, now creating Desktop-objects for Plucker..."
say ""

rc=Directory('installdir')

/* Create Folder on desktop */

Call Recreate.cmd installdir

/* Install the viewer using Pilot-Link */

Say "I'll now install the viewer to your Palm-device"

if lang='en' then viewer=installdir||"\viewer\binary\English\viewer.prc"
if lang='de' then viewer=installdir||"\viewer\binary\Deutsch\viewer.prc"
'pilot-xfer -p '||Port||' -i '||viewer||' '

say ""
say "Viewer should be installed properly."
say "Ok, now displaying the README.OS2, containing further"
say "instructions. Have a lot of fun..."
say ""
say " Created by Alexander Wagner, Team OS/2 Franken"
say "    -=[ We know where we want to go today ]=-"
say ""
say " In case of emergency"
say " mailto: a.wagner@teamos2.de"
call readme

/* display help */
help:
say " Usage:"
say "~~~~~~~~"
say ""
say "   ---o-o---  This installation-routine will";
say "   ---o--o--  install Plucker for OS/2 distribution,";
say "   ---o-o---  that is:";
say "   ---o-----  - the scripts necessary to run Plucker";
say "   ---o-----  - the viewer to your Palm";
say "              - the files needed in your Home";
say "              It will not install the programs Plucker";
say "              needs as system requirements.";
say "              Especially it will not install Perl and Pilot-Link.";
say " ";
say " Install_OS2 <path> <port> <lang>"
say ""
say " <path> represents the path to which you want to install"
say "        the plucker package."
say "        e.g. E:\PalmPilot\Plucker"
say " <port> represents the COM-Port you Palm is connected to."
say "        If no port is specified COM1 is assumed by default."
say "        e.g. COM1"
say " <lang> specify the language of the viewer. Currently supported:"
say "        en : English"
say "        de : Deutsch"
say ""
say "Using this a correct call of this script might be"
say ""
say "Install_OS2 E:\PalmPilot\Plucker COM1"
say "________________________________________________"
say ""
say ""
say " Created by Alexander Wagner, Team OS/2 Franken"
say "    -=[ We know where we want to go today ]=-  "
say ""
say " In case of emergency"
say " mailto: a.wagner@teamos2.de"
say ""
say ""
say " To requirements of this package see README.OS2"
say " Opening README.OS2 and exiting..."

readme:
'start /F e '||installdir||'\README.OS2'

Exit
