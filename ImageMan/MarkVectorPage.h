// MarkVectorPage.h : header file
//

#ifndef __MARKVECTORPAGE_H__
#define __MARKVECTORPAGE_H__

class CMarkVectorObj;

// CMarkVectorPage dialog

class CMarkVectorPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMarkVectorPage)

// Construction
public:
	CMarkVectorPage(CMarkVectorObj* VectorObj);
	~CMarkVectorPage();

// Dialog Data
	//{{AFX_DATA(CMarkVectorPage)
	enum { IDD = IDD_MARKVECTOR_PAGE };
	UINT	m_CircleRadius;
	UINT	m_TailLength;
	UINT	m_TailWidth;
	UINT	m_CircleWidth;
	UINT	m_DepLevel;
	int     m_HeadStyle;
	BOOL	m_bPrintOriginPara;
	BOOL	m_bPrintTruePara;
	CString	m_ParaFormat;
	BOOL	m_bPrintTrueDip;
	BOOL	m_bPrintOriginDip;
	//}}AFX_DATA

// Attributes	
	CMarkVectorObj* pObj;
	CCJColorPicker m_CircleColor;
	CCJColorPicker m_TailColor;

// Operations
	void Preview();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMarkVectorPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMarkVectorPage)
	afx_msg void OnChangeCircleWidth();
	afx_msg void OnChangeTailLength();
	afx_msg void OnChangeTailWidth();
	afx_msg void OnChangeCircleRadius();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio2();
	afx_msg void OnHeadStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __MarkVectorPage_H__
