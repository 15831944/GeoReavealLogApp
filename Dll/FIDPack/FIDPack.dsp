# Microsoft Developer Studio Project File - Name="FIDPack" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FIDPack - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FIDPack.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FIDPack.mak" CFG="FIDPack - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FIDPack - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FIDPack - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FIDPack - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\LinkLib"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"D:\≤‚æÆΩ‚ Õ∆ΩÃ®\System\FIDPack.dll"

!ELSEIF  "$(CFG)" == "FIDPack - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FIDPack - Win32 Release"
# Name "FIDPack - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\compress.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=.\ExFIDPack.cpp
# End Source File
# Begin Source File

SOURCE=.\FIDPack.cpp
# End Source File
# Begin Source File

SOURCE=.\FIDPack.def
# End Source File
# Begin Source File

SOURCE=.\FIDPack.rc
# End Source File
# Begin Source File

SOURCE=.\Zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\infblock.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\infcodes.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\infutil.c
# End Source File
# Begin Source File

SOURCE=.\Shared\SEFileInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Shared\SelfExtractor.cpp
# End Source File
# Begin Source File

SOURCE=.\Shared\ShellFileOp.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Zlib\trees.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=.\ExFIDPack.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\infblock.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\infcodes.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\infutil.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Shared\SEFileInfo.h
# End Source File
# Begin Source File

SOURCE=.\Shared\SelfExtractor.h
# End Source File
# Begin Source File

SOURCE=.\Shared\ShellFileOp.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\trees.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\zutil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\FIDPack.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
