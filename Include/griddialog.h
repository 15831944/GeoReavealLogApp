#ifndef __GRIDDIALOG_H__
#define __GRIDDIALOG_H__

/////////////////////////////////////////////////////////////////////////////
// CGridPage dialog

#include "Style.h"
#include "grid.h"
#include "plotres.h"
#include "chicdial.h"

/////////////////////////////////////////////////////////////////////////////
// CGridPage dialog

class CGridPage : public CCSPropertyPage
{
// Construction
public:
	CGridPage(CGridObj* pGridObj = NULL);	// standard constructor
	CGridObj* pObj;
	CCJColorPicker m_LineColor;
	COLORREF m_Color[7];
	int m_Width[7],m_Num[7];
	int lineno;
	CRect rect; 
	CPoint c_down;
	int* m_ZerX;
	int m_VerLine;

// Dialog Data
	//{{AFX_DATA(CGridPage)
	enum { IDD = IDD_GRID };
	CSpinButtonCtrl	m_LineNum_SpinCtrl;
	CSpinButtonCtrl	m_LineWidth_SpinCtrl;
	CEdit m_LineNumCtrl;
	CEdit m_LineWidthCtrl;
	CEdit m_LeftValueCtrl;
	CEdit m_RightValueCtrl;
	UINT	m_LineNum;
	int		m_SelLineNo;
	UINT	m_LineWidth;
	BOOL	m_bDrawHorLine;
	BOOL	m_bDrawVerLine;
	double	m_LeftValue;
	double	m_RightValue;
	int		m_GridType;
	BOOL	m_bHoriOpaque;
	BOOL	m_bVertOpaque;
	//}}AFX_DATA

// Operations
protected:
	int HitTest(CPoint& point);
	void Preview();
	void CreateVerLine(CRect rect);
	void SetNumRange();
	void SetWidthRange();
	static const DWORD m_nHelpIDs[];
	virtual const DWORD* GetHelpIDs() {return m_nHelpIDs;}
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CGridPage)
	afx_msg void OnLogType();
	afx_msg void OnLineType();
	afx_msg void OnChangeLineWidth();
	afx_msg void OnChangeLinenum();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnVerline();
	afx_msg void OnHorline();
	virtual void OnOK();
	afx_msg void OnSelchangeSelectItem();
	//}}AFX_MSG
	afx_msg void OnSelEndOK(UINT lParam, LONG wParam);
	DECLARE_MESSAGE_MAP()
};

#endif // __GRIDDIA_H__
