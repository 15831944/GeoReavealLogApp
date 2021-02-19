# Microsoft Developer Studio Project File - Name="LogSys" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=LogSys - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LogSys.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LogSys.mak" CFG="LogSys - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LogSys - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\\LinkLib"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
F90=df.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "FWD_CHINESE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXEXT" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 FIDio.lib /nologo /subsystem:windows /dll /machine:I386 /out:"D:\≤‚æÆΩ‚ Õ∆ΩÃ®\System\LogSys.dll" /libpath:"..\..\LinkLib"
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "LogSys - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Editdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LogProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\LogSys.cpp
# End Source File
# Begin Source File

SOURCE=.\LogSys.def
# End Source File
# Begin Source File

SOURCE=.\LogSys.rc
# End Source File
# Begin Source File

SOURCE=.\ParameterEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\editdlg.h
# End Source File
# Begin Source File

SOURCE=.\FIDio.h
# End Source File
# Begin Source File

SOURCE=.\LogProcess.h
# End Source File
# Begin Source File

SOURCE=.\ParameterEdit.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\addtab1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\addtab2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\addtab3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\CUR134.CUR
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\deladdta.bmp
# End Source File
# Begin Source File

SOURCE=.\res\delcon1.cur
# End Source File
# Begin Source File

SOURCE=.\res\deltab.bmp
# End Source File
# Begin Source File

SOURCE=.\res\deltab1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drag1.cur
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\LogSys.rc2
# End Source File
# Begin Source File

SOURCE=.\res\moveupic.cur
# End Source File
# Begin Source File

SOURCE=.\res\ruler1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rulerup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\savepara.bmp
# End Source File
# Begin Source File

SOURCE=.\res\savetab1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\savetab2.bmp
# End Source File
# End Group
# End Target
# End Project
