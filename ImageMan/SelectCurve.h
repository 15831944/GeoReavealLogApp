#if !defined(AFX_SELECTCURVE_H__BD9B39D2_EBFC_4015_B1E6_86750C848F7D__INCLUDED_)
#define AFX_SELECTCURVE_H__BD9B39D2_EBFC_4015_B1E6_86750C848F7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectCurve.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectCurve dialog

class CSelectCurve : public CDialog
{
// Construction
public:
	void OpenWell(CString StrPath);
	CSelectCurve(CString OpenPath,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectCurve)
	enum { IDD = IDD_SELECT_CURVE };
	CCheckListBox	m_ProCurveList;
	BOOL	m_bAlikePro;
	//}}AFX_DATA
	CString m_OpenPath;
	CStringArray m_PreCurveNameArray,m_SelCurveNameArray;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectCurve)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectCurve)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTCURVE_H__BD9B39D2_EBFC_4015_B1E6_86750C848F7D__INCLUDED_)
