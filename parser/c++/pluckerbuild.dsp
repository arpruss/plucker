# Microsoft Developer Studio Project File - Name="pluckerbuild" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=pluckerbuild - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pluckerbuild.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pluckerbuild.mak" CFG="pluckerbuild - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pluckerbuild - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "pluckerbuild - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pluckerbuild - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "c:\projects\gnuwin32\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib libz.a /nologo /subsystem:console /machine:I386 /out:"Release/plucker-build.exe" /libpath:"c:\projects\gnuwin32\lib"

!ELSEIF  "$(CFG)" == "pluckerbuild - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "c:\projects\gnuwin32\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib libz.a /nologo /subsystem:console /debug /machine:I386 /out:"Debug/plucker-build.exe" /pdbtype:sept /libpath:"c:\projects\gnuwin32\lib"

!ENDIF 

# Begin Target

# Name "pluckerbuild - Win32 Release"
# Name "pluckerbuild - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\FileRetriever.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlParser.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpRetriever.cpp
# End Source File
# Begin Source File

SOURCE=.\Message.cpp
# End Source File
# Begin Source File

SOURCE=.\Parser.cpp
# End Source File
# Begin Source File

SOURCE=.\PDBWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\pluckerbuild.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerCategoryDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerIndexDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerLinkIndexDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerLinksDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerMailtoDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerMetaDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerMetaSubRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerTextDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\PluckerTextParagraph.cpp
# End Source File
# Begin Source File

SOURCE=.\Resolver.cpp
# End Source File
# Begin Source File

SOURCE=.\Retriever.cpp
# End Source File
# Begin Source File

SOURCE=.\Spider.cpp
# End Source File
# Begin Source File

SOURCE=.\TextParser.cpp
# End Source File
# Begin Source File

SOURCE=.\Url.cpp
# End Source File
# Begin Source File

SOURCE=.\Writer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\FileRetriever.h
# End Source File
# Begin Source File

SOURCE=.\GlobalFunctions.h
# End Source File
# Begin Source File

SOURCE=.\HtmlParser.h
# End Source File
# Begin Source File

SOURCE=.\HttpRetriever.h
# End Source File
# Begin Source File

SOURCE=.\Message.h
# End Source File
# Begin Source File

SOURCE=.\Parser.h
# End Source File
# Begin Source File

SOURCE=.\PDBWriter.h
# End Source File
# Begin Source File

SOURCE=.\PluckerCategoryDocument.h
# End Source File
# Begin Source File

SOURCE=.\PluckerDocument.h
# End Source File
# Begin Source File

SOURCE=.\PluckerIndexDocument.h
# End Source File
# Begin Source File

SOURCE=.\PluckerLinkIndexDocument.h
# End Source File
# Begin Source File

SOURCE=.\PluckerLinksDocument.h
# End Source File
# Begin Source File

SOURCE=.\PluckerMailtoDocument.h
# End Source File
# Begin Source File

SOURCE=.\PluckerMetaDocument.h
# End Source File
# Begin Source File

SOURCE=.\PluckerMetaSubRecord.h
# End Source File
# Begin Source File

SOURCE=.\PluckerTextDocument.h
# End Source File
# Begin Source File

SOURCE=.\PluckerTextParagraph.h
# End Source File
# Begin Source File

SOURCE=.\Resolver.h
# End Source File
# Begin Source File

SOURCE=.\Retriever.h
# End Source File
# Begin Source File

SOURCE=.\Spider.h
# End Source File
# Begin Source File

SOURCE=.\TextParser.h
# End Source File
# Begin Source File

SOURCE=.\Url.h
# End Source File
# Begin Source File

SOURCE=.\Writer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
