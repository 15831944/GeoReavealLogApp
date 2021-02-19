#if !defined(AFX_BACKUPDLG_H__28F790AA_BE1C_4B0D_B92D_5CF2B0CA9B87__INCLUDED_)
#define AFX_BACKUPDLG_H__28F790AA_BE1C_4B0D_B92D_5CF2B0CA9B87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackupDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CBackupDlg dialog
#include "BackupData.h"
#include "ListEditCtrl.h"

class CBackupDlg : public CDialog
{
// Construction
public:
	CBackupDlg(CBackupData *pObj,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBackupDlg)
	enum { IDD = IDD_BACKUPDLG };
	CComboBox	m_Combo;
	CComboBox	m_SeriesCtrl;
	CListEditCtrl	m_InCtrl;
	CString	m_Message;
	float	m_EnProDep;
	float	m_StProDep;
	float	m_StartDep;
	float	m_EndDep;
	float	m_Bits;
	float	m_Rm;
	//}}AFX_DATA
	CBackupData *m_pObj;

	CImageList	ListIcon;
	BOOL		m_bGain;
	CString		m_szWellName;
// Implementation
public:
	BOOL GetInitDep();
	void GetCurvePara();
	void InitCombo();
	BOOL InitSeries();
	void SetCurvePara();
	CString GetSeries();
	CString m_szSeries;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackupDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBackupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCloseupComboSeries();
	afx_msg void OnSelchangeName();
	afx_msg void OnEditchangeName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKUPDLG_H__28F790AA_BE1C_4B0D_B92D_5CF2B0CA9B87__INCLUDED_)
