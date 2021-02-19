#if !defined(AFX_INPUTBOX_H__2B815162_98DB_4DC4_B46F_D9733D18393B__INCLUDED_)
#define AFX_INPUTBOX_H__2B815162_98DB_4DC4_B46F_D9733D18393B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputBox dialog

class CInputBox : public CDialog
{
// Construction
public:
	CString m_Title;
	CInputBox(CWnd* pParent = NULL);   // standard constructor
	CString m_CurveName,m_DepRange;
// Dialog Data
	//{{AFX_DATA(CInputBox)
	enum { IDD = IDD_INPUTBOX };
	float	m_Value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputBox)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTBOX_H__2B815162_98DB_4DC4_B46F_D9733D18393B__INCLUDED_)
