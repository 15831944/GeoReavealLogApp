#pragma once


// CCreateImageList �Ի���

class CCreateImageListDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateImageListDlg)

public:
	CCreateImageListDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCreateImageListDlg();

	CString m_szProjectName;
	CString m_szPorName;
	CString m_szFVDC;//�ѷ��ܶ� 
	CString m_szFVPA;//�ѷ��϶������

	float	m_MinValue;
	float	m_MinThick;
// �Ի�������
	enum { IDD = IDD_CREATEIMAGELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
