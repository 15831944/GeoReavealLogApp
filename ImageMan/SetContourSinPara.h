#pragma once


// CSetContourSinPara 对话框

class CSetContourSinPara : public CDialog
{
	DECLARE_DYNAMIC(CSetContourSinPara)

public:
	CSetContourSinPara(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetContourSinPara();
	int m_iPickupSinWidth;
	BOOL m_bBedding,m_bOpen,m_bFill,m_bCrack,m_bBedding1,m_bFill1;
	CString m_sPickupSinLabel;
// 对话框数据
	enum { IDD = IDD_CONTOUR_SINPARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
