#ifndef _RMFSCALEDLG_H__
#define _RMFSCALEDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RMFScaleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRMFScaleDlg dialog
#include "WndShadow.h"

class CRMFScaleDlg : public CDialog
{
// Construction
public:
	CRMFScaleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRMFScaleDlg)
	enum { IDD = IDD_RMFSCALEDLG };
	BOOL	m_bResistance;
	CString	m_szRMFWell;//��׼�����ʾ���
	CString	m_szRMFName;//ǳ����̶� �������ɸ߷ֱ��ʵ���������
	CComboBox	m_NameComCtrl;
	//}}AFX_DATA
	BOOL	m_bCreateImage;         //���ɿ̶�ͼ�񵽲⾮��Ŀ��
	BOOL	m_bCreateRxo;           //�ϳɸ߷ֱ����������

	float m_fRlev;					//��������
	float m_fWin;                   //��������
	float m_LeftScal,m_RightScal;   //ǳ����̶ȷ�Χ
	int	  m_iImage;                 //ѡ��̶�ͼ��0--��̬ͼ��   1--��̬ͼ��
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRMFScaleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	float m_EnProDep,m_StProDep;
	void InitComCtrl();
	void GetMaxMinValue();
// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CRMFScaleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	afx_msg void OnCloseupComboRmfname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RMFSCALEDLG_H__67D294E5_F617_45DD_89B1_A2D611FC06D8__INCLUDED_)
