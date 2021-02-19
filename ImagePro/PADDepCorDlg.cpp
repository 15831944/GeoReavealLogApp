// PADDepCorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flowchart.h"
#include "PADDepCorDlg.h"
#include "FIDio.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPADDepCorDlg dialog


CPADDepCorDlg::CPADDepCorDlg(CPadCorrect *pObj,CWnd* pParent /*=NULL*/)
	: CDialog(CPADDepCorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPADDepCorDlg)
	m_Message = _T("");
	m_bDipDepPro = TRUE;
	m_bRADSDepPro = TRUE;
	//}}AFX_DATA_INIT
	m_pObj=pObj;
	m_MinSemblance=99.5f;
}

void CPADDepCorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPADDepCorDlg)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_LIST1, m_ParaListCtrl);
	DDX_Text(pDX, IDC_MESSAGEBOX, m_Message);
	DDX_Check(pDX, IDC_CHECK1, m_bDipDepPro);
	DDX_Check(pDX, IDC_CHECK2, m_bRADSDepPro);
	DDX_Check(pDX, IDC_CHECK3, m_bACDepPro);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPADDepCorDlg, CDialog)
	//{{AFX_MSG_MAP(CPADDepCorDlg)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPADDepCorDlg message handlers

BOOL CPADDepCorDlg::OnInitDialog() 
{
	m_bDipDepPro=m_pObj->m_bDipDepPro;
	m_bRADSDepPro=m_pObj->m_bRADSDepPro;
	m_Message=m_pObj->m_szSeries;
	m_bACDepPro=m_pObj->m_bACDepPro;
	m_MinSemblance=m_pObj->m_MinSemblance;
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ListView_SetExtendedListViewStyle(m_ParaListCtrl, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
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

	int i;
	CString str;
	m_ParaListCtrl.GetClientRect(&rect);
	w=(rect.Width ()-70)/m_pObj->m_PadNum-2;
	for(i=0;i<m_pObj->m_PadNum;i++)
	{
		str.Format("极板%d",i+1);
		m_ParaListCtrl.InsertColumn(i,str, LVCFMT_LEFT, w);
	}
	m_ParaListCtrl.InsertColumn(m_pObj->m_PadNum,"电扣间距", LVCFMT_LEFT, 70);
	str.Format("%g",m_pObj->m_DepDalta[0]);
	m_ParaListCtrl.InsertItem(0,str);

	for(i=1;i<m_pObj->m_PadNum;i++)
	{
		str.Format("%g",m_pObj->m_DepDalta[i]);
		m_ParaListCtrl.SetItem(0,i,LVIF_TEXT,str,-1,0,0,0);
	}
	str.Format("%g",m_pObj->m_ButDalta);
	m_ParaListCtrl.SetItem(0,m_pObj->m_PadNum,LVIF_TEXT,str,-1,0,0,0);

	m_ParaListCtrl.m_StartCol=0;
	m_ParaListCtrl.m_EndCol=m_pObj->m_PadNum+1;
	m_ParaListCtrl.InitCtrl(&m_Edit);
	UpdateData(FALSE);
	m_Shadow.Create(GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPADDepCorDlg::OnOK() 
{
	// TODO: Add extra validation here
	int i;
	CString str;
	//读取各极板间距
	for(i=0;i<m_pObj->m_PadNum;i++)
	{
		str=m_ParaListCtrl.GetItemText (0,i);
		str.TrimLeft ();
		str.TrimRight ();
		m_pObj->m_DepDalta[i]=(float)atof(str);
	}
	//读取电扣间距
	str=m_ParaListCtrl.GetItemText (0,m_pObj->m_PadNum);
	str.TrimLeft ();
	str.TrimRight ();
	m_pObj->m_ButDalta=(float)atof(str);
	UpdateData();
	m_pObj->m_bDipDepPro=m_bDipDepPro;
	m_pObj->m_bRADSDepPro=m_bRADSDepPro;
	m_pObj->m_bACDepPro=m_bACDepPro;
	m_pObj->m_MinSemblance=m_MinSemblance;
	CDialog::OnOK();
}

void CPADDepCorDlg::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if(m_ParaListCtrl.m_SelCol>-1)
	{
		CString str;
		m_Edit.GetWindowText(str);
		m_ParaListCtrl.SetItem(0,m_ParaListCtrl.m_SelCol,LVIF_TEXT,str,-1,0,0,0);
	}
}
