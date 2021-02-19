// HeightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "HeightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeightDialog dialog


CHeightDialog::CHeightDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CHeightDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHeightDialog)
	m_Height = 0;
	m_MaxHeight = 0;
	m_MinHeight = 0;
	//}}AFX_DATA_INIT
}


void CHeightDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHeightDialog)
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	DDV_MinMaxUInt(pDX, m_Height, m_MinHeight, m_MaxHeight);
	DDX_Text(pDX, IDC_MAX_HEIGHT, m_MaxHeight);
	DDX_Text(pDX, IDC_MIN_HEIGHT, m_MinHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHeightDialog, CDialog)
	//{{AFX_MSG_MAP(CHeightDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeightDialog message handlers
