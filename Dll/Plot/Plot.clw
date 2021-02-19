; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CPlotDepthDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "plot.h"
LastPage=0

ClassCount=14
Class1=CCSDialog
Class2=CCSPropertyPage
Class3=CCSPropertySheet
Class4=CPlotDepthDialog
Class5=CDepthScaleDialog
Class6=CFillListPage
Class7=CGridPage
Class8=CLineListPage
Class9=CLocalComboBox
Class10=CFontComboBox
Class11=CMinFontPage
Class12=CLinePage
Class13=CPatternPage
Class14=CInputTextDialog

ResourceCount=10
Resource1=IDD_MIN_FONT
Resource2=IDD_LINE
Resource3=IDD_INPUT_TEXT
Resource4=IDD_GRID
Resource5=IDD_FILL_LIST
Resource6=IDD_LINELIST
Resource7=IDD_PLOT_DEPTH
Resource8=IDD_FILL_PATTERN
Resource9=IDD_DEPTH_SCALE
Resource10=CG_IDR_POPUP_SELECT_DIR (English (U.S.))

[CLS:CCSDialog]
Type=0
BaseClass=CDialog
HeaderFile=CHICDIAL.H
ImplementationFile=CHICDIAL.CPP
LastObject=CCSDialog

[CLS:CCSPropertyPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=CHICDIAL.H
ImplementationFile=CHICDIAL.CPP

[CLS:CCSPropertySheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=CHICDIAL.H
ImplementationFile=CHICDIAL.CPP

[CLS:CPlotDepthDialog]
Type=0
BaseClass=CCSDialog
HeaderFile=DepthDialog.h
ImplementationFile=depthdialog.cpp
LastObject=IDC_PLOT_STDEP

[CLS:CDepthScaleDialog]
Type=0
BaseClass=CCSDialog
HeaderFile=DepthDialog.h
ImplementationFile=depthdialog.cpp
Filter=D
VirtualFilter=dWC

[CLS:CFillListPage]
Type=0
BaseClass=CCSPropertyPage
HeaderFile=FillListPage.h
ImplementationFile=FillListPage.cpp

[CLS:CGridPage]
Type=0
BaseClass=CCSPropertyPage
HeaderFile=griddialog.h
ImplementationFile=GridDialog.cpp
LastObject=IDC_VERT_OPAQUE

[CLS:CLineListPage]
Type=0
BaseClass=CCSPropertyPage
HeaderFile=LineListPage.h
ImplementationFile=LineListPage.cpp

[CLS:CLocalComboBox]
Type=0
BaseClass=CComboBox
HeaderFile=MinFontDialog.h
ImplementationFile=MinFontDialog.cpp

[CLS:CFontComboBox]
Type=0
BaseClass=CLocalComboBox
HeaderFile=MinFontDialog.h
ImplementationFile=MinFontDialog.cpp

[CLS:CMinFontPage]
Type=0
BaseClass=CCSPropertyPage
HeaderFile=MinFontDialog.h
ImplementationFile=MinFontDialog.cpp
LastObject=CMinFontPage

[CLS:CLinePage]
Type=0
BaseClass=CPropertyPage
HeaderFile=styledialog.h
ImplementationFile=StyleDialog.cpp

[CLS:CPatternPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=styledialog.h
ImplementationFile=StyleDialog.cpp

[CLS:CInputTextDialog]
Type=0
BaseClass=CCSDialog
HeaderFile=styledialog.h
ImplementationFile=StyleDialog.cpp
Filter=D
VirtualFilter=dWC

[DLG:IDD_PLOT_DEPTH]
Type=1
Class=CPlotDepthDialog
ControlCount=7
Control1=IDC_PLOT_STDEP,edit,1350631552
Control2=IDC_PLOT_ENDEP,edit,1350631552
Control3=IDCANCEL,button,1342242816
Control4=IDOK,button,1342242817
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308352

[DLG:IDD_DEPTH_SCALE]
Type=1
Class=CDepthScaleDialog
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_DEPTH_SCALE,edit,1350631552
Control4=IDC_STATIC,button,1342177287

[DLG:IDD_FILL_LIST]
Type=1
Class=CFillListPage
ControlCount=11
Control1=IDC_STATIC,static,1342308352
Control2=IDC_FILL_ITEM,combobox,1344339971
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_PATTERN_STYLE,combobox,1344340003
Control6=IDC_PATTERN_SYSTEM,button,1342373897
Control7=IDC_PATTERN_LTH,button,1342177289
Control8=IDC_STATIC,button,1342177287
Control9=IDC_PATTERN_PREVIEW,static,1342177286
Control10=IDC_STATIC,button,1342177287
Control11=IDC_PATTERN_COLOR,button,1342242827

[DLG:IDD_GRID]
Type=1
Class=CGridPage
ControlCount=21
Control1=IDC_STATIC,static,1342308352
Control2=IDC_GRID_SELECT_ITEM,combobox,1344339971
Control3=IDC_STATIC,static,1342308352
Control4=IDC_GRID_LINENUM,edit,1350633600
Control5=IDC_GRID_SPIN_LINENUM,msctls_updown32,1342177334
Control6=IDC_GRID_LINE_WIDTH,edit,1350633600
Control7=IDC_GRID_SPIN_LINEWIDTH,msctls_updown32,1342177334
Control8=IDC_GRID_VERLINE,button,1342242819
Control9=IDC_GRID_HORLINE,button,1342242819
Control10=IDC_GRID_LEFT_VALUE,edit,1350631552
Control11=IDC_GRID_RIGHT_VALUE,edit,1350631552
Control12=IDC_GRID_LINE_TYPE,button,1342406665
Control13=IDC_GRID_LOG_TYPE,button,1342210057
Control14=IDC_TRACK_GRID,button,1342177287
Control15=IDC_GRID_STATIC_TYPE,button,1342177287
Control16=IDC_VERT_OPAQUE,button,1342242819
Control17=IDC_HORI_OPAQUE,button,1342242819
Control18=IDC_STATIC,button,1342177287
Control19=IDC_STATIC,button,1342177287
Control20=IDC_STATIC,button,1342177287
Control21=IDC_GRID_LINE_COLOR,button,1342242827

[DLG:IDD_LINELIST]
Type=1
Class=CLineListPage
ControlCount=11
Control1=IDC_STATIC,static,1342308352
Control2=IDC_LINE_ITEM,combobox,1344339971
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_LINE_STYLE,combobox,1344340003
Control6=IDC_STATIC,static,1342308352
Control7=IDC_LINE_WIDTH,edit,1350633600
Control8=IDC_LINE_WIDTH_SPIN,msctls_updown32,1342177335
Control9=IDC_STATIC,button,1342177287
Control10=IDC_LINE_PREVIEW,static,1342177286
Control11=IDC_LINE_COLOR,button,1342242827

[DLG:IDD_MIN_FONT]
Type=1
Class=CMinFontPage
ControlCount=16
Control1=IDC_STATIC,static,1342308352
Control2=IDC_ITEM,combobox,1344339971
Control3=IDC_STATIC,static,1342308352
Control4=IDC_TEXT,edit,1350635648
Control5=IDC_STATIC,static,1342308352
Control6=IDC_FONT,combobox,1344340818
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_FONT_SIZE,combobox,1344339970
Control10=IDC_STATIC,static,1342308352
Control11=IDC_FONT_STYLE,combobox,1344342081
Control12=IDC_STATIC,button,1342177287
Control13=IDC_HSAMPRECT,static,1342177286
Control14=IDC_VSAMPRECT,static,1073741830
Control15=IDC_ROTATE,button,1342242819
Control16=IDC_FOREGROUND,button,1342242827

[DLG:IDD_LINE]
Type=1
Class=CLinePage
ControlCount=9
Control1=IDC_LINE_STYLE,combobox,1344340003
Control2=IDC_STATIC,button,1342177287
Control3=IDC_LINE_PREVIEW,static,1342177286
Control4=IDC_LINE_WIDTH,edit,1350633600
Control5=IDC_LINE_WIDTH_SPIN,msctls_updown32,1342177335
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,button,1342177287
Control9=IDC_LINE_COLOR,button,1342242827

[DLG:IDD_FILL_PATTERN]
Type=1
Class=CPatternPage
ControlCount=6
Control1=IDC_PATTERN_STYLE,combobox,1344340003
Control2=IDC_STATIC,button,1342177287
Control3=IDC_PATTERN_PREVIEW,static,1342177286
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,button,1342177287
Control6=IDC_PATTERN_COLOR,button,1342242827

[DLG:IDD_INPUT_TEXT]
Type=1
Class=CInputTextDialog
ControlCount=4
Control1=IDC_EDIT_TEXT,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,button,1342177287

[MNU:CG_IDR_POPUP_SELECT_DIR (English (U.S.))]
Type=1
Class=?
Command1=MN_LARGEICONS
Command2=MN_SMALLICONS
Command3=MN_LIST
Command4=MN_REPORT
CommandCount=4

