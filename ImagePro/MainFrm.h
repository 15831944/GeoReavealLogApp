// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__F2844EFD_9F6C_48A0_A8A9_C99B3E1DAC2C__INCLUDED_)
#define AFX_MAINFRM_H__F2844EFD_9F6C_48A0_A8A9_C99B3E1DAC2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIFlatBar.h"
#include "UIMenuBar.h"
#include "SkinWin.h"

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	enum {DOCK_LEFT,DOCK_TOP,DOCK_RIGHT};
protected:	
	BOOL m_bDocked;			//是否停靠
	int m_iDockState;		//当前停靠位置
	BOOL m_bHided;			//是否隐藏
	CPoint m_curPos;		//当前窗口的左上角的坐标
	CRect m_curRect;		//窗口当前位置
	CSize m_sizeScreen;		//屏幕尺寸

// Operations
public:
	BOOL m_bStart;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CFlatToolBar    m_wndToolBar;
	CSkinWin		m_skinWin;
	CMenuBar		m_wndMenuBar;
	CWnd m_wndOwner;		//为在任务栏隐藏而假设的父窗口

//	void Show();			//显示
	void AdjustPosition();	//调整位置
	void Hide();			//隐藏
	void InitInter();		//初始化
	void GetSizeOfScreen();

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void Show();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F2844EFD_9F6C_48A0_A8A9_C99B3E1DAC2C__INCLUDED_)
