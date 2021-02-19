#pragma once
#include "WndShadow.h"
// CPORComputDlg 对话框

class CPORComputDlg : public CDialog
{
	DECLARE_DYNAMIC(CPORComputDlg)

public:
	CPORComputDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPORComputDlg();

	BOOL	m_bSpectFilter;
	BOOL    m_bResistance;                 //电导率成像数据
	float   m_Rm,m_XM,m_XN,m_XA,m_XB,m_StatWin,m_StatRlev,m_PORMax,m_MinStat;
	CString m_szExtPorWell,m_szExtPorName;
	CComboBox m_NameComCtrl;
// 对话框数据
	enum { IDD = IDD_POROSPECT_DLG };
	void InitComCtrl();
protected:
	CWndShadow m_Shadow;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnCbnCloseupCombo1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
};

#pragma once


