#if !defined(AFX_BUSYMESSAGE_H__A871879D_EE3E_4781_A8D9_5D2C4A92C5AE__INCLUDED_)
#define AFX_BUSYMESSAGE_H__A871879D_EE3E_4781_A8D9_5D2C4A92C5AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BusyMessage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBusyMessage dialog
#include "KCBusyProgressCtrl.h"
#include "Resource.h"
class CBusyMessage : public CDialog
{
// Construction
public:
	void SetWaitMessage(CString MsgStr);
	CBusyMessage(CWnd* pParent = NULL);   // standard constructor
	~CBusyMessage();
// Dialog Data
	//{{AFX_DATA(CBusyMessage)
	enum { IDD = IDD_BUSYMESSAGE };
	CKCBusyProgressCtrl	m_Busy;
	CString	m_WaitMessage;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBusyMessage)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBusyMessage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUSYMESSAGE_H__A871879D_EE3E_4781_A8D9_5D2C4A92C5AE__INCLUDED_)
