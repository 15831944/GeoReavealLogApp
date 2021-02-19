// FlowChartView.cpp : implementation of the CFlowChartView class
//

#include "stdafx.h"
#include "FlowChart.h"

#include "FlowChartDoc.h"
#include "FlowChartView.h"
#include "FlowNode.h"
#include "LinkWellData.h"
#include "BackupData.h"
#include <io.h>
#include <direct.h>
#include ".\flowchartview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlowChartView

IMPLEMENT_DYNCREATE(CFlowChartView, CFormView)

BEGIN_MESSAGE_MAP(CFlowChartView, CFormView)
	//{{AFX_MSG_MAP(CFlowChartView)
	ON_WM_SIZE()
	ON_COMMAND(ID_DEL, OnDel)
	ON_COMMAND(ID_RUN, OnRun)
	ON_UPDATE_COMMAND_UI(ID_RUN, OnUpdateRun)
	ON_COMMAND(ID_DEL_DATABUF, OnDelDatabuf)
	ON_UPDATE_COMMAND_UI(ID_DEL_DATABUF, OnUpdateDelDatabuf)
	ON_COMMAND(ID_RUN_ALL, OnRunAll)
	ON_UPDATE_COMMAND_UI(ID_RUN_ALL, OnUpdateRunAll)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	ON_COMMAND(ID_FLOW_DBCLICK,OnFlowDbClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlowChartView construction/destruction

CFlowChartView::CFlowChartView()
	: CFormView(CFlowChartView::IDD)
{
	//{{AFX_DATA_INIT(CFlowChartView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_bDataBufRun=FALSE;
}

CFlowChartView::~CFlowChartView()
{
}

void CFlowChartView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlowChartView)
	DDX_Control(pDX, IDC_TREE1, m_lnkTreeCtrl);
	//}}AFX_DATA_MAP
}

BOOL CFlowChartView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CFlowChartView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	LOGFONT TempFont=
	{
		13,			 		        //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_NORMAL,						//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"宋体" 			//lfFaceName
	};
	HFONT hFont = ::CreateFontIndirect (&TempFont) ;
	m_lnkTreeCtrl.SendMessage(WM_SETFONT, (WPARAM) hFont);
	m_lnkTreeCtrl.SetItemSpaceY(60);
	m_lnkTreeCtrl.SetItemSpaceX(24);
	CSize cs;
	int s, t, b, r, l;
	if (AfxGetApp()->GetProfileInt("Settings", "SavePosition", 1))
	{
		// only restore if there is a previously saved position
		if ( -1 != (s = AfxGetApp()->GetProfileInt("Settings", "FrameStatus",   -1)) &&
			 -1 != (t = AfxGetApp()->GetProfileInt("Settings", "FrameTop",      -1)) &&
			 -1 != (l = AfxGetApp()->GetProfileInt("Settings", "FrameLeft",     -1)) &&
			 -1 != (b = AfxGetApp()->GetProfileInt("Settings", "FrameBottom",   -1)) &&
			 -1 != (r = AfxGetApp()->GetProfileInt("Settings", "FrameRight",    -1))
		   ) 
		{
			cs.cx = r - l;
			cs.cy = b - t;
		}
	}	
	if(m_lnkTreeCtrl.GetSafeHwnd())
	{
		m_lnkTreeCtrl.MoveWindow(0, 0, cs.cx-26, cs.cy-112);
		m_lnkTreeCtrl.SetFocus ();
		pDoc = GetDocument();
		if(pDoc!=NULL)
			SetFlowChart();	
	}
	if(FindItemRunResut("初始化数据缓冲区"))
		m_bDataBufRun=TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CFlowChartView printing

BOOL CFlowChartView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFlowChartView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFlowChartView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CFlowChartView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CFlowChartView diagnostics

#ifdef _DEBUG
void CFlowChartView::AssertValid() const
{
	CFormView::AssertValid();
}

