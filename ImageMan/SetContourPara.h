#pragma once


// CSetContourPara �Ի���

class CSetContourPara : public CDialog
{
	DECLARE_DYNAMIC(CSetContourPara)

public:
	CSetContourPara(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetContourPara();

// �Ի�������
	enum { IDD = IDD_CONTOUR_PARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ����ֵ������
	int m_KPara;
	float m_HoleMinAcr;
	//��ʰȡС��m_HoleMaxAcr�Ŀ׶�
	BOOL  m_bHoleMaxAcr;
	//��ʰȡ����m_HoleMinAcr�Ŀ׶�
	float m_HoleMaxAcr;
	BOOL  m_bHoleMinAcr;

	//��ʰȡԲ�ȴ���m_HoleMinRound�Ŀ׶�
	BOOL m_bMinRound;
	float m_HoleMinRound;
	BOOL m_bMaxRound;
	float m_HoleMaxRound;

	CString m_DataMessage;
	virtual BOOL OnInitDialog();
};
