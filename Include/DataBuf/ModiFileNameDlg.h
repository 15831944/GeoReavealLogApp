#if !defined(AFX_MODIFILENAMEDLG_H_INCLUDED_)
#define AFX_MODIFILENAMEDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModiFileNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModiFileNameDlg dialog
#include "Resource.h"
class CModiFileNameDlg : public CDialog
{
// Construction
public:
	CModiFileNameDlg(char* Editer,CWnd* pParent = NULL);   // standard constructor
	void CheckExist();
	bool bExist;
//	char* m_Editer;
// Dialog Data
	//{{AFX_DATA(CModiFileNameDlg)
	enum { IDD = IDD_NEWFILENAME_DLG };
	CEdit	m_Edit;
	CEdit	m_ctlStatus;
	CString	m_ManualFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModiFileNameDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModiFileNameDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeManualfilenameEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFILENAMEDLG_H_INCLUDED_)