void CFlowChartView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CFlowChartDoc* CFlowChartView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFlowChartDoc)));
	return (CFlowChartDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFlowChartView message handlers

void CFlowChartView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(m_lnkTreeCtrl.GetSafeHwnd())
		m_lnkTreeCtrl.MoveWindow(0, 0, cx, cy);
}

void CFlowChartView::SetFlowChart()
{
	char str[256];
	TV_INSERTSTRUCT Root;
	Root.hParent = NULL; 
	Root.hInsertAfter = TVI_LAST ;
	Root.itemex.mask = TVIF_TEXT | TVIF_CHILDREN; 
	Root.itemex.cChildren = 1;
	CFlowNode *pObj=NULL;
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{			
			strcpy(str,pObj->m_szNodeName);
			Root.itemex.pszText=str;
			Root.hParent= m_lnkTreeCtrl.InsertItem(&Root);
		}
	}
	TV_INSERTSTRUCT parent;	
	TV_INSERTSTRUCT curTreeItem;
	curTreeItem.itemex.mask = TVIF_TEXT; 
	curTreeItem.hInsertAfter = TVI_LAST;
	pos = pDoc->m_ChildList0.GetHeadPosition();
	if(pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList0.GetNext(pos);
		if (pObj != NULL) 
		{			
			strcpy(str,pObj->m_szNodeName);
			curTreeItem.itemex.pszText=str;
			curTreeItem.hParent = Root.hParent;
			parent.hParent= m_lnkTreeCtrl.InsertItem(&curTreeItem);
		}
	}
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList0.GetNext(pos);
		if (pObj != NULL) 
		{			
			strcpy(str,pObj->m_szNodeName);
			curTreeItem.itemex.pszText=str;
			curTreeItem.hParent = parent.hParent;
			parent.hParent= m_lnkTreeCtrl.InsertItem(&curTreeItem);
		}
	}
	
	pos = pDoc->m_ChildList1.GetHeadPosition();
	if(pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList1.GetNext(pos);
		if (pObj != NULL) 
		{			
			strcpy(str,pObj->m_szNodeName);
			curTreeItem.itemex.pszText=str;
			curTreeItem.hParent = Root.hParent;
			parent.hParent= m_lnkTreeCtrl.InsertItem(&curTreeItem);
		}
	}
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList1.GetNext(pos);
		if (pObj != NULL) 
		{			
			strcpy(str,pObj->m_szNodeName);
			curTreeItem.itemex.pszText=str;
			curTreeItem.hParent = parent.hParent;
			parent.hParent= m_lnkTreeCtrl.InsertItem(&curTreeItem);
		}
	}
	
	pos = pDoc->m_ChildList2.GetHeadPosition();
	if(pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList2.GetNext(pos);
		if (pObj != NULL) 
		{			
			strcpy(str,pObj->m_szNodeName);
			curTreeItem.itemex.pszText=str;
			curTreeItem.hParent = Root.hParent;
			parent.hParent= m_lnkTreeCtrl.InsertItem(&curTreeItem);
		}
	}
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList2.GetNext(pos);
		if (pObj != NULL) 
		{			
			strcpy(str,pObj->m_szNodeName);
			curTreeItem.itemex.pszText=str;
			curTreeItem.hParent = parent.hParent;
			parent.hParent= m_lnkTreeCtrl.InsertItem(&curTreeItem);
		}
	}
	m_lnkTreeCtrl.ExpandAllItems();
	UpdateItemEable();
}

void CFlowChartView::OnFlowDbClick()
{
	OnRun();
}

