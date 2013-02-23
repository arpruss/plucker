# Microsoft Developer Studio Project File - Name="xrcdemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xrcdemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plucker-desktop.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plucker-desktop.mak" CFG="xrcdemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xrcdemo - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 DLL Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 DLL Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 DLL Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 DLL Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xrcdemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xrcdemo - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswunivu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivudll\xrcdemo.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswunivu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivudll\xrcdemo.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28u_xrc.lib wxmswuniv28u_html.lib wxmswuniv28u_adv.lib wxmswuniv28u_core.lib wxbase28u_xml.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivudll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv28u_xrc.lib wxmswuniv28u_html.lib wxmswuniv28u_adv.lib wxmswuniv28u_core.lib wxbase28u_xml.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivudll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswunivud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivuddll\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswunivud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivuddll\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28ud_xrc.lib wxmswuniv28ud_html.lib wxmswuniv28ud_adv.lib wxmswuniv28ud_core.lib wxbase28ud_xml.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivuddll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv28ud_xrc.lib wxmswuniv28ud_html.lib wxmswuniv28ud_adv.lib wxmswuniv28ud_core.lib wxbase28ud_xml.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivuddll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswuniv" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivdll\xrcdemo.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswuniv" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivdll\xrcdemo.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswuniv" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswuniv" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28_xrc.lib wxmswuniv28_html.lib wxmswuniv28_adv.lib wxmswuniv28_core.lib wxbase28_xml.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivdll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv28_xrc.lib wxmswuniv28_html.lib wxmswuniv28_adv.lib wxmswuniv28_core.lib wxbase28_xml.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivdll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswunivd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivddll\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswunivd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivddll\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswunivd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28d_xrc.lib wxmswuniv28d_html.lib wxmswuniv28d_adv.lib wxmswuniv28d_core.lib wxbase28d_xml.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivddll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv28d_xrc.lib wxmswuniv28d_html.lib wxmswuniv28d_adv.lib wxmswuniv28d_core.lib wxbase28d_xml.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivddll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswudll\xrcdemo.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\mswu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswudll\xrcdemo.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28u_xrc.lib wxmsw28u_html.lib wxmsw28u_adv.lib wxmsw28u_core.lib wxbase28u_xml.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswudll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw28u_xrc.lib wxmsw28u_html.lib wxmsw28u_adv.lib wxmsw28u_core.lib wxbase28u_xml.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswudll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuddll\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuddll\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28ud_xrc.lib wxmsw28ud_html.lib wxmsw28ud_adv.lib wxmsw28ud_core.lib wxbase28ud_xml.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswuddll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw28ud_xrc.lib wxmsw28ud_html.lib wxmsw28ud_adv.lib wxmsw28ud_core.lib wxbase28ud_xml.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswuddll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\msw" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswdll\xrcdemo.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_dll\msw" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswdll\xrcdemo.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\msw" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\msw" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_core.lib wxbase28_xml.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswdll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_core.lib wxbase28_xml.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswdll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswddll\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_dll\mswd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswddll\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_dll\mswd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_core.lib wxbase28d_xml.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswddll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_core.lib wxbase28d_xml.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswddll\xrcdemo.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswunivu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivu\xrcdemo.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswunivu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivu\xrcdemo.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28u_xrc.lib wxmswuniv28u_html.lib wxmswuniv28u_adv.lib wxmswuniv28u_core.lib wxbase28u_xml.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivu\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv28u_xrc.lib wxmswuniv28u_html.lib wxmswuniv28u_adv.lib wxmswuniv28u_core.lib wxbase28u_xml.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivu\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswunivud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivud\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswunivud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivud\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28ud_xrc.lib wxmswuniv28ud_html.lib wxmswuniv28ud_adv.lib wxmswuniv28ud_core.lib wxbase28ud_xml.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivud\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv28ud_xrc.lib wxmswuniv28ud_html.lib wxmswuniv28ud_adv.lib wxmswuniv28ud_core.lib wxbase28ud_xml.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivud\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswuniv" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuniv\xrcdemo.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswuniv" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuniv\xrcdemo.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswuniv" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswuniv" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28_xrc.lib wxmswuniv28_html.lib wxmswuniv28_adv.lib wxmswuniv28_core.lib wxbase28_xml.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswuniv\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv28_xrc.lib wxmswuniv28_html.lib wxmswuniv28_adv.lib wxmswuniv28_core.lib wxbase28_xml.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswuniv\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswunivd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivd\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswunivd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivd\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswunivd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28d_xrc.lib wxmswuniv28d_html.lib wxmswuniv28d_adv.lib wxmswuniv28d_core.lib wxbase28d_xml.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivd\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv28d_xrc.lib wxmswuniv28d_html.lib wxmswuniv28d_adv.lib wxmswuniv28d_core.lib wxbase28d_xml.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivd\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\plucker-desktop"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswu\xrcdemo.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\mswu" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswu\plucker-desktop.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswu" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28u_xrc.lib wxmsw28u_html.lib wxmsw28u_adv.lib wxmsw28u_core.lib wxbase28u_xml.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswu\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw28u_xrc.lib wxmsw28u_html.lib wxmsw28u_adv.lib wxmsw28u_core.lib wxbase28u_xml.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\plucker-desktop"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswud\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswud" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswud\plucker-desktop.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswud" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28ud_xrc.lib wxmsw28ud_html.lib wxmsw28ud_adv.lib wxmsw28ud_core.lib wxbase28ud_xml.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswud\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw28ud_xrc.lib wxmsw28ud_html.lib wxmsw28ud_adv.lib wxmsw28ud_core.lib wxbase28ud_xml.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\msw" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_msw\xrcdemo.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\lib\vc_lib\msw" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_msw\xrcdemo.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\msw" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\msw" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_core.lib wxbase28_xml.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_msw\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_core.lib wxbase28_xml.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_msw\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "xrcdemo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\xrcdemo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\xrcdemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswd\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\lib\vc_lib\mswd" /I ".\..\..\include" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswd\xrcdemo.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\lib\vc_lib\mswd" /i ".\..\..\include" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_core.lib wxbase28d_xml.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswd\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_core.lib wxbase28d_xml.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswd\xrcdemo.exe" /libpath:".\..\..\lib\vc_lib"

