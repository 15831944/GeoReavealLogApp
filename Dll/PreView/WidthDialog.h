// WidthDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWidthDialog dialog

class CWidthDialog : public CDialog
{
// Construction
public:
	CWidthDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWidthDialog)
	enum { IDD = IDD_SET_WIDTH };
	UINT	m_MaxWidth;
	UINT	m_MinWidth;
	UINT	m_Width;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWidthDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWidthDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
