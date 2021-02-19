// ImageCreatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flowchart.h"
#include "ImageCreatDlg.h"
#include <math.h>
#include "FIDio.h"
#include "Gradient.h"
#include "Guage.h"
#include ".\imagecreatdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageCreatDlg dialog


CImageCreatDlg::CImageCreatDlg(CImageCreat *pObj,CWnd* pParent /*=NULL*/)
	: CDialog(CImageCreatDlg::IDD, pParent)
	, m_Bits(0)
{
	//{{AFX_DATA_INIT(CImageCreatDlg)
	m_Message = _T("");
	m_bFilter = TRUE;
	//}}AFX_DATA_INIT
	m_pObj=pObj;
	m_Bits=152.f;
	m_bColor=TRUE;
	m_Palsize=64;
	m_MaxFre=60.f;
	m_nClass=8.f;		//Kmeans¾ÛÀà×åÊý          
	m_PadMin=1.f;				  
}


void CImageCreatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageCreatDlg)
	DDX_Text(pDX, IDC_EDIT1, m_nClass);
	DDV_MinMaxFloat(pDX, m_nClass, 2.f,128.f);
	DDX_Text(pDX, IDC_EDIT2, m_PadMin);
	DDV_MinMaxFloat(pDX, m_PadMin,0.f,30.f);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT3, m_Bits);
	DDX_Check(pDX, IDC_CHECK3, m_bButPro);
	DDX_Check(pDX, IDC_CHECK6, m_bPadPro);
	DDX_Check(pDX, IDC_CHECK5, m_bCompose);
}


BEGIN_MESSAGE_MAP(CImageCreatDlg, CDialog)
	//{{AFX_MSG_MAP(CImageCreatDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK5, OnBnClickedCheck5)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageCreatDlg message handlers

void CImageCreatDlg::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
	m_pObj->m_bFilter=m_bFilter;
	m_pObj->m_nClass=m_nClass;
	m_pObj->m_PadMin=m_PadMin;
	m_pObj->m_Bits=m_Bits;
	m_pObj->m_bRepair=m_bRepair;
	m_pObj->m_radius=m_radius;
	m_pObj->m_bButPro=m_bButPro;
	m_pObj->m_bCompose=m_bCompose;
	m_pObj->m_bPadPro=m_bPadPro;
}

BOOL CImageCreatDlg::OnInitDialog() 
{
	m_CurveName ="DATAST";
	m_szBufPath =m_pObj->m_szBufPath;
	m_EnProDep  =m_pObj->m_EnProDep;
	m_StProDep  =m_pObj->m_StProDep;
	m_Rlev      =m_pObj->m_Rlev;
	m_bFilter   =m_pObj->m_bFilter;
	m_nClass    =m_pObj->m_nClass;
	m_PadMin    =m_pObj->m_PadMin;
	m_Message   =m_pObj->m_szSeries;
	m_Bits      =m_pObj->m_Bits;
	m_bRepair   =m_pObj->m_bRepair;
	m_radius    =m_pObj->m_radius;
	m_bButPro   =m_pObj->m_bButPro;	
	m_bPadPro   =m_pObj->m_bPadPro;
	m_bCompose  =m_pObj->m_bCompose;
	CDialog::OnInitDialog();
	CenterWindow();
	GetDlgItem(IDC_EDIT1)->EnableWindow(m_bCompose);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CImageCreatDlg::OnBnClickedCheck5()
{
	UpdateData();
	GetDlgItem(IDC_EDIT1)->EnableWindow(m_bCompose);
}
