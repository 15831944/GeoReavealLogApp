#if !defined(AFX_MARKDIPCORRE_H__EAC1E400_2E40_4CF8_8414_7290FEB3DF5F__INCLUDED_)
#define AFX_MARKDIPCORRE_H__EAC1E400_2E40_4CF8_8414_7290FEB3DF5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MarkDipCorre.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMarkDipCorre dialog
#include "BrowseCtrl.h"

class CMarkDipCorre : public CDialog
{
// Construction
public:
	CMarkDipCorre(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMarkDipCorre)
	enum { IDD = IDD_MARK_CORREPARA };
	CComboBox	m_SeriesCtrl;
	CString	m_sAZ1Name;
	CString	m_sSeries;
	CString	m_sRBName;
	CString	m_sDEVName;
	CString	m_sDAZName;
	float	m_fDip1;
	float	m_fDir1;
	//}}AFX_DATA

	CString m_WellName;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMarkDipCorre)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	
	CBrowseCtrl	m_wndBrowseCtrl;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMarkDipCorre)
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupComboSeries();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MARKDIPCORRE_H__EAC1E400_2E40_4CF8_8414_7290FEB3DF5F__INCLUDED_)
