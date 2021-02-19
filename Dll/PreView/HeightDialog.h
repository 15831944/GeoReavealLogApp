// HeightDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHeightDialog dialog

class CHeightDialog : public CDialog
{
// Construction
public:
	CHeightDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHeightDialog)
	enum { IDD = IDD_SET_HEIGHT };
	UINT	m_Height;
	UINT	m_MaxHeight;
	UINT	m_MinHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeightDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHeightDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
