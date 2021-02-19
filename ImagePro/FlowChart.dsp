# Microsoft Developer Studio Project File - Name="FlowChart" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FlowChart - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FlowChart.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FlowChart.mak" CFG="FlowChart - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FlowChart - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FlowChart - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FlowChart - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 FIDio.Lib TopUI.Lib LogSys.Lib GeoMath.Lib /nologo /subsystem:windows /machine:I386 /out:"D:\测井解释平台\System\FlowChart.exe" /libpath:"..\LinkLib"

!ELSEIF  "$(CFG)" == "FlowChart - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 FIDio.Lib TopUI.Lib LogSys.Lib /nologo /subsystem:windows /debug /machine:I386 /out:"F:\测井解释平台\System\FlowChart.exe" /pdbtype:sept /libpath:"..\LinkLib"

!ENDIF 

# Begin Target

# Name "FlowChart - Win32 Release"
# Name "FlowChart - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AACCorrect.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupData.cpp
# End Source File
# Begin Source File

SOURCE=.\BackupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomVerticalTree.cpp
# End Source File
# Begin Source File

SOURCE=.\DataUnite.cpp
# End Source File
# Begin Source File

SOURCE=.\DepCorrect.cpp
# End Source File
# Begin Source File

SOURCE=.\DIPCompute.cpp
# End Source File
# Begin Source File

SOURCE=.\DIPComputeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowChart.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowChart.rc
# End Source File
# Begin Source File

SOURCE=.\FlowChartDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowChartView.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowNode.cpp
# End Source File
# Begin Source File

SOURCE=.\GainRegress.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageAzCorrect.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageCrea.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageCreatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageGeoCorrect.cpp
# End Source File
# Begin Source File

SOURCE=.\ImagePoisePro.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageRMFScale.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageToneup.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageToneupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImgFrequency.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkWellData.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ManPro.cpp
# End Source File
# Begin Source File

SOURCE=.\PadCorrect.cpp
# End Source File
# Begin Source File

SOURCE=.\PADDepCorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayBack.cpp
# End Source File
# Begin Source File

SOURCE=.\PORCompute.cpp
# End Source File
# Begin Source File

SOURCE=.\ProItem.cpp
# End Source File
# Begin Source File

SOURCE=.\RMFScaleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AACCorrect.h
# End Source File
# Begin Source File

SOURCE=.\BackupData.h
# End Source File
# Begin Source File

SOURCE=.\BackupDlg.h
# End Source File
# Begin Source File

SOURCE=.\CustomVerticalTree.h
# End Source File
# Begin Source File

SOURCE=.\DataRMFScale.h
# End Source File
# Begin Source File

SOURCE=.\DataUnite.h
# End Source File
# Begin Source File

SOURCE=.\DepCorrect.h
# End Source File
# Begin Source File

SOURCE=.\DIPCompute.h
# End Source File
# Begin Source File

SOURCE=.\DIPComputeDlg.h
# End Source File
# Begin Source File

SOURCE=.\FlowChart.h
# End Source File
# Begin Source File

SOURCE=.\FlowChartDoc.h
# End Source File
# Begin Source File

SOURCE=.\FlowChartView.h
# End Source File
# Begin Source File

SOURCE=.\FlowNode.h
# End Source File
# Begin Source File

SOURCE=.\GainRegress.h
# End Source File
# Begin Source File

SOURCE=.\ImageAzCorrect.h
# End Source File
# Begin Source File

SOURCE=.\ImageCreat.h
# End Source File
# Begin Source File

SOURCE=.\ImageCreatDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImageGeoCorrect.h
# End Source File
# Begin Source File

SOURCE=.\ImagePoisePro.h
# End Source File
# Begin Source File

SOURCE=.\ImageRMFScale.h
# End Source File
# Begin Source File

SOURCE=.\ImageToneup.h
# End Source File
# Begin Source File

SOURCE=.\ImageToneupDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImgFrequency.h
# End Source File
# Begin Source File

SOURCE=.\LinkWellData.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ManPro.h
# End Source File
# Begin Source File

SOURCE=.\PadCorrect.h
# End Source File
# Begin Source File

SOURCE=.\PADDepCorDlg.h
# End Source File
# Begin Source File

SOURCE=.\PlayBack.h
# End Source File
# Begin Source File

SOURCE=.\PORCompute.h
# End Source File
# Begin Source File

SOURCE=.\ProItem.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RMFScaleDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\6.ICO
# End Source File
# Begin Source File

SOURCE=.\res\9.ICO
# End Source File
# Begin Source File

SOURCE=.\res\FlowChart.ico
# End Source File
# Begin Source File

SOURCE=.\res\FlowChart.rc2
# End Source File
# Begin Source File

SOURCE=.\res\FlowChartDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\HotToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\XPTHEME.BIN
# End Source File
# End Group
# Begin Source File

SOURCE=.\FlowChart.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
