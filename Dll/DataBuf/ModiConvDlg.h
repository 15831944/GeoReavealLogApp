#if !defined(AFX_MODICONVDLG_H_INCLUDED_)
#define AFX_MODICONVDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModiConvDlg.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CModiConvDlg dialog

class CModiConvDlg : public CDialog
{
// Construction
public:
	CModiConvDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModiConvDlg)
	enum { IDD = IDD_SETCONVINDEX_DLG };
	float	m_NewRlev;
	float	m_NewStart;
	float	m_NewStop;
	float	m_OldRlev;
	float	m_OldStart;
	float	m_OldStop;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModiConvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModiConvDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODICONVDLG_H_INCLUDED_)