!ENDIF 

# Begin Target

# Name "xrcdemo - Win32 DLL Universal Unicode Release"
# Name "xrcdemo - Win32 DLL Universal Unicode Debug"
# Name "xrcdemo - Win32 DLL Universal Release"
# Name "xrcdemo - Win32 DLL Universal Debug"
# Name "xrcdemo - Win32 DLL Unicode Release"
# Name "xrcdemo - Win32 DLL Unicode Debug"
# Name "xrcdemo - Win32 DLL Release"
# Name "xrcdemo - Win32 DLL Debug"
# Name "xrcdemo - Win32 Universal Unicode Release"
# Name "xrcdemo - Win32 Universal Unicode Debug"
# Name "xrcdemo - Win32 Universal Release"
# Name "xrcdemo - Win32 Universal Debug"
# Name "xrcdemo - Win32 Unicode Release"
# Name "xrcdemo - Win32 Unicode Debug"
# Name "xrcdemo - Win32 Release"
# Name "xrcdemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\about_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\about_dialog.h
# End Source File
# Begin Source File

SOURCE=.\add_channel_wizard.cpp
# End Source File
# Begin Source File

SOURCE=.\add_channel_wizard.h
# End Source File
# Begin Source File

SOURCE=.\art_provider.cpp
# End Source File
# Begin Source File

SOURCE=.\art_provider.h
# End Source File
# Begin Source File

SOURCE=.\blocked_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\blocked_dialog.h
# End Source File
# Begin Source File

SOURCE=.\body_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\body_dialog.h
# End Source File
# Begin Source File

SOURCE=.\bookmark_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\bookmark_dialog.h
# End Source File
# Begin Source File

SOURCE=.\build_progress_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\build_progress_dialog.h
# End Source File
# Begin Source File

SOURCE=.\build_progress_wrappers.cpp
# End Source File
# Begin Source File

SOURCE=.\build_progress_wrappers.h
# End Source File
# Begin Source File

SOURCE=.\channel_check_listctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\channel_check_listctrl.h
# End Source File
# Begin Source File

SOURCE=.\channel_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\channel_dialog.h
# End Source File
# Begin Source File

SOURCE=.\channel_list_wizardpage.cpp
# End Source File
# Begin Source File

SOURCE=.\channel_list_wizardpage.h
# End Source File
# Begin Source File

SOURCE=.\channel_listctrl_base.cpp
# End Source File
# Begin Source File

SOURCE=.\channel_listctrl_base.h
# End Source File
# Begin Source File

SOURCE=.\channel_type.cpp
# End Source File
# Begin Source File

SOURCE=.\channel_type.h
# End Source File
# Begin Source File

SOURCE=.\checkbox_message_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\checkbox_message_dialog.h
# End Source File
# Begin Source File

SOURCE=.\checkbox_wizardpage.cpp
# End Source File
# Begin Source File

SOURCE=.\checkbox_wizardpage.h
# End Source File
# Begin Source File

SOURCE=.\commandline_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\commandline_parser.h
# End Source File
# Begin Source File

