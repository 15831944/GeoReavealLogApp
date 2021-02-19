#pragma once


// CContourStatDlg �Ի���

class CContourStatDlg : public CDialog
{
	DECLARE_DYNAMIC(CContourStatDlg)

public:
	CContourStatDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CContourStatDlg();

// �Ի�������
	enum { IDD = IDD_CONTOUR_STATPARA };
	
	CString m_WellName;
	float m_StatWin;//��������
	float m_StatRlev;//��������
	int m_nSpcNum;//Ƶ�����ߺ���̶�������
	float m_ContourMax;//�������׶�ֱ��
	//�Կ����׶�ֱ�����з���ͳ��
	float m_StatDiam1;//С��m_StatDiam1��ΪС��
	float m_StatDiam2;//����m_StatDiam1Ϊ�пף�С��m_StatDiam2
	float m_StatDiam3;//����m_StatDiam2Ϊ��ף�С��m_StatDiam3����m_StatDiam3Ϊ�ܶ�
	//�׶��̶ȹ�ϵ���� y=A*x+B
	//�ѷ�̶ȹ�ϵ���� y=C*x+D
	float m_fA,m_fB,m_fC,m_fD;
	//��ָͨ�����絼�������Сֵ
	float m_ImageMax,m_ImageMin;
	float m_StatSdep,m_StatEdep;//ͳ�ƾ���
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
