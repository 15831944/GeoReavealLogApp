// CurveDep.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCurveDep dialog

class CCurveDep : public CDialog
{
	DECLARE_DYNCREATE(CCurveDep)

// Construction
public:
	CCurveDep(CFIDDrawObj* pObj = NULL);
	~CCurveDep();


// Dialog Data
	//{{AFX_DATA(CCurveDep)
	enum { IDD = IDD_CURVE_DEP };
	float	m_Sdep;
	float	m_Edep;
	float	m_Rlev;
	CString	m_Name;
	CString	m_WellName;
	float	m_Extend;
	float	m_ExSdep;
	float	m_ExEdep;
	float	m_DepPoint;
	float	m_NicetyDepPoint;
	BOOL	m_BPreFixDep;
	//}}AFX_DATA

// Attributes	
	CFIDDrawObj* pObj;

// Operations
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCurveDep)
	public:
	virtual void OnOK();
	virtual void OnCANCEL();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCurveDep)
	virtual BOOL OnInitDialog();
	afx_msg void OnComputecoef();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
