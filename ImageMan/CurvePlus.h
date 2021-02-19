// CurveDep.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCurveDep dialog

class CCurvePlus : public CDialog
{
	DECLARE_DYNCREATE(CCurvePlus)

// Construction
public:
	BOOL m_BSubPLus;
	float m_SubPlusCoef;
	float m_Value0;
	CCurvePlus(CCurveObj* pObj = NULL);
	~CCurvePlus();

	int m_CurveStyle;
// Dialog Data
	//{{AFX_DATA(CCurveDep)
	enum { IDD = IDD_CURVE_PLUS};
	float	m_Sdep;
	float	m_Edep;
	float	m_Rlev;
	BOOL	m_IniDep;
	float	m_Sdep0;
	float	m_Edep0;
	CString	m_Name;
	CString	m_WellName;

	float m_MultiplyCoef,m_PlusCoef;
	float m_Point1Dep,m_Point1Value;
	float m_Point2Dep,m_Point2Value;
	//}}AFX_DATA

// Attributes	
	CCurveObj* pObj;

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
	afx_msg void OnComputeCoef();
	afx_msg void OnPreVerge();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
