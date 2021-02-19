// PageSetup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageSetup dialog

class CPageSetup : public 	CDialog
{
// Construction
public:
	CPageSetup(CLogPlotDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPageSetup)
	enum { IDD = IDD_PAGE_SETUP };
	UINT	m_LeftSpace;
	UINT	m_Height;
	UINT	m_Width;
	UINT	m_BottomSpace;
	UINT	m_RightSpace;
	UINT	m_TopSpace;
	UINT	m_HeadBottomSpace;
	UINT	m_HeadTopSpace;
	//}}AFX_DATA
// Attributes
	CLogPlotDoc* pDoc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPageSetup)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
