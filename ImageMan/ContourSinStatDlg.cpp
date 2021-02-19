// ContourSinStatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ContourSinStatDlg.h"
#include "FIDio.h"
#include "Database.h"

// CContourSinStatDlg 对话框

IMPLEMENT_DYNAMIC(CContourSinStatDlg, CDialog)
CContourSinStatDlg::CContourSinStatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CContourSinStatDlg::IDD, pParent)
{
	m_SinStatWin=0.3048f;//1英尺
	m_SinStatRlev=0.0254f;//1英寸
	//泥浆电阻率
	m_Rm=0.15f;
	m_ParC=0.004801f;
	m_ParB=0.863f;
	//交互解释统计井段
	m_SinStatSdep=m_SinStatEdep=-9999.f;
}

CContourSinStatDlg::~CContourSinStatDlg()
{
}

void CContourSinStatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RMFWELL, m_sRxoWell);
	DDX_Text(pDX, IDC_EDIT2, m_SinStatWin);
	DDX_Text(pDX, IDC_EDIT3, m_SinStatRlev);
	DDX_Text(pDX, IDC_EDIT4, m_SinStatSdep);
	DDX_Text(pDX, IDC_EDIT5, m_SinStatEdep);
	DDX_Text(pDX, IDC_EDIT6, m_ParC);
	DDX_Text(pDX, IDC_EDIT7, m_ParB);
	DDX_Text(pDX, IDC_EDIT8, m_Rm);
	DDX_CBString(pDX, IDC_COMBO_RMFNAME, m_sRxoName);
	DDX_Control(pDX, IDC_COMBO_RMFNAME, m_NameComCtrl);
}


BEGIN_MESSAGE_MAP(CContourSinStatDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_CBN_CLOSEUP(IDC_COMBO_RMFNAME, OnCloseupComboRmfname)
END_MESSAGE_MAP()


// CContourSinStatDlg 消息处理程序

BOOL CContourSinStatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitComCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CContourSinStatDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = 
	"测井数据(*.FID)|*.FID|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "测井数据(*.FID)", "*.FID", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if(Browse.DoModal()==IDOK)
	{
		m_sRxoWell= Browse.GetPathName();
		GetDlgItem(IDC_RMFWELL)->SetWindowText(m_sRxoWell);
		InitComCtrl();
	}
}

void CContourSinStatDlg::InitComCtrl()
{
	m_NameComCtrl.ResetContent();
	CString Path=m_sRxoWell.Left (m_sRxoWell.ReverseFind ('\\'));
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
		m_dBaseFile.GetField("曲线类型", szBuff);
		Type=szBuff;
		Type.TrimLeft();Type.TrimRight();
		if(Type=="常规")
			m_NameComCtrl.AddString (Name);
	}
	m_dBaseFile.Close();
	m_NameComCtrl.SetWindowText(m_sRxoName);
}

void CContourSinStatDlg::OnCloseupComboRmfname() 
{
	// TODO: Add your control notification handler code here
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
	if (m_sRxoName != str)
		m_sRxoName = str;
	GetDlgItem(IDC_COMBO_RMFNAME)->SetWindowText(m_sRxoName);
}