#pragma once


// CContourSinStatDlg 对话框

class CContourSinStatDlg : public CDialog
{
	DECLARE_DYNAMIC(CContourSinStatDlg)

public:
	CContourSinStatDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CContourSinStatDlg();

// 对话框数据
	enum { IDD = IDD_CONTOUR_SINSTATPARA };
	CComboBox	m_NameComCtrl;
	CString	m_sRxoWell;//标准电阻率井名
	CString	m_sRxoName;//浅侧向曲线

	float m_SinStatWin;//滑动窗长
	float m_SinStatRlev;//滑动步长
	float m_SinStatSdep,m_SinStatEdep;//统计井段

	float m_Rm;	//泥浆电阻率
	float m_ParC;//水动力参数C
	float m_ParB;//水动力参数B
	
	void InitComCtrl();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnButton1();
	afx_msg void OnCloseupComboRmfname();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
