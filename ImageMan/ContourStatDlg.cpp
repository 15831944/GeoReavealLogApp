// ContourStatDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ContourStatDlg.h"


// CContourStatDlg �Ի���

IMPLEMENT_DYNAMIC(CContourStatDlg, CDialog)
CContourStatDlg::CContourStatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CContourStatDlg::IDD, pParent)
{
	m_WellName=_T("");
	//��������ͳ�Ʋ���
	m_StatWin=0.3048f;//1Ӣ��
	m_StatRlev=0.0254f;//1Ӣ��
	//�����׶�ֱ��Ƶ�׿̶�������
	m_nSpcNum=100;
	//�������׶�ֱ��
	m_ContourMax=100.f;

	//�Կ����׶�ֱ�����з���ͳ��
	m_StatDiam1=5.f;
	m_StatDiam2=10.f;
	
	//�׶��̶ȹ�ϵ���� y=A*x+B
	//�ѷ�̶ȹ�ϵ���� y=C*x+D
	m_fA=1.f;
	m_fB=0.f;
	m_fC=1.f;
	m_fD=0.f;
	//��ָͨ�����絼�������Сֵ
	m_ImageMax=1024.f;
	m_ImageMin=440.f;
	//��������ͳ�ƾ���
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


// CContourStatDlg ��Ϣ�������
