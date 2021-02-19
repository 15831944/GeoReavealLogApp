# Microsoft Developer Studio Project File - Name="XGRAPH" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=XGRAPH - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XGRAPH.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XGRAPH.mak" CFG="XGRAPH - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XGRAPH - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/XGRAPH", BQFAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\LinkLib"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XGRAPH_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386 /def:".\xgraph.def" /out:"D:\≤‚æÆΩ‚ Õ∆ΩÃ®\System\XGRAPH10.dll" /implib:"lib/XGRAPH10.lib"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Target

# Name "XGRAPH - Win32 Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AxisDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChartDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChartPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ColourPopup.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\gfxutils.cpp
# End Source File
# Begin Source File

SOURCE=.\LabelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MovingAvgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TrendDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\vector2d.cpp
# End Source File
# Begin Source File

SOURCE=.\XGRAPH.cpp
# End Source File
# Begin Source File

SOURCE=.\xgraph.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\XGraph.rc
# End Source File
# Begin Source File

SOURCE=.\XGraph10.cpp
# End Source File
# Begin Source File

SOURCE=.\XGraphAxis.cpp
# End Source File
# Begin Source File

SOURCE=.\XGraphDataNotation.cpp
# End Source File
# Begin Source File

SOURCE=.\XGraphDataSerie.cpp
# End Source File
# Begin Source File

SOURCE=.\XGraphLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\XGraphObjectBase.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AxisDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChartDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChartPage.h
# End Source File
# Begin Source File

SOURCE=.\ColorButton.h
# End Source File
# Begin Source File

SOURCE=.\ColourPopup.h
# End Source File
# Begin Source File

SOURCE=.\CurveDlg.h
# End Source File
# Begin Source File

SOURCE=.\GfxUtils.h
# End Source File
# Begin Source File

SOURCE=.\LabelDlg.h
# End Source File
# Begin Source File

SOURCE=.\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\MovingAvgDlg.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TrendDlg.h
# End Source File
# Begin Source File

SOURCE=.\vector2d.h
# End Source File
# Begin Source File

SOURCE=.\XGRAPH.h
# End Source File
# Begin Source File

SOURCE=XGraphTest\..\xgraph10.h
# End Source File
# Begin Source File

SOURCE=.\XGraphAxis.h
# End Source File
# Begin Source File

SOURCE=.\XGraphDataNotation.h
# End Source File
# Begin Source File

SOURCE=.\XGraphDataSerie.h
# End Source File
# Begin Source File

SOURCE=.\XGraphLabel.h
# End Source File
# Begin Source File

SOURCE=.\XGraphObjectBase.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bs5.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