SOURCE=.\configuration.cpp
# End Source File
# Begin Source File

SOURCE=.\configuration.h
# End Source File
# Begin Source File

SOURCE=.\destinations_wizardpage.cpp
# End Source File
# Begin Source File

SOURCE=.\destinations_wizardpage.h
# End Source File
# Begin Source File

SOURCE=.\details_progress_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\details_progress_dialog.h
# End Source File
# Begin Source File

SOURCE=.\details_progress_listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\details_progress_listbox.h
# End Source File
# Begin Source File

SOURCE=.\editor_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\editor_dialog.h
# End Source File
# Begin Source File

SOURCE=.\email_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\email_dialog.h
# End Source File
# Begin Source File

SOURCE=.\exclusion_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\exclusion_dialog.h
# End Source File
# Begin Source File

SOURCE=.\exclusion_listctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\exclusion_listctrl.h
# End Source File
# Begin Source File

SOURCE=.\font_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\font_dialog.h
# End Source File
# Begin Source File

SOURCE=.\handheld_dest_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\handheld_dest_dialog.h
# End Source File
# Begin Source File

SOURCE=.\handheld_dest_type.cpp
# End Source File
# Begin Source File

SOURCE=.\handheld_dest_type.h
# End Source File
# Begin Source File

SOURCE=.\handheld_dest_wrappers.cpp
# End Source File
# Begin Source File

SOURCE=.\handheld_dest_wrappers.h
# End Source File
# Begin Source File

SOURCE=.\help_controller.cpp
# End Source File
# Begin Source File

SOURCE=.\help_controller.h
# End Source File
# Begin Source File

SOURCE=.\hhictrl.cpp
# End Source File
# Begin Source File

SOURCE=.\hhictrl.h
# End Source File
# Begin Source File

SOURCE=.\hr_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\hr_dialog.h
# End Source File
# Begin Source File

SOURCE=.\html_tip_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\html_tip_dialog.h
# End Source File
# Begin Source File

SOURCE=.\hyperlink_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\hyperlink_dialog.h
# End Source File
# Begin Source File

SOURCE=.\image_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\image_dialog.h
# End Source File
# Begin Source File

SOURCE=.\image_list.cpp
# End Source File
# Begin Source File

SOURCE=.\image_list.h
# End Source File
# Begin Source File

SOURCE=.\import_controller.cpp
# End Source File
# Begin Source File

SOURCE=.\import_controller.h
# End Source File
# Begin Source File

SOURCE=.\install_viewer_wizard.cpp
# End Source File
# Begin Source File

SOURCE=.\install_viewer_wizard.h
# End Source File
# Begin Source File

SOURCE=.\internationalization.cpp
# End Source File
# Begin Source File

SOURCE=.\internationalization.h
# End Source File
# Begin Source File

SOURCE=.\limits_wizardpage.cpp
# End Source File
# Begin Source File

SOURCE=.\limits_wizardpage.h
# End Source File
# Begin Source File

SOURCE=.\main_frame.cpp
# End Source File
# Begin Source File

SOURCE=.\main_frame.h
# End Source File
# Begin Source File

SOURCE=.\main_listctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\main_listctrl.h
# End Source File
# Begin Source File

SOURCE=.\message_wizardpage.cpp
# End Source File
# Begin Source File

SOURCE=.\message_wizardpage.h
# End Source File
# Begin Source File

SOURCE=.\ol_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ol_dialog.h
# End Source File
# Begin Source File

SOURCE=.\os_msw.cpp
# End Source File
# Begin Source File

SOURCE=.\os_msw.h
# End Source File
# Begin Source File

SOURCE=.\palm_installer.h
# End Source File
# Begin Source File

SOURCE=.\palm_installer_base.cpp
# End Source File
# Begin Source File

SOURCE=.\palm_installer_base.h
# End Source File
# Begin Source File

SOURCE=.\palm_installer_gtk.cpp
# End Source File
# Begin Source File

SOURCE=.\palm_installer_gtk.h
# End Source File
# Begin Source File

SOURCE=.\palm_installer_mac.cpp
# End Source File
# Begin Source File

SOURCE=.\palm_installer_mac.h
# End Source File
# Begin Source File

SOURCE=.\palm_installer_msw.cpp
# End Source File
# Begin Source File

SOURCE=.\palm_installer_msw.h
# End Source File
# Begin Source File

SOURCE=.\pda_listctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\pda_listctrl.h
# End Source File
# Begin Source File

SOURCE=.\piped_process.cpp
# End Source File
# Begin Source File

SOURCE=.\piped_process.h
# End Source File
# Begin Source File

