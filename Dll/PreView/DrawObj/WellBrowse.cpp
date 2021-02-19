// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PreView.h"
#include "WellBrowse.h"
#include "BusyMessage.h"

// CWellBrowse 对话框

IMPLEMENT_DYNAMIC(CWellBrowse, CDialog)
CWellBrowse::CWellBrowse(CWnd* pParent /*=NULL*/)
	: CDialog(CWellBrowse::IDD, pParent)
{
	pBusy=NULL;
	m_WellName=_T("");
	m_RootName=_T("");
	m_RootID=0;
	m_ProjectID=0;
	m_ProjectName=_T("");
}

CWellBrowse::~CWellBrowse()
{
	m_Storage.Disconnect();
}

void CWellBrowse::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_WELLNAME, m_WellName);
	DDX_Control(pDX, IDC_WELLTREE, m_TreeCtrl);
	DDX_Control(pDX, IDC_WELL_LISTCTRL, m_WellCtrl);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWellBrowse, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_WELLTREE, OnTvnSelchangedWelltree)
	ON_NOTIFY(NM_CLICK, IDC_WELL_LISTCTRL, OnNMClickWellListctrl)
	ON_BN_CLICKED(IDC_BROWSE_ALL, OnBnClickedBrowseAll)
	ON_NOTIFY(NM_DBLCLK, IDC_WELL_LISTCTRL, OnNMDblclkWellListctrl)
END_MESSAGE_MAP()


// CWellBrowse 消息处理程序

BOOL CWellBrowse::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	WellimageTree.Create(16, 16, ILC_COLORDDB|ILC_MASK, 1, 0 );
	WellimageTree.Add(AfxGetApp()->LoadIcon(IDI_ICON_DB));
	WellimageTree.Add(AfxGetApp()->LoadIcon(IDI_ICONWELL));	
	WellimageTree.Add(AfxGetApp()->LoadIcon(IDI_ICONWELLOPEN));
	WellimageTree.Add(AfxGetApp()->LoadIcon(IDI_ICONCURRENTUSER));
	m_TreeCtrl.SetImageList(&WellimageTree, TVSIL_NORMAL);
	
	ListView_SetExtendedListViewStyle(m_WellCtrl,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_WellCtrl.ModifyStyle(0,LVS_REPORT);
	m_WellCtrl.SetImageList(&WellimageTree, LVSIL_SMALL);
	m_WellCtrl.InsertColumn(0,"井名", LVCFMT_LEFT, 150);
	m_WellCtrl.InsertColumn(1,"ID号", LVCFMT_LEFT, 80);

	if(m_Storage.InitData(m_szSever))
	{
		if(m_RootID>0)
		{
			m_Storage.GetData(m_RootID);
			DWORD UpperID=m_Storage.m_UpperID;
			//获取工区ID
			m_Storage.GetData(UpperID);
			UpperID=m_Storage.m_UpperID;
			//获取工区名
			m_Storage.GetData(UpperID);
			CString WorkSpaceName=m_Storage.m_StorageName;
			//获取工区下所有构造
			m_Storage.LoadStorageInfo(UpperID);
			m_TreeCtrl.DeleteAllItems();
			HTREEITEM hRoot=m_TreeCtrl.InsertItem(WorkSpaceName,0,0,TVI_ROOT, TVI_SORT);
			HTREEITEM hChild;
			for(int i=0; i<m_Storage.a_UpperID.GetSize(); i++)
			{
				hChild=m_TreeCtrl.InsertItem(m_Storage.a_StorageName.GetAt(i),1,2, hRoot);
				// 使用SetItemData()函数将部门编号值也保存在新节点中
				m_TreeCtrl.SetItemData(hChild, atol(m_Storage.a_StorageID.GetAt(i)));		
			}
			m_TreeCtrl.Expand(hRoot,TVE_EXPAND); 
		}
		else
		{
			BeginWaitCursor();
			pBusy= new CBusyMessage;
			pBusy->Create(IDD_BUSYMESSAGE);
			pBusy->SetWaitMessage("搜索网络极睿数据库...");
			pBusy->ShowWindow(SW_SHOW);
			pBusy->m_Busy.Start();
			ChangTreeContent();
			pBusy->m_Busy.End();
			delete pBusy;
			EndWaitCursor();
		}
	}
	return TRUE;
}
void CWellBrowse::ChangTreeContent()
{
	m_TreeCtrl.DeleteAllItems();
	HTREEITEM hRoot=m_TreeCtrl.InsertItem("GeoDB", 0,0,TVI_ROOT, TVI_SORT );
	//加载除测井项目和油气井外存储空间信息到字符串数组中
	m_Storage.LoadWorkSpaceInfo(pBusy);
	AddtoTree(m_TreeCtrl,hRoot,0);
	m_TreeCtrl.Expand(hRoot,TVE_EXPAND); 
}

