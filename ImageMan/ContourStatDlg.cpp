// ContourStatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ContourStatDlg.h"


// CContourStatDlg 对话框

IMPLEMENT_DYNAMIC(CContourStatDlg, CDialog)
CContourStatDlg::CContourStatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CContourStatDlg::IDD, pParent)
{
	m_WellName=_T("");
	//交互解释统计参数
	m_StatWin=0.3048f;//1英尺
	m_StatRlev=0.0254f;//1英寸
	//颗粒孔洞直径频谱刻度区间数
	m_nSpcNum=100;
	//最大颗粒孔洞直径
	m_ContourMax=100.f;

	//以颗粒孔洞直径进行分类统计
	m_StatDiam1=5.f;
	m_StatDiam2=10.f;
	
	//孔洞刻度关系参数 y=A*x+B
	//裂缝刻度关系参数 y=C*x+D
	m_fA=1.f;
	m_fB=0.f;
	m_fC=1.f;
	m_fD=0.f;
	//连通指数，电导率最大最小值
	m_ImageMax=1024.f;
	m_ImageMin=440.f;
	//交互解释统计井段
	m_StatSdep=m_StatEdep=-9999.f;
}

CContourStatDlg::~CContourStatDlg()
{
}

void CContourStatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_WellName);
	DDX_Text(pDX, IDC_EDIT2, m_StatWin);
	DDX_Text(pDX, IDC_EDIT3, m_StatRlev);
	DDX_Text(pDX, IDC_EDIT4, m_nSpcNum);
	DDX_Text(pDX, IDC_EDIT5, m_ContourMax);
	DDX_Text(pDX, IDC_EDIT6, m_StatDiam1);
	DDX_Text(pDX, IDC_EDIT7, m_StatDiam2);
	DDX_Text(pDX, IDC_EDIT8, m_StatDiam3);

	DDX_Text(pDX, IDC_EDIT9, m_StatSdep);
	DDX_Text(pDX, IDC_EDIT10, m_StatEdep);

	DDX_Text(pDX, IDC_EDIT11, m_fA);
	DDX_Text(pDX, IDC_EDIT12, m_fB);
	DDX_Text(pDX, IDC_EDIT13, m_fC);
	DDX_Text(pDX, IDC_EDIT14, m_fD);

	DDX_Text(pDX, IDC_EDIT15, m_ImageMax);
	DDX_Text(pDX, IDC_EDIT16, m_ImageMin);
}


BEGIN_MESSAGE_MAP(CContourStatDlg, CDialog)
END_MESSAGE_MAP()


// CContourStatDlg 消息处理程序
