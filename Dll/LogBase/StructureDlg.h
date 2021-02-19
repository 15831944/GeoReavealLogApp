#if !defined(AFX_STRUCTUREDLG_H__D6AE457A_CA5E_4F84_90D1_DF05FF3F4BBB__INCLUDED_)
#define AFX_STRUCTUREDLG_H__D6AE457A_CA5E_4F84_90D1_DF05FF3F4BBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StructureDlg.h : header file
//
#include "resource.h"
#include "DialogResize.h"
class CStructureDlg : public CDialogResize
{
// Construction
public:
	CString m_strTitle;
	CStringArray m_strFieldArray;
	CStructureDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStructureDlg)
	enum { IDD = IDD_STRUCTURE };
	CButton	m_btnAdd;
	CListCtrl	m_FieldListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStructureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bModified;
	// Generated message map functions
	//{{AFX_MSG(CStructureDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDblclkFields(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DECLARE_DLGRESIZE_MAP;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRUCTUREDLG_H__D6AE457A_CA5E_4F84_90D1_DF05FF3F4BBB__INCLUDED_)