BOOL CFlowChartView::RemoveFlowAt(CString ItemName)
{
	CFlowNode *pObj=NULL;
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pos=pDoc->m_ParentList.Find(pObj,NULL);
				pDoc->m_ParentList.RemoveAt(pos);
				return TRUE;
			}
		}
	}
	pos = pDoc->m_ChildList0.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList0.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pos=pDoc->m_ChildList0.Find(pObj,NULL);
				pDoc->m_ChildList0.RemoveAt(pos);
				return TRUE;
			}
		}
	}
	pos = pDoc->m_ChildList1.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList1.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pos=pDoc->m_ChildList1.Find(pObj,NULL);
				pDoc->m_ChildList1.RemoveAt(pos);
				return TRUE;
			}
		}
	}
	pos = pDoc->m_ChildList2.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList2.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pos=pDoc->m_ChildList2.Find(pObj,NULL);
				pDoc->m_ChildList2.RemoveAt(pos);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CFlowChartView::OnDel() 
{
	// TODO: Add your command handler code here
	if(m_lnkTreeCtrl.m_SelItemView!=NULL)
	{
		HTREEITEM nItem=m_lnkTreeCtrl.m_SelItemView->item;
		CString SelLabel=m_lnkTreeCtrl.GetItemText (nItem);
		CString str;
		str.Format ("是否将 [%s] 从流程链中删除        ",SelLabel);
		if(AfxMessageBox(str,MB_YESNO|MB_ICONQUESTION )==IDYES)
		{
			if(!m_lnkTreeCtrl.ItemHasChildren(nItem))
				m_lnkTreeCtrl.DeleteItem (nItem);
			else
			{
				if(RemoveFlowAt(SelLabel))
				{
					m_lnkTreeCtrl.CleanAllBuf();
					SetFlowChart();
					return;
				}
				str.Format ("[%s] 从流程链中删除失败        ",SelLabel);
				AfxMessageBox(str);
			}
		}
	}
}

void CFlowChartView::OnRun() 
{
	// TODO: Add your command handler code here
	if(m_lnkTreeCtrl.m_SelItemView!=NULL)
	{
		if(m_lnkTreeCtrl.m_SelItemView->m_bEnable)
		{
			HTREEITEM nItem=m_lnkTreeCtrl.m_SelItemView->item;
			CString SelLabel=m_lnkTreeCtrl.GetItemText (nItem);
			BOOL bResult=FALSE;
			POSITION pos = pDoc->m_ParentList.GetHeadPosition();
			CFlowNode* pSelObj=NULL;
			while (pos != NULL)
			{
				pSelObj=(CFlowNode*)pDoc->m_ParentList.GetNext(pos);
				if(pSelObj!=NULL)
					if(pSelObj->m_szNodeName ==SelLabel)
					{
						bResult=pSelObj->Run();
						if(pSelObj->IsKindOf(RUNTIME_CLASS(CLinkWellData)))
						{
							CString str=((CLinkWellData*)pSelObj)->m_szSeries;
							while(str.GetLength()<16)
								str+=" ";
							strcpy(pDoc->m_Series,str);
						}
					}
			}	
			pos = pDoc->m_ChildList0.GetHeadPosition();
			while (pos != NULL)
			{
				pSelObj=(CFlowNode*)pDoc->m_ChildList0.GetNext(pos);
				if(pSelObj!=NULL)
					if(pSelObj->m_szNodeName ==SelLabel)
						bResult=pSelObj->Run();
			}
			pos = pDoc->m_ChildList1.GetHeadPosition();
			while (pos != NULL)
			{
				pSelObj=(CFlowNode*)pDoc->m_ChildList1.GetNext(pos);
				if(pSelObj!=NULL)
					if(pSelObj->m_szNodeName ==SelLabel)
						bResult=pSelObj->Run();
			}
			pos = pDoc->m_ChildList2.GetHeadPosition();
			while (pos != NULL)
			{
				pSelObj=(CFlowNode*)pDoc->m_ChildList2.GetNext(pos);
				if(pSelObj!=NULL)
					if(pSelObj->m_szNodeName ==SelLabel)
						bResult=pSelObj->Run();
			}	
			if(bResult)
			{
				CString ItemName; 
				HTREEITEM ChildItem=m_lnkTreeCtrl.GetChildItem (nItem);
				if(ChildItem!=NULL)
				{
					ItemName=m_lnkTreeCtrl.GetItemText(ChildItem);
					if(InitNextItemPara(ItemName))
						m_lnkTreeCtrl.EnableItem(ChildItem);
					ChildItem=m_lnkTreeCtrl.GetNextItem(ChildItem,TVGN_NEXT);
					while(ChildItem!=NULL)
					{
						ItemName=m_lnkTreeCtrl.GetItemText(ChildItem);
						if(InitNextItemPara(ItemName))
							m_lnkTreeCtrl.EnableItem(ChildItem);		
						ChildItem=m_lnkTreeCtrl.GetNextItem (ChildItem,TVGN_NEXT);
					}
					m_lnkTreeCtrl.Invalidate ();	
				}
				if(FindItemRunResut("初始化数据缓冲区"))
					m_bDataBufRun=TRUE;
			}
		}
	}
}

