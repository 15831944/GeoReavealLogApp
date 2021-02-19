// PORComputDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "ImageMan.h"
#include "CreateSpectDlg.h"
#include "Database.h"
#include "FIDio.h"
#include "LogProcess.h"
// CCreateSpectDlg 对话框

IMPLEMENT_DYNAMIC(CCreateSpectDlg, CDialog)
CCreateSpectDlg::CCreateSpectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateSpectDlg::IDD, pParent)
{
	//孔隙度谱分析
	m_bResistance=FALSE;
	m_bSpectFilter=TRUE;
	m_PORMax=30.f;
	m_XM=2.f;
	m_XN=2.f;
	m_XA=1.f;
	m_XB=1.f;
	m_szExtPorWell="";
	m_szExtPorName="POR";
	m_szExtRxoWell="";
	m_szExtRxoName="RXO";

	m_szWell="";
	m_szName="IMAGE_STAT";
	m_StatWin=0.254f;     //1英尺
	m_StatRlev=0.0254f;   //1英寸
	m_MinStat=1.f;
	m_SpcNum=30;
	m_Rm=0.036f;
}

CCreateSpectDlg::~CCreateSpectDlg()
{
}

void CCreateSpectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT18, m_StatWin);
	DDX_Text(pDX, IDC_EDIT19,m_StatRlev);
	DDX_Text(pDX, IDC_EDIT14,m_Rm);
	DDX_Text(pDX, IDC_EDIT10, m_XM);
	DDX_Text(pDX, IDC_EDIT11, m_XN);
	DDX_Text(pDX, IDC_EDIT13, m_XA);
	DDX_Text(pDX, IDC_EDIT15, m_XB);
	DDX_Text(pDX,IDC_EDIT20,m_PORMax);
	DDX_Text(pDX,IDC_EDIT21,m_MinStat);
	DDX_Text(pDX, IDC_EDIT2, m_szExtPorWell);
	DDX_Control(pDX, IDC_COMBO1, m_NamePorCtrl);
	DDX_CBString(pDX, IDC_COMBO1, m_szExtPorName);

	DDX_Text(pDX, IDC_EDIT17, m_szExtRxoWell);
	DDX_Control(pDX, IDC_COMBO2, m_NameRxoCtrl);
	DDX_CBString(pDX, IDC_COMBO2, m_szExtRxoName);
	DDX_Check(pDX, IDC_CHECK1, m_bSpectFilter);

	DDX_Control(pDX, IDC_COMBO3, m_NameCtrl);
	DDX_CBString(pDX, IDC_COMBO3, m_szName);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(CCreateSpectDlg, CDialog)
	ON_CBN_CLOSEUP(IDC_COMBO1, OnCbnCloseupCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_CBN_CLOSEUP(IDC_COMBO2, OnCbnCloseupCombo2)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()

BOOL CCreateSpectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_szWell!="")
		InitComCtrl();
	if(m_szExtPorWell!="")
		InitComCtrl1();
	if(m_szExtRxoWell!="")
		InitComCtrl2();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CCreateSpectDlg::InitComCtrl()
{
	//初始化孔隙度曲线名选择框
	m_NamePorCtrl.ResetContent();
	CString Path=m_szWell.Left(m_szWell.ReverseFind ('\\'));
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
		m_dBaseFile.GetField("曲线类型", szBuff);
		Type=szBuff;
		Type.TrimLeft();Type.TrimRight();
		if(Type=="阵列")
			m_NameCtrl.AddString (Name);
	}
	m_dBaseFile.Close();
	m_NameCtrl.SetWindowText(m_szName);
}

