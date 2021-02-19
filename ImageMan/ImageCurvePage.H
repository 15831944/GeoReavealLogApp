// ImagePage.h : header file
//

#ifndef __IMAGECURVEPAGE_H__
#define __IMAGECURVEPAGE_H__
/////////////////////////////////////////////////////////////////////////////
// CImageCurvePage dialog
class CImageCurveObj;
#include "Style.h"
class CImageCurvePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CImageCurvePage)

// Construction
public:
	CImageCurvePage(CImageCurveObj* pImageObj=NULL);
	~CImageCurvePage();

// Dialog Data
	//{{AFX_DATA(CImageCurvePage)
	enum { IDD = IDD_IMAGECURVE_PAGE };
	float	m_LeftValue;
	float	m_RightValue;
	CString m_sCurveArray;
	int		m_HorOff;
	int		m_ScaleType;
	BOOL	m_bGDIPlus;
	float	m_HorOffVal;
	//}}AFX_DATA
	int	m_CrMode;

//Attributes
	CImageCurveObj* pObj;
	CCJColorPicker m_CurveColorCtrl,m_FillColorCtrl;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImageCurvePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg void OnCbnCloseupScaleType();
	// Generated message map functions
	//{{AFX_MSG(CImageCurvePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
// CImageCurveBase dialog

class CImageCurveBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CImageCurveBase)

// Construction
public:
	void AddCurveList(CString path);
	CImageCurveBase(CImageCurveObj* pImageObj=NULL);
	~CImageCurveBase();

// Dialog Data
	//{{AFX_DATA(CImageCurveBase)
	enum { IDD = IDD_IMAGECURVE_BASE };
	CString	m_AliasName;
	CString	m_Unit;
	CString	m_WellName;
	//}}AFX_DATA

	CSliderCtrl	m_SliderSdep;
	CSliderCtrl	m_SliderEdep;
	CEdit	m_SdepCtrl;
	CEdit	m_EdepCtrl;
	float	m_Edep;
	float	m_Sdep;

	void EnableDepth(BOOL bf);
	void HideDepth();
	void SetSliderDepth(float sdep, float edep);
	BOOL InitDepthSlider();

// Attributes
	CImageCurveObj* pObj;
	CString m_Name;
// Operations

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImageCurveBase)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImageCurveBase)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseWell();
	afx_msg void OnChangeWellname();
	afx_msg void OnCloseupName();
	//}}AFX_MSG
	afx_msg void OnChangeEdep();
	afx_msg void OnChangeSdep();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEditchangeName();

	DECLARE_MESSAGE_MAP()
};
#endif // __IMAGEPAGE_H__/////////////////////////////////////////////////////////////////////////////