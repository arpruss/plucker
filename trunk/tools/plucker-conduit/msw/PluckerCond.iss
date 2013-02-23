;
; Setup script for the Plucker Conduit
; Bill Nalen
; January 31, 2003

[Setup]
AppName=Plucker Conduit for Windows
AppVerName=Plucker Conduit for Windows 0.1.2
AppPublisher=Bill Nalen
AppPublisherURL=http://bill.nalens.com
AppSupportURL=http://bill.nalens.com
AppUpdatesURL=http://bill.nalens.com
DefaultDirName={pf}\Plucker
DisableProgramGroupPage=yes
LicenseFile=c:\projects\pluckercond\Copying
InfoBeforeFile=c:\projects\pluckercond\readme.txt
;InfoAfterFile=c:\projects\pluckercond\uninstall.txt
AllowNoIcons=yes
OutputBaseFilename=Setup-PluckerConduit
OutputDir=c:\inetpub\wwwroot\HostRocketSite-bill\programs

[Types]
Name: "full"; Description: "Full installation"

[Components]
Name: "main"; Description: "Conduit"; Types: full; Flags: fixed

[UninstallRun]
Filename: "{app}\condinst.exe"; Parameters: "-uninstall -creator Plkr"

[Run]
Filename: "{app}\condinst.exe"; Parameters: "-install -creator Plkr -name Plucker Conduit -title Plucker Conduit -conduit {app}\PluckerCond.dll"; Flags: nowait

[Files]
Source: "c:\projects\condinst\Release\condinst.exe"; DestDir: "{app}"; Components: main; CopyMode: alwaysoverwrite
Source: "c:\projects\pluckercond\release\pluckercond.dll"; DestDir: "{app}"; Components: main; CopyMode: alwaysoverwrite
Source: "c:\projects\pluckercond\changelog.txt"; DestDir: "{app}"; Components: main; CopyMode: alwaysoverwrite
Source: "c:\projects\condinst\changelog.condinst.txt"; DestDir: "{app}"; Components: main; CopyMode: alwaysoverwrite
Source: "c:\projects\pluckercond\readme.txt"; DestDir: "{app}"; Components: main; CopyMode: alwaysoverwrite
Source: "c:\projects\condinst\readme.condinst.txt"; DestDir: "{app}"; Components: main; CopyMode: alwaysoverwrite

