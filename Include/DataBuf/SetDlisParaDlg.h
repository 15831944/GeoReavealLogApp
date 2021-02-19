#if !defined(AFX_SetDlisParaDlg_H_INCLUDED_)
#define AFX_SetDlisParaDlg_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetDlisParaDlg.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CSetDlisParaDlg dialog

class CSetDlisParaDlg : public CDialog
{
// Construction
public:
	CSetDlisParaDlg(CWnd* pParent = NULL);   // standard constructor
	bool m_bAllIgnore; //忽略对所有文件的参数修改
// Dialog Data
	//{{AFX_DATA(CSetDlisParaDlg)
	enum { IDD = IDD_SETDLISPARA_DLG };
	CString	m_Company;
	float	m_DefaultVal;
	CString	m_Field;
	CString	m_Well;
	CString	m_FileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetDlisParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetDlisParaDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnAllignoreBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SetDlisParaDlg_H_INCLUDED_)
