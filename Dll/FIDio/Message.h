#if !defined(AFX_MESSAGE_H__30D00029_218B_4140_80C1_ED5C6F03B59D__INCLUDED_)
#define AFX_MESSAGE_H__30D00029_218B_4140_80C1_ED5C6F03B59D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Message.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMessage dialog
#include "FIDRes.h"
#include "WndShadow.h"
class CMessage : public CDialog
{
// Construction
public:
	CMessage(BOOL bOK,BOOL bCancel,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMessage)
	enum { IDD = IDD_MESGDLG };
	CString	m_szMessage;
	//}}AFX_DATA

	BOOL m_bOK, m_bCancel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CMessage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGE_H__30D00029_218B_4140_80C1_ED5C6F03B59D__INCLUDED_)
