// DipDialog.h : header file
//

#ifndef __DEVDAZPAGE_H__
#define __DEVDAZPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CDevDazPage dialog

class CDevDazPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDevDazPage)

// Construction
public:
	CDevDazPage(CDevDazObj*);
	~CDevDazPage();

// Dialog Data
	//{{AFX_DATA(CDevDazPage)
	enum { IDD = IDD_DEVDAZ };
	UINT	m_CircleRadius;
	UINT	m_TailLength;
	UINT	m_TailWidth;
	UINT	m_CircleWidth;
	float	m_DepLevel;
	float	m_LeftVal;
	float	m_RightVal;
	//}}AFX_DATA

// Attributes	
	CDevDazObj* pObj;
	CCJColorPicker m_CircleColor;
	CCJColorPicker m_TailColor;

// Operations
	void Preview();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDevDazPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDevDazPage)
	afx_msg void OnChangeCircleWidth();
	afx_msg void OnChangeTailLength();
	afx_msg void OnChangeTailWidth();
	afx_msg void OnChangeCircleRadius();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif  //__DEVDAZPAGE_H__
