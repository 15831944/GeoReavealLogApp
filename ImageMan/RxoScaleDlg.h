#ifndef _RMFSCALEDLG_H__
#define _RMFSCALEDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RMFScaleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRxoScaleDlg dialog

class CRxoScaleDlg : public CDialog
{
// Construction
public:
	CRxoScaleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRxoScaleDlg)
	enum { IDD = IDD_SCALE_DLG };
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

	CString m_szWell,m_szName;
	int		m_DataNum,m_DataType;
	float	m_StProDep,m_EnProDep,m_Rlev;
	BOOL CreateCrackImg();
	BOOL CreateCrackImg2();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRxoScaleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	void InitComCtrl();
	void GetMaxMinValue();
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRxoScaleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	afx_msg void OnCloseupComboRmfname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RMFSCALEDLG_H__67D294E5_F617_45DD_89B1_A2D611FC06D8__INCLUDED_)
