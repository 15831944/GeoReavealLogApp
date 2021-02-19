#if !defined(AFX_ATTRDLG_H__C1FADF81_36B6_11D6_AF8F_0000E890EF51__INCLUDED_)
#define AFX_ATTRDLG_H__C1FADF81_36B6_11D6_AF8F_0000E890EF51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttrDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CCurveAttrDlg dialog
class CCurveAttrDlg : public CDialog
{
// Construction
public:
	CCurveAttrDlg(CWnd* pParent = NULL);   // standard constructor
	int LineWidth,LineColor;

// Dialog Data
	//{{AFX_DATA(CCurveAttrDlg)
	enum { IDD = IDD_CURVE_ATTDLD };
	CComboBox	m_LineWidth;
	CComboBox	m_LineColor;
	float	m_Sdep;
	float	m_Edep;
	float	m_CurveMin;
	float	m_CurveMax;
	int		m_LineLog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveAttrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCurveAttrDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRDLG_H__C1FADF81_36B6_11D6_AF8F_0000E890EF51__INCLUDED_)
