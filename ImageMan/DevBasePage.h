// DevBasePage.h : header file

#ifndef __DEVBASEDIA_H__
#define __DEVBASEDIA_H__

/////////////////////////////////////////////////////////////////////////////
// CDevBasePage dialog

class CDevBasePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDevBasePage)
public:
	CDevBasePage(CDevDazObj*);
	~CDevBasePage();
	
// Dialog Data
	//{{AFX_DATA(CDevBasePage)
	enum { IDD = IDD_DEV_BASE };
	CSliderCtrl	m_SliderSdep;
	CSliderCtrl	m_SliderEdep;
	CEdit	m_SdepCtrl;
	CEdit	m_EdepCtrl;
	CString	m_AliasName;
	float	m_Edep;
	float	m_Sdep;
	CString	m_Unit;
	CString	m_WellName;
	CString	m_DevName;
	CString	m_DazName;
	//}}AFX_DATA
	
	CDevDazObj* pObj;
	void EnableDepth(BOOL bf);
	void HideDepth();
	void SetSliderDepth(float sdep, float edep);
	void AddCurveList(CString path);
	BOOL InitDepthSlider();

	//{{AFX_VIRTUAL(CDevBasePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CDevBasePage)
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
