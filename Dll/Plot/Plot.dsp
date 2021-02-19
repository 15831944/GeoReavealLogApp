# Microsoft Developer Studio Project File - Name="Plot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Plot - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Plot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Plot.mak" CFG="Plot - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Plot - Win32 Release" (based on "Win32 (x86) Console Application")
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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "FWD_CHINESE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 FIDio.Lib /nologo /subsystem:windows /dll /machine:I386 /out:"D:\≤‚æÆΩ‚ Õ∆ΩÃ®\System\Plot.dll" /libpath:"..\..\LinkLib"
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "Plot - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Chicdial.cpp
# End Source File
# Begin Source File

SOURCE=.\depthdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ExBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\FillListPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Gradient.cpp
# End Source File
# Begin Source File

SOURCE=.\GradientCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\GradientCtrlImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\Grid.cpp
# End Source File
# Begin Source File

SOURCE=.\GridDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageProcessors.cpp
# End Source File
# Begin Source File

SOURCE=.\LineListPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MinFontDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Plot.cpp
# End Source File
# Begin Source File

SOURCE=.\Plot.def
# End Source File
# Begin Source File

SOURCE=.\Plot.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Style.cpp
# End Source File
# Begin Source File

SOURCE=.\StyleDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TGraphics.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\chicdial.h
# End Source File
# Begin Source File

SOURCE=.\depthdialog.h
# End Source File
# Begin Source File

SOURCE=.\ExBitmap.h
# End Source File
# Begin Source File

SOURCE=.\FillListPage.h
# End Source File
# Begin Source File

SOURCE=.\Gradient.h
# End Source File
# Begin Source File

SOURCE=.\Gradientctrl.h
# End Source File
# Begin Source File

SOURCE=.\GradientCtrlImpl.h
# End Source File
# Begin Source File

SOURCE=.\Grid.h
# End Source File
# Begin Source File

SOURCE=.\GridDialog.h
# End Source File
# Begin Source File

SOURCE=.\helpid.h
# End Source File
# Begin Source File

SOURCE=.\ImageProcessors.h
# End Source File
# Begin Source File

SOURCE=.\LineListPage.h
# End Source File
# Begin Source File

SOURCE=.\MinFontDialog.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Style.h
# End Source File
# Begin Source File

SOURCE=.\StyleDialog.h
# End Source File
# Begin Source File

SOURCE=.\TGraphics.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\COREOG1.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\COREOG2.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\COREOG3.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\COREOG4.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\COREOG5.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\COREOG6.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\COREOG7.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH1.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH10.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH11.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH12.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH13.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH14.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH15.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH16.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH17.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH18.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH19.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH2.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH20.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH21.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH22.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH23.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH24.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH25.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH26.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH27.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH28.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH29.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH3.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH30.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH31.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH32.BMP
# End Source File
# Begin Source File

SOURCE=.\res\corlth33.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH4.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH5.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH6.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH7.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH8.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\CORLTH9.BMP
# End Source File
# Begin Source File

SOURCE=.\res\font.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\LINE1.BMP
# End Source File
# Begin Source File

SOURCE=.\res\LINE2.BMP
# End Source File
# Begin Source File

SOURCE=.\res\LINE3.BMP
# End Source File
# Begin Source File

SOURCE=.\res\LINE4.BMP
# End Source File
# Begin Source File

SOURCE=.\res\LINE5.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\line6.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\line7.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\line8.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\line9.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\LITH8.BMP
# End Source File
# Begin Source File

SOURCE=.\res\Plot.rc2
# End Source File
# Begin Source File

SOURCE=.\res\report1.bmp
# End Source File
# End Group
# End Target
# End Project
