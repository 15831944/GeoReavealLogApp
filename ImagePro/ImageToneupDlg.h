#if !defined(AFX_IMAGETONEUPDLG_H__0C74720E_B0B0_4AB4_A091_F8DDBCE17910__INCLUDED_)
#define AFX_IMAGETONEUPDLG_H__0C74720E_B0B0_4AB4_A091_F8DDBCE17910__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IamgeToneupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImageToneupDlg dialog
#include "ImageToneup.h"
#include "WndShadow.h"

class CImageToneupDlg : public CDialog
{
// Construction
public:
	CImageToneupDlg(CImageToneup *pObj,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImageToneupDlg)
	enum { IDD = IDD_IMGTONEUPDLG };
	BOOL	m_bGrads;
	BOOL	m_bFreq;
	BOOL	m_bFilter;
	CString	m_Message;
	BOOL	m_bMusic;
	//}}AFX_DATA
	BOOL  m_bRepair;                             //Í¼ÏóÐÞ¸´
	int m_nLevel;
	float m_radius;								 //ÐÞ¸´°ë¾¶
	float m_WinLongth,m_RlevLongth;
	short  m_MaxPos,m_MinVal;
	CImageToneup *m_pObj;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageToneupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImageToneupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGrads();
	afx_msg void OnBnClickedMusic();
	afx_msg void OnBnClickedFreq();
	afx_msg void OnBnClickedExpend();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IAMGETONEUPDLG_H__0C74720E_B0B0_4AB4_A091_F8DDBCE17910__INCLUDED_)
