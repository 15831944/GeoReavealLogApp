// SetContourSinPara.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageMan.h"
#include "SetContourSinPara.h"


// CSetContourSinPara 对话框

IMPLEMENT_DYNAMIC(CSetContourSinPara, CDialog)
CSetContourSinPara::CSetContourSinPara(CWnd* pParent /*=NULL*/)
	: CDialog(CSetContourSinPara::IDD, pParent)
{
	m_iPickupSinWidth=100;
	m_sPickupSinLabel=_T("");
	m_bBedding=m_bOpen=m_bFill=FALSE;
	m_bCrack=m_bBedding1=m_bFill1=FALSE;
}

CSetContourSinPara::~CSetContourSinPara()
{
}

void CSetContourSinPara::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_iPickupSinWidth);
	DDX_Check(pDX, IDC_CHECK1,m_bBedding);
	DDX_Check(pDX, IDC_CHECK2,m_bOpen);
	DDX_Check(pDX, IDC_CHECK3,m_bFill);
	DDX_Check(pDX, IDC_CHECK4,m_bCrack);
	DDX_Check(pDX, IDC_CHECK9,m_bBedding1);
	DDX_Check(pDX, IDC_CHECK10,m_bFill1);
}


BEGIN_MESSAGE_MAP(CSetContourSinPara, CDialog)
END_MESSAGE_MAP()

// CSetContourSinPara 消息处理程序

BOOL CSetContourSinPara::OnInitDialog()
{
	//m_bBedding,m_bOpen,m_bFill,m_bCrack,m_bBedding1,m_bFill1;
	if(m_sPickupSinLabel.Find("层理")>-1)
		m_bBedding=TRUE;
	if(m_sPickupSinLabel.Find("张开缝")>-1)
		m_bOpen=TRUE;
	if(m_sPickupSinLabel.Find("充填缝")>-1)
		m_bFill=TRUE;
	if(m_sPickupSinLabel.Find("压裂缝")>-1)
		m_bCrack=TRUE;
	if(m_sPickupSinLabel.Find("层间缝")>-1)
		m_bBedding1=TRUE;
	if(m_sPickupSinLabel.Find("半充填缝")>-1)
		m_bFill1=TRUE;
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSetContourSinPara::OnOK()
{
	CDialog::OnOK();
	m_sPickupSinLabel=_T("");
	if(m_bBedding) m_sPickupSinLabel+="层理";
	if(m_bOpen) m_sPickupSinLabel+="张开缝";
	if(m_bFill) m_sPickupSinLabel+="充填缝";
	if(m_bCrack) m_sPickupSinLabel+="压裂缝";
	if(m_bBedding1) m_sPickupSinLabel+="层间缝";
	if(m_bFill1) m_sPickupSinLabel+="半充填缝";
}
