// ACImagePre.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ACImagePre.h"
#include "FIDio.h"
#include "ImgFrequency.h"
#include <io.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CACImagePre dialog


CACImagePre::CACImagePre(CWnd* pParent /*=NULL*/)
	: CDialog(CACImagePre::IDD, pParent)
{
	//{{AFX_DATA_INIT(CACImagePre)
	m_ProStDep = 0.0f;
	m_ProEnDep = 0.0f;
	m_bAZIPro = TRUE;
	m_sSeries = _T("");
	m_WinLongth = 0.6f;
	m_bGrads = FALSE;
	m_bExp = FALSE;
	m_MaxValue = 0.0f;
	m_MinValue = 1024.0f;
	m_CurveName = _T("");
	//}}AFX_DATA_INIT
	m_ImgNum=250;
	m_StaScaleType=0;
	m_StaticScale=0;
}


void CACImagePre::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CACImagePre)
	DDX_Control(pDX, IDC_COMBO_NAME, m_ComName);
	DDX_Control(pDX, IDC_PROGRESS2, m_Progress);
	DDX_Text(pDX, IDC_PROSTDEP, m_ProStDep);
	DDX_Text(pDX, IDC_PROENDEP, m_ProEnDep);
	DDX_Check(pDX, IDC_CHECK_AZIMUTH, m_bAZIPro);
	DDX_CBString(pDX, IDC_COMBO_SERIES, m_sSeries);
	DDX_Text(pDX, IDC_EDIT_WINLONGTH, m_WinLongth);
	DDX_Check(pDX, IDC_CHECK_GRADS, m_bGrads);
	DDX_Check(pDX, IDC_CHECK_EXP, m_bExp);
	DDX_Text(pDX, IDC_EDIT_MAXVALUE, m_MaxValue);
	DDX_Text(pDX, IDC_EDIT_MINVALUE, m_MinValue);
	DDX_CBString(pDX, IDC_COMBO_NAME, m_CurveName);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_RADIO1, m_StaticScale);
	DDX_Radio(pDX, IDC_RADIO3, m_StaScaleType);
}


BEGIN_MESSAGE_MAP(CACImagePre, CDialog)
	//{{AFX_MSG_MAP(CACImagePre)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACImagePre message handlers

BOOL CACImagePre::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CenterWindow();
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	// TODO: Add extra initialization here
	m_WellName=m_WellName.Left (m_WellName.ReverseFind ('\\'));
	m_WellName=m_WellName+"\\"+m_WellName.Mid (m_WellName.ReverseFind ('#')+1);
	//初始化输入/输出参数
	if(InitIOPara())
		AddNameList();
	m_CurveName=m_InCurves[0];
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CACImagePre::InitIOPara()
{
	UpdateData();
	m_CurveNum=3;
	BOOL bflag=FALSE;
	if(m_sSeries=="CBIL")
	{
		bflag=TRUE;
		m_ImgNum=250;
		m_sAZ1Name="P1AZ";                           //I号极板方位曲线		
		m_InCurves[0]="BHTAQH";
		m_InCurves[1]="BHTTQH";
		m_InCurves[2]="ARAD";
	}
	else if(m_sSeries=="CAST")
	{
		bflag=TRUE;
		m_ImgNum=200;
		m_sAZ1Name="AZI";                            //I号极板方位曲线	
		m_InCurves[0]="AMP";
		m_InCurves[1]="TT";
		m_InCurves[2]="RADI";
	}
	//初始化输出曲线名
	if(bflag)
	{
		for(int i=0;i<m_CurveNum;i++)
			m_OutCurves[i]=m_InCurves[i]+"-PRO";
		CString WellName=m_WellName.Left (m_WellName.ReverseFind ('\\'));
		CFIDIO* pFIDIO= NULL;
		pFIDIO=new CFIDIO(m_WellName+"."+m_InCurves[0]);
		if(pFIDIO->Open())
		{
			m_StartDep=pFIDIO->GetStartDepth();
			m_EndDep=pFIDIO->GetEndDepth();
			m_Rlev=pFIDIO->GetDepthLevel();
			if(m_ProEnDep>m_EndDep)
				m_ProEnDep=m_EndDep;				
			if(m_ProStDep<m_StartDep)
				m_ProStDep=m_StartDep;
			UpdateData(FALSE);
			FIDCONTENT* Content = pFIDIO->GetContent(3);
			m_ImgNum = Content->nps;
		}
		delete pFIDIO;
	}
	if(bflag)
		this->GetDlgItem (IDOK)->EnableWindow();
	else
		this->GetDlgItem (IDOK)->EnableWindow(FALSE);
	return bflag;
}

