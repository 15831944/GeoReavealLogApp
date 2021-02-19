#if !defined(AFX_MODIFYHANDWORKDLG_H__4B62016D_5723_44FA_9366_E00A9866998D__INCLUDED_)
#define AFX_MODIFYHANDWORKDLG_H__4B62016D_5723_44FA_9366_E00A9866998D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyHandWorkDlg.h : header file
//
#include "WndShadow.h"
/////////////////////////////////////////////////////////////////////////////
// CModifyHandWorkDlg dialog

class CModifyHandWorkDlg : public CDialog
{
// Construction
public:
	CModifyHandWorkDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModifyHandWorkDlg)
	enum { IDD = IDD_MODIFY_HANDWORK_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int m_TypeSin;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyHandWorkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CModifyHandWorkDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYHANDWORKDLG_H__4B62016D_5723_44FA_9366_E00A9866998D__INCLUDED_)
