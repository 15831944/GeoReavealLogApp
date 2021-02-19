// IamgeToneupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flowchart.h"
#include "ImageToneupDlg.h"
#include ".\imagetoneupdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageToneupDlg dialog


CImageToneupDlg::CImageToneupDlg(CImageToneup *pObj,CWnd* pParent /*=NULL*/)
	: CDialog(CImageToneupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImageToneupDlg)
	m_bGrads = FALSE;
	m_bFreq = FALSE;
	m_bFilter = FALSE;
	m_Message = _T("");
	m_bMusic = FALSE;
	//}}AFX_DATA_INIT
	m_pObj=pObj;
	m_MaxPos=294;m_MinVal=6;
	m_WinLongth=0.3048f;
	m_RlevLongth=0.1524f;
}


void CImageToneupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageToneupDlg)
	DDX_Check(pDX, IDC_GRADS, m_bGrads);
	DDX_Check(pDX, IDC_FREQ, m_bFreq);
	DDX_Check(pDX, IDC_EXPEND, m_bRepair);
	DDX_Text(pDX, IDC_MESSAGEBOX, m_Message);
	DDX_Check(pDX, IDC_MUSIC, m_bMusic);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT2, m_MinVal);
	DDV_MinMaxInt(pDX, m_MinVal, 0,30);
	DDX_Text(pDX, IDC_EDIT8,m_nLevel);
	DDV_MinMaxUInt(pDX, m_nLevel, 2, 1024);
	DDX_Text(pDX, IDC_EDIT5,m_radius);
	DDX_Text(pDX, IDC_WINLENGTH, m_WinLongth);
	DDV_MinMaxFloat(pDX, m_WinLongth, 0.254f, 50.8f);
	DDX_Text(pDX, IDC_WINLENGTH2, m_RlevLongth);
	DDV_MinMaxFloat(pDX, m_WinLongth, 0.002f, 3.048f);
	DDX_Check(pDX,IDC_CHECK1,m_bFilter);
}


BEGIN_MESSAGE_MAP(CImageToneupDlg, CDialog)
	//{{AFX_MSG_MAP(CImageToneupDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_GRADS, OnBnClickedGrads)
	ON_BN_CLICKED(IDC_MUSIC, OnBnClickedMusic)
	ON_BN_CLICKED(IDC_FREQ, OnBnClickedFreq)
	ON_BN_CLICKED(IDC_EXPEND, OnBnClickedExpend)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageToneupDlg message handlers

BOOL CImageToneupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
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
	GetDlgItem(IDC_EDIT2)->EnableWindow(m_bGrads||m_bFreq);
	GetDlgItem(IDC_EDIT8)->EnableWindow(m_bMusic);
	GetDlgItem(IDC_EDIT5)->EnableWindow(m_bRepair);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImageToneupDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	m_pObj->m_bGrads=m_bGrads;	
	m_pObj->m_bFreq=m_bFreq;
	m_pObj->m_bFilter=m_bFilter;
	m_pObj->m_bMusic=m_bMusic;
	m_pObj->m_MaxPos=m_MaxPos;
	m_pObj->m_MinVal=m_MinVal;
	m_pObj->m_bRepair=m_bRepair;
	m_pObj->m_nLevel=m_nLevel;
	m_pObj->m_radius=m_radius;	
	m_pObj->m_WinLongth=m_WinLongth;
	m_pObj->m_RlevLongth=m_RlevLongth;
	CDialog::OnOK();
}

void CImageToneupDlg::OnBnClickedGrads()
{
	UpdateData();
	GetDlgItem(IDC_EDIT2)->EnableWindow(m_bGrads||m_bFreq);
}

void CImageToneupDlg::OnBnClickedMusic()
{
	UpdateData();
	GetDlgItem(IDC_EDIT8)->EnableWindow(m_bMusic);
}

void CImageToneupDlg::OnBnClickedFreq()
{
	UpdateData();
	GetDlgItem(IDC_EDIT2)->EnableWindow(m_bGrads||m_bFreq);
}

void CImageToneupDlg::OnBnClickedExpend()
{
	UpdateData();
	GetDlgItem(IDC_EDIT5)->EnableWindow(m_bRepair);
}
