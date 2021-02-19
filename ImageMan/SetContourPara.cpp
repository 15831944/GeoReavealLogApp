// SetContourPara.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageMan.h"
#include "SetContourPara.h"
#include ".\setcontourpara.h"


// CSetContourPara 对话框

IMPLEMENT_DYNAMIC(CSetContourPara, CDialog)
CSetContourPara::CSetContourPara(CWnd* pParent /*=NULL*/)
	: CDialog(CSetContourPara::IDD, pParent)
{
	// 特征值聚类数
	m_KPara=90;
	//仅拾取大于m_HoleMinAcr的孔洞
	m_HoleMinAcr=25.4f;
	m_bHoleMinAcr=FALSE;
	//仅拾取小于m_HoleMaxAcr的孔洞
	m_bHoleMaxAcr=FALSE;
	m_HoleMaxAcr=254.f;

	//仅拾取圆度大于m_HoleMinRound的孔洞
	m_bMinRound=FALSE;
	m_HoleMinRound=70.f;
	m_bMaxRound=FALSE;
	m_HoleMaxRound=30.f;

	m_DataMessage=_T("");
}

CSetContourPara::~CSetContourPara()
{
}

void CSetContourPara::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_KPara);
	DDV_MinMaxUInt(pDX, m_KPara, 0, 255);
	DDX_Text(pDX, IDC_EDIT2, m_HoleMaxAcr);
	DDX_Text(pDX, IDC_EDIT3, m_HoleMinAcr);
	DDX_Check(pDX, IDC_CHECK1, m_bHoleMaxAcr);
	DDX_Check(pDX, IDC_CHECK2, m_bHoleMinAcr);
	DDX_Check(pDX, IDC_CHECK4, m_bMinRound);
	DDX_Text(pDX, IDC_EDIT5, m_HoleMinRound);
	DDX_Check(pDX, IDC_CHECK5,m_bMaxRound);
	DDX_Text(pDX, IDC_EDIT6, m_HoleMaxRound);
}

BEGIN_MESSAGE_MAP(CSetContourPara, CDialog)
END_MESSAGE_MAP()


// CSetContourPara 消息处理程序

BOOL CSetContourPara::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_DataMessage.IsEmpty())
	{
		CRect rect;
		GetWindowRect(&rect);
		rect.bottom-=135;
		MoveWindow(&rect);
	}
	else
		GetDlgItem(IDC_DATA_MESSAGE)->SetWindowText(m_DataMessage);

	return TRUE;  // return TRUE unless you set the focus to a control
}
