// AzimFreqDlg.h : header file
//
#ifndef __MARKAZIPAGE_H__
#define __MARKAZIPAGE_H__


/////////////////////////////////////////////////////////////////////////////
// CMarkAziPage dialog

class CMarkAziPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMarkAziPage)

// Construction
public:
	CMarkAziPage(CMarkFreqObj*); 
	~CMarkAziPage(){};   // standard constructor

// Attributes	
	CMarkFreqObj* pObj;
	CCJColorPicker m_CircleColor;
	CCJColorPicker m_PieColor;
//operations

// Dialog Data
	//{{AFX_DATA(CMarkAziPage)
	enum { IDD = IDD_MARKAZI_PAGE };
	UINT	m_CircleWidth;
	UINT	m_AzimNum;
	UINT	m_PieWidth;
	UINT	m_CircleRadius;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMarkAziPage)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMarkAziPage)
	afx_msg void OnChangePieWidth();
	afx_msg void OnChangePieNum();
	afx_msg void OnChangeCircleWidth();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCircleRadius();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CMarkDipPage dialog

class CMarkDipPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMarkDipPage)

// Construction
public:
	CMarkDipPage(CMarkFreqObj*);   // standard constructor
	~CMarkDipPage(){};   // standard constructor

// Attributes	
	CMarkFreqObj* pObj;
	CCJColorPicker m_HoriColor;
	CCJColorPicker m_VertColor;

//operations

// Dialog Data
	//{{AFX_DATA(CMarkDipPage)
	enum { IDD = IDD_MARKDIP_PAGE };
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
	BOOL    m_bDrawDipFreq;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMarkDipPage)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMarkDipPage)
	afx_msg void OnChangeHoriLength();
	afx_msg void OnChangeHoriWidth();
	afx_msg void OnChangeVertHeight();
	afx_msg void OnChangeVertNum();
	afx_msg void OnChangeVertWidth();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif __MARKAZIPAGE_H__
