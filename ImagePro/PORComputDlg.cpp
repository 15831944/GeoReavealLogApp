// PORComputDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "FlowChart.h"
#include "PORComputDlg.h"
#include "Database.h"
#include "FIDio.h"

// CPORComputDlg �Ի���

IMPLEMENT_DYNAMIC(CPORComputDlg, CDialog)
CPORComputDlg::CPORComputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPORComputDlg::IDD, pParent)
{
	m_StatWin=0.3048f;
	m_bSpectFilter=TRUE;
	m_bResistance=FALSE;
}

CPORComputDlg::~CPORComputDlg()
{
}

void CPORComputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT18, m_StatWin);
	DDX_Text(pDX, IDC_EDIT19,m_StatRlev);
	DDX_Text(pDX, IDC_EDIT14,m_Rm);
	DDX_Text(pDX, IDC_EDIT10, m_XM);
	DDX_Text(pDX, IDC_EDIT11, m_XN);
	DDX_Text(pDX, IDC_EDIT13, m_XA);
	DDX_Text(pDX, IDC_EDIT15, m_XB);
	DDX_Text(pDX,IDC_EDIT20,m_PORMax);
	DDX_Text(pDX,IDC_EDIT21,m_MinStat);
	DDX_Text(pDX, IDC_EDIT2, m_szExtPorWell);
	DDX_Control(pDX, IDC_COMBO1, m_NameComCtrl);
	DDX_CBString(pDX, IDC_COMBO1, m_szExtPorName);
	DDX_Check(pDX, IDC_CHECK1, m_bSpectFilter);
}


BEGIN_MESSAGE_MAP(CPORComputDlg, CDialog)
	ON_CBN_CLOSEUP(IDC_COMBO1, OnCbnCloseupCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CPORComputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_szExtPorWell!="")
		InitComCtrl();
	// TODO: Add extra initialization here
	m_Shadow.Create(GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPORComputDlg::InitComCtrl()
{
	//��ʼ����϶��������ѡ���
	m_NameComCtrl.ResetContent();
	CString Path=m_szExtPorWell.Left (m_szExtPorWell.ReverseFind ('\\'));
	CDbaseFile m_dBaseFile;
	if(m_dBaseFile.Open(Path+"\\Curve.dbf")!=DBASE_SUCCESS)
		return;
	char szBuff[255];
	CString Name,Type;
	for(int rc=m_dBaseFile.GetFirstRecord();rc==DBASE_SUCCESS;rc=m_dBaseFile.GetNextRecord())
  	{
		if (m_dBaseFile.IsRecordDeleted())
			continue;
		m_dBaseFile.GetField(1, szBuff);
		Name=szBuff;
		Name.TrimLeft();Name.TrimRight();
		m_dBaseFile.GetField("��������", szBuff);
		Type=szBuff;
		Type.TrimLeft();Type.TrimRight();
		if(Type=="����")
			m_NameComCtrl.AddString (Name);
	}
	m_dBaseFile.Close();
	m_NameComCtrl.SetWindowText(m_szExtPorName);
}
// CPORComputDlg ��Ϣ�������

void CPORComputDlg::OnCbnCloseupCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	int nIndex = m_NameComCtrl.GetCurSel();
	if (nIndex == CB_ERR)
	{
		m_NameComCtrl.GetWindowText(str);
		return;
	}
	else
		m_NameComCtrl.GetLBText(nIndex, str);
	str.MakeUpper();
	if (m_szExtPorName != str)
		m_szExtPorName = str;
	m_NameComCtrl.SetWindowText(m_szExtPorName);
}

void CPORComputDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static char BASED_CODE szFilter[] = 
	"�⾮����(*.FID)|*.FID|�����ļ�(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "�⾮����(*.FID)", "*.FID", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "���ļ�";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if (Browse.DoModal()==IDOK)
	{
		CString str = Browse.GetPathName();
		int x=str.ReverseFind('.');
		if(x>0)
		{
			m_szExtPorWell=str.Left(x);
			GetDlgItem(IDC_EDIT2)->SetWindowText(m_szExtPorWell);
			InitComCtrl();
		}
	}
}


void CPORComputDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}