SOURCE=".\plucker-desktop.cpp"
# End Source File
# Begin Source File

SOURCE=".\plucker-desktop.h"
# End Source File
# Begin Source File

SOURCE=".\plucker-desktop.rc"
# End Source File
# Begin Source File

SOURCE=.\plucker_controller.cpp
# End Source File
# Begin Source File

SOURCE=.\plucker_controller.h
# End Source File
# Begin Source File

SOURCE=.\plucker_defines.h
# End Source File
# Begin Source File

SOURCE=.\plucker_wizard_base.cpp
# End Source File
# Begin Source File

SOURCE=.\plucker_wizard_base.h
# End Source File
# Begin Source File

SOURCE=.\plucker_wizard_wrappers.cpp
# End Source File
# Begin Source File

SOURCE=.\plucker_wizard_wrappers.h
# End Source File
# Begin Source File

SOURCE=.\popup_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\popup_dialog.h
# End Source File
# Begin Source File

SOURCE=.\preferences_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\preferences_dialog.h
# End Source File
# Begin Source File

SOURCE=.\process_progress_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\process_progress_dialog.h
# End Source File
# Begin Source File

SOURCE=.\processed_tip_provider.cpp
# End Source File
# Begin Source File

SOURCE=.\processed_tip_provider.h
# End Source File
# Begin Source File

SOURCE=.\proxy_prompt.cpp
# End Source File
# Begin Source File

SOURCE=.\proxy_prompt.h
# End Source File
# Begin Source File

SOURCE=.\proxy_wizardpage.cpp
# End Source File
# Begin Source File

SOURCE=.\proxy_wizardpage.h
# End Source File
# Begin Source File

SOURCE=.\setup.h
# End Source File
# Begin Source File

SOURCE=.\setup_wizard.cpp
# End Source File
# Begin Source File

SOURCE=.\setup_wizard.h
# End Source File
# Begin Source File

SOURCE=.\showcase_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\showcase_dialog.h
# End Source File
# Begin Source File

SOURCE=.\showcase_listctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\showcase_listctrl.h
# End Source File
# Begin Source File

SOURCE=.\showcase_splitterwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\showcase_splitterwindow.h
# End Source File
# Begin Source File

SOURCE=.\span_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\span_dialog.h
# End Source File
# Begin Source File

SOURCE=.\splashscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\splashscreen.h
# End Source File
# Begin Source File

SOURCE=.\starting_page_wizardpage.cpp
# End Source File
# Begin Source File

SOURCE=.\starting_page_wizardpage.h
# End Source File
# Begin Source File

SOURCE=.\startup_tips.cpp
# End Source File
# Begin Source File

SOURCE=.\startup_tips.h
# End Source File
# Begin Source File

SOURCE=.\table_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\table_dialog.h
# End Source File
# Begin Source File

SOURCE=.\td_th_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\td_th_dialog.h
# End Source File
# Begin Source File

SOURCE=.\textctrl_wizardpage.cpp
# End Source File
# Begin Source File

SOURCE=.\textctrl_wizardpage.h
# End Source File
# Begin Source File

SOURCE=.\time_control.cpp
# End Source File
# Begin Source File

SOURCE=.\time_control.h
# End Source File
# Begin Source File

SOURCE=.\ul_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ul_dialog.h
# End Source File
# Begin Source File

SOURCE=.\utils_clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\utils_clipboard.h
# End Source File
# Begin Source File

SOURCE=.\utils_controls.cpp
# End Source File
# Begin Source File

SOURCE=.\utils_controls.h
# End Source File
# Begin Source File

SOURCE=.\utils_datetime.cpp
# End Source File
# Begin Source File

SOURCE=.\utils_datetime.h
# End Source File
# Begin Source File

SOURCE=.\utils_execute.cpp
# End Source File
# Begin Source File

SOURCE=.\utils_execute.h
# End Source File
# Begin Source File

SOURCE=.\utils_string.cpp
# End Source File
# Begin Source File

SOURCE=.\utils_string.h
# End Source File
# Begin Source File

SOURCE=.\viewer_wizardpage.cpp
# End Source File
# Begin Source File

SOURCE=.\viewer_wizardpage.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Group "XRC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\resource\xrc\about_dialog.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\about_simple_dialog.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\channel_dialog.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\details_progress_dialog.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\editor_dialog.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\exclusion_dialog.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\main_frame.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\main_menubar.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\main_toolbar.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\preferences_dialog.xrc
# End Source File
# Begin Source File

SOURCE=.\resource\xrc\showcase_dialog.xrc
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\resource\icons_application\application.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\cross.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\rightarr.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\std.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\wx.manifest
# End Source File
# End Target
# End Project
