; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDlisCurDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "databuf.h"
LastPage=0

ClassCount=7
Class1=CDlisCurDlg
Class2=CModiConvDlg
Class3=CModiCurAttDlg
Class4=CModiFileNameDlg
Class5=CProgressDlg
Class6=CReCurNamDlg
Class7=CSetDlisParaDlg

ResourceCount=8
Resource1=IDD_NEWFILENAME_DLG
Resource2=IDD_CURVEATTRIB_DLG
Resource3=IDD_SETCONVINDEX_DLG
Resource4=IDD_SETDLISPARA_DLG
Resource5=CG_IDD_PROGRESS
Resource6=IDD_EDITDLISCURVES_DLG
Resource7=IDD_MODICURVE_DIALOG
Resource8=IDR_CURVENAME_POPUP

[CLS:CDlisCurDlg]
Type=0
BaseClass=CDialog
HeaderFile=DlisCurDlg.h
ImplementationFile=DlisCurDlg.cpp
LastObject=IDC_MAXLOGNUM_EDIT

[CLS:CModiConvDlg]
Type=0
BaseClass=CDialog
HeaderFile=ModiConvDlg.h
ImplementationFile=ModiConvDlg.cpp

[CLS:CModiCurAttDlg]
Type=0
BaseClass=CDialog
HeaderFile=ModiCurAttDlg.h
ImplementationFile=ModiCurAttDlg.cpp

[CLS:CModiFileNameDlg]
Type=0
BaseClass=CDialog
HeaderFile=ModiFileNameDlg.h
ImplementationFile=ModiFileNameDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_MANUALFILENAME_EDIT

[CLS:CProgressDlg]
Type=0
BaseClass=CDialog
HeaderFile=ProgDlg.h
ImplementationFile=ProgDlg.cpp

[CLS:CReCurNamDlg]
Type=0
BaseClass=CDialog
HeaderFile=ReCurNamDlg.h
ImplementationFile=ReCurNamDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_CURVE_STATUS_EDIT

[CLS:CSetDlisParaDlg]
Type=0
BaseClass=CDialog
HeaderFile=SetDlisParaDlg.h
ImplementationFile=SetDlisParaDlg.cpp
LastObject=CSetDlisParaDlg

[DLG:IDD_SETCONVINDEX_DLG]
Type=1
Class=CModiConvDlg
ControlCount=15
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_OLDSTART_EDIT,edit,1350633600
Control4=IDC_STATIC,static,1342308352
Control5=IDC_OLDSTOP_EDIT,edit,1350633600
Control6=IDC_STATIC,static,1342308352
Control7=IDC_OLDRLEV_EDIT,edit,1350633600
Control8=IDC_STATIC,button,1342177287
Control9=IDC_STATIC,static,1342308352
Control10=IDC_NEWSTART_EDIT,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_NEWSTOP_EDIT,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_NEWRLEV_EDIT,edit,1350631552
Control15=IDC_STATIC,button,1342177287

[DLG:IDD_CURVEATTRIB_DLG]
Type=1
Class=CModiCurAttDlg
ControlCount=9
Control1=IDC_NEWCURVE_EDIT,edit,1350631560
Control2=IDOK,button,1342242817
Control3=IDC_OLDCURVE_EDIT,edit,1350633600
Control4=IDC_OLDUNITS_EDIT,edit,1350633600
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_NEWUNITS_COMBO,combobox,1344339970

[DLG:IDD_NEWFILENAME_DLG]
Type=1
Class=CModiFileNameDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_MANUALFILENAME_EDIT,edit,1350631620
Control3=IDC_STATUS_EDIT,edit,1342244992

[DLG:CG_IDD_PROGRESS]
Type=1
Class=CProgressDlg
ControlCount=3
Control1=IDCANCEL,button,1342242817
Control2=CG_IDC_PROGDLG_PROGRESS,msctls_progress32,1342177280
Control3=CG_IDC_PROGDLG_PERCENT,static,1342308353

[DLG:IDD_MODICURVE_DIALOG]
Type=1
Class=CReCurNamDlg
ControlCount=19
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_MAXLEN_EDIT,edit,1350641792
Control4=IDC_CURVE_NAME_LIST,SysListView32,1350631937
Control5=IDC_DELETE_BTN,button,1342242816
Control6=IDC_STATIC,button,1342177287
Control7=IDC_MAXLOG_EDIT,edit,1350641792
Control8=IDC_STATIC,button,1342177287
Control9=IDC_OLDSTR_EDIT,edit,1350631552
Control10=IDC_NEWSTR_EDIT,edit,1350631552
Control11=IDC_MODIFYSTR_BTN,button,1342242816
Control12=IDC_STATIC,static,1342308352
Control13=IDC_NUMLOG_EDIT,edit,1350641792
Control14=IDC_STATIC,static,1342308352
Control15=IDC_GETLEFT_BTN,button,1342242816
Control16=IDC_GETRIGHT_BTN,button,1342242816
Control17=IDCANCEL,button,1342242816
Control18=IDC_CLEARERR_BTN,button,1342242816
Control19=IDC_STATIC,static,1342308352

[DLG:IDD_SETDLISPARA_DLG]
Type=1
Class=CSetDlisParaDlg
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_COMPANY_EDIT,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_WELL_EDIT,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_FIELD_EDIT,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_DEFAULTVAL_EDIT,edit,1350631552
Control10=IDC_STATIC,static,1342308352
Control11=IDC_FILENAME_EDIT,edit,1350633600
Control12=IDC_ALLIGNORE_BTN,button,1342242816

[MNU:IDR_CURVENAME_POPUP]
Type=1
Class=?
Command1=ID_CURVENAME_DELSELECT
Command2=ID_CURVENAME_ADDFRAMENO
Command3=ID_CURVENAME_ADDORIGIN
Command4=ID_CURVENAME_ADDCOPYNUM
Command5=ID_CURVENAME_DELBAD
Command6=ID_CURVENAME_DELSAME
Command7=ID_CURVENAME_DELTOOLEN
Command8=ID_CURVENAME_CLEARERROR
CommandCount=8

[DLG:IDD_EDITDLISCURVES_DLG]
Type=1
Class=CDlisCurDlg
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_DLISCURVES_LIST,SysListView32,1350631937
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_MAXLOGNUM_EDIT,edit,1350633600
Control7=IDC_NUMLOG_EDIT,edit,1350633600
Control8=IDC_CURVES_STATUS_EDIT,edit,1342244992

