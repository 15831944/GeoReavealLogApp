#if !defined(AFX_LAYERPAGE_H__1E4DF59A_6089_485F_8E73_91B633DA6ECE__INCLUDED_)
#define AFX_LAYERPAGE_H__1E4DF59A_6089_485F_8E73_91B633DA6ECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayerPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayerPage dialog
#include "GridCtrl.h"
#include "HistoryComboEx.h"
class CLayerPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CLayerPage)

// Construction
public:
	void LoadLayer();
	CLayerPage();
	~CLayerPage();

// Dialog Data
	//{{AFX_DATA(CLayerPage)
	enum { IDD = IDD_LAYER_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	CString m_FileName;
	CGridCtrl m_Grid;
	CHistoryComboEx	m_Box;
	CHistoryComboEx	m_Option;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLayerPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLayerPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnArrang();
	afx_msg void OnInsertrow();
	afx_msg void OnDelrow();
	//}}AFX_MSG
	afx_msg void OnBrowse();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnKillfocusCombo1();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnKillfocusCombo2();
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERPAGE_H__1E4DF59A_6089_485F_8E73_91B633DA6ECE__INCLUDED_)
