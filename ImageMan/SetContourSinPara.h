#pragma once


// CSetContourSinPara �Ի���

class CSetContourSinPara : public CDialog
{
	DECLARE_DYNAMIC(CSetContourSinPara)

public:
	CSetContourSinPara(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetContourSinPara();
	int m_iPickupSinWidth;
	BOOL m_bBedding,m_bOpen,m_bFill,m_bCrack,m_bBedding1,m_bFill1;
	CString m_sPickupSinLabel;
// �Ի�������
	enum { IDD = IDD_CONTOUR_SINPARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