void CACImagePre::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_TODO)->ShowWindow(SW_SHOW);
	int i;
	for(i=0;i<m_CurveNum;i++)
		DataCopy(m_InCurves[i],m_OutCurves[i]);
	if(m_bAZIPro)
	{		
		for(i=0;i<m_CurveNum;i++)
			AzimuthCorret(m_OutCurves[i]);
	}
/*	if(m_StaticScale>0)
	{
		for(i=0;i<m_CurveNum;i++)
			DataDynScale(m_OutCurves[i]);
	}
	else
	{		
		for(i=0;i<m_CurveNum;i++)
			DataStaticScale(m_OutCurves[i]);
	}*/
	CDialog::OnOK();
}

void CACImagePre::AzimuthCorret(CString OutCurve)
{
	CFIDIO* pFIDAZ1=NULL;
	pFIDAZ1 = new CFIDIO(m_WellName+"."+m_sAZ1Name);
	if(!pFIDAZ1->Open()) 
		{delete pFIDAZ1; return;}
	pFIDAZ1->SeekToDepth(m_ProStDep);	
		
	char s[256],fn[256];
	strcpy(s,m_WellName);
	strcat(s,".");strcat(s,OutCurve);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int RecordNum=(int)((m_ProEnDep-m_ProStDep)/m_Rlev);
		CString ss;
		ss.Format ("%s 数据方位校正...",OutCurve);
		GetDlgItem(IDC_STATIC_TODO)->SetWindowText(ss);
		m_Progress.ShowWindow(SW_SHOW);
		m_Progress.SetRange(0, RecordNum/100);
		m_Progress.SetPos (0);	
		int Block=m_ImgNum*4;	

		float Az1;int x,dk=0;
		float *Value;
		Value=new float[2*m_ImgNum+10];
		int k=0;
		pFIDAZ1->SeekToDepth(m_ProStDep);
		while(k<RecordNum)
		{
			MSG msg;				
			while (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )) 
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);			
			pFIDAZ1->Read(2,1,&Az1);
			pFIDAZ1->Next();
			if((Az1>=0.f)&&(Az1<=360.f))
			{
				dk=(int)(m_ImgNum*1.f*Az1/360.f);
				for(x=m_ImgNum-1;x>=0;x--)
					Value[x+dk]=Value[x];
				for(x=0;x<dk;x++)
					Value[x]=Value[m_ImgNum+x];
			}
			fp.Seek (k*Block,CFile::begin);
			fp.Write (Value,Block);	
			m_Progress.SetPos(k/100);
			k++;
		}
		delete Value;
	}
	fp.Close ();
	delete pFIDAZ1;	
}

BOOL CACImagePre::DataCopy(CString InCurve, CString OutCurve)
{
	m_Progress.ShowWindow(SW_SHOW);
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"S","m",REPR_FLOAT,4,24,0,0.f,1920.f,2.f},
							   {"P","mv",REPR_FLOAT,4,24,24,0.f,1920.f,2.f}};
	char adi[] = "DATID 0";			
	CFIDIO* PadI= NULL;
	CFIDIO* PadO= NULL;
	float *Value;
	Value=new float[m_ImgNum];
	CString ss;
	ss.Format ("%s 数据备份...",InCurve);
	GetDlgItem(IDC_STATIC_TODO)->SetWindowText(ss);
	m_Progress.SetPos(0);
	// Wake up the GUI
	MSG msg;
	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	PadI=new CFIDIO(m_WellName+"."+InCurve);
	if(!PadI->Open())
	{
		delete PadI;
		return FALSE;
	}
	int RecordNum=(int)((m_ProEnDep-m_ProStDep)/m_Rlev);
	m_Progress.SetRange(0, RecordNum/100);

	m_Content[0].min = m_ProStDep;
	m_Content[0].max = m_ProEnDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_ImgNum;
	m_Content[2].nps=m_ImgNum;
	m_Content[2].npw=m_ImgNum;
	strcpy(m_Content[2].name ,OutCurve);
	FIDINDEX m_Index;
	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	CFIDIO* pFID = NULL;
	ss=m_WellName+"."+OutCurve;
	pFID = new CFIDIO(ss,&m_Index);
	pFID->Close();
	delete pFID;
	
	PadO=new CFIDIO(ss);
	if(!PadO->Open())
	{
		delete PadI;delete PadO;
		return FALSE;
	}
	int j=0;
	PadI->SeekToDepth(m_ProStDep);
	while(j<=RecordNum)
	{
		m_Progress.SetPos (j/100);
		PadI->Read(3,m_ImgNum,Value);
		PadI->Next();
		PadO->Write(3,m_ImgNum,Value);
		PadO->Next();
		j++;
	}
	delete PadI;delete PadO;
	delete Value;
	return TRUE;
}

