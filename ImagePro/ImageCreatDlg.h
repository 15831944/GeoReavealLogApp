#if !defined(AFX_IMAGECREATDLG_H__438C43F0_892C_4C16_8D35_9AEA9953157A__INCLUDED_)
#define AFX_IMAGECREATDLG_H__438C43F0_892C_4C16_8D35_9AEA9953157A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageCreatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImageCreatDlg dialog
#include "ImageCreate.h"

class CImageCreatDlg : public CDialog
{
// Construction
public:
	CImageCreatDlg(CImageCreat *pObj,CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CImageCreatDlg)
	enum { IDD = IDD_IMGCREATDLG };
	CString	m_Message;
	float m_nClass;               //Kmeans聚类族数8-512
	float m_PadMin;               //极板最小频率值0-30
	BOOL	m_bFilter;
	BOOL	m_bPadPro;
	//}}AFX_DATA
	float m_Bits;
	CImageCreat *m_pObj;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageCreatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	BOOL  m_bRepair;							 //图象合成
	float m_radius;								 //修复半径
	BOOL  m_bCompose;							 //图象合成
	int m_Palsize;
	float m_MorMax,m_LitMin;
	BOOL m_bColor,m_bDraw,m_bButPro;
	CString m_szBufPath,m_CurveName;
	float m_StProDep,m_EnProDep,m_Rlev;
	float m_MaxValue,m_MinValue,m_MaxValue0,m_MinValue0,m_MaxFre;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImageCreatDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheck5();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGECREATDLG_H__438C43F0_892C_4C16_8D35_9AEA9953157A__INCLUDED_)
