// Image3DPage.h : header file
//

#ifndef __IMAGE3DPAGE_H__
#define __IMAGE3DPAGE_H__
/////////////////////////////////////////////////////////////////////////////
// CImage3DPage dialog
class CImage3D;
#include "GradientCtrl.h"
class CImage3DPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CImage3DPage)

// Construction
public:
	void AddColorScheme();
	CImage3DPage(CImage3D* pImage3DObj=NULL);
	~CImage3DPage();

	CGradientCtrl m_wndGradientCtrl;
// Dialog Data
	//{{AFX_DATA(CImage3DPage)
	enum { IDD = IDD_IMAGE3D };
	CEdit	m_GradeCtrl;
	CComboBox	m_CrMoldCtrl;
	float	m_LeftVal;
	float	m_RightVal;
	BOOL	m_bColor;
	int		m_Bit;
	float	m_LookAngle;
	int		m_StartPos;
	int		m_ColorGrade;
	CString	m_SchemeFile;
	int	m_HorScale;
	//}}AFX_DATA
	int	m_CrMode;
	CCJColorPicker m_BkColorCtrl;
//Attributes
	CImage3D* pObj;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImage3DPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImage3DPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupComboCrmoldl();
	afx_msg void OnChangeColorgrade();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif // __Image3DPAGE_H__/////////////////////////////////////////////////////////////////////////////
// CImage3DBase dialog

class CImage3DBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CImage3DBase)

// Construction
public:
	void AddCurveList(CString path);
	CImage3DBase(CImage3D* pImage3DObj=NULL);
	~CImage3DBase();

// Dialog Data
	//{{AFX_DATA(CImage3DBase)
	enum { IDD = IDD_IMAGE3D_BASE };
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
	CImage3D* pObj;
	CString m_Name;
// Operations

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImage3DBase)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImage3DBase)
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
