// WavePage.h : header file
//

#ifndef __ARRAYWAVEPAGE_H__
#define __ARRAYWAVEPAGE_H__
/////////////////////////////////////////////////////////////////////////////
// CArrayWavePage dialog
class CArrayWaveObj;
class CCJColorPicker;
class CArrayWavePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CArrayWavePage)

// Construction
public:
	CArrayWavePage(CArrayWaveObj* pWaveObj=NULL);
	~CArrayWavePage();

// Dialog Data
	//{{AFX_DATA(CArrayWavePage)
	enum { IDD = IDD_ARRAYWAVE };
	UINT	m_WfAmp;
	UINT	m_Level;
	UINT	m_PlotAmp;
	int		m_WaveType;
	UINT	m_LeftVal;
	UINT	m_RightVal;
	float	m_StartTime;
	int		m_WaveWidth;
	int		m_crNum;
	//}}AFX_DATA

//Attributes
	CArrayWaveObj* pObj;
	CCJColorPicker m_Color,m_Color0,m_Color1;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CArrayWavePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CArrayWavePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // __WAVEPAGE_H__/////////////////////////////////////////////////////////////////////////////
// CArrayWaveBase dialog

class CArrayWaveBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CArrayWaveBase)

// Construction
public:
	void AddCurveList(CString path);
	CArrayWaveBase(CArrayWaveObj* pWaveObj=NULL);
	~CArrayWaveBase();


// Dialog Data
	//{{AFX_DATA(CArrayWaveBase)
	enum { IDD = IDD_ARRAYWAVE_BASE };
	CEdit	m_SdepCtrl;
	CEdit	m_EdepCtrl;
	CSliderCtrl	m_SliderSdep;
	CSliderCtrl	m_SliderEdep;
	CString	m_AliasName;
	CString	m_Unit;
	CString	m_WellName;
	float	m_Edep;
	float	m_Sdep;
	CString	m_DelayCurve;
	CString	m_strDelayCurveNo;
	CString	m_PlusCurve;
	CString	m_strPlusCurveNo;
	//}}AFX_DATA

// Attributes
	CArrayWaveObj* pObj;
	CString m_Name;

// Operations
	void HideDepth();
	void EnableDepth(BOOL bf);
	void SetSliderDepth(float sdep, float edep);
	BOOL CArrayWaveBase::InitDepthSlider();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CArrayWaveBase)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CArrayWaveBase)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseWell();
	afx_msg void OnChangeSdep();
	afx_msg void OnChangeEdep();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeWellname();
	afx_msg void OnCloseupName();
	afx_msg void OnCloseupPlusCurve();
	afx_msg void OnCloseupDelayCurve();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
