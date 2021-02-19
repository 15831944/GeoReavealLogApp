// CurveFillPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCurveFillPage dialog

class CCurveFillPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCurveFillPage)
// Construction
public:
	CCurveFillPage(CCurveObj* pCurveObj = NULL);   // standard constructor
	~CCurveFillPage(); 

// Dialog Data
	//{{AFX_DATA(CCurveFillPage)
	enum { IDD = IDD_CURVE_FILL };
	CButton	m_FillLeftCtrl;
	CButton	m_FillRightCtrl;
	CButton	m_FillLeftRightCtrl;
	CEdit	m_FillTextCtrl;
	CListBox	m_UnSelectList;
	CListBox	m_SelectList;
	int		m_FillPattern;
	CString	m_FillText;
	CString	m_SourceCurve;
	CString	m_SelectCurve;
	
	int		m_FillMode;
	CString	m_SelectListName;
	CString	m_LthCurveName;
	BOOL	m_bHeadShowFill;
	//}}AFX_DATA

	CCJColorPicker m_FillColor;
	CBitmapButton addButton;
	CBitmapButton delButton;
	CCurveObj* m_pCurveObj;
	CTrackObj* m_pTrackObj;
	CArray<CCurveFill*,CCurveFill*> m_FillList;
	int m_SelectIndex;
	CCJColorPicker m_FillColorBox;
	CFillPatternBox m_FillPatternBox;
//operations
	void TransferData(BOOL bOutPut);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveFillPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCurveFillPage)
	afx_msg void OnAddCurve();
	afx_msg void OnDeleteCurve();
	afx_msg void OnSelchangeFillColor();
	afx_msg void OnSelchangeFillPattern();
	afx_msg void OnSelchangeSelectList();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
