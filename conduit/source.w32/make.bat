@ECHO OFF
SET COMPILE_WITH=VP

IF %COMPILE_WITH% == VP GOTO VP
IF %COMPILE_WITH% == DELPHI GOTO DELPHI
GOTO END



:VP
Set RC="C:\Programme\Delphi 3\Delphi 3\bin\brcc32.exe"
Set VP=C:\Vp21\
Set PATCH=Patch.exe
Copy .\PalmUserData\PalmUserData.pas .\PalmUserData.pas > nul
%PATCH% -i PalmUserData.pas.Patch
ECHO.
IF NOT EXIST "Resource.RES" %RC% Resource.rc
ECHO.
%VP%Bin.w32\Vpc.exe -CW -B -E.. -$PMTYPE:VIO Conduit.dpr -U%VP%Units.w32 -L%VP%Units.w32;%VP%Lib.w32 -R%VP%Res.w32
ECHO.
Del PalmUserData.pas > nul
Del Conduit.lnk > nul
Del Conduit.obj > nul
Del ConstStrings.lib > nul
Del ConstStrings.vpi > nul
Del DBTools.lib > nul
Del DBTools.vpi > nul
Del PalmConst.vpi > nul
Del PalmUserData.lib > nul
Del PalmUserData.vpi > nul
Del Tools.lib > nul
Del Tools.vpi > nul
ECHO.
GOTO END



:DELPHI
Set delphi="C:\Programme\Delphi 3\Delphi 3\bin\"
Set PATCH=Patch.exe
Copy .\PalmUserData\PalmUserData.pas .\PalmUserData.pas > nul
%PATCH% -i PalmUserData.pas.fix.Patch
ECHO.
IF NOT EXIST "Resource.RES" %delphi%brcc32.exe Resource.rc
ECHO.
%delphi%dcc32.exe Conduit.dpr -N..\ -E..\
ECHO.
Del PalmUserData.pas > nul
ECHO.
GOTO END



:END
