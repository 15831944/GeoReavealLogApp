// CurveBasePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCurveBasePage dialog

class CCurveBasePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCurveBasePage)

// Construction
public:
	CCurveBasePage(CFIDDrawObj* pObj = NULL);
	~CCurveBasePage();


// Dialog Data
	//{{AFX_DATA(CCurveBasePage)
	enum { IDD = IDD_CURVE_BASE };
	CSliderCtrl	m_SliderSdep;
	CSliderCtrl	m_SliderEdep;
	CEdit	m_SdepCtrl;
	CEdit	m_EdepCtrl;
	CString	m_AliasName;
	float	m_Edep;
	CString	m_Name;
	CString	m_Name1;
	float	m_Sdep;
	CString	m_Unit;
	CString	m_WellName;
	CString	m_ScripFileName;
	//}}AFX_DATA

// Attributes	
	CFIDDrawObj* pObj;
	CString m_WellName0;
	DWORD		m_RootID;
	CString		m_RootName;
// Operations
	void EnableDepth(BOOL bf);
	void HideDepth();
	void SetSliderDepth(float sdep, float edep);
	BOOL InitDepthSlider();
	void AddCurveList(CString path);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCurveBasePage)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCurveBasePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseWell();
	afx_msg void OnChangeEdep();
	afx_msg void OnChangeSdep();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusWellname();
	afx_msg void OnChangeWellname();
	afx_msg void OnSetfocusWellname();
	afx_msg void OnEditchangeName();
	afx_msg void OnCloseupName();
	afx_msg void OnBrowseWell2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
