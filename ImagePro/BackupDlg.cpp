// BackupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flowchart.h"
#include "BackupDlg.h"
#include "FIDio.h"
#include "Database.h"
#include <direct.h>
#include <math.h>
#include <io.h>
#include ".\backupdlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CBackupDlg dialog


CBackupDlg::CBackupDlg(CBackupData *pObj,CWnd* pParent /*=NULL*/)
	: CDialog(CBackupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBackupDlg)
	m_Message = _T("");
	m_EnProDep = 0.0f;
	m_StProDep = 0.0f;
	m_StartDep = 0.0f;
	m_EndDep = 0.0f;
	m_Bits = 152.f;
	m_Rm = 0.038f;
	//}}AFX_DATA_INIT
	m_pObj=pObj;
	m_szSeries=_T("");
	ListIcon.Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
	ListIcon.Add( AfxGetApp()->LoadIcon( IDI_ICON2));
	ListIcon.Add( AfxGetApp()->LoadIcon( IDI_ICON3));
	m_bGain=FALSE;
}


void CBackupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackupDlg)
	DDX_Control(pDX, IDC_NAME, m_Combo);
	DDX_Control(pDX, IDC_COMBO_SERIES, m_SeriesCtrl);
	DDX_Control(pDX, IDC_LIST_CURVE, m_InCtrl);
	DDX_Text(pDX, IDC_MESSAGEBOX, m_Message);
	DDX_Text(pDX, IDC_ENPRODEP, m_EnProDep);
	DDX_Text(pDX, IDC_STPRODEP, m_StProDep);
	DDX_Text(pDX, IDC_EDIT3, m_StartDep);
	DDX_Text(pDX, IDC_EDIT4, m_EndDep);
	DDX_Text(pDX, IDC_COMBO1, m_Bits);
	DDX_Text(pDX, IDC_EDIT2, m_Rm);
	DDV_MinMaxFloat(pDX, m_Rm, 1.e-003f, 1000.f);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK1, m_bGain);
	DDX_Text(pDX,IDC_EDIT5,m_szWellName);
}


BEGIN_MESSAGE_MAP(CBackupDlg, CDialog)
	//{{AFX_MSG_MAP(CBackupDlg)
	ON_CBN_CLOSEUP(IDC_COMBO_SERIES, OnCloseupComboSeries)
	ON_CBN_SELCHANGE(IDC_NAME, OnSelchangeName)
	ON_CBN_EDITCHANGE(IDC_NAME, OnEditchangeName)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSWELL, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackupDlg message handlers

