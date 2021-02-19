#pragma once


// CDiffrenceDlg 对话框

class CDiffrenceDlg : public CDialog
{
	DECLARE_DYNAMIC(CDiffrenceDlg)

public:
	CDiffrenceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDiffrenceDlg();

// 对话框数据
	enum { IDD = IDD_DIFF_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	float m_DaltaDep;
	CString m_szDifName,m_szStr;
	virtual BOOL OnInitDialog();
};
