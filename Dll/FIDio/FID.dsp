# Microsoft Developer Studio Project File - Name="FID" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=FID - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FID.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FID.mak" CFG="FID - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FID - Win32 Release" (based on "Win32 (x86) Console Application")
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
# PROP Output_Dir "..\..\LinkLib"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
F90=df.exe
# ADD F90 /browser
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "FWD_CHINESE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXEXT" /D "_AFXDLL" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 IPHlpApi.lib LogBase.Lib TopUI.Lib /nologo /subsystem:windows /dll /machine:I386 /out:"D:\GeoReveal2009\System\FIDio.dll" /libpath:"..\..\LinkLib"
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "FID - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AttrDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BROWSEFORFOLDER.CPP
# End Source File
# Begin Source File

SOURCE=.\Clouds.cpp
# End Source File
# Begin Source File

SOURCE=.\CreditDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveView.cpp
# End Source File
# Begin Source File

SOURCE=.\DragDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FaderWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\FID.cpp
# End Source File
# Begin Source File

SOURCE=.\FID.def
# End Source File
# Begin Source File

SOURCE=.\FID.rc
# End Source File
# Begin Source File

SOURCE=.\FIDio.cpp
# End Source File
# Begin Source File

SOURCE=.\Guage.cpp
# End Source File
# Begin Source File

SOURCE=.\Message.cpp
# End Source File
# Begin Source File

SOURCE=.\SBDESTINATION.CPP
# End Source File
# Begin Source File

SOURCE=.\StaticLink.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SysDisk.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AttrDlg.h
# End Source File
# Begin Source File

SOURCE=.\BROWSEFORFOLDER.H
# End Source File
# Begin Source File

SOURCE=.\Clouds.h
# End Source File
# Begin Source File

SOURCE=.\CreditDialog.h
# End Source File
# Begin Source File

SOURCE=.\CurveView.h
# End Source File
# Begin Source File

SOURCE=.\DragDialog.h
# End Source File
# Begin Source File

SOURCE=.\FaderWnd.h
# End Source File
# Begin Source File

SOURCE=.\FIDio.h
# End Source File
# Begin Source File

SOURCE=.\Guage.h
# End Source File
# Begin Source File

SOURCE=.\Message.h
# End Source File
# Begin Source File

SOURCE=.\SBDESTINATION.H
# End Source File
# Begin Source File

SOURCE=.\StaticLink.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\SysDisk.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\BackGround1.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Cur134.cur
# End Source File
# Begin Source File

SOURCE=.\RES\LogShell.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\RES\Hover.wav
# End Source File
# End Target
# End Project
