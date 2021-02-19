// chicdial.h : header file
//
#ifndef __CHICDIAL_H__
#define __CHICDIAL_H__
/////////////////////////////////////////////////////////////////////////////
// CCSDialog dialog

class AFX_EXT_CLASS CCSDialog : public CDialog
{
// Construction
public:
	CCSDialog();
	CCSDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	CCSDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// Attributes
	CString m_HelpPath;

// Implementation
protected:
	virtual const DWORD* GetHelpIDs() = 0;

	// Generated message map functions
	//{{AFX_MSG(CCSDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LONG OnHelp(UINT wParam, LONG lParam);
	afx_msg LONG OnHelpContextMenu(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
};

class AFX_EXT_CLASS CCSPropertyPage : public CPropertyPage
{
// Construction
public:
	CCSPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0);
	CCSPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

// Attributes
	CString m_HelpPath;

// Implementation
protected:
	virtual const DWORD* GetHelpIDs() = 0;

	// Generated message map functions
	//{{AFX_MSG(CCSPropertyPage)
	//}}AFX_MSG
	afx_msg LONG OnHelp(UINT wParam, LONG lParam);
	afx_msg LONG OnHelpContextMenu(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
};

class CCSPropertySheet : public CPropertySheet
{
// Construction
public:
	CCSPropertySheet(UINT nIDCaption, CWnd *pParentWnd = NULL, 
		UINT iSelectPage = 0);
	CCSPropertySheet(LPCTSTR pszCaption, CWnd *pParentWnd = NULL, 
		UINT iSelectPage = 0);

// Attributes
	CString m_HelpPath;

// Implementation
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	// Generated message map functions
	//{{AFX_MSG(CCSPropertySheet)
	//}}AFX_MSG
	afx_msg LONG OnHelp(UINT wParam, LONG lParam);
	afx_msg LONG OnHelpContextMenu(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
};

#endif // __CHICDIAL_H__