void CFlowChartView::OnUpdateRun(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	BOOL BAL=FALSE;
	if(m_lnkTreeCtrl.m_SelItemView!=NULL)
		if(m_lnkTreeCtrl.m_SelItemView->m_bEnable)
			BAL=TRUE;
	pCmdUI->Enable(BAL);
}

BOOL CFlowChartView::InitNextItemPara(CString ItemName)
{
	CFlowNode *pObj=NULL;
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pObj->InitPara(pDoc);
				return TRUE;
			}
		}
	}
	pos = pDoc->m_ChildList0.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList0.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pObj->InitPara(pDoc);
				return TRUE;
			}
		}
	}
	pos = pDoc->m_ChildList1.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList1.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pObj->InitPara(pDoc);
				return TRUE;
			}
		}
	}
	pos = pDoc->m_ChildList2.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList2.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pObj->InitPara(pDoc);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CFlowChartView::UpdateItemEable()
{
	m_lnkTreeCtrl.AllDisable();
	HTREEITEM nItem=m_lnkTreeCtrl.GetRootItem ();
	m_lnkTreeCtrl.EnableItem (nItem);
	CString ItemName; 
	ItemName=m_lnkTreeCtrl.GetItemText(nItem);
	if(!FindItemRunResut(ItemName))
		return;
	HTREEITEM ChildItem=m_lnkTreeCtrl.GetChildItem (nItem);
	while(ChildItem!=NULL)
	{
		ItemName=m_lnkTreeCtrl.GetItemText(ChildItem);
		while(FindItemRunResut(ItemName))
		{	
			nItem=m_lnkTreeCtrl.GetNextItem(ChildItem,TVGN_NEXT);
			while(nItem!=NULL)
			{
				ItemName=m_lnkTreeCtrl.GetItemText(nItem);
				while(FindItemRunResut(ItemName))
				{
					m_lnkTreeCtrl.EnableItem(nItem);
                    nItem=m_lnkTreeCtrl.GetChildItem (nItem);
					if(nItem==NULL)
						break;
					else
						ItemName=m_lnkTreeCtrl.GetItemText(nItem);
				}
				m_lnkTreeCtrl.EnableItem(nItem);
				nItem=m_lnkTreeCtrl.GetNextItem (nItem,TVGN_NEXT);	
			}	
			m_lnkTreeCtrl.EnableItem(ChildItem);
 		    ChildItem=m_lnkTreeCtrl.GetChildItem (ChildItem);
			if(ChildItem==NULL)
				break; 
			else
				ItemName=m_lnkTreeCtrl.GetItemText(ChildItem);
		}
		m_lnkTreeCtrl.EnableItem(ChildItem);
		ChildItem=m_lnkTreeCtrl.GetNextItem (ChildItem,TVGN_NEXT);
	}
	m_lnkTreeCtrl.Invalidate (); 
}

BOOL CFlowChartView::FindItemRunResut(CString ItemName)
{
	CFlowNode *pObj=NULL;
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pObj->InitPara(pDoc);
				return pObj->m_bResult;
			}
		}
	}
	pos = pDoc->m_ChildList0.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList0.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pObj->InitPara(pDoc);
				return pObj->m_bResult;
			}
		}
	}
	pos = pDoc->m_ChildList1.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList1.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pObj->InitPara(pDoc);
				return pObj->m_bResult;
			}
		}
	}
	pos = pDoc->m_ChildList2.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList2.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
			{
				pObj->InitPara(pDoc);
				return pObj->m_bResult;
			}
		}
	}
	return FALSE;
}

