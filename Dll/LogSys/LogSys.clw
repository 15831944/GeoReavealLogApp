; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CParameterEdit
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "logsys.h"
LastPage=0

ClassCount=12
Class1=CEditdlg
Class2=CAboutEdit
Class3=CParameterEdit
Class4=CDefConCtrl
Class5=CEditCtrl
Class6=CInCtrl
Class7=CConCtrl
Class8=COutCtrl
Class9=CRuler
Class10=CTabNameDlg
Class11=CDCEdit
Class12=CDepthDlg

ResourceCount=7
Resource1=IDD_CHANGEDEPTH
Resource2=IDD_EDITDLG
Resource3=IDD_ParameterEdit
Resource4=CG_IDR_POPUP_RULER
Resource5=IDR_EDITMENU
Resource6=IDD_INPUT_TABNAME
Resource7=IDR_PARAMETERMEDITMENU

[CLS:CEditdlg]
Type=0
BaseClass=CDialog
HeaderFile=Editdlg.h
ImplementationFile=Editdlg.cpp

[CLS:CAboutEdit]
Type=0
BaseClass=CDialog
HeaderFile=Editdlg.h
ImplementationFile=Editdlg.cpp
LastObject=CAboutEdit

[CLS:CParameterEdit]
Type=0
BaseClass=CDialog
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp
LastObject=IDC_INCTRL

[CLS:CDefConCtrl]
Type=0
BaseClass=CListCtrl
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp

[CLS:CEditCtrl]
Type=0
BaseClass=CListCtrl
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp

[CLS:CInCtrl]
Type=0
BaseClass=CEditCtrl
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp

[CLS:CConCtrl]
Type=0
BaseClass=CEditCtrl
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp

[CLS:COutCtrl]
Type=0
BaseClass=CEditCtrl
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp

[CLS:CRuler]
Type=0
BaseClass=CStatic
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp

[CLS:CTabNameDlg]
Type=0
BaseClass=CDialog
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp

[CLS:CDCEdit]
Type=0
BaseClass=CEdit
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp

[CLS:CDepthDlg]
Type=0
BaseClass=CDialog
HeaderFile=ParameterEdit.h
ImplementationFile=ParameterEdit.cpp

[DLG:IDD_EDITDLG]
Type=1
Class=CEditdlg
ControlCount=1
Control1=IDC_EDITBOX,edit,1353781700

[DLG:IDD_ABOUTEDIT]
Type=1
Class=CAboutEdit

[DLG:IDD_ParameterEdit]
Type=1
Class=CParameterEdit
ControlCount=19
Control1=IDC_CONCTRL,SysListView32,1350631749
Control2=IDC_DEFAULTCONCTRL,SysListView32,1350631701
Control3=IDC_INCTRL,SysListView32,1350631765
Control4=IDC_OUTCTRL,SysListView32,1350631765
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816
Control7=IDC_CONTAB,SysTabControl32,1342197760
Control8=IDC_RULER,static,1342308608
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT,edit,1073807496
Control13=IDC_COMBO,combobox,1344348483
Control14=IDC_DEPSLIDER,msctls_trackbar32,1342242836
Control15=IDC_ZOOMIN,button,1342242816
Control16=IDC_ZOOMOUT,button,1342242816
Control17=IDC_NOC,static,1342308352
Control18=IDC_NOI,static,1342308352
Control19=IDC_NOO,static,1342308352

[DLG:IDD_INPUT_TABNAME]
Type=1
Class=CTabNameDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308353
Control4=IDC_TABNAME,edit,1350631552

[DLG:IDD_CHANGEDEPTH]
Type=1
Class=CDepthDlg
ControlCount=8
Control1=IDC_DEP,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308353
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_SDEP,edit,1342244864
Control8=IDC_EDEP,edit,1342244992

[MNU:CG_IDR_POPUP_RULER]
Type=1
Class=?
Command1=ID_DEPTH_SPLIT
Command2=ID_DEPTH_DELETE
Command3=ID_DEPTH_CHANGE
CommandCount=3

[MNU:IDR_EDITMENU]
Type=1
Class=?
Command1=ID_EDIT_SAVE
Command2=ID_EDIT_PRINT
Command3=ID_EDIT_EXIT
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_CLEAR
Command9=ID_EDIT_FIND
Command10=ID_EDIT_NEXT
Command11=ID_EDIT_REPLACE
Command12=ID_EDIT_SELECT_ALL
Command13=ID_EDIT_READONLY
Command14=ID_FONT_FONTSET
Command15=ID_EDITOR_HELP_INDEX
CommandCount=15

[MNU:IDR_PARAMETERMEDITMENU]
Type=1
Class=?
Command1=IDC_ADDTAB
Command2=IDC_DELTAB
Command3=IDC_CHTAB
Command4=IDC_SAVEPARA
Command5=IDC_SAVEDEFAULTPARA
Command6=IDC_SAVETAB
Command7=ID_EDIT
Command8=IDC_TEXTHELP
CommandCount=8

