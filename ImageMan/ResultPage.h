// ResultPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResultPage dialog
#include "GridCtrl.h"
class CResultPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CResultPage)

// Construction
public:
	CResultPage();
	CGridCtrl m_Grid;
	CComboBox	m_Box;
// Dialog Data
	//{{AFX_DATA(CResultPage)
	enum { IDD = IDD_RESULT_PAGE };
	int		m_ResultLR;
	CString	m_FileName;
	CString m_GroupCaption,m_LeftCaption,m_RightCaption;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CResultPage)
	protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CResultPage)
		// NOTE: the ClassWizard will add member functions here
		afx_msg void OnBrowseWell();
		afx_msg void OnDelRow();
		afx_msg void OnInsertRow();
	//}}AFX_MSG	
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnKillfocusCombo1();
	afx_msg void OnCompositor();
	DECLARE_MESSAGE_MAP()
};
