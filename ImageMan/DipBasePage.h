// DipBasePage.h : header file

#ifndef __DIPBASEDIA_H__
#define __DIPBASEDIA_H__

/////////////////////////////////////////////////////////////////////////////
// CDipBasePage dialog
class CDangDdirObj;
class CDipBasePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDipBasePage)
public:
	CDipBasePage(CDangDdirObj*);
	~CDipBasePage();
	
// Dialog Data
	//{{AFX_DATA(CDipBasePage)
	enum { IDD = IDD_DIP_BASE };
	CSliderCtrl	m_SliderSdep;
	CSliderCtrl	m_SliderEdep;
	CEdit	m_SdepCtrl;
	CEdit	m_EdepCtrl;
	CString	m_AliasName;
	float	m_Edep;
	float	m_Sdep;
	CString	m_Unit;
	CString	m_WellName;
	CString	m_Name;
	float	m_MinGrade;
	CString m_DirName;
	CString	m_GradName;
	//}}AFX_DATA
	
	CDangDdirObj* pObj;
	void EnableDepth(BOOL bf);
	void HideDepth();
	void SetSliderDepth(float sdep, float edep);
	void AddCurveList(CString path);
	BOOL InitDepthSlider();

	//{{AFX_VIRTUAL(CDipBasePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CDipBasePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSdep();
	afx_msg void OnChangeEdep();
	afx_msg void OnChangeName();
	afx_msg void OnBrowseWell();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeWellname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
