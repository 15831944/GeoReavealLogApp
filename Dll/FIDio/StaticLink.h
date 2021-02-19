#if !defined(AFX_STATICLINK_H__9D38C649_F60A_40F3_A455_32B1B9642528__INCLUDED_)
#define AFX_STATICLINK_H__9D38C649_F60A_40F3_A455_32B1B9642528__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CStaticLink : public CStatic
{
// Construction
public:
	CStaticLink();

// Attributes
public:
     COLORREF m_clrText;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticLink)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	HCURSOR m_hCursor;
	void SetURL(LPCTSTR lpszURL);
	virtual ~CStaticLink();

	// Generated message map functions
protected:
	CString m_strURL;
     CFont m_font;

	//{{AFX_MSG(CStaticLink)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICLINK_H__9D38C649_F60A_40F3_A455_32B1B9642528__INCLUDED_)
