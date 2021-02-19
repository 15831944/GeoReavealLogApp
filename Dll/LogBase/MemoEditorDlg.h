#if !defined(AFX_MEMOEDITORDLG_H__87CA416A_CDEA_4DC1_9B92_DB36943DBD20__INCLUDED_)
#define AFX_MEMOEDITORDLG_H__87CA416A_CDEA_4DC1_9B92_DB36943DBD20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MemoEditorDlg.h : header file
//
#include "resource.h"
#include "DialogResize.h"
/////////////////////////////////////////////////////////////////////////////
// CMemoEditorDlg dialog

class CMemoEditorDlg : public CDialogResize
{
// Construction
public:
	CMemoEditorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMemoEditorDlg)
	enum { IDD = IDD_MEMO_EDITOR };
	CString	m_strText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemoEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMemoEditorDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DECLARE_DLGRESIZE_MAP;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEMOEDITORDLG_H__87CA416A_CDEA_4DC1_9B92_DB36943DBD20__INCLUDED_)
