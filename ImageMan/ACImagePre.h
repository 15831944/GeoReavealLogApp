#if !defined(AFX_ACIMAGEPRE_H__58427382_2BFB_41A7_ABD5_D44F46EBB54A__INCLUDED_)
#define AFX_ACIMAGEPRE_H__58427382_2BFB_41A7_ABD5_D44F46EBB54A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ACImagePre.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CACImagePre dialog

class CACImagePre : public CDialog
{
// Construction
public:
	void AddNameList();
	BOOL DataDynScale(CString OutName);
	BOOL DataStaticScale(CString OutName);
	BOOL DataCopy(CString InCurve,CString OutCurve);
	void AzimuthCorret(CString OutCurve);
	BOOL InitIOPara();
	CACImagePre(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CACImagePre)
	enum { IDD = IDD_ACIMAGEPREP_DIALOG };
	CComboBox	m_ComName;
	CProgressCtrl	m_Progress;
	float	m_ProStDep;
	float	m_ProEnDep;
	BOOL	m_bAZIPro;
	CString	m_sSeries;
	float	m_WinLongth;
	BOOL	m_bGrads;
	BOOL	m_bExp;
	float	m_MaxValue;
	float	m_MinValue;
	CString	m_CurveName;
	//}}AFX_DATA
	int		m_StaScaleType;
	int     m_StaticScale;
	CString m_WellName;	
	CString m_sAZ1Name;                   //I号极板方位曲线
	int     m_ImgNum;                     //每一深度图像数据点数
	int     m_CurveNum;                   //处理曲线数
	CString m_InCurves[8];	              //处理曲线名
	CString m_OutCurves[8];               //输出曲线名
	float m_StartDep,m_EndDep,m_Rlev;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CACImagePre)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CACImagePre)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButton1();
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACIMAGEPRE_H__58427382_2BFB_41A7_ABD5_D44F46EBB54A__INCLUDED_)
