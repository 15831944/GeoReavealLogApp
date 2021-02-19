// AzimFreqDlg.h : header file
//
#ifndef __AZIMFREQPAGE_H__
#define __AZIMFREQPAGE_H__


/////////////////////////////////////////////////////////////////////////////
// CAzimFreqPage dialog

class CAzimFreqPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAzimFreqPage)

// Construction
public:
	CAzimFreqPage(CAzimFreqObj*); 
	~CAzimFreqPage(){};   // standard constructor

// Attributes	
	CAzimFreqObj* pObj;
	CCJColorPicker m_CircleColor;
	CCJColorPicker m_PieColor;
//operations
	void Preview() ;

// Dialog Data
	//{{AFX_DATA(CAzimFreqPage)
	enum { IDD = IDD_AZIM_FREQ };
	UINT	m_CircleWidth;
	UINT	m_AzimNum;
	UINT	m_PieWidth;
	UINT	m_CircleRadius;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAzimFreqPage)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAzimFreqPage)
	afx_msg void OnChangePieWidth();
	afx_msg void OnChangePieNum();
	afx_msg void OnChangeCircleWidth();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnChangeCircleRadius();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDangFreqPage dialog

class CDangFreqPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDangFreqPage)

// Construction
public:
	CDangFreqPage(CAzimFreqObj*);   // standard constructor
	~CDangFreqPage(){};   // standard constructor

// Attributes	
	CAzimFreqObj* pObj;
	CCJColorPicker m_HoriColor;
	CCJColorPicker m_VertColor;

//operations
	void Preview() ;

// Dialog Data
	//{{AFX_DATA(CDangFreqPage)
	enum { IDD = IDD_DANG_FREQ };
	UINT	m_HoriLength;
	UINT	m_HoriWidth;
	UINT	m_VertHeight;
	UINT	m_VertNum;
	UINT	m_VertWidth;
	UINT	m_MaxDang;
	UINT	m_MinDang;
	UINT	m_DepLevel;
	BOOL	m_DrawDirect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDangFreqPage)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDangFreqPage)
	afx_msg void OnChangeHoriLength();
	afx_msg void OnChangeHoriWidth();
	afx_msg void OnChangeVertHeight();
	afx_msg void OnChangeVertNum();
	afx_msg void OnChangeVertWidth();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif __AZIMFREQPAGE_H__
