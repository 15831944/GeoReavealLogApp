// MarkDipCorre.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "MarkDipCorre.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMarkDipCorre dialog


CMarkDipCorre::CMarkDipCorre(CWnd* pParent /*=NULL*/)
	: CDialog(CMarkDipCorre::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMarkDipCorre)
	m_sAZ1Name = _T("");
	m_sSeries = _T("");
	m_sRBName = _T("");
	m_sDEVName = _T("");
	m_sDAZName = _T("");
	m_fDip1 = 0.0f;
	m_fDir1 = 0.0f;
	//}}AFX_DATA_INIT
}


void CMarkDipCorre::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarkDipCorre)
	DDX_Control(pDX, IDC_COMBO_SERIES, m_SeriesCtrl);
	DDX_CBString(pDX, IDC_COMBO2, m_sAZ1Name);
	DDX_CBString(pDX, IDC_COMBO_SERIES, m_sSeries);
	DDX_CBString(pDX, IDC_COMBO3, m_sRBName);
	DDX_CBString(pDX, IDC_COMBO4, m_sDEVName);
	DDX_CBString(pDX, IDC_COMBO5, m_sDAZName);
	DDX_Text(pDX, IDC_EDIT1, m_fDip1);
	DDV_MinMaxFloat(pDX, m_fDip1, 0.f, 90.f);
	DDX_Text(pDX, IDC_EDIT2, m_fDir1);
	DDV_MinMaxFloat(pDX, m_fDir1, 0.f, 360.f);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BROWS, m_wndBrowseCtrl);
}


BEGIN_MESSAGE_MAP(CMarkDipCorre, CDialog)
	//{{AFX_MSG_MAP(CMarkDipCorre)
	ON_CBN_CLOSEUP(IDC_COMBO_SERIES, OnCloseupComboSeries)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarkDipCorre message handlers

BOOL CMarkDipCorre::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	DWORD dwStyle = m_wndBrowseCtrl.GetButtonStyle();
	dwStyle |=BC_CTL_ALLOWEDIT;
	dwStyle |= BC_BTN_ICON;                    
	dwStyle |= BC_BTN_FLAT;                    
	dwStyle &= ~BC_ICO_ARROWFOLDER;            
	dwStyle &= ~BC_ICO_FOLDER;
	dwStyle |= BC_ICO_EXPLORER;                
	dwStyle |= BC_CTL_FOLDERSONLY;             
	m_wndBrowseCtrl.SetButtonStyle(dwStyle);
	m_wndBrowseCtrl.SetDialogTitle(_T("选择井名"));
	m_wndBrowseCtrl.SetDialogBanner(_T("请在下面目录树中选择井名"));
	m_wndBrowseCtrl.SetFolderFlags(BIF_EDITBOX); 

	m_WellName=m_WellName.Left (m_WellName.ReverseFind ('\\'));
	m_wndBrowseCtrl.SetPathName(m_WellName);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMarkDipCorre::OnCloseupComboSeries() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int nIndex = m_SeriesCtrl.GetCurSel();
	if (nIndex == CB_ERR)
		m_SeriesCtrl.GetWindowText(m_sSeries);
	else
		m_SeriesCtrl.GetLBText(nIndex, m_sSeries);
	m_sSeries.MakeUpper();
	CString ss;
	if(m_sSeries=="EMI")
	{
		m_sAZ1Name = _T("");
		m_sRBName = _T("");
		m_sDEVName = _T("");
		m_sDAZName = _T("");
	}
	else if(m_sSeries=="STARII")
	{
		m_sAZ1Name="AZDPQH";
		m_sRBName="RBDPQH";
		m_sDEVName="DEVDPQH";
		m_sDAZName="DAZDPQH";
	}
	else
	{
		m_sAZ1Name = _T("");
		m_sRBName = _T("");
		m_sDEVName = _T("");
		m_sDAZName = _T("");
	}	
	UpdateData(FALSE);	
}
