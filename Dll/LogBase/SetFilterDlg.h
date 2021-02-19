#if !defined(AFX_SETFILTERDLG_H__F286E97E_2733_4F0E_ABDC_36ADA3A1F560__INCLUDED_)
#define AFX_SETFILTERDLG_H__F286E97E_2733_4F0E_ABDC_36ADA3A1F560__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetFilterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetFilterDlg dialog
#include "resource.h"

class CSetFilterDlg : public CDialog
{
// Construction
public:
	CSetFilterDlg(CWnd* pParent = NULL);   // standard constructor

	CStringArray m_strFieldArray;
// Dialog Data
	//{{AFX_DATA(CSetFilterDlg)
	enum { IDD = IDD_DIALOG_SETFILTER };
	CString	m_FilterCriteria;
	CString	m_FilterFieldName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetFilterDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETFILTERDLG_H__F286E97E_2733_4F0E_ABDC_36ADA3A1F560__INCLUDED_)
