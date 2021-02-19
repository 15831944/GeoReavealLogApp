; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=2
ResourceCount=5
NewFileInclude1=#include "stdafx.h"
Resource1=IDD_ADDFIELD
Resource2=IDD_MEMO_EDITOR
Resource3=IDD_DIALOG_SETFILTER
Class1=CGressDlg
LastClass=CProgressDlg
LastTemplate=CDialog
Class2=CProgressDlg
Resource4=IDD_STRUCTURE
Resource5=IDD_PROGRESS

[DLG:IDD_ADDFIELD]
Type=1
Class=?
ControlCount=10
Control1=IDC_NAME,edit,1350631432
Control2=IDC_LENGTH,edit,1350639616
Control3=IDC_DECIMALS,edit,1350639616
Control4=IDC_TYPE,combobox,1344339971
Control5=IDOK,button,1342243585
Control6=IDCANCEL,button,1342243584
Control7=IDC_STATIC,static,1342177280
Control8=IDC_LENGTH_STATIC,static,1342177280
Control9=IDC_DECIMALS_STATIC,static,1342177280
Control10=IDC_STATIC,static,1342308352

[DLG:IDD_STRUCTURE]
Type=1
Class=?
ControlCount=6
Control1=IDC_FIELDS,SysListView32,1350598669
Control2=IDC_ADD,button,1342243584
Control3=IDC_EDIT,button,1342243584
Control4=IDC_DELETE,button,1342243584
Control5=IDOK,button,1342243585
Control6=IDCANCEL,button,1342243584

[DLG:IDD_DIALOG_SETFILTER]
Type=1
Class=?
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_EDIT1,edit,1350631552
Control5=IDC_STATIC,button,1342177287
Control6=IDC_COMBO1,combobox,1344340226

[DLG:IDD_MEMO_EDITOR]
Type=1
Class=?
ControlCount=3
Control1=IDC_TEXT,edit,1353781444
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816

[CLS:CGressDlg]
Type=0
HeaderFile=GressDlg.h
ImplementationFile=GressDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_MESSAGE
VirtualFilter=dWC

[CLS:CProgressDlg]
Type=0
HeaderFile=ProgDlg.h
ImplementationFile=ProgDlg.cpp
BaseClass=CDialog
LastObject=CG_IDC_PROGDLG_PERCENT

[DLG:IDD_PROGRESS]
Type=1
Class=CProgressDlg
ControlCount=2
Control1=CG_IDC_PROGDLG_PROGRESS,msctls_progress32,1342177280
Control2=CG_IDC_PROGDLG_PERCENT,static,1342308352

