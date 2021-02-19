// TextList.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TextList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextList dialog


CTextList::CTextList(const char *WellName,const char *CurveName,CWnd* pParent /*=NULL*/)
	: CDialog(CTextList::IDD, pParent)
{
	m_WellName = WellName;
	m_CurveName=CurveName;
	//{{AFX_DATA_INIT(CTextList)
	//}}AFX_DATA_INIT
}


void CTextList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextList)
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_TOTALLGRID, m_TotallGrid);
	DDX_GridControl(pDX, IDC_LAYERGRID, m_LayerGrid);
}


BEGIN_MESSAGE_MAP(CTextList, CDialog)
	//{{AFX_MSG_MAP(CTextList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextList message handlers

BOOL CTextList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString m_FileName;
	m_FileName=m_WellName+".STAT";
	GetDlgItem(IDC_STATIC1)->SetWindowText(m_FileName);
	m_FileName=m_WellName+m_CurveName+"STAT.txt";
	GetDlgItem(IDC_STATIC2)->SetWindowText(m_FileName);

	char PathName[256];
	strcpy(PathName,m_WellName);strcat(PathName,m_CurveName);
	char fpstr[256];
	
	m_TotallGrid.SetGridColor (RGB(0xC0, 0xC0, 0xC0));
	strcpy(fpstr,PathName);
	strcat(fpstr,"StatTotall.List");
	m_TotallGrid.Load(fpstr);
	m_TotallGrid.SetFixedRowCount(1);
	m_TotallGrid.SetFixedColumnCount(1);

	m_LayerGrid.SetGridColor (RGB(0xC0, 0xC0, 0xC0));
	strcpy(fpstr,PathName);
	strcat(fpstr,"StatLayer.List");
	m_LayerGrid.Load(fpstr);
	m_LayerGrid.SetFixedRowCount(1);
	m_LayerGrid.SetFixedColumnCount(1);
	return TRUE;  
}
