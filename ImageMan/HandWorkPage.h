#if !defined(AFX_HANDWORKPAGE_H__1E4DF59A_6089_485F_8E73_91B633DA6ECE__INCLUDED_)
#define AFX_HANDWORKPAGE_H__1E4DF59A_6089_485F_8E73_91B633DA6ECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HandWorkPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHandWorkPage dialog
#include "GridCtrl.h"
class CHandWorkPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CHandWorkPage)

// Construction
public:
	CHandWorkPage();
	~CHandWorkPage();

// Dialog Data
	//{{AFX_DATA(CHandWorkPage)
	enum { IDD = IDD_HANDWORK_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	CString m_FileName,m_PlotContent;
	CGridCtrl m_Grid;
	BOOL m_Check1,m_Check6,m_Check7,m_Check8,m_Check9,m_Check10,m_Check11;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHandWorkPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHandWorkPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnArrang();
	afx_msg void OnInsertrow();
	afx_msg void OnDelrow();
	//}}AFX_MSG
	afx_msg void OnBrowse();
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HandWorkPAGE_H__1E4DF59A_6089_485F_8E73_91B633DA6ECE__INCLUDED_)
