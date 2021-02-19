#ifndef _RMFSCALEDLG_H__
#define _RMFSCALEDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RMFScaleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScaleDlg dialog

class CScaleDlg : public CDialog
{
// Construction
public:
	CScaleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScaleDlg)
	enum { IDD = IDD_SCALE_DLG };
	BOOL	m_bResistance;
	CString	m_szRMFWell;//标准电阻率井名
	CString	m_szRMFName;//浅侧向刻度 用于生成高分辨率电阻率曲线
	CComboBox	m_NameComCtrl;
	//}}AFX_DATA
	BOOL	m_bCreateImage;         //生成刻度图像到测井项目中
	BOOL	m_bCreateRxo;           //合成高分辩电阻率曲线

	float m_fRlev;					//滑动步长
	float m_fWin;                   //滑动窗长
	float m_LeftScal,m_RightScal;   //浅侧向刻度范围

	CString m_szWell,m_szName;
	int		m_DataNum,m_DataType;
	float	m_StProDep,m_EnProDep,m_Rlev;
	BOOL CreateCrackImg();
	BOOL CreateCrackImg2();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	void InitComCtrl();
	void GetMaxMinValue();
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScaleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	afx_msg void OnCloseupComboRmfname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RMFSCALEDLG_H__67D294E5_F617_45DD_89B1_A2D611FC06D8__INCLUDED_)
