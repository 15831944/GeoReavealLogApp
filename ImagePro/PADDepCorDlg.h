#if !defined(AFX_PADDEPCORDLG_H__2CEFCF30_B5FD_423D_AAC6_CABE52438AFE__INCLUDED_)
#define AFX_PADDEPCORDLG_H__2CEFCF30_B5FD_423D_AAC6_CABE52438AFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PADDepCorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPADDepCorDlg dialog
#include "ListEditCtrl.h"
#include "PadCorrect.h"
#include "WndShadow.h"

class CPADDepCorDlg : public CDialog
{
// Construction
public:
	CPADDepCorDlg(CPadCorrect *pObj,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPADDepCorDlg)
	enum { IDD = IDD_PADDEPCORDLG };
	CEdit	m_Edit;
	CListEditCtrl	m_ParaListCtrl;
	CString	m_Message;
	BOOL	m_bDipDepPro;
	BOOL	m_bRADSDepPro;
	BOOL	m_bACDepPro;
	//}}AFX_DATA
	float	m_MinSemblance;
	CPadCorrect *m_pObj;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPADDepCorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CPADDepCorDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PADDEPCORDLG_H__2CEFCF30_B5FD_423D_AAC6_CABE52438AFE__INCLUDED_)
