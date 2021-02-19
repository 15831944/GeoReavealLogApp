// PrintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "PrintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPrintDlg dialog


CMyPrintDlg::CMyPrintDlg(CImageManDoc* pDoc1,CWnd* pParent /*=NULL*/)
	: CDialog(CMyPrintDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyPrintDlg)
	m_bPlotHead = FALSE;
	m_bMaxPageSize = FALSE;
	m_bPlotTrack = FALSE;
	m_bPlotResultList = FALSE;
	//}}AFX_DATA_INIT
	pDoc = pDoc1;
}


void CMyPrintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyPrintDlg)
	DDX_Check(pDX, IDC_PLOT_HEAD, m_bPlotHead);
	DDX_Check(pDX, IDC_MAX_PAGESIZE, m_bMaxPageSize);
	DDX_Check(pDX, IDC_PLOT_TRACK, m_bPlotTrack);
	DDX_Check(pDX, IDC_PLOT_RESULTLIST, m_bPlotResultList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyPrintDlg, CDialog)
	//{{AFX_MSG_MAP(CMyPrintDlg)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyPrintDlg message handlers

void CMyPrintDlg::OnPrint() 
{
	UpdateData(TRUE);
	if (m_bMaxPageSize) 
	{
		pDoc->m_bPlotHead = m_bPlotHead;
		pDoc->m_bPlotTrack = m_bPlotTrack;
		pDoc->m_bPlotResultList = m_bPlotResultList;
		pDoc->m_bMaxPageSize = m_bMaxPageSize;
		CSize size = pDoc->ComputerPrintSize();
		((CImageManApp*)AfxGetApp())->SetDevMode(size);
	}
}

BOOL CMyPrintDlg::OnInitDialog() 
{
	m_bPlotHead = pDoc->m_bPlotHead;
	m_bPlotTrack = pDoc->m_bPlotTrack;
	m_bPlotResultList = pDoc->m_bPlotResultList;
	m_bMaxPageSize = pDoc->m_bMaxPageSize;
	CDialog::OnInitDialog();
	return TRUE;  
}

void CMyPrintDlg::OnOK() 
{
	CDialog::OnOK();
	pDoc->m_bPlotHead = m_bPlotHead;
	pDoc->m_bPlotTrack = m_bPlotTrack;
	pDoc->m_bMaxPageSize = m_bMaxPageSize;
	pDoc->m_bPlotResultList = m_bPlotResultList;
}
