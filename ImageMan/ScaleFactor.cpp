// ScaleFactor.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ScaleFactor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScaleFactor dialog


CScaleFactor::CScaleFactor(CWnd* pParent /*=NULL*/)
	: CDialog(CScaleFactor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScaleFactor)
	m_ScaleFactor = 100;
	//}}AFX_DATA_INIT
}


void CScaleFactor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScaleFactor)
	DDX_Text(pDX, IDC_EDIT1, m_ScaleFactor);
	DDV_MinMaxInt(pDX, m_ScaleFactor, 0, 9999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScaleFactor, CDialog)
	//{{AFX_MSG_MAP(CScaleFactor)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleFactor message handlers