void CCreateSpectDlg::InitComCtrl1()
{
	//初始化孔隙度曲线名选择框
	m_NamePorCtrl.ResetContent();
	CString Path=m_szExtPorWell.Left (m_szExtPorWell.ReverseFind ('\\'));
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
		m_dBaseFile.GetField("曲线类型", szBuff);
		Type=szBuff;
		Type.TrimLeft();Type.TrimRight();
		if(Type=="常规")
			m_NamePorCtrl.AddString (Name);
	}
	m_dBaseFile.Close();
	m_NamePorCtrl.SetWindowText(m_szExtPorName);
}
void CCreateSpectDlg::InitComCtrl2()
{
	//初始化孔隙度曲线名选择框
	m_NameRxoCtrl.ResetContent();
	CString Path=m_szExtRxoWell.Left (m_szExtRxoWell.ReverseFind ('\\'));
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
		m_dBaseFile.GetField("曲线类型", szBuff);
		Type=szBuff;
		Type.TrimLeft();Type.TrimRight();
		if(Type=="常规")
			m_NameRxoCtrl.AddString (Name);
	}
	m_dBaseFile.Close();
	m_NameRxoCtrl.SetWindowText(m_szExtRxoName);
}
// CCreateSpectDlg 消息处理程序

void CCreateSpectDlg::OnCbnCloseupCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int nIndex = m_NamePorCtrl.GetCurSel();
	if (nIndex == CB_ERR)
	{
		m_NamePorCtrl.GetWindowText(str);
		return;
	}
	else
		m_NamePorCtrl.GetLBText(nIndex, str);
	str.MakeUpper();
	if (m_szExtPorName != str)
		m_szExtPorName = str;
	m_NamePorCtrl.SetWindowText(m_szExtPorName);
}

void CCreateSpectDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	static char BASED_CODE szFilter[] = 
	"测井数据(*.FID)|*.FID|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "测井数据(*.FID)", "*.FID", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if (Browse.DoModal()==IDOK)
	{
		CString str = Browse.GetPathName();
		int x=str.ReverseFind('.');
		if(x>0)
		{
			m_szExtPorWell=str.Left(x);
			GetDlgItem(IDC_EDIT2)->SetWindowText(m_szExtPorWell);
			InitComCtrl1();
		}
	}
}
void CCreateSpectDlg::OnCbnCloseupCombo2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int nIndex = m_NameRxoCtrl.GetCurSel();
	if (nIndex == CB_ERR)
	{
		m_NameRxoCtrl.GetWindowText(str);
		return;
	}
	else
		m_NameRxoCtrl.GetLBText(nIndex, str);
	str.MakeUpper();
	if (m_szExtRxoName != str)
		m_szExtRxoName = str;
	m_NameRxoCtrl.SetWindowText(m_szExtRxoName);
}

void CCreateSpectDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	static char BASED_CODE szFilter[] = 
	"测井数据(*.FID)|*.FID|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "测井数据(*.FID)", "*.FID", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if(Browse.DoModal()==IDOK)
	{
		CString str = Browse.GetPathName();
		int x=str.ReverseFind('.');
		if(x>0)
		{
			m_szExtRxoWell=str.Left(x);
			GetDlgItem(IDC_EDIT17)->SetWindowText(m_szExtRxoWell);
			InitComCtrl2();
		}
	}
}
void CCreateSpectDlg::OnBnClickedButton3()
{
	UpdateData();
	m_ProgressCtrl.ShowWindow(SW_SHOW);
	BOOL flag=FALSE;
	if(m_DataType==4)
		flag=CreatSpect(m_szName,"PORSPC");
	else
		flag=CreatSpect2(m_szName,"PORSPC");
	if(flag)
	{
		StaticSectionPor("PORSPC");
		if(m_bSpectFilter)
			Curve_Filter(1);
		WellRefresh();
	}
	m_ProgressCtrl.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_MSG)->SetWindowText("OK");
	OnOK();
}
void CCreateSpectDlg::WellRefresh() 
{
	CString WellName=m_szWell.Left(m_szWell.ReverseFind('.'));
	char TargetCurveName[256];
	strcpy(TargetCurveName,WellName);
	strcat(TargetCurveName,".FID");
	RefurbishFID(TargetCurveName);
}
void CCreateSpectDlg::PumpMessages()
{
    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);  
    }
}
BOOL CCreateSpectDlg::CreatSpect(CString InName,CString OutName)
{
	int iWin=(int)(m_StatWin/m_Rlev);
	if(iWin<2) iWin=2;
	int iRlev=(int)(m_StatRlev/m_Rlev);
	if(iRlev+1>iWin) iRlev=iWin-1;
	
	float Scale=m_PORMax/m_SpcNum;

	//对滑动窗长和滑动步长重新计算
	m_StatRlev=m_Rlev*iRlev;
	m_StatWin=m_Rlev*iWin;
	float OffDep=m_StatWin/2.f;
	FIDCONTENT ContentW3[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
							   {"P","%",REPR_FLOAT,4,1,0,0.f,100.f,0.5f},
   						       {"PORSPC","%",REPR_FLOAT,4,1,1,0.f,100.f,0.5f}};
	ContentW3[0].min = m_StProDep+OffDep;
	ContentW3[0].max = m_EnProDep-OffDep;
	ContentW3[0].rlev= m_StatRlev;
	ContentW3[1].nps=m_SpcNum;
	ContentW3[1].min = 0.f;
	ContentW3[1].max = m_PORMax;
	ContentW3[1].rlev=Scale;
	ContentW3[2].nps=m_SpcNum;
	ContentW3[2].npw=m_SpcNum;
	ContentW3[2].min = 0.f;
	ContentW3[2].max = 100.f;
	ContentW3[2].rlev=Scale;
	strcpy(ContentW3[2].name,OutName);
	FIDINDEX Index;
	Index.dimension = 3;
	Index.Contentlist = ContentW3;
	char adi[] = "==END==";
	Index.adi = adi;

	CString szPath=m_szWell.Left(m_szWell.ReverseFind('.')+1);
	CFIDIO* pFID = new CFIDIO(szPath+OutName,&Index);
	pFID->Open();
	pFID->Close();
	delete pFID;

	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	m_ProgressCtrl.SetRange32(0,RecordNum);
	GetDlgItem(IDC_STATIC_MSG)->SetWindowText("计算孔隙度谱");

	char s[256],fn[256];
	strcpy(s,szPath);strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile Infp;
	BOOL bRunOK=TRUE;
	if(Infp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		strcpy(s,szPath);strcat(s,OutName);
		CFIDIO::GetFileName(s,fn,'D');
		CFile Outfp;
		if(Outfp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			int Block=m_DataNum*4;
			int SpctBlock=m_SpcNum*4;
			float *Value=new float[(m_DataNum+1)*iWin];

			int i,j,k,k1,pos,Num,m;
			float *pFreq=new float[m_SpcNum];
			Num=iWin*m_DataNum/2;

			float dep,dep0,tmp,ave;
			float SPOR,fXM=1.f/m_XM;

			float por0;
			BOOL bPOR=FALSE;
			CString PORstr=m_szExtPorWell.Left(m_szExtPorWell.ReverseFind('.')+1)+m_szExtPorName;
			CFIDIO* pPOR= new CFIDIO(PORstr);
			if(pPOR->Open()) 
				bPOR=TRUE;
			
			float fLLs;
			BOOL bLLs=FALSE;	
			CString LLsStr=m_szExtRxoWell.Left(m_szExtRxoWell.ReverseFind('.')+1)+m_szExtRxoName;
			CFIDIO* pLLs = new CFIDIO(LLsStr);
			if(pLLs->Open()) 
				bLLs=TRUE;

			dep=m_StProDep;
			k=0;
			k1=iWin/2;
			while(k<RecordNum-iWin)
			{
				//读取阵列电阻率数据
				Infp.Seek (k*Block,CFile::begin);
				Infp.Read (Value,Block*iWin);
				//计算窗口内各区间频率点数
				for(i=0;i<m_SpcNum;i++)
					pFreq[i]=0;
				//读取滑动窗口中部深度位置的外部孔隙度
				por0=m_PORMax;
				dep0=dep+m_StatWin/2.f;
				if(bPOR)
				{
					if(pPOR->SeekToDepth(dep0)>-1)
						pPOR->Read(2,1,&por0);
				}
				//读取浅侧向值
				fLLs=m_Rm;
				if(bLLs)
				{
					if(pLLs->SeekToDepth(dep0)>-1)
						pLLs->Read(2,1,&fLLs);
				}
				//计算平均电导率
				ave=0.f;m=0;
				for(i=0;i<m_DataNum;i++)
				{		
					if(Value[i]>0.f)
					{
						ave+=Value[i];
						m++;
					}
				}
				if(m>0) ave/=m;
				if(ave>0.f)
				{
					SPOR=por0;
					tmp=m_XA*fLLs/ave;
					SPOR/=pow(tmp,fXM);
					for(j=0;j<iWin;j++)
					{
						for(i=0;i<m_DataNum;i++)
						{	
							tmp=m_XA*fLLs/Value[j*m_DataNum+i];
							if(tmp>0.f)
							{		
								pos=(int)(pow(tmp,fXM)*SPOR*m_SpcNum/m_PORMax);
								if(pos>m_SpcNum-1) 
									pos=m_SpcNum-1;
								if(pos<0) pos=0;
								pFreq[pos]++;
							}
						}
					}
				}
				
				for(i=0;i<m_SpcNum;i++)
					pFreq[i]=pFreq[i]*100.f/Num;
				//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
				for(i=2;i<m_SpcNum-1;i++)
					pFreq[i]=pFreq[i-1]*0.212f+pFreq[i]*0.576f+pFreq[i+1]*0.212f;	
				Outfp.Write (pFreq,SpctBlock);
				k+=iRlev;k1++;
				dep=m_StProDep+m_Rlev*k;
				m_ProgressCtrl.SetPos(k);
				PumpMessages();
			}
			delete Value;
			delete pPOR;delete pLLs;
			delete pFreq;
		}
		else bRunOK=FALSE;
		Outfp.Close ();
	}
	else bRunOK=FALSE;
	Infp.Close ();
	return bRunOK;
}

