// SetContourSinPara.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImageMan.h"
#include "SetContourSinPara.h"


// CSetContourSinPara �Ի���

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

// CSetContourSinPara ��Ϣ�������

BOOL CSetContourSinPara::OnInitDialog()
{
	//m_bBedding,m_bOpen,m_bFill,m_bCrack,m_bBedding1,m_bFill1;
	if(m_sPickupSinLabel.Find("����")>-1)
		m_bBedding=TRUE;
	if(m_sPickupSinLabel.Find("�ſ���")>-1)
		m_bOpen=TRUE;
	if(m_sPickupSinLabel.Find("�����")>-1)
		m_bFill=TRUE;
	if(m_sPickupSinLabel.Find("ѹ�ѷ�")>-1)
		m_bCrack=TRUE;
	if(m_sPickupSinLabel.Find("����")>-1)
		m_bBedding1=TRUE;
	if(m_sPickupSinLabel.Find("������")>-1)
		m_bFill1=TRUE;
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSetContourSinPara::OnOK()
{
	CDialog::OnOK();
	m_sPickupSinLabel=_T("");
	if(m_bBedding) m_sPickupSinLabel+="����";
	if(m_bOpen) m_sPickupSinLabel+="�ſ���";
	if(m_bFill) m_sPickupSinLabel+="�����";
	if(m_bCrack) m_sPickupSinLabel+="ѹ�ѷ�";
	if(m_bBedding1) m_sPickupSinLabel+="����";
	if(m_bFill1) m_sPickupSinLabel+="������";
}
