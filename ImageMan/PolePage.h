#if !defined(AFX_POLEPAGE_H__1E4DF59A_6089_485F_8E73_91B633DA6ECE__INCLUDED_)
#define AFX_POLEPAGE_H__1E4DF59A_6089_485F_8E73_91B633DA6ECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PolePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPolePage dialog
#include "GridCtrl.h"
class CPolePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPolePage)

// Construction
public:
	CPolePage();
	~CPolePage();

// Dialog Data
	//{{AFX_DATA(CPolePage)
	enum { IDD = IDD_POLE_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	CString m_FileName;
	CGridCtrl m_Grid;int m_DrawType;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPolePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	float m_LookAngle;
	int   m_StartPos;
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPolePage)
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

#endif // !defined(AFX_POLEPAGE_H__1E4DF59A_6089_485F_8E73_91B633DA6ECE__INCLUDED_)
