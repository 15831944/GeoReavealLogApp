// curvedia.h : header file
//

#ifndef __CURVEDIA_H__
#define __CURVEDIA_H__

/////////////////////////////////////////////////////////////////////////////
// CCurvePage dialog

#include "Style.h"

class CCurveObj;
class CCurvePage : public CPropertyPage
{
// Construction
public:
	CCurvePage(CCurveObj* pCurveObj = NULL);	// standard constructor
	~CCurvePage();
public:
	int flag;
protected:
	CCurveObj* m_pCurveObj;
	CCJColorPicker m_CurveColorBox;
	CLineStyleBox m_CurveStyleBox;
	CCJColorPicker m_CurveColor;
// Dialog Data
	//{{AFX_DATA(CCurvePage)
	enum { IDD = IDD_CURVE_PAGE };
	float	m_LeftValue;
	float	m_RightValue;
	int		m_Factor;
	int		m_ScaleType;
	int		m_CurveStyle;
	
	int		m_CurveWidth;
	CSpinButtonCtrl	m_CurveWidth_Spin;
	BOOL	m_bEditEnable;
	//}}AFX_DATA

protected:
	void Preview();
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual BOOL OnInitDialog();
	// Generated message map functions
	//{{AFX_MSG(CCurvePage)
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeCurveStyle();
	afx_msg void OnChangeCurveWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__CURVEDIA_H__
