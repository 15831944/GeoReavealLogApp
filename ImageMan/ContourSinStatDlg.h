#pragma once


// CContourSinStatDlg �Ի���

class CContourSinStatDlg : public CDialog
{
	DECLARE_DYNAMIC(CContourSinStatDlg)

public:
	CContourSinStatDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CContourSinStatDlg();

// �Ի�������
	enum { IDD = IDD_CONTOUR_SINSTATPARA };
	CComboBox	m_NameComCtrl;
	CString	m_sRxoWell;//��׼�����ʾ���
	CString	m_sRxoName;//ǳ��������

	float m_SinStatWin;//��������
	float m_SinStatRlev;//��������
	float m_SinStatSdep,m_SinStatEdep;//ͳ�ƾ���

	float m_Rm;	//�ཬ������
	float m_ParC;//ˮ��������C
	float m_ParB;//ˮ��������B
	
	void InitComCtrl();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnButton1();
	afx_msg void OnCloseupComboRmfname();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
