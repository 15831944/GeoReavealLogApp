#pragma once


// CDiffrenceDlg �Ի���

class CDiffrenceDlg : public CDialog
{
	DECLARE_DYNAMIC(CDiffrenceDlg)

public:
	CDiffrenceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDiffrenceDlg();

// �Ի�������
	enum { IDD = IDD_DIFF_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	float m_DaltaDep;
	CString m_szDifName,m_szStr;
	virtual BOOL OnInitDialog();
};
