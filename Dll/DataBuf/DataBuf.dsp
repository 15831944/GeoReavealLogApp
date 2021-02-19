# Microsoft Developer Studio Project File - Name="DataBuf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DataBuf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DataBuf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DataBuf.mak" CFG="DataBuf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DataBuf - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DataBuf - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DataBuf - Win32 Release"

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
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"D:\≤‚æÆΩ‚ Õ∆ΩÃ®\System\DataBuf.dll"

!ELSEIF  "$(CFG)" == "DataBuf - Win32 Debug"

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

# Name "DataBuf - Win32 Release"
# Name "DataBuf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BitR.cpp
# End Source File
# Begin Source File

SOURCE=.\BitToDLis.cpp
# End Source File
# Begin Source File

SOURCE=.\BitToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\BitToSlc716.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeSP.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveSplit.cpp
# End Source File
# Begin Source File

SOURCE=.\DataBuf.cpp
# End Source File
# Begin Source File

SOURCE=.\DataBuf.def
# End Source File
# Begin Source File

SOURCE=.\DataBuf.rc
# End Source File
# Begin Source File

SOURCE=.\DlisCurDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DlisInput.cpp
# End Source File
# Begin Source File

SOURCE=.\DlisOut.cpp
# End Source File
# Begin Source File

SOURCE=.\DlisRead.cpp
# End Source File
# Begin Source File

SOURCE=.\DLisToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\DLisToLis.cpp
# End Source File
# Begin Source File

SOURCE=.\DLisToSlc716.cpp
# End Source File
# Begin Source File

SOURCE=.\DLisToWis.cpp
# End Source File
# Begin Source File

SOURCE=.\DLisToXtf.cpp
# End Source File
# Begin Source File

SOURCE=.\DlisWrite.cpp
# End Source File
# Begin Source File

SOURCE=.\FidRead.cpp
# End Source File
# Begin Source File

SOURCE=.\FidToDLis.cpp
# End Source File
# Begin Source File

SOURCE=.\FidToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\FidToLis.cpp
# End Source File
# Begin Source File

SOURCE=.\FidToQh716.cpp
# End Source File
# Begin Source File

SOURCE=.\FidToSlc716.cpp
# End Source File
# Begin Source File

SOURCE=.\FidToTxt.cpp
# End Source File
# Begin Source File

SOURCE=.\FidToWis.cpp
# End Source File
# Begin Source File

SOURCE=.\FidToXtf.cpp
# End Source File
# Begin Source File

SOURCE=.\FidWrite.cpp
# End Source File
# Begin Source File

SOURCE=.\FwdTxtR.cpp
# End Source File
# Begin Source File

SOURCE=.\FwdTxtW.cpp
# End Source File
# Begin Source File

SOURCE=.\LisRead.cpp
# End Source File
# Begin Source File

SOURCE=.\LisToDlis.cpp
# End Source File
# Begin Source File

SOURCE=.\LisToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\LisWrite.cpp
# End Source File
# Begin Source File

SOURCE=.\ModiConvDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModiCurAttDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModiFileNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Qh716R.cpp
# End Source File
# Begin Source File

SOURCE=.\Qh716ToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\Qh716W.cpp
# End Source File
# Begin Source File

SOURCE=.\ReCurNamDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SetDlisParaDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Skc92R.cpp
# End Source File
# Begin Source File

SOURCE=.\Skc92ToDLis.cpp
# End Source File
# Begin Source File

SOURCE=.\Skc92ToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\Skc92ToSlc716.cpp
# End Source File
# Begin Source File

SOURCE=.\Skc98R.cpp
# End Source File
# Begin Source File

SOURCE=.\Skc98ToDLis.cpp
# End Source File
# Begin Source File

SOURCE=.\Skc98ToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\Skc98ToSlc716.cpp
# End Source File
# Begin Source File

SOURCE=.\Skh2000R.cpp
# End Source File
# Begin Source File

SOURCE=.\Skh2000ToDLis.cpp
# End Source File
# Begin Source File

SOURCE=.\Skh2000ToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\Skh2000ToSlc716.cpp
# End Source File
# Begin Source File

SOURCE=.\Slc716R.cpp
# End Source File
# Begin Source File

SOURCE=.\Slc716ToDLis.cpp
# End Source File
# Begin Source File

SOURCE=.\Slc716ToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\Slc716ToSlc716.cpp
# End Source File
# Begin Source File

SOURCE=.\Slc716ToWis.cpp
# End Source File
# Begin Source File

SOURCE=.\Slc716ToXtf.cpp
# End Source File
# Begin Source File

SOURCE=.\Slc716W.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SysDisk.cpp
# End Source File
# Begin Source File