BOOL CBackupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ListView_SetExtendedListViewStyle(m_InCtrl, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_InCtrl.m_StartCol=0;
	m_InCtrl.m_EndCol=0;
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

	m_InCtrl.InsertColumn(0,"曲线名", LVCFMT_LEFT, 80);
	m_InCtrl.InsertColumn(1,"说明", LVCFMT_LEFT, 140);
	m_InCtrl.SetImageList( &ListIcon, LVSIL_SMALL);
	m_InCtrl.GetWindowRect (&rect);
	m_InCtrl.InsertColumn(2,"状态", LVCFMT_LEFT, rect.Width ()-245);
	m_InCtrl.SetImageList( &ListIcon, LVSIL_SMALL);
	
	CString str=m_pObj->m_szBufPath;
	str=str.Mid (str.ReverseFind ('\\')+1);
	m_Message+=str;
	// TODO: Add extra initialization here
 
	m_szWellName=m_pObj->m_szWellName;
	m_szSeries=m_pObj->m_szSeries;
	if(m_szSeries!=""&&m_szWellName!="")
	{
		m_SeriesCtrl.SetWindowText(m_szSeries);
		InitCombo();
		m_InCtrl.InitCtrl(&m_Combo);
		SetCurvePara();
		GetInitDep();
	}
	m_StProDep=m_pObj->m_StProDep;
	m_EnProDep=m_pObj->m_EnProDep;
	if(m_StProDep<0||m_EnProDep<0)
	{
		m_StProDep=m_StartDep;
		m_EnProDep=m_EndDep;
	}
	m_Bits=m_pObj->m_Bits;
	m_Rm=m_pObj->m_Rm;
	m_bGain=m_pObj->m_bGain;
	UpdateData(FALSE);
	if(m_szSeries.Find("STAR")>-1)
	{
		GetDlgItem(IDC_CHECK1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK1)->SetWindowText("增益恢复校正");
	}
	else if(m_szSeries.Find("FMI")>-1)
	{
		GetDlgItem(IDC_CHECK1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK1)->SetWindowText("高清HD");
	}
	else
		GetDlgItem(IDC_CHECK1)->ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CBackupDlg::GetSeries()
{
	CString sSeries="";
	CString Path=m_szWellName.Left(m_szWellName.ReverseFind ('\\'));
	if(DetectFileExist(Path+"\\*.P1BTN"))
			sSeries="STARII";
	else if(DetectFileExist(Path+"\\*.P1BTNQH"))
		sSeries="STARV";
	else if(DetectFileExist(Path+"\\*.PAD1"))
		sSeries="EMI";
	else if(DetectFileExist(Path+"\\*.FCA1"))
	{
		if(DetectFileExist(Path+"\\*.FCA3")
			&&DetectFileExist(Path+"\\*.FCA4"))
			sSeries="FMI_8PAD";
		else
			sSeries="FMI_4PAD";
		if(DetectFileExist(Path+"\\*.BCEGA1"))
			m_bGain=TRUE;
	}
	else if(DetectFileExist(Path+"\\*.BT1L")
			&&DetectFileExist(Path+"\\*.BT1U")
			&&DetectFileExist(Path+"\\*.BT8U"))
			sSeries="CMI";						//威德福
	else if(DetectFileExist(Path+"\\*.BTN1"))
		sSeries="DIP6";
	else if(DetectFileExist(Path+"\\*.PDD1"))
		sSeries="SED";
	else if(DetectFileExist(Path+"\\*.BHTAQH"))
		sSeries="CBIL";
	else if(DetectFileExist(Path+"\\*.AMP"))
		sSeries="CAST";
	else if(DetectFileExist(Path+"\\*.BZ1")
		&&DetectFileExist(Path+"\\*.BZ2")
		&&DetectFileExist(Path+"\\*.BZ3")
		&&DetectFileExist(Path+"\\*.BZ4")
		&&DetectFileExist(Path+"\\*.BZ5")
		&&DetectFileExist(Path+"\\*.BZ6")
		&&DetectFileExist(Path+"\\*.BZ7")
		&&DetectFileExist(Path+"\\*.BZ8"))
		sSeries="AMK2000";
	else if(DetectFileExist(Path+"\\*.IAI01")
		&&DetectFileExist(Path+"\\*.IAI12")
		&&DetectFileExist(Path+"\\*.IAI03")
		&&DetectFileExist(Path+"\\*.IAI14")
		&&DetectFileExist(Path+"\\*.IAI05")
		&&DetectFileExist(Path+"\\*.IAI16")
		&&DetectFileExist(Path+"\\*.IAI07")
		&&DetectFileExist(Path+"\\*.IAI09")
		&&DetectFileExist(Path+"\\*.IAI11"))
		sSeries="ARI_16";
	return sSeries;
}

void CBackupDlg::SetCurvePara()
{
	int i;CString ss;
	m_InCtrl.DeleteAllItems();
	CString Path=m_szWellName.Left (m_szWellName.ReverseFind ('\\'))+"\\*.";
	if((m_szSeries=="CAST")||(m_szSeries=="CBIL")||(m_szSeries=="UBI"))
	{		
		if(!DetectFileExist(Path+m_pObj->m_szAZ1Name))
		{
			m_InCtrl.InsertItem(0,m_pObj->m_szAZ1Name,0);
			m_InCtrl.SetItem(0,2,LVIF_TEXT,"重定向",-1,0,0,0);
		}
		else
			m_InCtrl.InsertItem(0,m_pObj->m_szAZ1Name,1);
		m_InCtrl.SetItem(0,1,LVIF_TEXT,"I号极板方位",-1,0,0,0);

		if(!DetectFileExist(Path+m_pObj->m_szAMPName))
		{
			m_InCtrl.InsertItem(1,m_pObj->m_szAMPName,0);
			m_InCtrl.SetItem(1,2,LVIF_TEXT,"重定向",-1,0,0,0);
		}
		else
			m_InCtrl.InsertItem(1,m_pObj->m_szAMPName,1);
		m_InCtrl.SetItem(1,1,LVIF_TEXT,"幅度阵列数据",-1,0,0,0);

		if(!DetectFileExist(Path+m_pObj->m_szTTName))
		{		
			m_InCtrl.InsertItem(2,m_pObj->m_szTTName,0);
			m_InCtrl.SetItem(2,2,LVIF_TEXT,"重定向",-1,0,0,0);
		}
		else
			m_InCtrl.InsertItem(2,m_pObj->m_szTTName,1);
		m_InCtrl.SetItem(2,1,LVIF_TEXT,"时间阵列数据",-1,0,0,0);
		m_pObj->m_PadNum=1;
		m_pObj->m_ButtonNum=250;
		if(m_szSeries=="CAST")
			m_pObj->m_ButtonNum=200;
	}
	else if(m_szSeries=="AMK2000")
	{
		if(!DetectFileExist(Path+m_pObj->m_szAZ1Name))
		{
			m_InCtrl.InsertItem(0,m_pObj->m_szAZ1Name,0);
			m_InCtrl.SetItem(0,2,LVIF_TEXT,"重定向",-1,0,0,0);
		}
		else
			m_InCtrl.InsertItem(0,m_pObj->m_szAZ1Name,1);
		m_InCtrl.SetItem(0,1,LVIF_TEXT,"仪器高端方位",-1,0,0,0);

		if(!DetectFileExist(Path+m_pObj->m_szRBName))
		{
			m_InCtrl.InsertItem(1,m_pObj->m_szRBName,0);
			m_InCtrl.SetItem(1,2,LVIF_TEXT,"重定向",-1,0,0,0);
		}
		else
			m_InCtrl.InsertItem(1,m_pObj->m_szRBName,1);
		m_InCtrl.SetItem(1,1,LVIF_TEXT,"自然伽玛曲线",-1,0,0,0);

		for(i=0;i<m_pObj->m_PadNum;i++)
		{		
			if(!DetectFileExist(Path+m_pObj->m_szCalNames[i]))
			{
				m_InCtrl.InsertItem(2+i,m_pObj->m_szCalNames[i],0);
				m_InCtrl.SetItem(2+i,2,LVIF_TEXT,"重定向",-1,0,0,0);
			}
			else
				m_InCtrl.InsertItem(2+i,m_pObj->m_szCalNames[i],1);
			ss.Format ("%d号方位密度计数率",i+1);
			m_InCtrl.SetItem(2+i,1,LVIF_TEXT,ss,-1,0,0,0);
		}
	}
	else if(m_szSeries=="ARI_16")
	{
		if(!DetectFileExist(Path+m_pObj->m_szRBName))
		{
			m_InCtrl.InsertItem(0,m_pObj->m_szRBName,0);
			m_InCtrl.SetItem(0,2,LVIF_TEXT,"重定向",-1,0,0,0);
		}
		else
			m_InCtrl.InsertItem(0,m_pObj->m_szRBName,1);
		m_InCtrl.SetItem(0,1,LVIF_TEXT,"相对方位",-1,0,0,0);
		
		if(!DetectFileExist(Path+m_pObj->m_szAZ1Name))
		{
			m_InCtrl.InsertItem(1,m_pObj->m_szAZ1Name,0);
			m_InCtrl.SetItem(1,2,LVIF_TEXT,"重定向",-1,0,0,0);	
		}
		else
			m_InCtrl.InsertItem(1,m_pObj->m_szAZ1Name,1);
		m_InCtrl.SetItem(1,1,LVIF_TEXT,"1号极板方位",-1,0,0,0);
		
		if(!DetectFileExist(Path+m_pObj->m_szDevName))
		{
			m_InCtrl.InsertItem(2,m_pObj->m_szDevName,0);
			m_InCtrl.SetItem(2,2,LVIF_TEXT,"重定向",-1,0,0,0);
		}
		else
			m_InCtrl.InsertItem(2,m_pObj->m_szDevName,1);
		m_InCtrl.SetItem(2,1,LVIF_TEXT,"井斜角",-1,0,0,0);

		if(!DetectFileExist(Path+m_pObj->m_szDazName))
		{
			m_InCtrl.InsertItem(3,m_pObj->m_szDazName,0);
			m_InCtrl.SetItem(3,2,LVIF_TEXT,"重定向",-1,0,0,0);	
		}
		else
			m_InCtrl.InsertItem(3,m_pObj->m_szDazName,1);
		m_InCtrl.SetItem(3,1,LVIF_TEXT,"井斜方位",-1,0,0,0);
		for(i=0;i<m_pObj->m_PadNum;i++)
		{		
			if(!DetectFileExist(Path+m_pObj->m_szCalNames[i]))
			{
				m_InCtrl.InsertItem(5+i,m_pObj->m_szCalNames[i],0);
				m_InCtrl.SetItem(5+i,2,LVIF_TEXT,"重定向",-1,0,0,0);
			}
			else
				m_InCtrl.InsertItem(5+i,m_pObj->m_szCalNames[i],1);
			ss.Format ("%d号方位电阻率曲线",i+1);
			m_InCtrl.SetItem(5+i,1,LVIF_TEXT,ss,-1,0,0,0);
		}
	}
	else
	{		
		if(!DetectFileExist(Path+m_pObj->m_szRBName))
		{
			m_InCtrl.InsertItem(0,m_pObj->m_szRBName,0);
			m_InCtrl.SetItem(0,2,LVIF_TEXT,"重定向",-1,0,0,0);
		}
		else
			m_InCtrl.InsertItem(0,m_pObj->m_szRBName,1);
		m_InCtrl.SetItem(0,1,LVIF_TEXT,"相对方位",-1,0,0,0);
		
		if(!DetectFileExist(Path+m_pObj->m_szAZ1Name))
		{
			m_InCtrl.InsertItem(1,m_pObj->m_szAZ1Name,0);
			m_InCtrl.SetItem(1,2,LVIF_TEXT,"重定向",-1,0,0,0);	
		}
		else
			m_InCtrl.InsertItem(1,m_pObj->m_szAZ1Name,1);
		m_InCtrl.SetItem(1,1,LVIF_TEXT,"1号极板方位",-1,0,0,0);
		
		if(!DetectFileExist(Path+m_pObj->m_szDevName))
		{
			m_InCtrl.InsertItem(2,m_pObj->m_szDevName,0);
			m_InCtrl.SetItem(2,2,LVIF_TEXT,"重定向",-1,0,0,0);
		}
		else
			m_InCtrl.InsertItem(2,m_pObj->m_szDevName,1);
		m_InCtrl.SetItem(2,1,LVIF_TEXT,"井斜角",-1,0,0,0);

		if(!DetectFileExist(Path+m_pObj->m_szDazName))
		{
			m_InCtrl.InsertItem(3,m_pObj->m_szDazName,0);
			m_InCtrl.SetItem(3,2,LVIF_TEXT,"重定向",-1,0,0,0);	
		}
		else
			m_InCtrl.InsertItem(3,m_pObj->m_szDazName,1);
		m_InCtrl.SetItem(3,1,LVIF_TEXT,"井斜方位",-1,0,0,0);

		//半径曲线
		for(i=0;i<m_pObj->m_CalNum;i++)
		{		
			if(!DetectFileExist(Path+m_pObj->m_szCalNames[i]))
			{
				m_InCtrl.InsertItem(4+i,m_pObj->m_szCalNames[i],0);
				m_InCtrl.SetItem(4+i,2,LVIF_TEXT,"重定向",-1,0,0,0);
			}
			else
				m_InCtrl.InsertItem(4+i,m_pObj->m_szCalNames[i],1);
			ss.Format ("%d号臂半径",i+1);
			m_InCtrl.SetItem(4+i,1,LVIF_TEXT,ss,-1,0,0,0);
		}
		//倾角处理曲线
		int Num=m_pObj->m_CalNum+4;
		int PNum=m_pObj->m_PadNum;
		if(m_szSeries.Find("FMI")>-1)
			PNum=4;
		for(i=0;i<PNum;i++)
		{		
			if(!DetectFileExist(Path+m_pObj->m_szDIPNames[i]))
			{
				m_InCtrl.InsertItem(Num+i,m_pObj->m_szDIPNames[i],0);
				m_InCtrl.SetItem(Num+i,2,LVIF_TEXT,"重定向",-1,0,0,0);
			}
			else
				m_InCtrl.InsertItem(Num+i,m_pObj->m_szDIPNames[i],1);
			ss.Format ("%d极板倾角计算",i+1);
			m_InCtrl.SetItem(Num+i,1,LVIF_TEXT,ss,-1,0,0,0);
		}
		if(m_szSeries=="DIP6"||m_szSeries=="SED") return;
		//板板曲线
		Num+=PNum;
		if(m_szSeries.Find("FMI")>-1||m_szSeries.Find("CMI")>-1)
		{
			for(i=0;i<m_pObj->m_PadNum;i++)
			{
				if(!DetectFileExist(Path+m_pObj->FCName[i][0]))
				{
					m_InCtrl.InsertItem(Num+i*2,m_pObj->FCName[i][0],0);
					m_InCtrl.SetItem(Num+i*2,2,LVIF_TEXT,"重定向",-1,0,0,0);
				}
				else
					m_InCtrl.InsertItem(Num+i*2,m_pObj->FCName[i][0],1);
				ss.Format ("%d极板阵列数据1",i+1);
				m_InCtrl.SetItem(Num+i*2,1,LVIF_TEXT,ss,-1,0,0,0);
				
				if(!DetectFileExist(Path+m_pObj->FCName[i][1]))
				{
					m_InCtrl.InsertItem(Num+i*2+1,m_pObj->FCName[i][1],0);
					m_InCtrl.SetItem(Num+i*2+1,2,LVIF_TEXT,"重定向",-1,0,0,0);
				}
				else
					m_InCtrl.InsertItem(Num+i*2+1,m_pObj->FCName[i][1],1);
				ss.Format ("%d极板阵列数据2",i+1);
				m_InCtrl.SetItem(Num+i*2+1,1,LVIF_TEXT,ss,-1,0,0,0);
			}
			Num+=m_pObj->m_PadNum*2;
			//加速度校正曲线
			if(!DetectFileExist(Path+m_pObj->m_szGAZFName))
			{
				m_InCtrl.InsertItem(Num,m_pObj->m_szGAZFName,0);
				m_InCtrl.SetItem(Num,2,LVIF_TEXT,"重定向",-1,0,0,0);
			}
			else
				m_InCtrl.InsertItem(Num,m_pObj->m_szGAZFName,1);
			m_InCtrl.SetItem(Num,1,LVIF_TEXT,"仪器探头加速度",-1,0,0,0);
			if(!DetectFileExist(Path+m_pObj->m_szTimeName))
			{
				m_InCtrl.InsertItem(Num+1,m_pObj->m_szTimeName,0);
				m_InCtrl.SetItem(Num+1,2,LVIF_TEXT,"重定向",-1,0,0,0);
			}
			else
				m_InCtrl.InsertItem(Num+1,m_pObj->m_szTimeName,1);
			m_InCtrl.SetItem(Num+1,1,LVIF_TEXT,"采样时间间隔",-1,0,0,0);
		}
		else if(m_szSeries!="")
		{
			for(i=0;i<m_pObj->m_PadNum;i++)
			{		
				if(!DetectFileExist(Path+m_pObj->m_szPadNames[i]))
				{
					m_InCtrl.InsertItem(Num+i,m_pObj->m_szPadNames[i],0);
					m_InCtrl.SetItem(Num+i,2,LVIF_TEXT,"重定向",-1,0,0,0);
				}
				else
					m_InCtrl.InsertItem(Num+i,m_pObj->m_szPadNames[i],1);
				ss.Format ("%d极板阵列数据",i+1);
				m_InCtrl.SetItem(Num+i,1,LVIF_TEXT,ss,-1,0,0,0);
			}
			Num+=m_pObj->m_PadNum;
			//加速度校正曲线
			if(!DetectFileExist(Path+m_pObj->m_szGAZFName))
			{
				m_InCtrl.InsertItem(Num,m_pObj->m_szGAZFName,0);
				m_InCtrl.SetItem(Num,2,LVIF_TEXT,"重定向",-1,0,0,0);
			}
			else
				m_InCtrl.InsertItem(Num,m_pObj->m_szGAZFName,1);
			m_InCtrl.SetItem(Num,1,LVIF_TEXT,"仪器探头加速度",-1,0,0,0);
			if(!DetectFileExist(Path+m_pObj->m_szTimeName))
			{
				m_InCtrl.InsertItem(Num+1,m_pObj->m_szTimeName,0);
				m_InCtrl.SetItem(Num+1,2,LVIF_TEXT,"重定向",-1,0,0,0);
			}
			else
				m_InCtrl.InsertItem(Num+1,m_pObj->m_szTimeName,1);
			m_InCtrl.SetItem(Num+1,1,LVIF_TEXT,"采样时间间隔",-1,0,0,0);

			if(m_szSeries.Find("STAR")>-1)
			{
				//增益恢复曲线
				Num+=2;
				if(!DetectFileExist(Path+m_pObj->m_szGrdVol))
					m_InCtrl.InsertItem(Num,m_pObj->m_szGrdVol,0);
				else
					m_InCtrl.InsertItem(Num,m_pObj->m_szGrdVol,1);
				ss="屏蔽测量电压阵列数据";
				m_InCtrl.SetItem(Num,1,LVIF_TEXT,ss,-1,0,0,0);

				if(!DetectFileExist(Path+m_pObj->m_szPadGan))
					m_InCtrl.InsertItem(Num+1,m_pObj->m_szPadGan,0);
				else
					m_InCtrl.InsertItem(Num+1,m_pObj->m_szPadGan,1);
				ss="钮扣电极增益";
				m_InCtrl.SetItem(Num+1,1,LVIF_TEXT,ss,-1,0,0,0);

				if(!DetectFileExist(Path+m_pObj->m_szGrdGan))
					m_InCtrl.InsertItem(Num+2,m_pObj->m_szGrdGan,0);
				else
					m_InCtrl.InsertItem(Num+2,m_pObj->m_szGrdGan,1);
				ss="屏蔽电极增益";
				m_InCtrl.SetItem(Num+2,1,LVIF_TEXT,ss,-1,0,0,0);	
			}	
		}	
	}
}

BOOL CBackupDlg::InitSeries()
{
	int i;
	CString ss;
	if(m_szSeries!="")
	{
		//清空曲线名
		for(i=0;i<10;i++)
		{
			m_pObj->m_szPadNames[i]=_T("");   //极板曲线名
			m_pObj->m_szCalNames[i]=_T("");   //半径曲线名
			m_pObj->m_szDIPNames[i]=_T("");   //倾角处理电导率曲线名
		}

		m_pObj->m_szAZ1Name=_T("");           //I号极板方位
		m_pObj->m_szDazName=_T("");           //井斜方位角
		m_pObj->m_szDevName=_T("");           //井斜角
		m_pObj->m_szRBName=_T("");            //相对方位角
		
		m_pObj->m_szGrdVol=_T("");            //屏蔽电压曲线 PD6G
		m_pObj->m_szGrdGan=_T("");            //屏蔽增益曲线 GG01
		m_pObj->m_szPadGan=_T("");            //极板增益曲线 PADG

		//MAXIS 500 电成像极板
		for(i=0;i<8;i++)
		{
			m_pObj->FCName[i][0]=_T(""); 
			m_pObj->FCName[i][1]=_T("");
		}

		m_pObj->m_ButtonNum=24;
		m_pObj->m_ButtonMid=12;
		//根据不同的测井系列初始曲线名
		if(m_szSeries=="FMI_8PAD")
		{
			m_pObj->m_PadNum=8;
			m_pObj->m_CalNum=2;
			m_pObj->FCName[0][0]="FCA1";m_pObj->FCName[0][1]="FCA2";
			m_pObj->FCName[1][0]="FCA3";m_pObj->FCName[1][1]="FCA4";
			m_pObj->FCName[2][0]="FCB1";m_pObj->FCName[2][1]="FCB2";
			m_pObj->FCName[3][0]="FCB3";m_pObj->FCName[3][1]="FCB4";
			m_pObj->FCName[4][0]="FCC1";m_pObj->FCName[4][1]="FCC2";
			m_pObj->FCName[5][0]="FCC3";m_pObj->FCName[5][1]="FCC4";
			m_pObj->FCName[6][0]="FCD1";m_pObj->FCName[6][1]="FCD2";
			m_pObj->FCName[7][0]="FCD3";m_pObj->FCName[7][1]="FCD4";		
			m_pObj->m_szAZ1Name="P1AZ"; 	
			m_pObj->m_szCalNames[0]="C1";
			m_pObj->m_szCalNames[1]="C2";

			for(i=0;i<4;i++)
			{
				ss.Format("DB%d",i+1);
				m_pObj->m_szDIPNames[i]=ss; 
			}
			m_pObj->m_szDazName=_T("HAZIM");            
			m_pObj->m_szDevName=_T("DEVIM");            
			m_pObj->m_szRBName=_T("RB");
			m_pObj->m_szGAZFName=_T("FCAZ");            //仪器探头加速度(GAZFQH)
			m_pObj->m_szTimeName=_T("FTIM");           //测井采样时间间隔(ETMIDQH)
		}
		else if(m_szSeries=="FMI_4PAD")
		{
			m_pObj->m_PadNum=4;
			m_pObj->m_CalNum=2;
			m_pObj->FCName[0][0]="FCA1";m_pObj->FCName[0][1]="FCA2";
			m_pObj->FCName[1][0]="FCB1";m_pObj->FCName[1][1]="FCB2";
			m_pObj->FCName[2][0]="FCC1";m_pObj->FCName[2][1]="FCC2";
			m_pObj->FCName[3][0]="FCD1";m_pObj->FCName[3][1]="FCD2";
			m_pObj->m_szAZ1Name="P1AZ"; 
			m_pObj->m_szCalNames[0]="C1";
			m_pObj->m_szCalNames[1]="C2";

			for(i=0;i<4;i++)
			{
				ss.Format("DB%d",i+1);
				m_pObj->m_szDIPNames[i]=ss; 
			}
			m_pObj->m_szDazName=_T("HAZIM");            
			m_pObj->m_szDevName=_T("DEVIM");            
			m_pObj->m_szRBName=_T("RB");
			m_pObj->m_szGAZFName=_T("FCAZ");            //仪器探头加速度(GAZFQH)
			m_pObj->m_szTimeName=_T("FTIM");           //测井采样时间间隔(ETMIDQH)
		}
		else if(m_szSeries=="EMI")
		{
			//初始化输入曲线名
			m_pObj->m_PadNum=6;
			m_pObj->m_CalNum=6;
			m_pObj->m_ButtonNum=25;
			for(i=0;i<m_pObj->m_PadNum;i++)
			{
				ss.Format ("PAD%i",i+1);
				m_pObj->m_szPadNames[i]=ss;  
				ss.Format ("RAD%d",i+1);
				m_pObj->m_szCalNames[i]=ss; 
				ss.Format("F%dB1",i+1);
				m_pObj->m_szDIPNames[i]=ss; 
			} 
			m_pObj->m_szAZ1Name="AZI1";
			m_pObj->m_szDazName=_T("HAZI");            
			m_pObj->m_szDevName=_T("DEVI");            
			m_pObj->m_szRBName=_T("RB");
			m_pObj->m_szGAZFName=_T("ZACC");            //仪器探头加速度(GAZFQH)
			m_pObj->m_szTimeName=_T("ETIM");            //测井采样时间间隔(ETMIDQH)
		}
		else if(m_szSeries=="STARII")
		{
			//初始化输入曲线名
			m_pObj->m_PadNum=6;
			m_pObj->m_CalNum=6;
			for(i=0;i<m_pObj->m_PadNum;i++)
			{
				ss.Format ("P%iBTN",i+1);
				m_pObj->m_szPadNames[i]=ss; 
				ss.Format ("RAD%d",i+1);
				m_pObj->m_szCalNames[i]=ss; 
				ss.Format("DIP%d",i+1);
				m_pObj->m_szDIPNames[i]=ss; 
			}
			m_pObj->m_szAZ1Name="AZQH"; 
			m_pObj->m_szDazName=_T("DAZQH");            
			m_pObj->m_szDevName=_T("DEVQH");            
			m_pObj->m_szRBName=_T("RBQH");
			m_pObj->m_szGrdVol=_T("PD6G");              //屏蔽测量电压阵列数据 PD6G
			m_pObj->m_szPadGan=_T("PADG");			    //屏蔽电极增益曲线 GG01
			m_pObj->m_szGrdGan=_T("GG01");			    //极增益曲线 PADG
			
			m_pObj->m_szGAZFName=_T("GAZFQH");          //仪器探头加速度(GAZFQH)
			m_pObj->m_szTimeName=_T("ETIMDQH");         //测井采样时间间隔(ETMIDQH)
		}
		else if(m_szSeries=="STARV")
		{
			//初始化输入曲线名
			m_pObj->m_PadNum=6;
			m_pObj->m_CalNum=6;
			for(i=0;i<m_pObj->m_PadNum;i++) 
			{
				ss.Format ("P%iBTNQH",i+1);
				m_pObj->m_szPadNames[i]=ss; 
				ss.Format ("RAD%dQH",i+1);
				m_pObj->m_szCalNames[i]=ss; 
				ss.Format("DIP%dQH",i+1);
				m_pObj->m_szDIPNames[i]=ss; 
			}
			m_pObj->m_szAZ1Name="AZSTQH";
			m_pObj->m_szDazName="DAZSTQH";            
			m_pObj->m_szDevName="DEVSTQH";            
			m_pObj->m_szRBName="RBSTQH";
			m_pObj->m_szGrdVol=_T("PD6GQH");
			m_pObj->m_szPadGan=_T("PADGQH");
			m_pObj->m_szGrdGan=_T("GG01QH");
			m_pObj->m_szGAZFName=_T("GAZFQH");            //仪器探头加速度(GAZFQH)
			m_pObj->m_szTimeName=_T("ETIMDQH");           //测井采样时间间隔(ETMIDQH)
		}
		else if(m_szSeries=="CMI")
		{
			m_pObj->m_PadNum=8;
			m_pObj->m_CalNum=8;
			//初始化输入曲线名
			for(i=0;i<m_pObj->m_PadNum;i++)
			{
				ss.Format ("BT%iU",i+1);
				m_pObj->FCName[i][0]=ss;
				ss.Format ("BT%iL",i+1);
				m_pObj->FCName[i][1]=ss;
				/*if((i+1)%2==0)
				{
					ss.Format ("IEC%d",i+1);
					m_pObj->m_szCalNames[i]=ss; 
				}
				else*/
				{
					ss.Format ("CAL%d",i+1);
					m_pObj->m_szCalNames[i]=ss; 
				}
				ss.Format("BTM%d",i+1);
				m_pObj->m_szDIPNames[i]=ss; 
			} 
			m_pObj->m_szAZ1Name="P1AZ";
			m_pObj->m_szDazName=_T("AZIM");            
			m_pObj->m_szDevName=_T("DEVI");            
			m_pObj->m_szRBName=_T("RB");
			m_pObj->m_szGAZFName=_T("ZACC");            //仪器探头加速度(GAZFQH)
			m_pObj->m_szTimeName=_T("ETIM");            //测井采样时间间隔(ETMIDQH)
		}
		else if(m_szSeries=="CBIL")
		{
			m_pObj->m_szAMPName=_T("BHTAQH"); 
			m_pObj->m_szTTName=_T("BHTTQH");  
			m_pObj->m_szAZ1Name="AZDPQH";
		}
		else if(m_szSeries=="CAST")
		{
			m_pObj->m_szAMPName=_T("AMP"); 
			m_pObj->m_szTTName=_T("TT");  
			m_pObj->m_szAZ1Name="P1AZ";
		}
		else if(m_szSeries=="UBI")
		{
			m_pObj->m_szAMPName=_T("AMP"); 
			m_pObj->m_szTTName=_T("TT");  
			m_pObj->m_szRADName=_T("RADI"); 
			m_pObj->m_szAZ1Name="P1AZ";
		}
		else if(m_szSeries=="DIP6")
		{
			//初始化输入曲线名
			m_pObj->m_PadNum=6;
			m_pObj->m_CalNum=6;
			for(i=0;i<m_pObj->m_PadNum;i++) 
			{
				ss.Format ("BTN%d",i+1);
				m_pObj->m_szDIPNames[i]=ss; 
				ss.Format ("RAD%d",i+1);
				m_pObj->m_szCalNames[i]=ss; 
			}
			m_pObj->m_szAZ1Name="AZDPQH";
			m_pObj->m_szDazName="DAZDPQH";            
			m_pObj->m_szDevName="DEVDPQH";            
			m_pObj->m_szRBName="RBDPQH";
		}
		else if(m_szSeries=="SED")
		{
			//初始化输入曲线名
			m_pObj->m_PadNum=6;
			m_pObj->m_CalNum=6;
			for(i=0;i<m_pObj->m_PadNum;i++) 
			{
				ss.Format ("PDD%d",i+1);
				m_pObj->m_szDIPNames[i]=ss; 
				ss.Format ("CAL%d",i+1);
				m_pObj->m_szCalNames[i]=ss; 
			}
			m_pObj->m_szAZ1Name="AZI1";
			m_pObj->m_szDazName="HAZI";            
			m_pObj->m_szDevName="DEVI";            
			m_pObj->m_szRBName="RB";
		}
		else if(m_szSeries=="AMK2000")
		{
			//初始化输入曲线名
			m_pObj->m_PadNum=8;
			m_pObj->m_CalNum=8;
			for(i=0;i<m_pObj->m_PadNum;i++) 
			{
				ss.Format ("BZ%d",i+1);
				m_pObj->m_szCalNames[i]=ss; 
			}
			m_pObj->m_szAZ1Name="UGOL";
			m_pObj->m_szRBName="GR";
		}
		else if(m_szSeries=="ARI_16")
		{
			//初始化输入曲线名
			m_pObj->m_PadNum=16;
			m_pObj->m_CalNum=16;
			for(i=0;i<m_pObj->m_PadNum;i++) 
			{
				if(i<9)
					ss.Format ("IAI0%d",i+1);
				else
					ss.Format ("IAI%d",i+1);
				m_pObj->m_szCalNames[i]=ss; 
			}
			m_pObj->m_szAZ1Name="AZI1";
			m_pObj->m_szDazName=_T("HAZI");            
			m_pObj->m_szDevName=_T("DEVI");            
			m_pObj->m_szRBName=_T("RB");
		}
		return TRUE;
	}
	return FALSE;
}

void CBackupDlg::OnCloseupComboSeries() 
{
	// TODO: Add your control notification handler code here
	CString str;
	int nIndex = m_SeriesCtrl.GetCurSel();
	if (nIndex == CB_ERR)
	{
		m_SeriesCtrl.GetWindowText(str);
		return;
	}
	else
		m_SeriesCtrl.GetLBText(nIndex, str);
	str.MakeUpper();
	if(str!=m_szSeries)
	{	
		m_szSeries=str;
		InitSeries();
		SetCurvePara();
		GetInitDep();
		m_StProDep=m_StartDep;
		m_EnProDep=m_EndDep;
		UpdateData(FALSE);
		if(m_szSeries.Find("STAR")>-1)
		{
			GetDlgItem(IDC_CHECK1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_CHECK1)->SetWindowText("增益恢复校正");
		}
		else if(m_szSeries.Find("FMI")>-1)
		{
			GetDlgItem(IDC_CHECK1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_CHECK1)->SetWindowText("高清HD");
		}
		else
			GetDlgItem(IDC_CHECK1)->ShowWindow(SW_HIDE);
	}
}

void CBackupDlg::InitCombo()
{
	LOGFONT TempFont=
	{
		12,					        //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_NORMAL,					//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"宋体"			            //lfFaceName//"Courier New" "Times New Roman"
	};
	HFONT hFont = ::CreateFontIndirect (&TempFont) ;
	m_Combo.SendMessage(WM_SETFONT, (WPARAM) hFont);
	hFont = ::CreateFontIndirect (&TempFont) ;
	m_InCtrl.SendMessage(WM_SETFONT, (WPARAM) hFont);
	m_Combo.ResetContent();

	CString Path=m_szWellName.Left (m_szWellName.ReverseFind ('\\'));
	CDbaseFile m_dBaseFile;
	if(m_dBaseFile.Open(Path+"\\Curve.dbf")!=DBASE_SUCCESS)
		return;
	char szBuff[255];
	CString Name,Type;
	for(int rc=m_dBaseFile.GetFirstRecord();rc==DBASE_SUCCESS;rc=m_dBaseFile.GetNextRecord())
  	{
		if (m_dBaseFile.IsRecordDeleted())
			continue;
		m_dBaseFile.GetField(1, szBuff);
		Name=szBuff;
		Name.TrimLeft();Name.TrimRight();
		m_Combo.AddString (Name);
	}
	m_dBaseFile.Close();
}

void CBackupDlg::OnSelchangeName() 
{
	// TODO: Add your control notification handler code here
	if (m_Combo.GetCurSel( ) == -1) return;
	CString szEdit;
	m_Combo.GetLBText(m_Combo.GetCurSel( ), szEdit);
	if (m_InCtrl.m_hWnd != NULL) 
	{
		int row = m_InCtrl.GetNextItem( -1, LVNI_SELECTED );
		if (row != -1) 
		{
			CString ms=m_InCtrl.GetItemText( row,1);
			m_InCtrl.InsertItem(row,szEdit,1);
			m_InCtrl.SetItemText( row, 1, ms);
			m_InCtrl.DeleteItem(row+1);
			this->UpdateData();
			//寻找初始井段
			ms=m_InCtrl.GetItemText( row,0);
			CFIDIO* PadI= NULL;
			PadI=new CFIDIO(m_szWellName+"."+ms);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				this->UpdateData(FALSE);
			}
			delete PadI;
		}
	}
}