void CFlowChartView::OnDelDatabuf() 
{
	// TODO: Add your command handler code here
	BeginWaitCursor();
	CFlowNode *pObj=NULL;
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{	
			if(pObj->m_szNodeName=="初始化数据缓冲区")
			{
				CBackupData* pDataBuf=(CBackupData*)pObj;
				if(pDataBuf!=NULL)
				{
					CString Path=pDataBuf->m_szBufPath.Left(pDataBuf->m_szBufPath.ReverseFind('\\'));
					DelDirAllFile(Path);
				}
				m_bDataBufRun=FALSE;
			}		
			pObj->m_bResult=FALSE;
		}
	}
	pos = pDoc->m_ChildList0.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList0.GetNext(pos);
		if (pObj != NULL) 
			pObj->m_bResult=FALSE;
	}
	pos = pDoc->m_ChildList1.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList1.GetNext(pos);
		if (pObj != NULL) 
			pObj->m_bResult=FALSE;
	}
	pos = pDoc->m_ChildList2.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList2.GetNext(pos);
		if (pObj != NULL) 
			pObj->m_bResult=FALSE;
	}
	UpdateItemEable();
	EndWaitCursor();
}

void CFlowChartView::OnUpdateDelDatabuf(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bDataBufRun);
}

void CFlowChartView::OnRunAll() 
{
	// TODO: Add your command handler code here
	pDoc->m_bAunAll=TRUE;
		BeginWaitCursor();

	CFlowNode *pObj=NULL;
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	BOOL bResult=FALSE;
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{	
			if(!pObj->m_bResult&&pObj->m_szNodeName!="链接数据源"
				&&pObj->m_szNodeName!="原始资料回放"
				&&pObj->m_szNodeName!="初始化数据缓冲区")
			{
				pObj->InitPara(pDoc);
				if(!pObj->Run())
				{
					AfxMessageBox(pObj->m_szNodeName+"----模块执行失败        ");
					goto Err;
				}
			}
				
		}
	}
	pos = pDoc->m_ChildList0.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList0.GetNext(pos);
		if (pObj != NULL) 
		{
			if(!pObj->m_bResult)
			{
				pObj->InitPara(pDoc);
				if(!pObj->Run())
				{
					AfxMessageBox(pObj->m_szNodeName+"----模块执行失败        ");
					goto Err;
				}
			}
		}
	}
	pos = pDoc->m_ChildList1.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList1.GetNext(pos);
		if (pObj != NULL) 
		{
			if(!pObj->m_bResult)
			{
				pObj->InitPara(pDoc);
				if(!pObj->Run())
				{
					AfxMessageBox(pObj->m_szNodeName+"----模块执行失败        ");
					goto Err;
				}
			}
		}
	}
	pos = pDoc->m_ChildList2.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)pDoc->m_ChildList2.GetNext(pos);
		if (pObj != NULL) 
		{
			if(!pObj->m_bResult)
			{
				pObj->InitPara(pDoc);
				if(!pObj->Run())
				{
					AfxMessageBox(pObj->m_szNodeName+"----模块执行失败        ");
					goto Err;
				}
			}
		}
	}
Err:
	UpdateItemEable();
	EndWaitCursor();
	pDoc->m_bAunAll=FALSE;
}

void CFlowChartView::OnUpdateRunAll(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bDataBufRun);
}

void CFlowChartView::DelDirAllFile(CString DirName)
{
	CString DelFileName=DirName+"\\*.*";
	struct _finddata_t c_file;
    long hFile;

    if( (hFile = _findfirst( DelFileName, &c_file )) != -1L )
    {
		_findnext( hFile, &c_file );
        while( _findnext( hFile, &c_file ) == 0 )
		{
			DelFileName=DirName+"\\"+c_file.name;
			if(c_file.attrib&_A_SUBDIR)
			{		
				DelDirAllFile(DelFileName);
				rmdir(DelFileName);
			}
		    else
				remove(DelFileName);
		}
        _findclose( hFile );
	}
	rmdir(DirName);
}