SOURCE=.\ToDLisFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ToFidFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ToLisFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ToQh716File.cpp
# End Source File
# Begin Source File

SOURCE=.\ToSlc716File.cpp
# End Source File
# Begin Source File

SOURCE=.\ToTxtFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ToWisFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ToXtfFile.cpp
# End Source File
# Begin Source File

SOURCE=.\TxtToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\WisRead.cpp
# End Source File
# Begin Source File

SOURCE=.\WisToDLis.cpp
# End Source File
# Begin Source File

SOURCE=.\WisToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\WisToQh716.cpp
# End Source File
# Begin Source File

SOURCE=.\WisToSlc716.cpp
# End Source File
# Begin Source File

SOURCE=.\WisToTxt.cpp
# End Source File
# Begin Source File

SOURCE=.\WisToXtf.cpp
# End Source File
# Begin Source File

SOURCE=.\WisWrite.cpp
# End Source File
# Begin Source File

SOURCE=.\Xb716R.cpp
# End Source File
# Begin Source File

SOURCE=.\Xb716ToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\Xb716ToWis.cpp
# End Source File
# Begin Source File

SOURCE=.\XtfRead.cpp
# End Source File
# Begin Source File

SOURCE=.\XtfToDLis.cpp
# End Source File
# Begin Source File

SOURCE=.\XtfToFid.cpp
# End Source File
# Begin Source File

SOURCE=.\XtfToSlc716.cpp
# End Source File
# Begin Source File

SOURCE=.\XtfToWis.cpp
# End Source File
# Begin Source File

SOURCE=.\XtfWrite.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\_comdat.h
# End Source File
# Begin Source File

SOURCE=.\_comext.h
# End Source File
# Begin Source File

SOURCE=.\Bit_Head.h
# End Source File
# Begin Source File

SOURCE=.\BitR.h
# End Source File
# Begin Source File

SOURCE=.\BitToDLis.h
# End Source File
# Begin Source File

SOURCE=.\BitToFid.h
# End Source File
# Begin Source File

SOURCE=.\BitToSlc716.h
# End Source File
# Begin Source File

SOURCE=.\CodeSP.h
# End Source File
# Begin Source File

SOURCE=.\CurveSplit.h
# End Source File
# Begin Source File

SOURCE=.\Dlis_Head.h
# End Source File
# Begin Source File

SOURCE=.\DlisCurDlg.h
# End Source File
# Begin Source File

SOURCE=.\DlisInput.h
# End Source File
# Begin Source File

SOURCE=.\DlisOut.h
# End Source File
# Begin Source File

SOURCE=.\DlisRead.h
# End Source File
# Begin Source File

SOURCE=.\DLisToFid.h
# End Source File
# Begin Source File

SOURCE=.\DLisToLis.h
# End Source File
# Begin Source File

SOURCE=.\DLisToSlc716.h
# End Source File
# Begin Source File

SOURCE=.\DLisToWis.h
# End Source File
# Begin Source File

SOURCE=.\DLisToXtf.h
# End Source File
# Begin Source File

SOURCE=.\DlisWrite.h
# End Source File
# Begin Source File

SOURCE=.\Fid_Head.h
# End Source File
# Begin Source File

SOURCE=.\FidRead.h
# End Source File
# Begin Source File

SOURCE=.\FidToDLis.h
# End Source File
# Begin Source File

SOURCE=.\FidToFid.h
# End Source File
# Begin Source File

SOURCE=.\FidToLis.h
# End Source File
# Begin Source File

SOURCE=.\FidToQh716.h
# End Source File
# Begin Source File

SOURCE=.\FidToSlc716.h
# End Source File
# Begin Source File

SOURCE=.\FidToTxt.h
# End Source File
# Begin Source File

SOURCE=.\FidToWis.h
# End Source File
# Begin Source File

SOURCE=.\FidToXtf.h
# End Source File
# Begin Source File

SOURCE=.\FidWrite.h
# End Source File
# Begin Source File

SOURCE=.\FwdTxt_Head.h
# End Source File
# Begin Source File

SOURCE=.\FwdTxtR.h
# End Source File
# Begin Source File

SOURCE=.\FwdTxtW.h
# End Source File
# Begin Source File

SOURCE=.\Lis_Head.h
# End Source File
# Begin Source File

SOURCE=.\LisRead.h
# End Source File
# Begin Source File

SOURCE=.\LisToDlis.h
# End Source File
# Begin Source File

SOURCE=.\LisToFid.h
# End Source File
# Begin Source File

SOURCE=.\LisWrite.h
# End Source File
# Begin Source File

SOURCE=.\ModiConvDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModiCurAttDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModiFileNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\port32.h
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.h
# End Source File
# Begin Source File