BOOL CACImagePre::DataDynScale(CString OutName)
{
	int RecordNum;
	int Block=m_ImgNum*4;
	GetDlgItem(IDC_STATIC_TODO)->SetWindowText(OutName+" 动态刻度图像数据...");
	RecordNum=(int)((m_ProEnDep-m_ProStDep)/m_Rlev);
	m_Progress.ShowWindow(SW_SHOW);
	m_Progress.SetRange(0, RecordNum/100);
	m_Progress.SetPos(0);
	int iWin=(int)(m_WinLongth/m_Rlev);
	char s[256],fn[256];
	strcpy(s,m_WellName);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		float *Value;
		Value=new float[m_ImgNum*iWin+10];
		float dalta;
		int i,k=0;
		while(k<RecordNum)
		{
			MSG msg;
			while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}				
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block*iWin);
			fp.Seek (k*Block,CFile::begin);
			//平方增强
			if(m_bExp)
				for(i=0;i<m_ImgNum*iWin;i++)
					Value[i]*=Value[i];
			//梯度增强
			if(m_bGrads)
				for(i=0;i<(m_ImgNum-1)*iWin;i++)
				{
					dalta=Value[i+1]-Value[i]+Value[m_ImgNum+i]-Value[i];
					Value[i]+=dalta;
				}

			//动态刻度
			dalta=Value[0];
			for(i=1;i<m_ImgNum*iWin;i++)
				dalta=(dalta+Value[i])/2.f;
			
			if(k+iWin+1>=RecordNum)
			{	
				for(i=0;i<m_ImgNum*iWin;i++)
					Value[i]=Value[i]/dalta*512.f;
				fp.Write (Value,Block*iWin);
				break;
			}
			else
			{
				for(i=0;i<m_ImgNum;i++)
					Value[i]=Value[i]/dalta*512.f;
				fp.Write (Value,Block);	
			}
			m_Progress.SetPos(k/100);
			k++;
		}
		delete Value;
	}
	fp.Close ();
	return TRUE;
}

void CACImagePre::OnButton1() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CImgFrequency Dlg;
	Dlg.m_WellName =m_WellName;
	Dlg.m_CurveName =m_CurveName;
	Dlg.m_ProEnDep =m_ProEnDep;
	Dlg.m_ProStDep =m_ProStDep;
	Dlg.m_Rlev=m_Rlev;
	Dlg.m_MaxValue=m_MaxValue;
	Dlg.m_MinValue=m_MinValue;
	Dlg.DoModal();
	m_MaxValue=Dlg.m_MaxValue;
	m_MinValue=Dlg.m_MinValue;
	UpdateData(FALSE);
}

void CACImagePre::OnButton3() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CFIDIO* pFIDR = NULL;
	pFIDR = new CFIDIO(m_WellName+"."+m_CurveName);
	if(!pFIDR->Open())
	{
		AfxMessageBox(m_WellName+"."+m_CurveName+"打开失败    ");
		delete pFIDR;return;
	}
	float Rlev=m_Rlev;
	float Sdep = m_ProStDep;
	float Edep = m_ProEnDep;	
	int dim=pFIDR->GetDimenSion();
	FIDCONTENT* Content;
	Content = pFIDR->GetContent(dim);
	if (Content == NULL)
	{
		delete pFIDR; return;
	}
	BeginWaitCursor();
	int nps= Content->nps;
	short* ibuf = NULL;
	float* Value = NULL;	
	char* cbuf = NULL;
	Value = new float[nps];
	if(Content->type != REPR_FLOAT)
	{
		if(Content->type == REPR_CHAR) 
			cbuf = new char[nps];
		else ibuf = new short[nps];
	}
	int i;
	float fdata,dep;
	//求出平均值	
	dep=Sdep;
	pFIDR->SeekToDepth(dep);
	if (Content->type == REPR_FLOAT)
		pFIDR->Read(dim,nps,Value);
	else if (Content->type == REPR_CHAR)
	{
		pFIDR->Read(dim,nps,cbuf);
		for(i=0;i<nps;i++)
			Value[i]=(float)cbuf[i];
	}
	else 
	{
		pFIDR->Read(dim,nps,ibuf);
		for(i=0;i<nps;i++)
			Value[i]=(float)ibuf[i];
	}
	pFIDR->Next();dep+=Rlev;
	fdata=Value[0];
	for(i=1;i<nps;i++)
		if((Value[i]!=-9999.f)&&(Value[i]!=-999.f))
			fdata=(fdata+Value[i])/2.f;	
	while(dep<Edep)
	{
		pFIDR->SeekToDepth(dep);
		if (Content->type == REPR_FLOAT)
			pFIDR->Read(dim,nps,Value);
		else if (Content->type == REPR_CHAR)
		{
			pFIDR->Read(dim,nps,cbuf);
			for(i=0;i<nps;i++)
				Value[i]=(float)cbuf[i];
		}
		else 
		{
			pFIDR->Read(dim,nps,ibuf);
			for(i=0;i<nps;i++)
				Value[i]=(float)ibuf[i];
		}
		dep+=Rlev;	
		for(i=0;i<nps;i++)
		{
			if((Value[i]!=-9999.f)&&(Value[i]!=-999.f))
				fdata=(fdata+Value[i])/2.f;	
		}
	}
	//最小值0 最大值fdata*2
	m_MinValue=0.f;
	m_MaxValue=fdata*2.f;
	delete pFIDR;delete Value;
	if (cbuf != NULL)	delete cbuf;
	if (ibuf != NULL)	delete ibuf;
	EndWaitCursor();
	UpdateData(FALSE);
}

