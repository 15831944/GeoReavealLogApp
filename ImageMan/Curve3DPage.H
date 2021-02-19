// Curve3DPage.h : header file
//

#ifndef __CURVE3DPAGE_H__
#define __CURVE3DPAGE_H__
/////////////////////////////////////////////////////////////////////////////
// CCurve3DPage dialog
class CCurve3D;
//class CCJColorPicker;
class CCurve3DPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCurve3DPage)

// Construction
public:
	CCurve3DPage(CCurve3D* pCurve3D=NULL);
	~CCurve3DPage();

// Dialog Data
	//{{AFX_DATA(CCurve3DPage)
	enum { IDD = IDD_CURVE3D };	
	int 	m_ColorGrade;
	int	m_FilterType;
	short	m_LeftVal;
	short	m_RightVal;
	BOOL	m_bColor;
	//}}AFX_DATA
	int	m_CrMode;

//Attributes
	CCurve3D* pObj;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCurve3DPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCurve3DPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif 
// CCurve3DBase dialog

class CCurve3DBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CCurve3DBase)

// Construction
public:
	void AddCurveList(CString path);
	CCurve3DBase(CCurve3D* pCurve3D=NULL);
	~CCurve3DBase();

// Dialog Data
	//{{AFX_DATA(CCurve3DBase)
	enum { IDD = IDD_CURVE3D_BASE };
	CCheckListBox	m_CurveList;
	CString	m_AliasName;
	CString	m_Unit;
	CString	m_WellName;
	UINT	m_Space;
	BOOL	m_Daz;
	BOOL	m_PoleArray;
	//}}AFX_DATA

	int		m_HorScale;
	int		m_StartPos;
	int		m_Bit;
// Attributes
	CCurve3D* pObj;
	CString m_Name;
	float m_LookAngle;
// Operations

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCurve3DBase)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCurve3DBase)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseWell();
	afx_msg void OnChangeWellname();
	afx_msg void OnCloseupName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