BOOL CCreateSpectDlg::CreatSpect2(CString InName,CString OutName)
{	
	int iWin=(int)(m_StatWin/m_Rlev);
	if(iWin<2) iWin=2;
	int iRlev=(int)(m_StatRlev/m_Rlev);
	if(iRlev+1>iWin) iRlev=iWin-1;
	
	float Scale=m_PORMax/m_SpcNum;

	//对滑动窗长和滑动步长重新计算
	m_StatRlev=m_Rlev*iRlev;
	m_StatWin=m_Rlev*iWin;
	float OffDep=m_StatWin/2.f;
	FIDCONTENT ContentW3[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
							   {"P","%",REPR_FLOAT,4,1,0,0.f,100.f,0.5f},
   						       {"PORSPC","%",REPR_FLOAT,4,1,1,0.f,100.f,0.5f}};
	ContentW3[0].min = m_StProDep+OffDep;
	ContentW3[0].max = m_EnProDep-OffDep;
	ContentW3[0].rlev= m_StatRlev;
	ContentW3[1].nps=m_SpcNum;
	ContentW3[1].min = 0.f;
	ContentW3[1].max = m_PORMax;
	ContentW3[1].rlev=Scale;
	ContentW3[2].nps=m_SpcNum;
	ContentW3[2].npw=m_SpcNum;
	ContentW3[2].min = 0.f;
	ContentW3[2].max = 100.f;
	ContentW3[2].rlev=Scale;
	strcpy(ContentW3[2].name,OutName);
	FIDINDEX Index;
	Index.dimension = 3;
	Index.Contentlist = ContentW3;
	char adi[] = "==END==";
	Index.adi = adi;

	CString szPath=m_szWell.Left(m_szWell.ReverseFind('.')+1);
	CFIDIO* pFID = new CFIDIO(szPath+OutName,&Index);
	pFID->Open();
	pFID->Close();
	delete pFID;

	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	m_ProgressCtrl.SetRange32(0,RecordNum);
	GetDlgItem(IDC_STATIC_MSG)->SetWindowText("计算孔隙度谱");

	char s[256],fn[256];
	strcpy(s,szPath);strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile Infp;
	BOOL bRunOK=TRUE;
	if(Infp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		strcpy(s,szPath);strcat(s,OutName);
		CFIDIO::GetFileName(s,fn,'D');
		CFile Outfp;
		if(Outfp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			int Block=m_DataNum*2;
			int SpctBlock=m_SpcNum*4;
			short *Value=new short[(m_DataNum+1)*iWin];

			int i,j,k,k1,pos,Num,m;
			float *pFreq=new float[m_SpcNum];
			Num=iWin*m_DataNum/2;

			BOOL bPOR=FALSE;
			float dep,dep0,por0,tmp,ave;
			float SPOR,fXM=1.f/m_XM;

			CString PORstr=m_szExtPorWell.Left(m_szExtPorWell.ReverseFind('.')+1)+m_szExtPorName;
			CFIDIO* pPOR=NULL;
			pPOR = new CFIDIO(PORstr);
			if(pPOR->Open()) 
				bPOR=TRUE;
		
			float fLLs;
			BOOL bLLs=FALSE;	
			CFIDIO* pLLs=NULL;
			CString LLsStr=m_szExtRxoWell.Left(m_szExtRxoWell.ReverseFind('.')+1)+m_szExtRxoName;
			pLLs = new CFIDIO(LLsStr);
			if(pLLs->Open()) 
				bLLs=TRUE;

			dep=m_StProDep;
			k=0,k1=0;
			while(k<RecordNum-iWin)
			{
				//读取阵列电阻率数据
				Infp.Seek (k*Block,CFile::begin);
				Infp.Read (Value,Block*iWin);
				//计算窗口内各区间频率点数
				for(i=0;i<m_SpcNum;i++)
					pFreq[i]=0;
				//读取滑动窗口中部深度位置的外部孔隙度
				por0=m_PORMax;
				dep0=dep+iWin*m_Rlev/2.f;
				if(bPOR)
				{
					if(pPOR->SeekToDepth(dep0)>-1)
						pPOR->Read(2,1,&por0);
				}
				//读取浅侧向值
				fLLs=m_Rm;
				if(bLLs)
				{
					if(pLLs->SeekToDepth(dep0)>-1)
						pLLs->Read(2,1,&fLLs);
				}
				//计算平均电导率
				ave=0.f;m=0;
				for(i=0;i<m_DataNum;i++)
				{		
					if(Value[i]>0.f)
					{
						ave+=Value[i];
						m++;
					}
				}
				if(m>0) ave/=m;
				if(ave>0.f)
				{
					SPOR=por0;
					tmp=m_XA*fLLs/ave;
					SPOR=SPOR/pow(tmp,fXM);
					for(j=0;j<iWin;j++)
					{
						for(i=0;i<m_DataNum;i++)
						{	
							tmp=m_XA*fLLs/Value[j*m_DataNum+i];
							if(tmp>0.f)
							{		
								pos=(int)(pow(tmp,fXM)*SPOR/Scale);
								if(pos>m_SpcNum-1) pos=m_SpcNum-1;
								if(pos<0) pos=0;
								pFreq[pos]++;
							}
						}
					}
				}
				for(i=0;i<m_SpcNum;i++)
					pFreq[i]=pFreq[i]*100.f/Num;
				//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
				for(i=2;i<m_SpcNum-1;i++)
					pFreq[i]=pFreq[i-1]*0.212f+pFreq[i]*0.576f+pFreq[i+1]*0.212f;	
				Outfp.Seek (k1*SpctBlock,CFile::begin);
				Outfp.Write (pFreq,SpctBlock);
				k+=iRlev;k1++;
				dep=m_StProDep+m_Rlev*k;
				m_ProgressCtrl.SetPos(k);
				PumpMessages();
			}
			delete Value;
			delete pPOR;delete pLLs;
			delete pFreq;
		}
		else bRunOK=FALSE;
		Outfp.Close ();
	}
	else bRunOK=FALSE;
	Infp.Close ();
	return bRunOK;
}

