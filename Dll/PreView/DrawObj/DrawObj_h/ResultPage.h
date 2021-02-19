// ResultPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResultPage dialog
class CResultPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CResultPage)

// Construction
public:
	CResultPage();
	CListCtrl	m_ResultCtrl;
	CString		m_szSever;
	DWORD		m_WellID;
	void LoadResult();
// Dialog Data
	//{{AFX_DATA(CResultPage)
	enum { IDD = IDD_RESULT_PAGE };
	CButton	m_Static;
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
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};
