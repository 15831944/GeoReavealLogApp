// PageSetup.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "PageSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageSetup dialog


CPageSetup::CPageSetup(CImageManDoc* pDoc1,CWnd* pParent)
	: CDialog(CPageSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageSetup)
	m_LeftSpace = 0;
	m_Width = 0;
	m_BottomSpace = 0;
	m_RightSpace = 0;
	m_TopSpace = 0;
	//}}AFX_DATA_INIT
	pDoc = pDoc1;
	ASSERT(pDoc != NULL);
}


void CPageSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSetup)
	DDX_Text(pDX, IDC_LEFT_SPACE, m_LeftSpace);
	DDX_Text(pDX, IDC_PAGE_WIDTH, m_Width);
	DDV_MinMaxUInt(pDX, m_Width, 10, 10000);
	DDX_Text(pDX, IDC_BOTTOM_SPACE, m_BottomSpace);
	DDX_Text(pDX, IDC_RIGHT_SPACE, m_RightSpace);
	DDX_Text(pDX, IDC_TOP_SPACE, m_TopSpace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageSetup, CDialog)
	//{{AFX_MSG_MAP(CPageSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageSetup message handlers

void CPageSetup::OnOK() 
{
	UpdateData(TRUE);
	if (m_LeftSpace + m_RightSpace > m_Width)
	{
		AfxMessageBox(IDS_MARGIN_TOO_BIG,MB_OK | MB_ICONSTOP);
		return;
	}
	m_LeftSpace = (int) (m_LeftSpace / 0.254 + 0.5);
	m_RightSpace = (int) (m_RightSpace / 0.254 + 0.5);
	m_TopSpace = (int) (m_TopSpace / 0.254 + 0.5);
	m_BottomSpace = (int) (m_BottomSpace / 0.254 + 0.5);
	m_Width = (int) (m_Width / 0.254 + 0.5);
	CDialog::EndDialog(IDOK);
}

BOOL CPageSetup::OnInitDialog() 
{
	m_LeftSpace = (int) (pDoc->m_LeftSpace * 0.254 + 0.5);
	m_RightSpace = (int) (pDoc->m_RightSpace * 0.254 + 0.5);
	m_TopSpace = (int) (pDoc->m_TopSpace * 0.254 + 0.5);
	m_BottomSpace = (int) (pDoc->m_BottomSpace * 0.254 + 0.5);
	m_Width = (int) (pDoc->m_Rect.Width() * 0.254 + 0.5);
	CDialog::OnInitDialog();
	return TRUE;  
}