void CBackupDlg::OnEditchangeName() 
{
	// TODO: Add your control notification handler code here
	CString szEdit;
	m_Combo.GetLBText(m_Combo.GetCurSel( ), szEdit);
	if (m_InCtrl.m_hWnd != NULL) 
	{
		int row = m_InCtrl.GetNextItem( -1, LVNI_SELECTED );
		if (row != -1) 
		{
			m_InCtrl.SetItemText( row, 0, szEdit );
			CString ms=m_InCtrl.GetItemText( row,1);
			m_InCtrl.InsertItem(row,szEdit,1);
			m_InCtrl.SetItemText( row, 1, ms);
			m_InCtrl.DeleteItem(row+1);
		}
	}
}

void CBackupDlg::GetCurvePara()
{
	int i;CString ss;
	//清空曲线名
	for(i=0;i<64;i++)
	{
		m_pObj->m_szPadNames[i]=_T("");   //极板曲线名
		m_pObj->m_szCalNames[i]=_T("");   //井径曲线名
		m_pObj->m_szDIPNames[i]=_T("");   //倾角处理电导率曲线名
	}
	m_pObj->m_szAZ1Name=_T("");           //I号极板方位
	m_pObj->m_szDazName=_T("");           //井斜方位
	m_pObj->m_szDevName=_T("");           //井斜角
	m_pObj->m_szRBName=_T("");            //相对方位

	m_pObj->m_szGAZFName=_T("");                 //仪器探头加速度(GAZFQH)
	m_pObj->m_szTimeName=_T("");                 //测井采样时间间隔(ETMIEQH)

	//MAXIS 500 电成像极板
	for(i=0;i<8;i++)
	{
		m_pObj->FCName[i][0]=_T(""); 
		m_pObj->FCName[i][1]=_T("");
	}
	if((m_szSeries=="CAST")||(m_szSeries=="CBIL"))
	{
		m_pObj->m_szAZ1Name=m_InCtrl.GetItemText(0,0);
		m_pObj->m_szAMPName=m_InCtrl.GetItemText(1,0);
		m_pObj->m_szTTName=m_InCtrl.GetItemText(2,0);
		m_pObj->m_PadNum=1;
		m_pObj->m_ButtonNum=250;
		if(m_szSeries=="CAST")
			m_pObj->m_ButtonNum=200;
	}
	else if(m_szSeries=="AMK2000")
	{
		m_pObj->m_szAZ1Name=m_InCtrl.GetItemText(0,0);
		m_pObj->m_szRBName=m_InCtrl.GetItemText(1,0);
		for(i=0;i<m_pObj->m_PadNum;i++)
			m_pObj->m_szCalNames[i]=m_InCtrl.GetItemText(2+i,0);
	}
	else if(m_szSeries=="ARI_16")
	{
		m_pObj->m_szRBName=m_InCtrl.GetItemText(0,0);
		m_pObj->m_szAZ1Name=m_InCtrl.GetItemText(1,0);
		m_pObj->m_szDevName=m_InCtrl.GetItemText(2,0);
		m_pObj->m_szDazName=m_InCtrl.GetItemText(3,0);
		for(i=0;i<m_pObj->m_PadNum;i++)
			m_pObj->m_szCalNames[i]=m_InCtrl.GetItemText(5+i,0);
	}
	else
	{
		m_pObj->m_szRBName=m_InCtrl.GetItemText(0,0);
		m_pObj->m_szAZ1Name=m_InCtrl.GetItemText(1,0);
		m_pObj->m_szDevName=m_InCtrl.GetItemText(2,0);
		m_pObj->m_szDazName=m_InCtrl.GetItemText(3,0);

		//井径曲线
		for(i=0;i<m_pObj->m_CalNum;i++)
			m_pObj->m_szCalNames[i]=m_InCtrl.GetItemText(4+i,0);

		//倾角处理曲线
		int Num=m_pObj->m_CalNum+4;
		int PNum=m_pObj->m_PadNum;
		if(m_szSeries.Find("FMI")>-1)
			PNum=4;
		for(i=0;i<PNum;i++)
			m_pObj->m_szDIPNames[i]=m_InCtrl.GetItemText(Num+i,0);
		if(m_szSeries=="DIP6"||m_szSeries=="SED") return;
		//根据不同的测井系列读取控件上的极板曲线名
		Num+=PNum;		
		if(m_szSeries.Find("FMI")>-1||m_szSeries.Find("CMI")>-1)
		{
			for(i=0;i<m_pObj->m_PadNum;i++)
			{
				m_pObj->FCName[i][0]=m_InCtrl.GetItemText(Num+i*2,0);
				m_pObj->FCName[i][1]=m_InCtrl.GetItemText(Num+i*2+1,0);
			}
			Num+=m_pObj->m_PadNum*2;	
			m_pObj->m_szGAZFName=m_InCtrl.GetItemText(Num,0);   //仪器探头加速度(GAZFQH)
			m_pObj->m_szTimeName=m_InCtrl.GetItemText(Num+1,0);  //测井采样时间间隔(ETMIEQH)		
		}
		else if(m_szSeries!="")
		{	
			for(i=0;i<m_pObj->m_PadNum;i++)
				m_pObj->m_szPadNames[i]=m_InCtrl.GetItemText(Num+i,0);
			Num+=m_pObj->m_PadNum;	
			m_pObj->m_szGAZFName=m_InCtrl.GetItemText(Num,0);  //仪器探头加速度(GAZFQH)
			m_pObj->m_szTimeName=m_InCtrl.GetItemText(Num+1,0);  //测井采样时间间隔(ETMIEQH)		
			if(m_szSeries.Find("STAR")>-1)
			{
				Num+=2;
				m_pObj->m_szGrdVol=m_InCtrl.GetItemText(Num,0);    //屏蔽电极测量电压阵列数据 PD6G
				m_pObj->m_szPadGan=m_InCtrl.GetItemText(Num+1,0);	 //屏蔽电极增益曲线 GG01
				m_pObj->m_szGrdGan=m_InCtrl.GetItemText(Num+2,0);	 //钮扣电极增益曲线     PADG	
			}
		}
	}
}

void CBackupDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if(m_StProDep>=m_EnProDep
		||m_StProDep<m_StartDep
		||m_EnProDep>m_EndDep)
	{
		AfxMessageBox("处理井段错误        ");
		return;
	}
	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	m_pObj->m_StProDep=m_StProDep;
	m_pObj->m_EnProDep=m_EnProDep;
	m_pObj->m_Bits=m_Bits;
	m_pObj->m_Rm=m_Rm;
	m_pObj->m_bGain=m_bGain;
	m_pObj->m_szWellName=m_szWellName;
	GetCurvePara();
	m_pObj->m_szSeries=m_szSeries;
	if(!GetInitDep())
	{
		AfxMessageBox("未知的测井系列        ");
		return;
	}
	CDialog::OnOK();
}

BOOL CBackupDlg::GetInitDep()
{
	//寻找初始井段
	CString Path=m_szWellName.Left (m_szWellName.ReverseFind ('\\'));
	CFIDIO* PadI= NULL;
	FIDCONTENT* Content=NULL;
	if(m_szSeries=="STARII")
	{
		if(DetectFileExist(Path+"\\*."+m_pObj->m_szPadNames[0]))
		{
			PadI=new CFIDIO(m_szWellName+"."+m_pObj->m_szPadNames[0]);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				m_pObj->m_Rlev=PadI->GetDepthLevel();
				Content = PadI->GetContent(3);
				m_pObj->m_ButtonNum=Content->nps;
			}
			delete PadI;
			return TRUE;
		}
	}
	else if(m_szSeries=="STARV")
	{
		if(DetectFileExist(Path+"\\*."+m_pObj->m_szPadNames[0]))
		{
			PadI=new CFIDIO(m_szWellName+"."+m_pObj->m_szPadNames[0]);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				m_pObj->m_Rlev=PadI->GetDepthLevel();
				Content = PadI->GetContent(3);
				m_pObj->m_ButtonNum=Content->nps;
			}
			delete PadI;
			return TRUE;
		}
	} 
	else if(m_szSeries=="EMI")
	{
		if(DetectFileExist(Path+"\\*."+m_pObj->m_szPadNames[0]))
		{	
			PadI=new CFIDIO(m_szWellName+"."+m_pObj->m_szPadNames[0]);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				m_pObj->m_Rlev=PadI->GetDepthLevel();
				Content = PadI->GetContent(3);
				m_pObj->m_ButtonNum=Content->nps;
			}
			delete PadI;
			return TRUE;
		}
	}
	else if((m_szSeries=="CAST")||(m_szSeries=="CBIL"))
	{
		if(DetectFileExist(Path+"\\*."+m_pObj->m_szAMPName))
		{	
			PadI=new CFIDIO(m_szWellName+"."+m_pObj->m_szAMPName);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				m_pObj->m_Rlev=PadI->GetDepthLevel();
				Content = PadI->GetContent(3);
				m_pObj->m_ButtonNum=Content->nps;
			}
			delete PadI;
			return TRUE;
		}
	}
	else if(m_szSeries=="DIP6"||m_szSeries=="SED")
	{
		if(DetectFileExist(Path+"\\*."+m_pObj->m_szDIPNames[0]))
		{	
			PadI=new CFIDIO(m_szWellName+"."+m_pObj->m_szDIPNames[0]);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				m_pObj->m_Rlev=PadI->GetDepthLevel();
				m_pObj->m_ButtonNum=1;
			}
			delete PadI;
			return TRUE;
		}
	}
	else if(m_szSeries=="AMK2000")
	{
		if(DetectFileExist(Path+"\\*."+m_pObj->m_szCalNames[0]))
		{	
			PadI=new CFIDIO(m_szWellName+"."+m_pObj->m_szCalNames[0]);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				m_pObj->m_Rlev=PadI->GetDepthLevel();
				m_pObj->m_ButtonNum=1;
			}
			delete PadI;
			return TRUE;
		}
	}
	else if(m_szSeries=="ARI_16")
	{
		if(DetectFileExist(Path+"\\*."+m_pObj->m_szCalNames[0]))
		{	
			PadI=new CFIDIO(m_szWellName+"."+m_pObj->m_szCalNames[0]);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				m_pObj->m_Rlev=PadI->GetDepthLevel();
				m_pObj->m_ButtonNum=1;
			}
			delete PadI;
			return TRUE;
		}
	}
	else if(m_szSeries=="CMI")
	{
		if(DetectFileExist(Path+"\\*."+m_pObj->FCName[0][0]))
		{
			PadI=new CFIDIO(m_szWellName+"."+m_pObj->FCName[0][0]);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				m_pObj->m_Rlev=PadI->GetDepthLevel();
				m_pObj->m_ButtonNum=24;
			}
			delete PadI;
			return TRUE;
		}
	}
	else
	{
		if(DetectFileExist(Path+"\\*."+m_pObj->FCName[0][0]))
		{
			PadI=new CFIDIO(m_szWellName+"."+m_pObj->FCName[0][0]);
			if(PadI->Open())
			{
				m_StartDep=PadI->GetStartDepth();
				m_EndDep=PadI->GetEndDepth();
				m_pObj->m_Rlev=PadI->GetDepthLevel();
				Content = PadI->GetContent(3);
				m_pObj->m_ButtonNum=Content->nps*2;
			}
			delete PadI;
			return TRUE;
		}
	}
	m_StartDep=-9999.f;
	m_EndDep=-9999.f;
	m_pObj->m_Rlev=-9999.f;
	return FALSE;
}

