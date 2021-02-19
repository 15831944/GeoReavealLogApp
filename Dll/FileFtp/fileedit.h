#if !defined(AFX_FILEEDIT_H__7A50F411_3AAE_4BC2_989A_53D44291B643__INCLUDED_)
#define AFX_FILEEDIT_H__7A50F411_3AAE_4BC2_989A_53D44291B643__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// fileedit.h : header file
//

#include "enedit.h"
#include "sysimagelist.h"

/////////////////////////////////////////////////////////////////////////////
// CFileEdit window

enum
{
	FES_NOBROWSE		= 0x0001,
	FES_FOLDERS			= 0x0002,
	FES_COMBOSTYLEBTN	= 0x0004, // draws the browse button like a combo box rather than a button
	FES_GOBUTTON		= 0x0008,
};

class CFileEdit : public CEnEdit
{
// Construction
public:
	CFileEdit(int nStyle = 0, LPCTSTR szFilter = "All Files (*.*)|*.*||");

	void EnableStyle(int nStyle, BOOL bEnable = TRUE);
	void SetFilter(LPCTSTR szFilter) { m_sFilter = szFilter; }

// Attributes
protected:
	BOOL m_bTipNeeded;
	CSysImageList m_ilSys;
	CString m_sFilter;
	int m_nStyle;
	
	const UINT ICON_WIDTH;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileEdit)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileEdit)
	afx_msg void OnPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnChange();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnSetReadOnly(WPARAM /*wp*/, LPARAM /*lp*/);
	afx_msg LRESULT OnSetText(WPARAM /*wp*/, LPARAM /*lp*/);
	afx_msg void OnNeedTooltip(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnBtnClick(UINT nID);
	virtual void NcPaint(CDC* pDC, const CRect& rWindow);
	virtual void RecalcBtnRects();
	DECLARE_MESSAGE_MAP()

protected:
	CRect GetIconRect() const; // screen coords
	inline BOOL HasStyle(int nStyle) const { return (m_nStyle & nStyle); }
	void DrawFileIcon(CDC* pDC, const CRect& rWindow);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEEDIT_H__7A50F411_3AAE_4BC2_989A_53D44291B643__INCLUDED_)
