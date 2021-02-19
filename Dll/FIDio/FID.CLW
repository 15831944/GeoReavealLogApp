; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCreditDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "fid.h"
LastPage=0

ClassCount=5
Class1=CCreditDialog
Class2=CCreditStatic
Class3=CGuage
Class4=CStaticLink

ResourceCount=5
Resource1=FID_IDD_DISPLAY
Resource2=IDD_LOGATT_DLG
Class5=CCurveView
Resource3=FID_IDD_PROGRESS
Resource4=IDD_LOGVIEW
Resource5=IDD_MESGDLG

[CLS:CCreditDialog]
Type=0
BaseClass=CDialog
HeaderFile=CreditDialog.h
ImplementationFile=CreditDialog.cpp
LastObject=CCreditDialog
Filter=D
VirtualFilter=dWC

[CLS:CCreditStatic]
Type=0
BaseClass=CStatic
HeaderFile=CreditStatic.h
ImplementationFile=CreditStatic.cpp

[CLS:CGuage]
Type=0
BaseClass=CDialog
HeaderFile=Guage.h
ImplementationFile=Guage.cpp
LastObject=CG_IDC_CURDEP

[CLS:CStaticLink]
Type=0
BaseClass=CStatic
HeaderFile=StaticLink.h
ImplementationFile=StaticLink.cpp

[DLG:FID_IDD_PROGRESS]
Type=1
Class=CGuage
ControlCount=6
Control1=IDCANCEL,button,1342242817
Control2=FID_IDC_PROGDLG_PROGRESS,msctls_progress32,1342177281
Control3=FID_IDC_PROGDLG_PERCENT,static,1342308352
Control4=FID_IDC_STATIC,static,1342308352
Control5=FID_IDC_CURDEP,static,1342308352
Control6=FID_IDC_TTITLE,static,1342308352

[DLG:FID_IDD_DISPLAY]
Type=1
Class=CCreditDialog
ControlCount=9
Control1=FID_IDC_DISPLAY_STATIC,static,1342312448
Control2=FID_IDC_STATIC1,static,1342308352
Control3=FID_IDC_STATIC2,static,1342308352
Control4=FID_IDC_STATIC_LINK,static,1342308352
Control5=FID_IDC_STATIC,button,1342177287
Control6=FID_EDITOR,static,1342308352
Control7=IDC_STATIC,button,1342177287
Control8=IDCANCEL,button,1342242816
Control9=IDC_STATIC,static,1342177283

[DLG:IDD_LOGVIEW]
Type=1
Class=CCurveView
ControlCount=2
Control1=IDC_STYLE,button,1342177280
Control2=IDC_VALUE,static,1342308352

[CLS:CCurveView]
Type=0
HeaderFile=CurveView.h
ImplementationFile=CurveView.cpp
BaseClass=CDragDialog
Filter=W
LastObject=CCurveView
VirtualFilter=dWC

[DLG:IDD_LOGATT_DLG]
Type=1
Class=?
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_SDEP_EDIT,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDEP_EDIT,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_LEFT_EDIT,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_RIGHT_EDIT,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_LINEWIDTH_COMBO,combobox,1344340226
Control14=IDC_LINECOLOR_COMBO,combobox,1344340226
Control15=IDC_LINE_RADIO,button,1342308361
Control16=IDC_LOG_RADIO1,button,1342177289
Control17=IDC_STATIC,button,1342177287

[DLG:IDD_MESGDLG]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_MSG,static,1342308353
Control4=IDC_STATIC,button,1342177287