void CWellBrowse::AddtoTree(CTreeCtrl& pTree,HTREEITEM hNode, DWORD UpperID)
{
	HTREEITEM hChild;
	for(int i=0; i<m_Storage.a_UpperID.GetSize(); i++)
	{
		// 打开a_UpperID数组中等于UpperID的元素
		if(UpperID == atol(m_Storage.a_UpperID.GetAt(i)))
		{
			// 表明此元素为当前根节点的子节点，因此将其添加到Tree控件中，新节点为m_hChild
			if(m_Storage.a_StorageKind.GetAt(i)=="构造")
				hChild = pTree.InsertItem(m_Storage.a_StorageName.GetAt(i),1,2, hNode);
			else
				hChild = pTree.InsertItem(m_Storage.a_StorageName.GetAt(i),3,3, hNode);
			// 使用SetItemData()函数将部门编号值也保存在新节点中
			pTree.SetItemData(hChild, atol(m_Storage.a_StorageID.GetAt(i)));		
			pTree.Expand(hNode,TVE_EXPAND);
			// 以新节点为根节点，再次递归调用AddtoTree()函数，将其子节点添加到Tree控件中
			AddtoTree(pTree,hChild, atoi(m_Storage.a_StorageID.GetAt(i)));
		}
		pBusy->m_Busy.Invalidate();
		PumpMessages();
	}
}

void CWellBrowse::OnTvnSelchangedWelltree(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData();
	HTREEITEM hNode=m_TreeCtrl.GetSelectedItem();
	m_RootName=m_TreeCtrl.GetItemText(hNode);
	m_RootID=(DWORD)m_TreeCtrl.GetItemData(hNode);
	AddtoWell(m_RootID);
	*pResult = 0;
}

void CWellBrowse::AddtoWell(DWORD UpperID)
{
	m_WellCtrl.DeleteAllItems();
	if(UpperID<1) return;
	m_Storage.LoadStorageInfo(UpperID);	
	for(int i=0; i<m_Storage.a_UpperID.GetSize();i++)
	{
		m_WellCtrl.InsertItem(i,m_Storage.a_StorageName.GetAt(i),1);
		m_WellCtrl.SetItemText(i,1,m_Storage.a_StorageID.GetAt(i));
	}
}

void CWellBrowse::PumpMessages()
{
    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
		/*if(!IsDialogMessage(&msg))*/
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
    }
}

void CWellBrowse::OnNMClickWellListctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	m_RootName=m_WellCtrl.GetItemText(phdr->iItem,0);
	/*CString str=m_WellCtrl.GetItemText(phdr->iItem,1);
	str.Trim();
	m_RootID=(DWORD)atol(str);*/
	m_WellName="@GeoDB\\"+m_RootName;
	UpdateData(FALSE);
	*pResult = 0;
}

void CWellBrowse::OnBnClickedBrowseAll()
{
	BeginWaitCursor();
	pBusy= new CBusyMessage;
	pBusy->Create(IDD_BUSYMESSAGE);
	pBusy->SetWaitMessage("搜索网络极睿数据库...");
	pBusy->ShowWindow(SW_SHOW);
	pBusy->m_Busy.Start();
	ChangTreeContent();
	pBusy->m_Busy.End();
	delete pBusy;
	EndWaitCursor();
}

void CWellBrowse::OnNMDblclkWellListctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	m_RootName=m_WellCtrl.GetItemText(phdr->iItem,0);
	m_WellName="@GeoDB\\"+m_RootName;
	UpdateData(FALSE);
	*pResult = 0;
	OnOK();
}
