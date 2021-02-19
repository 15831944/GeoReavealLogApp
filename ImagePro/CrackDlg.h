#if !defined(AFX_CRACKDLG_H__)
#define AFX_CRACKDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CrackDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCrackDlg dialog
#include "CrackCompute.h"
#include "WndShadow.h"
#include "afxcmn.h"
#include "afxwin.h"

class CCrackDlg : public CDialog
{
// Construction
public:
	BOOL GetFrequency();
	void DrawFrequency();
	void DrawRibbon();
	void CreateImagePalette();
	CCrackDlg(CCrackCompute *pObj,CWnd* pParent = NULL);   // standard constructor
	CCrackCompute *m_pObj;
	BOOL m_bSelect;
	CRect m_rectselect;  //鼠村拖动区区域
// Dialog Data
	//{{AFX_DATA(CCrackDlg)
	enum { IDD = IDD_CRACKDLG };
	//}}AFX_DATA

	int m_Palsize;
	float m_MorMax,m_LitMin;
	BOOL m_bColor,m_bDraw;
	CPalette m_Palette;
	CString m_szBufPath,m_CurveName;
	float m_ProEnDep,m_ProStDep,m_Rlev;
	CString m_szCp;
	float m_MaxValue,m_MinValue;
	float m_Win,m_WinRlev;
	float m_WinWide,m_flagValue,m_ParA,m_ParB,m_FlagFre;
	BOOL m_Check1,m_Check2,m_Check3,m_Check4;
	int m_freq[300];
	unsigned long AllNum;
	CRect m_Rect,m_MouseRect;
	int m_Pt;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrackDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CCrackDlg)
	virtual BOOL OnInitDialog();
//	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButton2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CrackDlg_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_)
