#if !defined(AFX_CREDITDIALOG_H__97882521_4ABC_11D1_9E3C_00A0245800CF__INCLUDED_)
#define AFX_CREDITDIALOG_H__97882521_4ABC_11D1_9E3C_00A0245800CF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "FIDres.h"
#include "Clouds.h"
#include "StaticLink.h"
/////////////////////////////////////////////////////////////////////////////
// CCreditDialog dialog
#include "WndShadow.h"
class CCreditDialog : public CDialog
{
// Construction
public:
	BOOL m_Small;
	CCreditDialog(char* Editer,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreditDialog)
	enum { IDD = FID_IDD_DISPLAY };	
	CString	m_SoftName;
	//}}AFX_DATA
	char*	m_Editer;
	CStaticLink	m_MailLink;
	CStaticLink	m_HomePageLink;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreditDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	CCloudsCtrl m_static;
	// Generated message map functions
	//{{AFX_MSG(CCreditDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREDITDIALOG_H__97882521_4ABC_11D1_9E3C_00A0245800CF__INCLUDED_)
