// RMFScaleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flowchart.h"
#include "RMFScaleDlg.h"
#include "Database.h"
#include "FIDio.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRMFScaleDlg dialog


CRMFScaleDlg::CRMFScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRMFScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRMFScaleDlg)
	m_bResistance = TRUE;
	m_szRMFWell = _T("");
	m_szRMFName = _T("RXO");
	//}}AFX_DATA_INIT
	m_iImage=0;
}


void CRMFScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRMFScaleDlg)
	DDX_Check(pDX, IDC_CHECK1, m_bResistance);
	
	DDX_Text(pDX, IDC_RMFWELL, m_szRMFWell);
	DDX_CBString(pDX, IDC_COMBO_RMFNAME, m_szRMFName);
	DDX_Control(pDX, IDC_COMBO_RMFNAME, m_NameComCtrl);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT1, m_fWin);
	DDX_Text(pDX, IDC_EDIT5, m_fRlev);
	DDX_Text(pDX, IDC_EDIT3, m_LeftScal);
	DDX_Text(pDX, IDC_EDIT4, m_RightScal);
	DDX_Radio(pDX,IDC_RADIO1, m_iImage);
}


BEGIN_MESSAGE_MAP(CRMFScaleDlg, CDialog)
	//{{AFX_MSG_MAP(CRMFScaleDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_CBN_CLOSEUP(IDC_COMBO_RMFNAME, OnCloseupComboRmfname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRMFScaleDlg message handlers

BOOL CRMFScaleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	CRect rect;
	// get the current window size and position
	GetWindowRect( rect );
	int h=rect.Height ();
	int w=rect.Width ();
	rect.left =(GetSystemMetrics(SM_CXSCREEN)-w)/2;
	rect.top =(GetSystemMetrics(SM_CYSCREEN)-h)/2;
	rect.bottom =rect.top+h;
	rect.right =rect.left+w;
	MoveWindow(&rect);
	m_Shadow.Create(GetSafeHwnd());
	InitComCtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRMFScaleDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = 
	"测井数据(*.FID)|*.FID|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "测井数据(*.FID)", "*.FID", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if(Browse.DoModal()==IDOK)
	{
		CString str = Browse.GetPathName();
		int x=str.ReverseFind('.');
		if(x>0)
		{
			m_szRMFWell=str.Left(x);
			GetDlgItem(IDC_RMFWELL)->SetWindowText(m_szRMFWell);
			InitComCtrl();
		}
	}
}

void CRMFScaleDlg::InitComCtrl()
{
	m_NameComCtrl.ResetContent();
	CString Path=m_szRMFWell.Left (m_szRMFWell.ReverseFind ('\\'));
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
	m_NameComCtrl.SetWindowText(m_szRMFName);
}

void CRMFScaleDlg::OnCloseupComboRmfname() 
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
	if(m_szRMFName != str)
		m_szRMFName = str;
	GetDlgItem(IDC_COMBO_RMFNAME)->SetWindowText(m_szRMFName);
}