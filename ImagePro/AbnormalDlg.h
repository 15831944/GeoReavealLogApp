#if !defined(AFX_CRACKDLG_H__)
#define AFX_CRACKDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CrackDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbnormalDlg dialog
#include "CrackCompute.h"
#include "WndShadow.h"
#include "afxcmn.h"
#include "afxwin.h"

class CAbnormalDlg : public CDialog
{
// Construction
public:
	CAbnormalDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bSelect;
	CRect m_rectselect;  //鼠村拖动区区域
// Dialog Data
	//{{AFX_DATA(CAbnormalDlg)
	enum { IDD = IDD_ABNORMAL_DLG };
	//}}AFX_DATA

	int m_PadNum;                         //极板数
	int m_ButtonNum;                      //每个极板电扣数
	int m_DataNum;                        //总电扣数
	int m_Palsize,m_EdgeType;
	float m_MorMax,m_LitMin;
	BOOL m_bColor,m_bDraw,m_bCreatImg;
	CPalette m_Palette;
	CString m_szBufPath,m_szEdgeName;
	CString m_szOutImgName,m_szOutStaName;
	float m_StProDep,m_EnProDep,m_Rlev;
	float m_Win,m_WinRlev;
	float m_flagValue,m_FlagFre;
	float m_MinValue,m_MaxValue;
	int m_freq[300];
	unsigned long AllNum;
	CRect m_Rect,m_MouseRect;
	CComboBox	m_NameComCtrl;
	int m_Pt;

	void UpdateEdgeData();
	BOOL CopyCurve(CString InName,CString OutName);
	BOOL CreatEdgeData(CString InName,CString OutName);
	BOOL AllDataExtentPro(CString Path,CString Name);
	BOOL GetFrequency(CString CurveName);
	void DrawFrequency();
	void DrawRibbon();
	void CreateImagePalette();
	BOOL GetDataMaxMin(CString InName);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbnormalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CAbnormalDlg)
	virtual BOOL OnInitDialog();
//	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnCbnCloseupCombo1();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CrackDlg_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_)
