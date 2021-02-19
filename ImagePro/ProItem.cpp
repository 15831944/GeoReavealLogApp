// ProItem.cpp : implementation file
//

#include "stdafx.h"
#include "flowchart.h"
#include "ProItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProItem dialog


CProItem::CProItem(CWnd* pParent /*=NULL*/)
	: CDialog(CProItem::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProItem)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProItem)
	DDX_Control(pDX, IDC_LIST1, m_ItemListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProItem, CDialog)
	//{{AFX_MSG_MAP(CProItem)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProItem message handlers

BOOL CProItem::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ListView_SetExtendedListViewStyle(m_ItemListCtrl, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
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

	m_ItemListCtrl.InsertColumn(0,"曲线名", LVCFMT_LEFT, 150);
	m_ItemListCtrl.GetWindowRect (&rect);
	m_ItemListCtrl.InsertColumn(1,"说明", LVCFMT_LEFT, rect.Width ()-175);
	ListIcon.Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
	ListIcon.Add( AfxGetApp()->LoadIcon( IDI_ICON1));
	m_ItemListCtrl.SetImageList( &ListIcon, LVSIL_SMALL);

	m_ItemListCtrl.InsertItem(0,"电成像处理",0);
	m_ItemListCtrl.SetItem(0,1,LVIF_TEXT,"微电阻率成像(FMI STARII STARV EMI XRMI)",-1,0,0,0);

	m_ItemListCtrl.InsertItem(1,"倾角处理",0);
	m_ItemListCtrl.SetItem(1,1,LVIF_TEXT,"微电阻率成像(FMI STARII STARV EMI XRMI)",-1,0,0,0);

	m_ItemListCtrl.InsertItem(2,"声成像处理",0);
	m_ItemListCtrl.SetItem(2,1,LVIF_TEXT,"声波成像(UBI CAST CBIL)",-1,0,0,0);
	m_ItemListCtrl.InsertItem(3,"电磁探伤",0);
	m_ItemListCtrl.SetItem(3,1,LVIF_TEXT,"电磁探伤(MIDK)",-1,0,0,0);
	m_ItemListCtrl.InsertItem(4,"伽玛密度",0);
	m_ItemListCtrl.SetItem(4,1,LVIF_TEXT,"伽玛密度(AMK2000)",-1,0,0,0);
	m_ItemListCtrl.InsertItem(5,"方位电阻率",0);
	m_ItemListCtrl.SetItem(5,1,LVIF_TEXT,"扫描式侧向成像测井",-1,0,0,0);
	m_ProItemName=_T("");
	m_Shadow.Create(GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProItem::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	POSITION pos = m_ItemListCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	int Num=m_ItemListCtrl.GetNextSelectedItem(pos);
	m_ProItemName=m_ItemListCtrl.GetItemText (Num,0);
	if(m_ProItemName.IsEmpty())
		return;
	CDialog::OnOK();	
}

BOOL CProItem::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_ProItemName="";
	return CDialog::OnCommand(wParam, lParam);
}