SOURCE=.\Qh716_Head.h
# End Source File
# Begin Source File

SOURCE=.\Qh716R.h
# End Source File
# Begin Source File

SOURCE=.\Qh716ToFid.h
# End Source File
# Begin Source File

SOURCE=.\Qh716W.h
# End Source File
# Begin Source File

SOURCE=.\ReCurNamDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScanFile.h
# End Source File
# Begin Source File

SOURCE=.\SetDlisParaDlg.h
# End Source File
# Begin Source File

SOURCE=.\Skc92_Head.h
# End Source File
# Begin Source File

SOURCE=.\Skc92R.h
# End Source File
# Begin Source File

SOURCE=.\Skc92ToDLis.h
# End Source File
# Begin Source File

SOURCE=.\Skc92ToFid.h
# End Source File
# Begin Source File

SOURCE=.\Skc92ToSlc716.h
# End Source File
# Begin Source File

SOURCE=.\Skc98_Head.h
# End Source File
# Begin Source File

SOURCE=.\Skc98R.h
# End Source File
# Begin Source File

SOURCE=.\Skc98ToDLis.h
# End Source File
# Begin Source File

SOURCE=.\Skc98ToFid.h
# End Source File
# Begin Source File

SOURCE=.\Skc98ToSlc716.h
# End Source File
# Begin Source File

SOURCE=.\Skh2000_Head.h
# End Source File
# Begin Source File

SOURCE=.\Skh2000R.h
# End Source File
# Begin Source File

SOURCE=.\Skh2000ToDLis.h
# End Source File
# Begin Source File

SOURCE=.\Skh2000ToFid.h
# End Source File
# Begin Source File

SOURCE=.\Skh2000ToSlc716.h
# End Source File
# Begin Source File

SOURCE=.\Slc716_Head.h
# End Source File
# Begin Source File

SOURCE=.\Slc716R.h
# End Source File
# Begin Source File

SOURCE=.\Slc716ToDLis.h
# End Source File
# Begin Source File

SOURCE=.\Slc716ToFid.h
# End Source File
# Begin Source File

SOURCE=.\Slc716ToSlc716.h
# End Source File
# Begin Source File

SOURCE=.\Slc716ToWis.h
# End Source File
# Begin Source File

SOURCE=.\Slc716ToXtf.h
# End Source File
# Begin Source File

SOURCE=.\Slc716W.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SysDisk.h
# End Source File
# Begin Source File

SOURCE=.\ToDLisFile.h
# End Source File
# Begin Source File

SOURCE=.\ToFidFile.h
# End Source File
# Begin Source File

SOURCE=.\ToLisFile.h
# End Source File
# Begin Source File

SOURCE=.\ToQh716File.h
# End Source File
# Begin Source File

SOURCE=.\ToSlc716File.h
# End Source File
# Begin Source File

SOURCE=.\ToTxtFile.h
# End Source File
# Begin Source File

SOURCE=.\ToWisFile.h
# End Source File
# Begin Source File

SOURCE=.\ToXtfFile.h
# End Source File
# Begin Source File

SOURCE=.\TxtToFid.h
# End Source File
# Begin Source File

SOURCE=.\winio.h
# End Source File
# Begin Source File

SOURCE=.\Wis_Head.h
# End Source File
# Begin Source File

SOURCE=.\WisRead.h
# End Source File
# Begin Source File

SOURCE=.\WisToDLis.h
# End Source File
# Begin Source File

SOURCE=.\WisToFid.h
# End Source File
# Begin Source File

SOURCE=.\WisToQh716.h
# End Source File
# Begin Source File

SOURCE=.\WisToSlc716.h
# End Source File
# Begin Source File

SOURCE=.\WisToTxt.h
# End Source File
# Begin Source File

SOURCE=.\WisToXtf.h
# End Source File
# Begin Source File

SOURCE=.\WisWrite.h
# End Source File
# Begin Source File

SOURCE=.\Xb716_Head.h
# End Source File
# Begin Source File

SOURCE=.\Xb716R.h
# End Source File
# Begin Source File

SOURCE=.\Xb716ToFid.h
# End Source File
# Begin Source File

SOURCE=.\Xb716ToWis.h
# End Source File
# Begin Source File

SOURCE=.\Xtf_Head.h
# End Source File
# Begin Source File

SOURCE=.\XtfRead.h
# End Source File
# Begin Source File

SOURCE=.\XtfToDLis.h
# End Source File
# Begin Source File

SOURCE=.\XtfToFid.h
# End Source File
# Begin Source File

SOURCE=.\XtfToSlc716.h
# End Source File
# Begin Source File

SOURCE=.\XtfToWis.h
# End Source File
# Begin Source File

SOURCE=.\XtfWrite.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DataBuf.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
