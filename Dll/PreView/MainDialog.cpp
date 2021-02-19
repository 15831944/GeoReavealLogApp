// CurveView.cpp : implementation file
//

#include "stdafx.h"
#include "PreView.h"
#include "MainDialog.h"
#include "LogPlotView.h"
#include "GeoCurve.h"
#include "LogPlotDoc.h"

#include "GeoProduction.h"
#include "GeoTemplate.h"
#include "CurveView.h"
#include "GeoStorage.h"
#include ".\maindialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainDialog dialog


CMainDialog::CMainDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainDialog)
	//}}AFX_DATA_INIT
	m_pView=NULL;
	m_pDoc=NULL;
	m_pCurveView=NULL;
	//数据为链接字符串
	//如：10.132.204.16;sa;log;LogBase
	//m_szServer=_T("");	
	//m_szRole=_T("");	//用户角色
	//m_StorageID=0;	//存储体ID号
	//m_ProjectID=0;	//项目ID号
	//m_ProductionID=0;	//成果ID号
}

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	//{{AFX_MSG_MAP(CMainDialog)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDialog message handlers


BOOL CMainDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// 在LogShell程序中初始化OLE库
	InitialScrollView();
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMainDialog::InitialScrollView()
{
	CWaitCursor wait;
	BOOL flag=FALSE;
	if(m_pDoc)
	{
		m_pDoc->DeleteContents();
		delete m_pDoc;
		m_pDoc=NULL;
	}
	if(m_pView) 
	{ 
		m_pView ->DeleteContents();
		delete m_pView;
		m_pView = NULL; 
	}
	char szPath[MAX_PATH];
	GetTempPath(MAX_PATH,szPath);
	CString szStr;
	szStr.Format("%s\\xLogtemp\\",szPath);
	if(!m_szServer.IsEmpty())
	{
		if(m_ProductionID>0)
		{
			if(m_ProjectID<1)
			{
				CGeoProduction Production;
				if(Production.InitData(m_szServer)) 
				{
					if(Production.GetData(m_ProductionID))
					{
						DWORD len=Production.m_DocSize;
						DWORD ProjectID=Production.m_UpperID;
						char *pData=new char[len+10];
						len=Production.GetDocByte(m_ProductionID,Production.m_DocSize,pData,"DataByte1");
						if(len>0)
						{
							CTime time = CTime::GetCurrentTime();
							CString FileName=szStr+time.Format(_T("%m%d%H%M%S"))+".LPD";
							//创建文件
							CFile fl;
							if(fl.Open((const char*)FileName,CFile::modeCreate|CFile::modeWrite))
							{
								fl.Write(pData,len);
								fl.Close();
								delete pData;
								m_pDoc= (CLogPlotDoc* )RUNTIME_CLASS(CLogPlotDoc)->CreateObject();
								if(m_pDoc->OpenPlotDoc(FileName))
								{
									m_pDoc->m_szSever=m_szServer;
									m_pDoc->m_szRole=m_szRole;
									m_pDoc->m_ProjectID=ProjectID;
									m_pView=new CLogPlotView();
									if(m_pView) 
									{ 
										CRect rect;
										this->GetClientRect(&rect);
										if(m_pView ->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, 
											CRect(0,0,rect.Width(),rect.Height()), this, 3000))
										{
											m_pDoc->m_bRunFirst=TRUE;
											m_pView->m_pDoc=m_pDoc;
											m_pView ->OnInitialUpdate(); 
											m_pDoc->m_pView=m_pView;
											flag=TRUE;
											//获取井名和项目名称
											CGeoStorage Storage;
											if(Storage.InitData(m_szServer)) 
											{
												if(Storage.GetData(ProjectID))
												{
													if(Storage.m_StorageKind=="测井项目")
													{
														m_pDoc->m_ProjectID=ProjectID;
														m_pDoc->MatchAllObject();
														/*m_pDoc->m_szProjectName=Storage.m_StorageName;
														DWORD WellID=Storage.m_UpperID;
														if(Storage.GetData(WellID))
															m_pDoc->m_szWellName=Storage.m_StorageName;*/
													}
												}
												Storage.Disconnect();
											}
										}
									}
								}
							}
						}
					}
					Production.Disconnect();
				}
			}
			else
			{
				CGeoTemplate Template;
				if(Template.InitData(m_szServer)) 
				{
					if(Template.GetData(m_ProductionID))
					{
						DWORD len=Template.m_Size;
						char *pData=new char[len+10];
						len=Template.GetDocByte(m_ProductionID,len,pData,"DataByte1");
						if(len>0)
						{
							CTime time = CTime::GetCurrentTime();
							CString FileName=szStr+time.Format(_T("%m%d%H%M%S"))+".LPD";
							//创建文件
							CFile fl;
							if(fl.Open((const char*)FileName,CFile::modeCreate|CFile::modeWrite))
							{
								fl.Write(pData,len);
								fl.Close();
								delete pData;
								m_pDoc= (CLogPlotDoc* )RUNTIME_CLASS(CLogPlotDoc)->CreateObject();
								if(m_pDoc->OpenPlotDoc(FileName))
								{
									m_pDoc->m_szSever=m_szServer;
									m_pDoc->m_szRole=m_szRole;
									m_pDoc->m_ProjectID=m_ProjectID;
									m_pView=new CLogPlotView();
									if(m_pView) 
									{ 
										CRect rect;
										this->GetClientRect(&rect);
										if(m_pView ->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, 
											CRect(0,0,rect.Width(),rect.Height()), this, 3000))
										{
											m_pView->m_pDoc=m_pDoc;
											m_pView ->OnInitialUpdate(); 
											m_pDoc->m_pView=m_pView;
											if(m_pDoc->OpenWell())
												m_pView ->OnInitialUpdate();
											flag=TRUE;
										}
									}
								}
							}
						}
					}
					Template.Disconnect();
				}
			}
		}
		else if(m_CurveID>0)
		{
			m_pCurveView=new CCurveView();
			if(m_pCurveView) 
			{ 
				m_pCurveView ->m_CurveID=m_CurveID;
				m_pCurveView ->m_szServer=m_szServer;
				CRect rect;
				int w=280;
				rect.left =GetSystemMetrics(SM_CXSCREEN)-w-20;
				rect.top =GetSystemMetrics(SM_CYSCREEN)/10;
				rect.bottom =GetSystemMetrics(SM_CYSCREEN)-50;
				rect.right =GetSystemMetrics(SM_CXSCREEN)-20;
				m_pCurveView ->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, 
										CRect(0,0,rect.Width(),rect.Height()), this, 200);
				m_pCurveView ->OnInitialUpdate(); 
				MoveWindow(&rect);
				::SetWindowPos(
					m_hWnd ,        // handle to window
					HWND_TOPMOST,   // placement-order handle
					rect.left,      // horizontal position
					rect.top,       // vertical position
					rect.Width(),	// width
					rect.Height(),  // height
					SWP_FRAMECHANGED  // window-positioning options
					);
				SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
				return TRUE;
			}
		}
	}
	return flag;
}


BOOL CMainDialog::DestroyWindow()
{
	if(m_pDoc)
	{
		m_pDoc->DeleteContents();
		delete m_pDoc;
		m_pDoc=NULL;
	}
	if(m_pView) 
	{ 
		m_pView ->DeleteContents();
		delete m_pView;
		m_pView = NULL; 
	}
	
	if(m_pCurveView) //单条曲线预览
	{ 
		m_pCurveView ->DestroyWindow(); 
		m_pCurveView = NULL; 
	}
	return CDialog::DestroyWindow();
}

void CMainDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_pView!=NULL)
	{
		CRect rect;
		GetClientRect(&rect);
		m_pView->MoveWindow(rect);
	}
	else if(m_pCurveView!=NULL)//单条曲线预览
	{
		CRect rect;
		GetClientRect(&rect);
		m_pCurveView->MoveWindow(rect);
	}
}
