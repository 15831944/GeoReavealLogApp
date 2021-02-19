// ImagePage.h : header file
//

#ifndef __IMAGEPAGE_H__
#define __IMAGEPAGE_H__
/////////////////////////////////////////////////////////////////////////////
// CImagePage dialog
class CImageObj;
#include "GradientCtrl.h"
class CImagePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CImagePage)

// Construction
public:
	void AddColorScheme();
	CImagePage(CImageObj* pImageObj=NULL);
	~CImagePage();

	CGradientCtrl m_wndGradientCtrl;
// Dialog Data
	//{{AFX_DATA(CImagePage)
	enum { IDD = IDD_IMAGE };
	CEdit	m_GradeCtrl;
	CComboBox	m_CrMoldCtrl;
	float	m_LeftVal;
	float	m_RightVal;
	BOOL	m_bColor;
	int		m_Bit;
	int		m_ColorGrade;
	CString	m_SchemeFile;
	int	m_HorScale;
	//}}AFX_DATA
	int	m_CrMode;
//Attributes
	CImageObj* pObj;
	CCJColorPicker m_BkColorCtrl;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImagePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImagePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupComboCrmoldl();
	afx_msg void OnChangeColorgrade();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // __IMAGEPAGE_H__/////////////////////////////////////////////////////////////////////////////
// CImageBase dialog

class CImageBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CImageBase)

// Construction
public:
	void AddCurveList(CString path);
	CImageBase(CImageObj* pImageObj=NULL);
	~CImageBase();

// Dialog Data
	//{{AFX_DATA(CImageBase)
	enum { IDD = IDD_IMAGE_BASE };
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
	CImageObj* pObj;
	CString m_Name;
// Operations

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImageBase)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImageBase)
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
