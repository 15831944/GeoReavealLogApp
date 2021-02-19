// CJTabCtrlBar.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(CMYTABCTRLBAR_H_INCLUDED)
#define CMYTABCTRLBAR_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "CMYControlBar.h"

typedef struct
{
	CWnd *pWnd;
	char szLabel[32];
}TCB_ITEM;

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrlBar tab control bar

class AFX_EXT_CLASS CMyTabCtrlBar : public CMyControlBar
{
// Construction
public:
	CMyTabCtrlBar();

// Attributes
public:
	CTabCtrl m_tabctrl;
	CToolTipCtrl* m_ToolTip;

protected:
	int m_nActiveTab;
	CView* m_pActiveView;
	CList <TCB_ITEM *,TCB_ITEM *> m_views;

// Operations
public:
	void SetActiveView(int nNewTab);
	void SetActiveView(CRuntimeClass *pViewClass);
	CImageList* SetTabImageList(CImageList *pImageList);
	BOOL ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags);

// Overrides
public:
    // ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTabCtrlBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	CView* GetActiveView();
	CView* GetView(int nView);
	CView* GetView(CRuntimeClass *pViewClass);
	BOOL AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CCreateContext *pContext = NULL);
	void RemoveView(int nView);
	virtual ~CMyTabCtrlBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CMyTabCtrlBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // CJTABCTRLBAR_H_INCLUDED
