#if !defined(AFX_ADDFIELDDLG_H__EA54C4E2_25BF_49E7_82CE_34A810950D74__INCLUDED_)
#define AFX_ADDFIELDDLG_H__EA54C4E2_25BF_49E7_82CE_34A810950D74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddFieldDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CAddFieldDlg dialog

class CAddFieldDlg : public CDialog
{
// Construction
public:
	CString m_strTitle;
	CAddFieldDlg(CWnd* pParent = NULL);   // standard constructor
	int	m_nType;

// Dialog Data
	//{{AFX_DATA(CAddFieldDlg)
	enum { IDD = IDD_ADDFIELD };
	CComboBox	m_cmbType;
	int		m_nDecimals;
	int		m_nLength;
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddFieldDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddFieldDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeName();
	afx_msg void OnSelchangeType();
	//}}AFX_MSG
	void OnUpdateLength(CCmdUI* pCmdUI);
	void OnUpdateDecimals(CCmdUI* pCmdUI);
	void OnUpdateOK(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDFIELDDLG_H__EA54C4E2_25BF_49E7_82CE_34A810950D74__INCLUDED_)
