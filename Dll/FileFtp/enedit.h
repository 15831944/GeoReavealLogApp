#if !defined(AFX_ENEDIT_H__65D418F0_0107_431F_95B2_E31BF25FF286__INCLUDED_)
#define AFX_ENEDIT_H__65D418F0_0107_431F_95B2_E31BF25FF286__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// enedit.h : header file
//

#include "hottracker.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CEnEdit window

const int DEF_BTNWIDTH = ::GetSystemMetrics(SM_CXHTHUMB);
const UINT WM_EN_BTNCLICK = ::RegisterWindowMessage("WM_EN_BTNCLICK"); // wParam = CtrlID, lParam = BtnID

class CEnEdit : public CEdit
{
// Construction
public:
	CEnEdit(BOOL bComboStyle = TRUE);

	BOOL AddButton(UINT nID, LPCTSTR szCaption, LPCTSTR szTip, int nWidth = DEF_BTNWIDTH);
	BOOL InsertButton(int nPos, UINT nID, LPCTSTR szCaption, LPCTSTR szTip, int nWidth = DEF_BTNWIDTH);
	void EnableButton(UINT nID, BOOL bEnable = TRUE);
	void SetButtonTip(UINT nID, LPCTSTR szTip);
	CRect GetButtonRect(UINT nID);

// Attributes
protected:
	struct EDITBTN
	{
		UINT nID;
		CString sCaption;
		CString sTip;
		int nWidth;
		BOOL bEnabled;
	};

	CArray<EDITBTN, EDITBTN&> m_aButtons;
	BOOL m_bFirstShow;
	BOOL m_nButtonDown;
	CToolTipCtrl m_tooltip;
	BOOL m_bComboStyle;
	CHotTracker m_hotTrack;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEnEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnEdit)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg UINT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnHotChange(WPARAM /*wp*/, LPARAM /*lp*/);
	virtual void OnBtnClick(UINT nID) {}
	virtual void NcPaint(CDC* pDC, const CRect& rWindow);
	virtual void RecalcBtnRects();
	DECLARE_MESSAGE_MAP()

	CRect GetButtonRect(int nBtn) const; // screen coords
	void DrawButton(CDC* pDC, const CRect& rWindow, int nBtn, const CPoint& ptCursor) const;
	int ButtonHitTest(CPoint ptScreen) const;
	int ButtonHitTest(UINT nID) const;
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENEDIT_H__65D418F0_0107_431F_95B2_E31BF25FF286__INCLUDED_)
