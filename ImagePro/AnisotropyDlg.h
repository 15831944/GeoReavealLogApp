#ifndef _ANISOTROPYDLG_H__
#define _ANISOTROPYDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnisotropyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnisotropyDlg dialog
#include "WndShadow.h"

class CAnisotropyDlg : public CDialog
{
// Construction
public:
	CAnisotropyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnisotropyDlg)
	enum { IDD = IDD_ANISOTROPYDLG };
	float	m_AzMax;
	float	m_RxyMin;
	//}}AFX_DATA
	float	m_fWin;                   //��������
	float	m_fRlev;                  //��������
	float	m_NumMin;                 //������
	int		m_iImage;				  //ͼ��ѡ��1-��̬  2-��̬  3-�̶�
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnisotropyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CAnisotropyDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANISOTROPYDLG_H__67D294E5_F617_45DD_89B1_A2D611FC06D8__INCLUDED_)