void CACImagePre::AddNameList()
{
	m_ComName.ResetContent();
	char szFind[512],szFIDName[40];
	CString WellName=m_WellName;
	CFIDIO::GetFileName(WellName,szFind,'I');
	strcat(szFind,".*");
	CString szDir;
	CFIDIO::GetPath(WellName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();
	struct _finddata_t c_file;
	long hFile;
	if ((hFile = _findfirst(szFind, &c_file )) != -1L)
	{
		CFIDIO* pFIDIO = new CFIDIO(szDir + c_file.name);
		if (pFIDIO->Open())
		{
			CFIDIO::GetFIDName(c_file.name,szFIDName);
			if(pFIDIO->GetDimenSion() == 3)
				m_ComName.AddString (szFIDName);
		}
		delete pFIDIO;
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			CFIDIO* pFIDIO = new CFIDIO(szDir + c_file.name);
			if (pFIDIO->Open())
			{
				CFIDIO::GetFIDName(c_file.name,szFIDName);
				if(pFIDIO->GetDimenSion() == 3)
					m_ComName.AddString (szFIDName);
			}
			delete pFIDIO;
		}	        
		_findclose( hFile );
	}
	m_ComName.SetWindowText(m_CurveName);
}

//静态刻度图像数据
//对处理井段范围内的所有电扣数据求平均值
//用 0-平均值*2作为刻度范围
//对一深度点所有电扣数据相对刻度在0-1024
//最小值=0  最大值=平均值*2
//电扣值=(原电扣值-最小值)/(最大值-最小值)*1024
//电扣值=原电扣值/平均值*512
BOOL CACImagePre::DataStaticScale(CString OutName)
{
	int RecordNum;
	char s[256],fn[256];
	int Block=m_ImgNum*4;

	RecordNum=(int)((m_ProEnDep-m_ProStDep)/m_Rlev);
	m_Progress.ShowWindow(SW_SHOW);
	m_Progress.SetRange(0, RecordNum/100);
	m_Progress.SetPos(0);

	strcpy(s,m_WellName);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		float *Value;
		Value=new float[m_ImgNum+10];
		float fdata,MinValue;
		int i,k=0;
		if(m_StaScaleType==0)
		{
			GetDlgItem(IDC_STATIC_TODO)->SetWindowText(OutName+" 统计图像数据...");
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);
			//求取处理井段内平均值
			fdata=Value[0];
			for(i=0;i<m_ImgNum;i++)
				fdata=(fdata+Value[i])/2.f;		
			while(k<RecordNum)
			{
				MSG msg;
				while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}				
				fp.Seek (k*Block,CFile::begin);
				fp.Read (Value,Block);
				for(i=0;i<m_ImgNum;i++)
					fdata=(fdata+Value[i])/2.f;	
				m_Progress.SetPos(k/100);
				k++;
			}
			MinValue=0.f;
			fdata*=2.f;
		}
		else
		{
			MinValue=m_MinValue;
			fdata=m_MaxValue-MinValue;
		}
		k=0;
		GetDlgItem(IDC_STATIC_TODO)->SetWindowText(OutName+" 静态刻度图像数据...");		
		while(k<RecordNum)
		{
			MSG msg;
			while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}				
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);
			for(i=0;i<m_ImgNum;i++)
				Value[i]=(Value[i]-MinValue)/fdata*1024.f;
			fp.Seek (k*Block,CFile::begin);
			fp.Write (Value,Block);				
			m_Progress.SetPos(k/100);
			k++;
		}	
		delete Value;
	}
	fp.Close ();
	return TRUE;
}
