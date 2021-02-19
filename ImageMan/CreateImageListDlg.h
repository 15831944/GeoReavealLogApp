#pragma once


// CCreateImageList 对话框

class CCreateImageListDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateImageListDlg)

public:
	CCreateImageListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCreateImageListDlg();

	CString m_szProjectName;
	CString m_szPorName;
	CString m_szFVDC;//裂缝密度 
	CString m_szFVPA;//裂缝孔隙度曲线

	float	m_MinValue;
	float	m_MinThick;
// 对话框数据
	enum { IDD = IDD_CREATEIMAGELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