void CBackupDlg::OnBnClickedButton1()
{
	static char BASED_CODE szFilter[] = 
	"测井项目(*.FID)|*.FID|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "测井项目(*.FID)", "*.FID", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	if(m_szWellName!="")
		Browse.m_ofn.lpstrInitialDir=m_szWellName;
	else
		Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if(Browse.DoModal()==IDOK)
	{
		CString Path = Browse.GetPathName();
		CString szWellName=Path.Left(Path.ReverseFind ('.'));
		if(szWellName!=m_szWellName)
		{
			m_szWellName=szWellName;
			m_szSeries= GetSeries();
			InitSeries();
			m_SeriesCtrl.SetWindowText(m_szSeries);
			InitCombo();
			m_InCtrl.InitCtrl(&m_Combo);
			SetCurvePara();
			GetInitDep();
			m_StProDep=m_StartDep;
			m_EnProDep=m_EndDep;
			UpdateData(FALSE);
			if(m_szSeries.Find("STAR")>-1)
			{
				GetDlgItem(IDC_CHECK1)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_CHECK1)->SetWindowText("增益恢复校正");
			}
			else if(m_szSeries.Find("FMI")>-1)
			{
				GetDlgItem(IDC_CHECK1)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_CHECK1)->SetWindowText("高清HD");
			}
			else
				GetDlgItem(IDC_CHECK1)->ShowWindow(SW_HIDE);
		}
	}
}

void CBackupDlg::OnBnClickedButton2()
{
	UpdateData();
	if(m_szWellName=="") return;
	char str[512];
	char *args[3];	
	CString ExcFileName=GetWorkPath("System")+"ImageMan";
	strcpy(str,ExcFileName);
	args[0]=str;
	CString DataFileName=m_szWellName+".FID>"+GetWorkPath("Template")+"Image\\"+m_szSeries+"_Curve.LPT";
	strcpy(str,DataFileName);
	args[1]=str;
	args[2] = NULL;
	_spawnv( _P_NOWAIT, ExcFileName, args );
}
