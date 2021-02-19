# Microsoft Developer Studio Project File - Name="TopUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TopUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TopUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TopUI.mak" CFG="TopUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TopUI - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TopUI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TopUI - Win32 Release"

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
F90=df.exe
# ADD BASE F90 /compile_only /dll /nologo /warn:nofileopt
# ADD F90 /compile_only /dll /nologo /warn:nofileopt
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"D:\≤‚æÆΩ‚ Õ∆ΩÃ®\System\TopUI.dll"

!ELSEIF  "$(CFG)" == "TopUI - Win32 Debug"

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
F90=df.exe
# ADD BASE F90 /check:bounds /compile_only /debug:full /dll /nologo /traceback /warn:argument_checking /warn:nofileopt
# ADD F90 /check:bounds /compile_only /debug:full /dll /nologo /traceback /warn:argument_checking /warn:nofileopt
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

# Name "TopUI - Win32 Release"
# Name "TopUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;f90;for;f;fpp"
# Begin Source File

SOURCE=.\AutoHideBar.cpp
# End Source File
# Begin Source File

SOURCE=.\BCMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\BkDialogST.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowseCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\ClockCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CMYButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CMYControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CMYTabctrlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DockPageBar.cpp
# End Source File
# Begin Source File

SOURCE=.\EnBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageTabWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\KCBusyProgressCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ListEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MDITabs.cpp
# End Source File
# Begin Source File

SOURCE=.\MyBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinButton.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinResource.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinWin.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TopUI.cpp
# End Source File
# Begin Source File

SOURCE=.\TopUI.def
# End Source File
# Begin Source File

SOURCE=.\TopUI.rc
# End Source File
# Begin Source File

SOURCE=.\UIFixTB.cpp
# End Source File
# Begin Source File

SOURCE=.\UIFlatBar.cpp
# End Source File
# Begin Source File

SOURCE=.\UIMenuBar.cpp
# End Source File
# Begin Source File

SOURCE=.\UISubclass.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# Begin Source File

SOURCE=.\WndShadow.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AutoHideBar.h
# End Source File
# Begin Source File

SOURCE=.\BCMenu.h
# End Source File
# Begin Source File

SOURCE=.\BkDialogST.h
# End Source File
# Begin Source File

SOURCE=.\BrowseCtrl.h
# End Source File
# Begin Source File

SOURCE=.\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\ClockCtrl.h
# End Source File
# Begin Source File

SOURCE=.\CMYButton.h
# End Source File
# Begin Source File

SOURCE=.\CMYControlBar.h
# End Source File
# Begin Source File

SOURCE=.\CMYTabCtrlBar.h
# End Source File
# Begin Source File

SOURCE=.\ColorListBox.h
# End Source File
# Begin Source File

SOURCE=.\DockPageBar.h
# End Source File
# Begin Source File

SOURCE=.\EnBitmap.h
# End Source File
# Begin Source File

SOURCE=.\ImageTabWnd.h
# End Source File
# Begin Source File

SOURCE=.\KCBusyProgressCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ListEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MDITabs.h
# End Source File
# Begin Source File

SOURCE=.\MyBitmap.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\sizecbar.h
# End Source File
# Begin Source File

SOURCE=.\SkinButton.h
# End Source File
# Begin Source File

SOURCE=.\SkinResource.h
# End Source File
# Begin Source File

SOURCE=.\SkinWin.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UIFixTB.h
# End Source File
# Begin Source File

SOURCE=.\UIFlatBar.h
# End Source File
# Begin Source File

SOURCE=.\UIMenuBar.h
# End Source File
# Begin Source File

SOURCE=.\UISubclass.h
# End Source File
# Begin Source File

SOURCE=.\WndShadow.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\TopUI.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
