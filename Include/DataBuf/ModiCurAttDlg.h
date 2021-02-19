#if !defined(AFX_MODICURATTDLG_H_INCLUDED_)
#define AFX_MODICURATTDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModiCurAttDlg.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CModiCurAttDlg dialog

class CModiCurAttDlg : public CDialog
{
// Construction
public:
	CModiCurAttDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModiCurAttDlg)
	enum { IDD = IDD_CURVEATTRIB_DLG };
	CComboBox	m_ctlNewUnits;
	CString	m_NewCurve;
	CString	m_OldCurve;
	CString	m_OldUnits;
	CString	m_NewUnits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModiCurAttDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModiCurAttDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODICURATTDLG_H_INCLUDED_)
