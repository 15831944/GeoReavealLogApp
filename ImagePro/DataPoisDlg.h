#if !defined(AFX_DataPoisDlg_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_)
#define AFX_DataPoisDlg_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataPoisDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataPoisDlg dialog
#include "DataPoisePro.h"
#include "afxcmn.h"
#include "Grippie.h"
class CDataPoisDlg : public CDialog
{
// Construction
public:
	int m_DataNum;BOOL m_bDataPro;
	int m_NumScale,m_iThickScale;
	void GetScaleNum();
	BOOL GetFrequency();
	void DrawFrequency();
	void DrawRibbon();
	void CreateImagePalette();
	BOOL DataPro();
	//备份数据用于预处理
	BOOL DataCopy(CString InName,CString OutName);
	//数据平滑滤波处理
	BOOL DataFilterPro(CString Path,CString Name);
	//指定电扣异常处理
	BOOL ButBalancePro(CString Path,CString Name);

	CDataPoisDlg(CDataPoisePro *pObj,CWnd* pParent = NULL);   // standard constructor
	CDataPoisePro *m_pObj;
	BOOL m_bSelect;
	CRect m_rectselect;  //鼠村拖动区区域
// Dialog Data
	//{{AFX_DATA(CDataPoisDlg)
	enum { IDD = IDD_POISEDLG };
	CComboBox	m_ButComCtrl;
	CComboBox	m_PadComCtrl;
	CString	m_szPadNo;
	CString	m_szButNo;
	
	
	BOOL	m_bFilter;
	//}}AFX_DATA
	BOOL	m_bAutoBut;		//电扣均衡处理
	BOOL	m_bAutoPad;		//极板均衡处理
	float	m_ButMin;		//处理电扣最小电扣值

	int m_Palsize;
	int m_SelPadNo;
	int m_SelButNo;
	BOOL m_bLogScal;
	float m_ScaleMin,m_ScaleMax;
	float m_MorMax,m_LitMin;
	BOOL m_bColor,m_bDraw;
	CPalette m_Palette;
	CString m_szBufPath,m_CurveName0,m_CurveName;
	float m_EnDep,m_StDep,m_EnProDep,m_StProDep,m_Rlev;
	int m_freq[310];
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataPoisDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDataPoisDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupCombo1();
	afx_msg void OnCloseupCombo2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void GetScalMaxMinValue();
	float m_MaxFre;
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCheck7();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnEnChangeEdit1();
private:
	CGrippie m_grippie;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DataPoisDlg_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_)
