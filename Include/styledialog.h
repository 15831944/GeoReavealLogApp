
#ifndef __STYLEDIALOG_H__
#define __STYLEDIALOG_H__

#define LINEGRID 0
#define LOGGRID 1 

#include "PlotRes.h"
#include "chicdial.h"
/////////////////////////////////////////////////////////////////////////////
// CLineDialog dialog

class CLinePage : public CPropertyPage
{
// Construction
public:
	CLinePage(LOGPEN* pLogPen = NULL);	// standard constructor
	CString m_szObjName;
protected:
	CCJColorPicker m_Color;
	CLineStyleBox m_StyleBox;
	LOGPEN* m_pLogPen;
	BOOL m_bWindowsPen;
// Dialog Data
	//{{AFX_DATA(CLinePage)
	enum { IDD = IDD_LINE };
	int		m_Width;
	int		m_Style;
	CSpinButtonCtrl	m_WidthSpinCtrl;
	//}}AFX_DATA

public:
	void SetLineIndirect(LOGPEN *m_LogPen);
	void GetLineIndirect(LOGPEN *m_LogPen);
	void SetPenType(BOOL bPenType);

protected:
	void Preview();
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLinePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnChangeWidth();
	afx_msg void OnSelchangeStyle();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPatternDialog dialog

class CPatternPage : public CPropertyPage
{
// Construction
public:
	CPatternPage(LOGBRUSH* pLogBrush = NULL);	// standard constructor

protected:
	CCJColorPicker m_Color;
	CFillPatternBox m_StyleBox;
	LOGBRUSH* m_pLogBrush;
	BOOL m_bWindowsPattern;
// Dialog Data
	//{{AFX_DATA(CPatternPage)
	enum { IDD = IDD_FILL_PATTERN };
	int		m_Style;
	//}}AFX_DATA

public:
	void SetPatternIndirect(LOGBRUSH *m_LogPen);
	void GetPatternIndirect(LOGBRUSH *m_LogPen);  
	void SetPatternType(BOOL bPatternType);
protected:
	void Preview();

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPatternPage)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CInputTextDialog dialog
class AFX_EXT_CLASS CInputTextDialog : public CCSDialog
{
// Construction
public:
	CInputTextDialog(CString msg,CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInputTextDialog)
	enum { IDD = IDD_INPUT_TEXT };
	CString	m_Text;
	CString	m_StaticText;
	//}}AFX_DATA

// opreation
	void SetText(CString text);
	CString GetText();
// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD* GetHelpIDs() {return m_nHelpIDs;}
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInputTextDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__STYLEDIA_H__
