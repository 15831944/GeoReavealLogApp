#pragma once
// CCreateSpectDlg �Ի���

class CCreateSpectDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateSpectDlg)

public:
	CCreateSpectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCreateSpectDlg();

	int		m_SpcNum,m_DataNum,m_DataType;
	BOOL	m_bSpectFilter;
	BOOL    m_bResistance;                 //�絼�ʳ�������
	float   m_Rm,m_XM,m_XN,m_XA,m_XB,m_StatWin,m_StatRlev,m_PORMax,m_MinStat;
	CString m_szExtPorWell,m_szExtPorName;
	CString m_szExtRxoWell,m_szExtRxoName;

	float m_StProDep,m_EnProDep,m_Rlev;
	CString m_szWell,m_szName;
	CComboBox m_NameCtrl,m_NamePorCtrl,m_NameRxoCtrl;
	CProgressCtrl m_ProgressCtrl;
// �Ի�������
	enum { IDD = IDD_CREATESPECT_DLG };
	void InitComCtrl();
	void InitComCtrl1();
	void InitComCtrl2();

	void PumpMessages();
	void WellRefresh();
	BOOL Curve_Filter(int nFilter);
	BOOL CreatSpect(CString InName,CString OutName);
	BOOL CreatSpect2(CString InName,CString OutName);
	BOOL StaticSectionPor(CString InName);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnCloseupCombo1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCbnCloseupCombo2();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};

#pragma once


