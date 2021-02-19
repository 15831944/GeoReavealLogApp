#if !defined(AFX_RECURNAMDLG_H_INCLUDED_)
#define AFX_RECURNAMDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReCurNamDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReCurNamDlg dialog
#include "Resource.h"
class CReCurNamDlg : public CDialog
{
// Construction
public:
	CReCurNamDlg(CWnd* pParent = NULL);   // standard constructor
	CStringArray m_SourceName;
	CStringArray m_ObjectName;
	CStringArray m_Units; 
	CWordArray m_NumDim;
	bool bSucceed;
	void OnCheckBtn();
	void EditCurveAttrib(int nItem); //±‡º≠«˙œﬂ Ù–‘

// Dialog Data
	//{{AFX_DATA(CReCurNamDlg)
	enum { IDD = IDD_MODICURVE_DIALOG };
	CListCtrl	m_CurveName;
	int		m_nMaxLen;
	int		m_nMaxLog;
	CString	m_sOldStr;
	CString	m_sNewStr;
	int		m_NumLog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReCurNamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReCurNamDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRclickCurveNameList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditCurveNameList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteBtn();
	afx_msg void OnDblclkCurveNameList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnModifystrBtn();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnGetleftBtn();
	afx_msg void OnGetrightBtn();
	virtual void OnCancel();
	afx_msg void OnClearerBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECURNAMDLG_H_INCLUDED_)
