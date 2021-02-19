#ifndef _THICKCPDLG_H__
#define _THICKCPDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThickCpDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThickCpDlg dialog
#include "WndShadow.h"

class CThickCpDlg : public CDialog
{
// Construction
public:
	CThickCpDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CThickCpDlg)
	enum { IDD = IDD_THICKCPDLG };
	//}}AFX_DATA
	float m_HighValue,m_LowValue,m_BoxLength,m_TubeThick,m_NormalThick;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThickCpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CThickCpDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THICKCPDLG_H__67D294E5_F617_45DD_89B1_A2D611FC06D8__INCLUDED_)
