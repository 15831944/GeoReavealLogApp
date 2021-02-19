#if !defined(AFX_PROMULTICURVEDEP_H__BD9B39D2_EBFC_4015_B1E6_86750C848F7D__INCLUDED_)
#define AFX_PROMULTICURVEDEP_H__BD9B39D2_EBFC_4015_B1E6_86750C848F7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectCurve.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProMultiCurveDep dialog
#include "ImageManView.h"
class CProMultiCurveDep : public CDialog
{
// Construction
public:
	void ProMultiCurve(int SelCurveNum);
	CProMultiCurveDep(CString OpenPath,CImageManView *pVew,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProMultiCurveDep)
	enum { IDD = IDD_MULTCURVE_DEP };
	CProgressCtrl	m_CurveProgress;
	CProgressCtrl	m_Progress;
	CListCtrl	m_DepList;
	CString	m_OptionStr;
	CCheckListBox	m_ProCurveList;
	CString	m_OpenPath;
	CString	m_StrID;
	//}}AFX_DATA
	BOOL m_AllCheck;
	CStringArray m_SelCurveNameArray;
	CStringArray m_DepArray,m_ReviseArray;
	int m_SectNum;
	//处理视的ReDO UnDo 操作
	CImageManView *m_pView;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProMultiCurveDep)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProMultiCurveDep)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROMULTICURVEDEP_H__BD9B39D2_EBFC_4015_B1E6_86750C848F7D__INCLUDED_)
