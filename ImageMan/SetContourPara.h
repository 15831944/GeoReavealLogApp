#pragma once


// CSetContourPara 对话框

class CSetContourPara : public CDialog
{
	DECLARE_DYNAMIC(CSetContourPara)

public:
	CSetContourPara(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetContourPara();

// 对话框数据
	enum { IDD = IDD_CONTOUR_PARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 特征值聚类数
	int m_KPara;
	float m_HoleMinAcr;
	//仅拾取小于m_HoleMaxAcr的孔洞
	BOOL  m_bHoleMaxAcr;
	//仅拾取大于m_HoleMinAcr的孔洞
	float m_HoleMaxAcr;
	BOOL  m_bHoleMinAcr;

	//仅拾取圆度大于m_HoleMinRound的孔洞
	BOOL m_bMinRound;
	float m_HoleMinRound;
	BOOL m_bMaxRound;
	float m_HoleMaxRound;

	CString m_DataMessage;
	virtual BOOL OnInitDialog();
};