BOOL CCreateSpectDlg::StaticSectionPor(CString InName)
{
	//打开外部孔隙度曲线
	CString szExtPorName=m_szExtPorWell.Left(m_szExtPorWell.ReverseFind('.')+1)+m_szExtPorName;
	CFIDIO* pPor = new CFIDIO(szExtPorName);
	if(!pPor->Open()) 
	{
		delete pPor;
		return FALSE;
	}
	CString szPath=m_szWell;
	if(szPath.Find(".")>0)
		szPath=szPath.Left(szPath.Find("."));
	CFIDIO* pSpc= new CFIDIO(szPath+"."+InName);
	if(!pSpc->Open()) 
	{
		delete pPor;
		delete pSpc;
		return FALSE;
	}

	//输出曲线信息
	CLogProcess *pLog = new CLogProcess(szPath,NULL);
	pLog->StartDep=m_StProDep;
	pLog->EndDep=m_EnProDep;
	pLog->DepLevel=m_StatRlev;
	//初始化输出
	OUTCURVE *pOutCurve = new OUTCURVE[8];
	strcpy(pOutCurve[0].name,"POR1");  // POR1 分区位置1
	strcpy(pOutCurve[1].name,"POR2");  // POR2 分区位置2
	strcpy(pOutCurve[2].name,"POR3");  // POR3 分区位置3

	strcpy(pOutCurve[3].name,"PORV1"); // PORV1 累加原生孔隙 
	strcpy(pOutCurve[4].name,"PORV2"); // PORV2 累加次生孔隙
	strcpy(pOutCurve[5].name,"PORV3"); // PORV3 累加缝洞孔隙
	strcpy(pOutCurve[6].name,"PORWD"); // PORWD 频谱宽度
	strcpy(pOutCurve[7].name,"PORMA"); // PORMA 频谱最高位置孔隙度
	float *OutXX= new float[8];
	pLog->InitOut(8,pOutCurve,(char *)OutXX);	
	pLog->InitBuffer();

	int Block=m_SpcNum*4;
	float *pFreq=new float[m_SpcNum];
	float por,dep,tmp;
	int i,k=0;
	int p0,p1,p2,pa;

	float Scale=m_PORMax/m_SpcNum;
	int nSect=3;
	dep=m_StProDep;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	m_ProgressCtrl.SetRange32(0,RecordNum);
	GetDlgItem(IDC_STATIC_MSG)->SetWindowText("计算分区孔隙");
	while(dep<m_EnProDep)
	{
		por=0.01f;
		if(pPor->SeekToDepth(dep)>-1)
			pPor->Read(2,1,&por);

		pSpc->SeekToDepth(dep);
		pSpc->Read(3,m_SpcNum,pFreq);

		for(i=0;i<7;i++)
			OutXX[i]=0.f;
		p0=0;p1=0;p2=0;tmp=0.f;
		for(i=0;i<m_SpcNum-1;i++)
		{
			if(pFreq[i]>m_MinStat)
			{
				p0=i;
				break;
			}
		}
		
		while(pFreq[i]>m_MinStat)
			i++;
		p1=i;

		i=m_SpcNum-1;
		while(pFreq[i]>0.f)
			i--;
		while(pFreq[i]<m_MinStat)
			i--;
		p2=i;
		if(p2<p1) p2=p1;

		tmp=pFreq[p0];p1=p0;
		for(i=p0;i<p2;i++)
		{
			if(pFreq[i]>tmp)
			{
				p1=i;
				tmp=pFreq[i];
			}
		}

		for(i=p1;i<p2;i++)
		{
			if(pFreq[i]<tmp/3.f)
			{
				p1=i;
				break;
			}
		}
		//频率最大位置孔隙度
		pa=0;
		tmp=m_MinStat;
		for(i=0;i<m_SpcNum-1;i++)
		{
			if(pFreq[i]>tmp)
			{
				tmp=pFreq[i];
				pa=i;
			}
		}
		
		if(p2>0)
		{		
			OutXX[0]=p0*1.f;
			OutXX[1]=(p1-p0)*1.f;
			OutXX[2]=(p2-p0)*1.f;
			OutXX[3]=p0*1.f;
			OutXX[4]=p1*1.f;
			OutXX[5]=p2*1.f;
			//外部孔隙度刻度
			tmp=OutXX[5];
			for(i=0;i<6;i++)
				OutXX[i]=OutXX[i]*por/tmp;
			OutXX[6]=(p2-p0)*Scale;
			OutXX[7]=(pa+1)*Scale;
		}
		dep=m_StProDep+m_StatRlev*k;
		pLog->Dep=dep;
		pLog->DatOut();
		
		k++;
		m_ProgressCtrl.SetPos(k);
	}
	delete OutXX;
	delete pOutCurve;
	delete pLog;
	delete pFreq;
	delete pPor;
	delete pSpc;

    return TRUE;
}

