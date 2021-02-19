#if !defined(AFX_SERVERDLG_H__104A903D_A221_47B3_B6CC_52755637D9B0__INCLUDED_)
#define AFX_SERVERDLG_H__104A903D_A221_47B3_B6CC_52755637D9B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerDlg.h : header file
//

#include "runtimedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CServerDlg dialog

enum AL_TYPE
{
	ANONLOGIN_HIDE	= -2,
	ANONLOGIN_AUTO	= -1,
	ANONLOGIN_NO	= 0,
	ANONLOGIN_YES	= 1,
};

class CServerDlg : public CRuntimeDlg
{
// Construction
public:
	CServerDlg(LPCTSTR szServer = NULL, LPCTSTR szUsername = NULL, 
				LPCTSTR szPassword = NULL, AL_TYPE nAnonymousLogin = ANONLOGIN_AUTO);   // standard constructor

	CString GetServer() { return m_sServer; }
	CString GetUsername() { return m_sUsername; }
	CString GetPassword() { return m_sPassword; }
	BOOL GetAnonynousLogin() { return (m_nAnonLogin == ANONLOGIN_YES); }

	int DoModal() { return CRuntimeDlg::DoModal("·þÎñÆ÷"); }

protected:
// Dialog Data
	//{{AFX_DATA(CServerDlg)
	CString	m_sServer;
	CString	m_sUsername;
	CString	m_sPassword;
	//}}AFX_DATA
	CComboBox m_cbServers;
	CComboBox m_cbUsernames;
	AL_TYPE m_nAnonLogin;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CServerDlg)
	afx_msg void OnChangeServer();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnAnonLogin();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERDLG_H__104A903D_A221_47B3_B6CC_52755637D9B0__INCLUDED_)
