// DipPage.h : header file
//

#ifndef __DIPPAGE_H__
#define __DIPPAGE_H__


// CDipPage dialog

class CDipPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDipPage)

// Construction
public:
	CDipPage(CDangDdirObj* pDangDdirObj);
	~CDipPage();

// Dialog Data
	//{{AFX_DATA(CDipPage)
	enum { IDD = IDD_DIP_PAGE };
	UINT	m_CircleRadius;
	UINT	m_TailLength;
	UINT	m_TailWidth;
	UINT	m_CircleWidth;
	UINT	m_DepLevel;
	int     m_HeadStyle;
	BOOL	m_PrintPara;
	CString	m_ParaFormat;
	//}}AFX_DATA

// Attributes	
	CDangDdirObj* pObj;
	CCJColorPicker m_CircleColor;
	CCJColorPicker m_TailColor;

// Operations
	void Preview();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDipPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDipPage)
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

#endif // __DIPPAGE_H__
