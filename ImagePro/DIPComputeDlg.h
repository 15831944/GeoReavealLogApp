#if !defined(AFX_DIPCOMPUTEDLG_H__C8910D51_D7C9_4D45_ABB1_2E585CE1633E__INCLUDED_)
#define AFX_DIPCOMPUTEDLG_H__C8910D51_D7C9_4D45_ABB1_2E585CE1633E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DIPComputeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDIPComputeDlg dialog
#include "DIPCompute.h"
#include "WndShadow.h"

class CDIPComputeDlg : public CDialog
{
// Construction
public:
	CDIPComputeDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CDIPComputeDlg)
	enum { IDD = IDD_DIPCOMPUTE_DIALOG };
	float	m_WinLength;
	float	m_StepLength;
	float	m_SearchLength;
	float	m_DangMax;
	float	m_RMin;
	float   m_GradCons;
	float   m_AzMax;
	//}}AFX_DATA
	float	m_NumMin;						   //������
	CString m_szSeries;
	CString	m_CalName[10];                     //����������
	CString	m_FCName[10];                      //�絼��������
	CString	m_DazName;                         //��б��λ
	CString	m_Az1Name;                         //I�ż��巽λ
	CString	m_DevName;                         //��б��
	CString	m_RBName;                          //��Է�λ
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDIPComputeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CDIPComputeDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIPCOMPUTEDLG_H__C8910D51_D7C9_4D45_ABB1_2E585CE1633E__INCLUDED_)
