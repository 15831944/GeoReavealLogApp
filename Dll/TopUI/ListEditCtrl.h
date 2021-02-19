#if !defined(AFX_LISTEDITCTRL_H__1BA51A6B_EC80_4CBB_9A3D_E9E235489320__INCLUDED_)
#define AFX_LISTEDITCTRL_H__1BA51A6B_EC80_4CBB_9A3D_E9E235489320__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListEditCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListEditCtrl window

class  AFX_EXT_CLASS CListEditCtrl : public CListCtrl
{
// Construction
public:
	CListEditCtrl();
	void InitCtrl(CWnd* pEdit);

// Attributes
public:
	CWnd*	m_Edit;
	int		m_StartCol;
	int     m_EndCol;
	int     m_SelCol;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListEditCtrl();
	virtual void PositionEdit();
	virtual int HitTest( CPoint point, UINT* pFlags = NULL );

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditCtrl)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTEDITCTRL_H__1BA51A6B_EC80_4CBB_9A3D_E9E235489320__INCLUDED_)