BOOL CCreateSpectDlg::Curve_Filter(int nFilter)
{
	CString InName[8]={"POR1","POR2","POR3","PORV1","PORV2","PORV3","PORWD","PORMA"};
	float f_Sdep,f_Edep,f_Rlev;
	BOOL bRun=TRUE;
	float fdat[10];

	int i,j,k,m,RecordNum;
	CFIDIO* pFIDtemp = NULL;
	CString szPath=m_szWell.Left(m_szWell.ReverseFind('.')+1);
	GetDlgItem(IDC_STATIC_MSG)->SetWindowText("曲线滤波");
	m_ProgressCtrl.SetRange32(0,8);
	for(j=0;j<8;j++)
	{
		pFIDtemp = new CFIDIO(szPath+InName[j]);
		if(pFIDtemp->Open())
		{
			f_Sdep = pFIDtemp->GetStartDepth();
			f_Edep = pFIDtemp->GetEndDepth();
			f_Rlev=pFIDtemp->GetDepthLevel();
			RecordNum=(int)((f_Edep-f_Sdep)/f_Rlev);
			for(k=0;k<nFilter;k++)
			{
				pFIDtemp->SeekToDepth(f_Sdep);
				pFIDtemp->Read(2,5,fdat);
				float data=fdat[4];
				m=5;
				float dep=f_Sdep+m*f_Rlev;
				while(dep<f_Edep)
				{		
					data=0.11f*(fdat[0]+fdat[4])+0.24f*(fdat[1]+fdat[3])+0.3f*fdat[2];
					fdat[2]=data;
					pFIDtemp->SeekToDepth(dep-2*f_Rlev);
					pFIDtemp->Write(2,1,&data);
					for(i=0;i<4;i++) 
						fdat[i]=fdat[i+1];
					pFIDtemp->SeekToDepth(dep);
					pFIDtemp->Read(2,1,&data);
					fdat[4]=data;

					m++;
					dep=f_Sdep+m*f_Rlev;
				}
				if(!bRun) break;
			}	
		}
		delete pFIDtemp;
		m_ProgressCtrl.SetPos(j);
		if(!bRun) break;
	}
	return bRun;
}