#pragma once


// CContourStatDlg 对话框

class CContourStatDlg : public CDialog
{
	DECLARE_DYNAMIC(CContourStatDlg)

public:
	CContourStatDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CContourStatDlg();

// 对话框数据
	enum { IDD = IDD_CONTOUR_STATPARA };
	
	CString m_WellName;
	float m_StatWin;//滑动窗长
	float m_StatRlev;//滑动步长
	int m_nSpcNum;//频谱曲线横向刻度区间数
	float m_ContourMax;//最大颗粒孔洞直径
	//以颗粒孔洞直径进行分类统计
	float m_StatDiam1;//小于m_StatDiam1的为小孔
	float m_StatDiam2;//大于m_StatDiam1为中孔，小于m_StatDiam2
	float m_StatDiam3;//大于m_StatDiam2为大孔，小于m_StatDiam3大于m_StatDiam3为溶洞
	//孔洞刻度关系参数 y=A*x+B
	//裂缝刻度关系参数 y=C*x+D
	float m_fA,m_fB,m_fC,m_fD;
	//连通指数，电导率最大最小值
	float m_ImageMax,m_ImageMin;
	float m_StatSdep,m_StatEdep;//统计井段
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
