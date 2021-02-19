// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_MAINFRM_H__3E78EEC5_0B01_4D30_906A_EB1C9C19E585__INCLUDED_)
#define AFX_MAINFRM_H__3E78EEC5_0B01_4D30_906A_EB1C9C19E585__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "UIFlatBar.h"
#include "UIMenuBar.h"
#include "SkinWin.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	int m_ScaleNo,m_ScaleNo0;
	int ScaleFactor[16];
	BOOL m_Fit;
// Operations
public:
	int CMainFrame::GetScaleFactor(int ScaleNo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CStatusBar      m_wndStatusBar;
	CSkinWin		m_skinWin;
	CMenuBar		m_wndMenuBar;
	CFlatToolBar    m_wndToolBar;
	CFlatToolBar    m_wndPlotBar;

// Generated message map functions
public:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnFitWindow();
	//}}AFX_MSG
	//User defined well_curve message 
	afx_msg LONG OnWellDataUpdate(UINT,LONG);
	afx_msg LONG OnCurveDataUpdate(UINT,LONG);
	afx_msg void OnScaleChange(UINT nCommandID);
	afx_msg void OnUpdateFitWindow(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__3E78EEC5_0B01_4D30_906A_EB1C9C19E585__INCLUDED_)
