// EchoPage.h : header file
//

#ifndef __ECHOPAGE_H__
#define __ECHOPAGE_H__
/////////////////////////////////////////////////////////////////////////////
// CEchoPage dialog
class CEchoObj;
class CCJColorPicker;
class CEchoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CEchoPage)

// Construction
public:
	CEchoPage(CEchoObj* pEchoObj=NULL);
	~CEchoPage();

// Dialog Data
	//{{AFX_DATA(CEchoPage)
	enum { IDD = IDD_ECHO };
	UINT	m_Level;
	UINT	m_PlotAmp;
	int		m_EchoType;
	float	m_LeftVal;
	float	m_RightVal;
	float  	m_WfMax;
	float	m_WfMin;
	int		m_EchoWidth;
	int		PALSIZE;
	BOOL	m_FillScale;
	BOOL	m_EchoFilter;
	//}}AFX_DATA

//Attributes
	CEchoObj* pObj;
	CCJColorPicker m_Color,m_FillColor;
	CString m_SchemeFile;
	BOOL m_bXlog,m_bYlog;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEchoPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEchoPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // __EchoPAGE_H__/////////////////////////////////////////////////////////////////////////////
// CEchoBase dialog

class CEchoBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CEchoBase)

// Construction
public:
	void AddCurveList(CString path);
	CEchoBase(CEchoObj* pEchoObj=NULL);
	~CEchoBase();

// Dialog Data
	//{{AFX_DATA(CEchoBase)
	enum { IDD = IDD_ECHO_BASE};
	CEdit	m_SdepCtrl;
	CEdit	m_EdepCtrl;
	CSliderCtrl	m_SliderSdep;
	CSliderCtrl	m_SliderEdep;
	CString	m_AliasName;
	CString	m_Unit;
	CString	m_WellName;
	float	m_Edep;
	float	m_Sdep;
	//}}AFX_DATA

// Attributes
	CEchoObj* pObj;
	CString m_Name;

// Operations
	void HideDepth();
	void EnableDepth(BOOL bf);
	void SetSliderDepth(float sdep, float edep);
	BOOL CEchoBase::InitDepthSlider();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEchoBase)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEchoBase)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseWell();
	afx_msg void OnChangeSdep();
	afx_msg void OnChangeEdep();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeWellname();
	afx_msg void OnCloseupName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
