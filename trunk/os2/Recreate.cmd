/* ReXX */

/* 
 * $Id:
 */

/* Create Plucker Object on Desktop  */

Call RxFuncAdd "SysLoadFuncs","RexxUtil","SysLoadFuncs"
Call SysLoadFuncs

Parse ARG installdir Port

'@Echo OFF'

if installdir = "" then installdir = Directory()

env      = 'OS2ENVIRONMENT'
/* homedir  = VALUE('HOME', , env) */
/* Python can't handle driveletters, so home has to  reside below the
 * actual Python-Conduit. We assure this by setting HOME explicitly
 * via hotsync.cmd */
  
homedir = installdir || '\parser\python';

/* Create Folder on desktop */
rc=SysCreateObject("WPFolder","Plucker^for OS/2","<WP_DESKTOP>","OBJECTID=<WP_Plucker>")
if rc then do
     say "Plucker-folder created sucessfully"
   end
else say "Plucker-folder could not be created: "||rc

rc=SysSetObjectData("<WP_Plucker>","ICONFILE="||installdir||"\os2\DOKUMENT.ICO;ICONNFILE=1,"||installdir||"\os2\DOKUMEN1.ICO")

/* Place the necessary objects in the folder */
rc=Directory(installdir)
CurrentDir=Directory()
FileName=CurrentDir||"\parser\python\hotsync.cmd"
ExeName="EXENAME="||FileName||";ICONFILE="||CurrentDir||"\os2\plucker.ico;STARTUPDIR="CurrentDir||"\parser\python"
If SysCreateObject("WPProgram","Plucker Hotsync","<WP_Plucker>",ExeName) then do
      Say "Desktop Object for Plucker was created."
      End


FileName=CurrentDir||"\parser\os2\irhotsync.cmd"
ExeName="EXENAME=pilot-xfer.exe;Parameters=-p [IR-Port?] -i "||homedir||"\.plucker\Plucker.pdb;ICONFILE="||CurrentDir||"\os2\radar.ico;STARTUPDIR="CurrentDir||"\parser\os2"
If SysCreateObject("WPProgram","Plucker IR-Hotsync","<WP_Plucker>",ExeName) then do
      Say "Desktop Object for IR-Plucker was created."
      End


ExeName="EXENAME=E.EXE;PARAMETERS="||homedir||"\.plucker\home.html;ICONFILE="||installdir||"\os2\news.ico"
If SysCreateObject("WPProgram","Plucker Channels", "<WP_Plucker>", Exename) then say "Homepage-Object created"

ExeName="EXENAME=netscape.exe;PARAMETERS=http://www.rubberchicken.org/products/plucker;ICONFILE="||installdir||"\os2\netscape.ico"
If SysCreateObject("WPProgram","Home of Plucker", "<WP_Plucker>", ExeName) then say "Link created"

Exename="EXENAME=pilot-xfer.exe;PARAMETERS=-i "||homedir||"\.plucker\PluckerDB.pdb;ICONFILE="||installdir||"\os2\Tricode.ico"
If SysCreateObject("WPProgram","Transfer data only","<WP_Plucker>",ExeName) then say "Sync-Object created"

Exename="EXENAME=e.exe;PARAMETERS="||installdir||"\README.OS2;ICONFILE="||installdir||"\os2\CWPROJEC.ICO"
If SysCreateObject("WPProgram","Readme^for OS/2","<WP_Plucker>",ExeName) then say "Readme added"

Exit
