#if !defined(AFX_PROITEM_H__217753BC_41B1_4419_8AE3_B3FD0AB34F78__INCLUDED_)
#define AFX_PROITEM_H__217753BC_41B1_4419_8AE3_B3FD0AB34F78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProItem dialog
#include "WndShadow.h"

class CProItem : public CDialog
{
// Construction
public:
	CProItem(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProItem)
	enum { IDD = IDD_PROJECT_DLG };
	CListCtrl	m_ItemListCtrl;
	//}}AFX_DATA

	CImageList	ListIcon;
	CString m_ProItemName;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CProItem)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROITEM_H__217753BC_41B1_4419_8AE3_B3FD0AB34F78__INCLUDED_)
