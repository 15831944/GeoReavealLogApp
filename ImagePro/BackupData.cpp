// BackupData.cpp: implementation of the CBackupData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowChart.h"
#include "FIDio.h"
#include "BackupData.h"
#include "FlowChartDoc.h"
#include "FlowNode.h"
#include "LinkWellData.h"
#include "BackupDlg.h"
#include "LogProcess.h"
#include "GainRegress.h"
#include "Guage.h"
#include "Clustering.h"
#include <direct.h>
#include <math.h>
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CBackupData, CFlowNode, 0)

CBackupData::CBackupData()
{
	m_PadNum=6;
	m_CalNum=6;
	m_ButtonNum=24;
	m_pDoc=NULL;
	m_Bits=152.f;
	m_szWellName=_T("");
	m_szSeries=_T("");
}

CBackupData::CBackupData(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_szSeries=_T("");
	m_PadNum=6; 
	m_ButtonNum=24;
	for(int i=0;i<64;i++)
		m_szPadNames[i]=_T(""); 
	m_CalNum=6; 
	for(i=0;i<64;i++)
		m_szCalNames[i]=_T("");               
	m_szAZ1Name=_T("");
	for(i=0;i<8;i++)
	{
		FCName[i][0]=_T(""); 
		FCName[i][1]=_T("");
	}

	//创建数据缓冲区路径
	CString szPath=GetWorkPath()+"Temp\\#PIC_";
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format(_T("%m%d%H%M%S"));
	szPath+=strTime;
	m_szBufPath=szPath+"\\PIC_"+strTime;
	m_StProDep=m_EnProDep=m_Rlev=-9999.f;

	m_szAMPName=_T("");                  //幅度曲线
	m_szTTName=_T("");                   //时间曲线
	m_szRADName=_T("");                  //半径曲线

	m_szGrdVol=_T("");                   //屏蔽电极测量电压阵列数据 PD6G
	m_szGrdGan=_T("");                   //屏蔽电极增益曲线 GG01
	m_szPadGan=_T("");                   //钮扣电极增益曲线     PADG

	m_szGAZFName=_T("");                 //仪器探头加速度(GAZFQH)
	m_szTimeName=_T("");                 //测井采样时间间隔(ETMIEQH)
	m_Rm=0.03f;
	m_Bits=152.f;
	m_bGain=FALSE;
	m_szWellName=_T("");
	m_szSeries=_T("");
}

CBackupData::~CBackupData()
{

}

BOOL CBackupData::Run()
{
	m_szOption=m_pDoc->m_szOption;
	CBackupDlg dlg(this);
	dlg.m_szWellName=m_szWellName;
	if(dlg.DoModal()==IDOK)
	{
		m_bResult=TRUE;
		m_szWellName=dlg.m_szWellName;
		CString szPath=m_szBufPath;	
		szPath=szPath.Mid(szPath.ReverseFind('\\')+1);
		m_szBufPath=m_szWellName.Left(m_szWellName.ReverseFind('\\')+1)+"#"+szPath;
		_mkdir(m_szBufPath);
		if(DetectFileExist(m_szBufPath))
		{
			m_szBufPath+="\\"+szPath;
			//声成像处理
			if((m_szSeries=="CBIL")||(m_szSeries=="CAST"))
			{
				m_bResult=ACDataCopy();
				if(m_bResult)
					SubDataCopy(0.00254f);//辅助曲线拷贝并重采样
				return m_bResult;
			}
			if(m_szSeries=="AMK2000")
			{
				m_bResult=CreatRADS("SGDT",m_Rlev);
				if(m_bResult)
				{
					m_bResult=CurveReSample(m_szBufPath+".SGDT","DATA00",0.025f);
					m_Rlev=0.025f;
					if(CurveReSample(m_szAZ1Name,"P1AZGPT",m_Rlev))
						BufCurveToWell("P1AZGPT","AZ1-PRO");
					CurveReSample(m_szRBName,"GRGPT",m_Rlev,FALSE);
				}
				return m_bResult;
			}
			if(m_szSeries=="ARI_16")
			{
				m_bResult=CreatRADS("ARI16",m_Rlev);
				if(m_bResult)
				{
					m_bResult=CurveReSample(m_szBufPath+".ARI16","DATA00",0.0025f);
					DataVerFilterPro("ARIGPT",1);
					if(m_pDoc!=NULL)
						m_pDoc->SetModifiedFlag();
				}
				return m_bResult;
			}
			if(m_szSeries=="DIP6"||m_szSeries=="SED")
			{
				m_bResult=DipDataCopy(0.00254f);
				if(m_bResult)
				{
					m_bResult=CreatRADS("CALARY",0.00254f);
					if(m_bResult)
						SubDataCopy(0.00254f);//辅助曲线拷贝并重采样
				}
				return m_bResult;
			}
			//
			//微电阻率成像处埋
			//
			//备份极板数据
			if(m_szSeries.Find("FMI")>-1)
				m_bResult=FMIDataCopy(0.00254f);          
			else if(m_szSeries.Find("CMI")>-1)
				m_bResult=CMIDataCopy(0.00254f);  
			else 
				m_bResult=EMISTARDataCopy(0.00254f); 
			if(m_bResult)
			{
				m_Rlev=0.00254f;
				if(CreatRADS("RADSGPT",0.00254f))
				{
					//倾角曲线拷贝并重采样
					DipDataCopy(0.00254f);
					//辅助曲线拷贝并重采样
					SubDataCopy(0.00254f);
					//STAR系列增益校正
					if(m_bGain&&m_szSeries.Find("STAR")>-1)
					{
						STARGainCopy(0.00254f); 
						m_GainPro.InitPara(m_pDoc);
						m_GainPro.Run();
					}
				}
			}
		}
		if(m_pDoc!=NULL)
			m_pDoc->SetModifiedFlag();
	}
	return m_bResult;
}

void CBackupData::InitPara(CFlowChartDoc* pDoc)
{
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		CFlowNode *pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{		
			if(pObj->IsKindOf(RUNTIME_CLASS(CLinkWellData)))
				m_szWellName=((CLinkWellData*)pObj)->m_szWellName;
		}
	}
	m_GainPro.InitPara(pDoc);
	m_pDoc=pDoc;
	CString szStr=m_pDoc->m_DocFileName;
	int x=szStr.Find("#");
	if(x>0)
	{
		CString szPath=szStr.Left(x);
		szStr=szStr.Mid(x+1);
		CString szWell=szStr.Left(szStr.Find("\\"));
		m_szWellName=szPath+"#"+szWell+"\\"+szWell;
		szStr=m_szBufPath.Mid(m_szBufPath.Find("\\#PIC"));
		m_szBufPath=szPath+"#"+szWell+szStr;
	}
}

void CBackupData::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	int i;CString ss="备用";
	if(ar.IsStoring())
	{
		ar<<m_szBufPath;
		ar<<m_szWellName;
		ar<<m_szSeries;
		ar<<m_StProDep;
		ar<<m_EnProDep;
		ar<<m_Rlev;
		ar<<m_PadNum; 
		ar<<m_ButtonNum;
		ar<<m_ButtonMid;
		for(i=0;i<m_PadNum;i++)
			ar<<m_szPadNames[i]; 
		ar<<m_CalNum; 
		for(i=0;i<m_CalNum;i++)
			ar<<m_szCalNames[i];                
		ar<<m_szAZ1Name;
		for(i=0;i<8;i++)
		{
			ar<<FCName[i][0]; 
			ar<<FCName[i][1];
		}

		ar<<m_szGAZFName;                 //仪器探头加速度(GAZFQH)
		ar<<m_szTimeName;                 //测井采样时间间隔(ETMIEQH)

		for(i=0;i<m_PadNum;i++)
			ar<<m_szDIPNames[i]; 
		ar<<m_szDazName;      
		ar<<m_szDevName;      
		ar<<m_szRBName; 

		ar<<m_szAMPName;
		ar<<m_szTTName;
		ar<<m_szRADName;

		ar<<m_szGrdVol;                  //屏蔽电极测量电压阵列数据 PD6G 
		ar<<m_szGrdGan;					 //屏蔽电极增益曲线 GG01
		ar<<m_szPadGan;					 //钮扣电极增益曲线     PADG
		ar<<m_Rm;
        
		ar<<ss;
		ar<<ss;
		ar<<m_Bits;
		ar<<m_bGain;
	}
	else
	{
		ar>>m_szBufPath;
		ar>>m_szWellName;
		ar>>m_szSeries;
		ar>>m_StProDep;
		ar>>m_EnProDep;
		ar>>m_Rlev;
		ar>>m_PadNum; 
		ar>>m_ButtonNum;
		ar>>m_ButtonMid;
		for(i=0;i<m_PadNum;i++)
			ar>>m_szPadNames[i]; 
		ar>>m_CalNum;
		for(i=0;i<m_CalNum;i++) 
			ar>>m_szCalNames[i];               
		ar>>m_szAZ1Name;
		for(i=0;i<8;i++)
		{
			ar>>FCName[i][0]; 
			ar>>FCName[i][1];
		}
		ar>>m_szGAZFName;                 //仪器探头加速度(GAZFQH)
		ar>>m_szTimeName;                 //测井采样时间间隔(ETMIEQH)

		for(i=0;i<m_PadNum;i++)
			ar>>m_szDIPNames[i]; 
		ar>>m_szDazName;      
		ar>>m_szDevName;      
		ar>>m_szRBName; 

		ar>>m_szAMPName;
		ar>>m_szTTName;
		ar>>m_szRADName;

		ar>>m_szGrdVol;                   //屏蔽电极测量电压阵列数据 PD6G
		ar>>m_szGrdGan;					  //屏蔽电极增益曲线 GG01
		ar>>m_szPadGan;					  //钮扣电极增益曲线     PADG
		ar>>m_Rm;

		ar>>ss;
		ar>>ss;
		ar>>m_Bits;
		ar>>m_bGain;
	}
}

BOOL CBackupData::ACDataCopy(float NewRlev/*=0.00254f*/)
{
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"S","m",REPR_FLOAT,2,250,0,0.f,1920.f,2.f},
							   {"P","mv",REPR_FLOAT,4,250,250,0.f,1920.f,2.f}};
	char adi[] = "DATID 0";			
	int i,j;	
	CFIDIO* PadI= NULL;
	CFIDIO* PadO= NULL;
	float *Value  =new float[m_ButtonNum];
	short *InValue=new short[m_ButtonNum];
	char  *ChValue=new char[m_ButtonNum];
	
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	

	float Rlev,data;
	BOOL bBreak=FALSE;

	unsigned k0,k;
	CString Name[3],OutName[3],szName[3];BOOL bFlag[3];
	bFlag[0]=FALSE;Name[0]=m_szAMPName;OutName[0]="AMPGPT";szName[0]="幅度曲线";
	bFlag[1]=FALSE;Name[1]=m_szTTName;OutName[1]="TTGPT";szName[1]="时间曲线";

	CString szStr;
	unsigned  RecNum;
	for(i=0;i<2;i++)
	{	
		if(szName[i]=="") 
			continue;
		szStr.Format ("%s数据备份...",szName[i]);
		pGuage->SetTitle(szStr);

		PadI=new CFIDIO(m_szWellName+"."+Name[i]);
		if(!PadI->Open())
		{
			delete PadI;
			continue;
		}
		Rlev=PadI->GetDepthLevel();

		FIDCONTENT* Content = PadI->GetContent(3);

		RecNum=(unsigned)((m_EnProDep-m_StProDep)/Rlev);
		m_Content[0].min = m_StProDep;
		m_Content[0].max = m_EnProDep;
		m_Content[0].rlev = NewRlev;
		m_Content[1].nps=m_ButtonNum;
		m_Content[2].nps=m_ButtonNum;
		m_Content[2].npw=m_ButtonNum;
		strcpy(m_Content[2].name ,OutName[i]);
		FIDINDEX m_Index;
		
		m_Index.Contentlist = m_Content;
		m_Index.dimension = 3;
		m_Index.adi = adi;

		szStr=m_szBufPath+"."+OutName[i];
		if(!PadO->Open())
		{
			delete PadI;delete PadO;
			bBreak=TRUE;
			break;
		}
		
		k0=0;k=0;
		PadI->SeekToDepth(m_StProDep);
		PadO->SeekToDepth(m_StProDep);
		if(Content->type == REPR_FLOAT)
		{
			while(k0<=RecNum)
			{
				PadI->Read(3,m_ButtonNum,Value);
				PadI->Next();
				data=0.375f;
				for(j=0;j<m_ButtonNum;j++)
				{
					if(Value[j]>0.375f)
					{
						data=Value[j];
						break;
					}
				}
				Value[0]=data;
				for(j=1;j<m_ButtonNum;j++)
				{
					if(Value[j]<0.375f)
						Value[j]=Value[j-1];
				}
				while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
				{
					PadO->Write(3,m_ButtonNum,Value);
					PadO->Next();
					k++;
				}
				k0++;
				pGuage->SetDepth(m_StProDep+Rlev*k0);
				if (pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		else if(Content->length== REPR_CHAR)
		{
			while(k0<=RecNum)
			{
				PadI->Read(3,m_ButtonNum,ChValue);
				PadI->Next();
				for(j=0;j<m_ButtonNum;j++)
					Value[j]=(float)(ChValue[j]);
				data=0.375f;
				for(j=0;j<m_ButtonNum;j++)
				{
					if(Value[j]>0.375f)
					{
						data=Value[j];
						break;
					}
				}
				Value[0]=data;
				for(j=1;j<m_ButtonNum;j++)
				{
					if(Value[j]<0.375f)
						Value[j]=Value[j-1];
				}
				while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
				{
					PadO->Write(3,m_ButtonNum,Value);
					PadO->Next();
					k++;
				}
				k0++;
				pGuage->SetDepth(m_StProDep+Rlev*k0);
				if(pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		else
		{
			while(k0<=RecNum)
			{
				PadI->Read(3,m_ButtonNum,InValue);
				PadI->Next();
				for(j=0;j<m_ButtonNum;j++)
					Value[j]=(float)InValue[j];
				data=0.375f;
				for(j=0;j<m_ButtonNum;j++)
				{
					if(Value[j]>0.375f)
					{
						data=Value[j];
						break;
					}
				}
				Value[0]=data;
				for(j=1;j<m_ButtonNum;j++)
				{
					if(Value[j]<0.375f)
						Value[j]=Value[j-1];
				}
				while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
				{
					PadO->Write(3,m_ButtonNum,Value);
					PadO->Next();
					k++;
				}
				k0++;
				pGuage->SetDepth(m_StProDep+Rlev*k0);
				if (pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		delete PadI;
		delete PadO;
		if(bBreak) break;
		bFlag[i]=TRUE;
	}
	m_Rlev=NewRlev;
	delete Value;delete InValue;delete ChValue;
	delete pGuage;
	if(bFlag[0]||bFlag[1]||bFlag[2]) return TRUE;
	else			                 return FALSE;
}

BOOL CBackupData::EMISTARDataCopy(float NewRlev/*=0.00254f*/)
{
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"S","mm",REPR_FLOAT,4,25,0,0.f,1920.f,2.f},
							   {"P","1/ohm",REPR_FLOAT,4,25,25,0.f,1920.f,2.f}};
	char adi[] = "DATID 0";			
	float *Value=NULL;
	short *iValue=NULL;
	char  *cValue=NULL;

	int i,j,nps;
	BOOL bBreak=FALSE;	
	FIDCONTENT* Content=NULL;
	CFIDIO* pFID=new CFIDIO(m_szWellName+"."+m_szPadNames[0]);
	if(pFID->Open())
	{

		Content = pFID->GetContent(3);
		if(Content != NULL)
		{
			nps=Content->nps;
			if(Content->type==REPR_CHAR) 
				cValue=new char[nps];
			else if(Content->type==REPR_SHORT)
				iValue=new short[nps];
		}
		else 
			bBreak=TRUE;
	}
	else bBreak=TRUE;
	delete pFID;
	if(bBreak) return FALSE;


	Value=new float[nps];

	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	

	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = NewRlev;
	m_Content[1].nps=nps;
	m_Content[2].nps=nps;
	m_Content[2].npw=nps;

	CString szStr;
	unsigned k0,k,RecNum;
	float dep,Rlev;
	CFIDIO* PadI=NULL;
	CFIDIO* PadO= NULL;
	for(i=0;i<m_PadNum;i++)
	{	
		szStr.Format ("%i号极板数据备份...",i+1);
		pGuage->SetTitle(szStr);
		PadI=new CFIDIO(m_szWellName+"."+m_szPadNames[i]);
		if(!PadI->Open())
		{
			delete PadI;
			bBreak=TRUE;
			break;
		}
		Rlev=PadI->GetDepthLevel();
		RecNum=(unsigned)((m_EnProDep-m_StProDep)/Rlev);

		szStr.Format ("P%iGPT",i);
		strcpy(m_Content[2].name ,szStr);
		FIDINDEX m_Index;
		
		m_Index.Contentlist = m_Content;
		m_Index.dimension = 3;
		m_Index.adi = adi;
		
		szStr=m_szBufPath+"."+szStr;
		PadO = new CFIDIO(szStr,&m_Index);
		if(!PadO->Open())
		{
			delete PadI;delete PadO;
			bBreak=TRUE;
			break;
		}
		dep=m_StProDep;
		PadI->SeekToDepth(m_StProDep);
		k0=0;k=0;
		if(Content->type == REPR_CHAR) 
		{
			while(k0<=RecNum)
			{
				PadI->Read(3,nps,cValue);
				PadI->Next();
				for(j=0;j<nps;j++)
					Value[j]=(float)cValue[j];
				while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
				{
					PadO->Write(3,nps,Value);
					PadO->Next();
					k++;
				}
				k0++;
				dep=m_StProDep+Rlev*k0;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		else if(Content->type == REPR_SHORT)
		{
			while(k0<=RecNum)
			{
				PadI->Read(3,nps,iValue);
				PadI->Next();
				for(j=0;j<nps;j++)
					Value[j]=(float)iValue[j];
				while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
				{
					PadO->Write(3,nps,Value);
					PadO->Next();
					k++;
				}
				k0++;
				dep=m_StProDep+Rlev*k0;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		else
		{
			while(k0<=RecNum)
			{
				PadI->Read(3,nps,Value);
				PadI->Next();
				while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
				{
					PadO->Write(3,nps,Value);
					PadO->Next();
					k++;
				}
				k0++;
				dep=m_StProDep+Rlev*k0;
				pGuage->SetDepth(dep);
				if(pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		delete PadI;delete PadO;
		if(bBreak) break;
	}
	m_Rlev=NewRlev;
	delete Value;delete pGuage;
	if(iValue!=NULL) delete iValue;
	if(cValue!=NULL) delete cValue;
	return !bBreak;
}
/*
BOOL CBackupData::FMIDataCopy(float NewRlev)
{
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"S","mm",REPR_FLOAT,4,24,0,0.f,1920.f,2.f},
							   {"P","1/ohm",REPR_FLOAT,4,24,24,0.f,1920.f,2.f}};
	char adi[] = "DATID 0";	
		
	int i,j,type,nps=12;	
	
	short* iV1 = NULL;
	short* iV2 = NULL;
	float* fV1 = NULL;
	float* fV2 = NULL;
	char* cV1 = NULL;
	char* cV2 = NULL;

	float* iBV1 = NULL;
	float* iBV2 = NULL;
	float* fBV1 = NULL;
	float* fBV2 = NULL;
	float* cBV1 = NULL;
	float* cBV2 = NULL;

	BOOL bBreak=FALSE;	
	FIDCONTENT* Content=NULL;
	CFIDIO* PadI=new CFIDIO(m_szWellName+"."+FCName[0][0]);
	if(PadI->Open())
	{
		Content = PadI->GetContent(3);
		if(Content != NULL)
		{
			nps=Content->nps;
			type=Content->type;
			if( type== REPR_CHAR) 
			{
				cV1 = new char[nps];
				cV2 = new char[nps];
				cBV1 = new float[nps];
				cBV2 = new float[nps];
			}
			else if(type == REPR_SHORT)
			{
				iV1 = new short[nps];
				iV2 = new short[nps];
				iBV1 = new float[nps];
				iBV2 = new float[nps];
			}
			else
			{
				fV1 = new float[nps];
				fV2 = new float[nps];
				fBV1 = new float[nps];
				fBV2 = new float[nps];
			}
		}
		else 
			bBreak=TRUE;
	}
	else bBreak=TRUE;
	delete PadI;
	if(bBreak) return FALSE;

	float *Value=new float[m_ButtonNum];
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);
	
	float dep,Rlev;
	unsigned k0,k,RecNum;
	bBreak=FALSE;
	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = NewRlev;

	CFIDIO* PadI1=NULL;
	CFIDIO* PadI2=NULL;
	CFIDIO* PadO =NULL;
	CFIDIO* Pin1 =NULL;
	CFIDIO* Pin2 =NULL;

	FIDINDEX m_Index;
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	CString szStr;
	BOOL bBCEG=TRUE;
	for(i=0;i<m_PadNum;i++)
	{	
		PadI1=new CFIDIO(m_szWellName+"."+FCName[i][0]);
		if(!PadI1->Open())
		{
			delete PadI1;
			bBreak=TRUE;
			break;
		}
		Rlev=PadI1->GetDepthLevel();
		RecNum=(unsigned)((m_EnProDep-m_StProDep)/Rlev);

		PadI2=new CFIDIO(m_szWellName+"."+FCName[i][1]);
		if(!PadI2->Open())
		{
			delete PadI1;delete PadI2;
			bBreak=TRUE;
			break;
		}
		szStr.Format ("P%iGPT",i);
		strcpy(m_Content[2].name ,szStr);
		szStr=m_szBufPath+"."+szStr;
		PadO = new CFIDIO(szStr,&m_Index);
		if(!PadO->Open())
		{
			delete PadI1;delete PadI2;delete PadO;
			bBreak=TRUE;
			break;
		}
		//探索是否有对应有BCEG曲线
		bBCEG=TRUE;
		szStr=FCName[i][0];
		szStr="BCEG"+szStr.Mid(szStr.GetLength()-2);
		Pin1=new CFIDIO(m_szWellName+"."+szStr);
		if(!Pin1->Open())
		{
			delete Pin1;
			bBCEG=FALSE;
		}
		szStr=FCName[i][1];
		szStr="BCEG"+szStr.Mid(szStr.GetLength()-2);
		Pin2=new CFIDIO(m_szWellName+"."+szStr);
		if(!Pin2->Open())
		{
			delete Pin2;
			bBCEG=FALSE;
		}
		szStr.Format ("%i号极板数据备份...",i+1);
		pGuage->SetTitle(szStr);
		dep=m_StProDep;
		PadI1->SeekToDepth(dep);
		PadI2->SeekToDepth(dep);
		k0=0;k=0;
		if(bBCEG&&m_bGain)//有BCEG曲线
		{
			Pin1->SeekToDepth(dep);
			Pin2->SeekToDepth(dep);
			if(type == REPR_CHAR) 
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,cV1);
					PadI1->Next();
					PadI2->Read(3,nps,cV2);
					PadI2->Next();
					
					Pin1->Read(3,nps,cBV1);
					Pin1->Next();
					Pin2->Read(3,nps,cBV2);
					Pin2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j]=((float)cV1[j])*((float)cBV1[j]);
						Value[2*j+1]=((float)cV2[j])*((float)cBV2[j]);
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else if(type == REPR_SHORT)
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,iV1);
					PadI1->Next();
					PadI2->Read(3,nps,iV2);
					PadI2->Next();
					
					Pin1->Read(3,nps,iBV1);
					Pin1->Next();
					Pin2->Read(3,nps,iBV2);
					Pin2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j]=(float)iV1[j]*iBV1[j];
						Value[2*j+1]=(float)iV2[j]*iBV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,fV1);
					PadI1->Next();
					PadI2->Read(3,nps,fV2);
					PadI2->Next();

					Pin1->Read(3,nps,fBV1);
					Pin1->Next();
					Pin2->Read(3,nps,fBV2);
					Pin2->Next();

					for(j=0;j<nps;j++)
					{
						Value[2*j]=fV1[j]*fBV1[j];
						Value[2*j+1]=fV2[j]*fBV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
		}
		else if(m_bGain)
		{
			if(type == REPR_CHAR) 
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,cV1);
					PadI1->Next();
					PadI2->Read(3,nps,cV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j]=fabs((float)cV1[j]);
						Value[2*j+1]=fabs((float)cV2[j]);
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else if(type == REPR_SHORT)
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,iV1);
					PadI1->Next();
					PadI2->Read(3,nps,iV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j]=fabs((float)iV1[j]);
						Value[2*j+1]=fabs((float)iV2[j]);
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,fV1);
					PadI1->Next();
					PadI2->Read(3,nps,fV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j]=fabs(fV1[j]);
						Value[2*j+1]=fabs(fV2[j]);
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
		}
		else
		{
			if(type == REPR_CHAR) 
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,cV1);
					PadI1->Next();
					PadI2->Read(3,nps,cV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j]=(float)cV1[j];
						Value[2*j+1]=(float)cV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else if(type == REPR_SHORT)
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,iV1);
					PadI1->Next();
					PadI2->Read(3,nps,iV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j]=(float)iV1[j];
						Value[2*j+1]=(float)iV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,fV1);
					PadI1->Next();
					PadI2->Read(3,nps,fV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j]=fV1[j];
						Value[2*j+1]=fV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
		}
		delete PadI1;delete PadI2;delete PadO;delete Pin1;delete Pin2;
		if(bBreak) break;
	}
	delete Value;delete pGuage;
	if(iV1 != NULL) delete iV1;if(iV2 != NULL) delete iV2;
	if(fV1 != NULL) delete fV1;if(fV2 != NULL) delete fV2;
	if(cV1 != NULL) delete cV1;if(cV2 != NULL) delete cV2;
	if(iBV1 != NULL) delete iBV1;if(iBV2 != NULL) delete iBV2;
	if(cBV1 != NULL) delete cBV1;if(cBV2 != NULL) delete cBV2;
	if(fBV1 != NULL) delete fBV1;if(fBV2 != NULL) delete fBV2;
	m_Rlev=NewRlev;

	return !bBreak;
}
*/

//张梦宁 2019-05-24
//FMI极板数据排列：原始16个极板数据合并为8个，即1个极板1个阵列数据。每个极板合并时按先下排后上排的顺序依次交错排列。
BOOL CBackupData::FMIDataCopy(float NewRlev)
{
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
	{"S","mm",REPR_FLOAT,4,24,0,0.f,1920.f,2.f},
	{"P","1/ohm",REPR_FLOAT,4,24,24,0.f,1920.f,2.f}};
	char adi[] = "DATID 0";	

	int i,j,type,nps=12;	

	short* iV1 = NULL;
	short* iV2 = NULL;
	float* fV1 = NULL;
	float* fV2 = NULL;
	char* cV1 = NULL;
	char* cV2 = NULL;

	float* iBV1 = NULL;
	float* iBV2 = NULL;
	float* fBV1 = NULL;
	float* fBV2 = NULL;
	float* cBV1 = NULL;
	float* cBV2 = NULL;

	BOOL bBreak=FALSE;	
	FIDCONTENT* Content=NULL;
	CFIDIO* PadI=new CFIDIO(m_szWellName+"."+FCName[0][0]);
	if(PadI->Open())
	{
		Content = PadI->GetContent(3);
		if(Content != NULL)
		{
			nps=Content->nps;
			type=Content->type;
			if( type== REPR_CHAR) 
			{
				cV1 = new char[nps];
				cV2 = new char[nps];
				cBV1 = new float[nps];
				cBV2 = new float[nps];
			}
			else if(type == REPR_SHORT)
			{
				iV1 = new short[nps];
				iV2 = new short[nps];
				iBV1 = new float[nps];
				iBV2 = new float[nps];
			}
			else
			{
				fV1 = new float[nps];
				fV2 = new float[nps];
				fBV1 = new float[nps];
				fBV2 = new float[nps];
			}
		}
		else 
			bBreak=TRUE;
	}
	else bBreak=TRUE;
	delete PadI;
	if(bBreak) return FALSE;

	float *Value=new float[m_ButtonNum];
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);

	float dep,Rlev;
	unsigned k0,k,RecNum;
	bBreak=FALSE;

	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = NewRlev;

	CFIDIO* PadI1=NULL;
	CFIDIO* PadI2=NULL;
	CFIDIO* PadO =NULL;
	CFIDIO* Pin1 =NULL;
	CFIDIO* Pin2 =NULL;

	FIDINDEX m_Index;
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	CString szStr;
	BOOL bBCEG=TRUE;
	for(i=0;i<m_PadNum;i++)
	{	
		PadI1=new CFIDIO(m_szWellName+"."+FCName[i][0]);
		if(!PadI1->Open())
		{
			delete PadI1;
			bBreak=TRUE;
			break;
		}
		Rlev=PadI1->GetDepthLevel();
		RecNum=(unsigned)((m_EnProDep-m_StProDep)/Rlev);

		PadI2=new CFIDIO(m_szWellName+"."+FCName[i][1]);
		if(!PadI2->Open())
		{
			delete PadI1;delete PadI2;
			bBreak=TRUE;
			break;
		}
		szStr.Format ("P%iGPT",i);
		strcpy(m_Content[2].name ,szStr);
		szStr=m_szBufPath+"."+szStr;
		PadO = new CFIDIO(szStr,&m_Index);
		if(!PadO->Open())
		{
			delete PadI1;delete PadI2;delete PadO;
			bBreak=TRUE;
			break;
		}
		//探索是否有对应有BCEG曲线
		bBCEG=TRUE;
		szStr=FCName[i][0];
		szStr="BCEG"+szStr.Mid(szStr.GetLength()-2);
		Pin1=new CFIDIO(m_szWellName+"."+szStr);
		if(!Pin1->Open())
		{
			delete Pin1;
			bBCEG=FALSE;
		}
		szStr=FCName[i][1];
		szStr="BCEG"+szStr.Mid(szStr.GetLength()-2);
		Pin2=new CFIDIO(m_szWellName+"."+szStr);
		if(!Pin2->Open())
		{
			delete Pin2;
			bBCEG=FALSE;
		}
		szStr.Format ("%i号极板数据备份...",i+1);
		pGuage->SetTitle(szStr);
		dep=m_StProDep;
		PadI1->SeekToDepth(dep);
		PadI2->SeekToDepth(dep);
		k0=0;k=0;
		if(bBCEG&&m_bGain)//有BCEG曲线
		{
			Pin1->SeekToDepth(dep);
			Pin2->SeekToDepth(dep);
			if(type == REPR_CHAR) 
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,cV1);
					PadI1->Next();
					PadI2->Read(3,nps,cV2);
					PadI2->Next();

					Pin1->Read(3,nps,cBV1);
					Pin1->Next();
					Pin2->Read(3,nps,cBV2);
					Pin2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j+1]=((float)cV1[j])*((float)cBV1[j]);  //第一排纽扣电极在后
						Value[2*j]=((float)cV2[j])*((float)cBV2[j]);    //第二排纽扣电极在前
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else if(type == REPR_SHORT)
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,iV1);
					PadI1->Next();
					PadI2->Read(3,nps,iV2);
					PadI2->Next();

					Pin1->Read(3,nps,iBV1);
					Pin1->Next();
					Pin2->Read(3,nps,iBV2);
					Pin2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j+1]=(float)iV1[j]*iBV1[j];
						Value[2*j]=(float)iV2[j]*iBV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,fV1);
					PadI1->Next();
					PadI2->Read(3,nps,fV2);
					PadI2->Next();

					Pin1->Read(3,nps,fBV1);
					Pin1->Next();
					Pin2->Read(3,nps,fBV2);
					Pin2->Next();

					for(j=0;j<nps;j++)
					{
						Value[2*j+1]=fV1[j]*fBV1[j];
						Value[2*j]=fV2[j]*fBV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
		}
		else if(m_bGain)
		{
			if(type == REPR_CHAR) 
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,cV1);
					PadI1->Next();
					PadI2->Read(3,nps,cV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j+1]=fabs((float)cV1[j]);
						Value[2*j]=fabs((float)cV2[j]);
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else if(type == REPR_SHORT)
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,iV1);
					PadI1->Next();
					PadI2->Read(3,nps,iV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j+1]=fabs((float)iV1[j]);
						Value[2*j]=fabs((float)iV2[j]);
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,fV1);
					PadI1->Next();
					PadI2->Read(3,nps,fV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j+1]=fabs(fV1[j]);
						Value[2*j]=fabs(fV2[j]);
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
		}
		else
		{
			if(type == REPR_CHAR) 
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,cV1);
					PadI1->Next();
					PadI2->Read(3,nps,cV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j+1]=(float)cV1[j];
						Value[2*j]=(float)cV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else if(type == REPR_SHORT)
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,iV1);
					PadI1->Next();
					PadI2->Read(3,nps,iV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j+1]=(float)iV1[j];
						Value[2*j]=(float)iV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
			else
			{
				while(k0<=RecNum)
				{
					PadI1->Read(3,nps,fV1);
					PadI1->Next();
					PadI2->Read(3,nps,fV2);
					PadI2->Next();
					for(j=0;j<nps;j++)
					{
						Value[2*j+1]=fV1[j];
						Value[2*j]=fV2[j];
					}
					while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
					{
						PadO->Write(3,m_ButtonNum,Value);
						PadO->Next();
						k++;
					}
					k0++;
					dep=m_StProDep+Rlev*k0;
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
		}
		delete PadI1;delete PadI2;delete PadO;delete Pin1;delete Pin2;
		if(bBreak) break;
	}
	delete Value;delete pGuage;
	if(iV1 != NULL) delete iV1;if(iV2 != NULL) delete iV2;
	if(fV1 != NULL) delete fV1;if(fV2 != NULL) delete fV2;
	if(cV1 != NULL) delete cV1;if(cV2 != NULL) delete cV2;
	if(iBV1 != NULL) delete iBV1;if(iBV2 != NULL) delete iBV2;
	if(cBV1 != NULL) delete cBV1;if(cBV2 != NULL) delete cBV2;
	if(fBV1 != NULL) delete fBV1;if(fBV2 != NULL) delete fBV2;
	m_Rlev=NewRlev;

	return !bBreak;
}

BOOL CBackupData::CMIDataCopy(float NewRlev/*=0.00254f*/)
{
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"S","mm",REPR_FLOAT,4,24,0,0.f,1920.f,2.f},
							   {"P","1/ohm",REPR_FLOAT,4,24,24,0.f,1920.f,2.f}};
	char adi[] = "DATID 0";	
		
	int i,j,type,nps=12;	
	
	short* iV1 = NULL;
	short* iV2 = NULL;
	float* fV1 = NULL;
	float* fV2 = NULL;
	char* cV1 = NULL;
	char* cV2 = NULL;
	BOOL bBreak=FALSE;	
	FIDCONTENT* Content=NULL;
	CFIDIO* PadI=new CFIDIO(m_szWellName+"."+FCName[0][0]);
	if(PadI->Open())
	{
		Content = PadI->GetContent(3);
		if(Content != NULL)
		{
			type=Content->type;
			if( type== REPR_CHAR) 
			{
				cV1 = new char[nps];
				cV2 = new char[nps];
			}
			else if(type == REPR_SHORT)
			{
				iV1 = new short[nps];
				iV2 = new short[nps];
			}
			else
			{
				fV1 = new float[nps];
				fV2 = new float[nps];
			}
		}
		else 
			bBreak=TRUE;
	}
	else bBreak=TRUE;
	delete PadI;
	if(bBreak) return FALSE;

	float *Value=new float[m_ButtonNum];
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);
	
	float dep,Rlev;
	unsigned k0,k;
	bBreak=FALSE;
	unsigned RecNum=(unsigned)((m_EnProDep-m_StProDep)/m_Rlev);
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev= NewRlev;
	
	CString szStr;
	CFIDIO* PadI1= NULL;
	CFIDIO* PadI2= NULL;
	CFIDIO* PadIO= NULL;
	for(i=0;i<m_PadNum;i++)
	{	
		szStr.Format ("%i号极板数据备份...",i+1);
		pGuage->SetTitle(szStr);

		PadI1=new CFIDIO(m_szWellName+"."+FCName[i][0]);
		if(!PadI1->Open())
		{
			delete PadI1;
			bBreak=TRUE;
			break;
		}
		Rlev=PadI1->GetDepthLevel();

		PadI2=new CFIDIO(m_szWellName+"."+FCName[i][1]);
		if(!PadI2->Open())
		{
			delete PadI1;delete PadI2;
			bBreak=TRUE;
			break;
		}
		nps=PadI2->GetContent(3)->nps;

		szStr.Format ("P%iGPT",i);
		strcpy(m_Content[2].name ,szStr);
		FIDINDEX m_Index;
		m_Index.Contentlist = m_Content;
		m_Index.dimension = 3;
		m_Index.adi = adi;
		CFIDIO* PadO = NULL;

		szStr=m_szBufPath+"."+szStr;
		PadO = new CFIDIO(szStr,&m_Index);
		if(!PadO->Open())
		{
			delete PadI1;delete PadI2;delete PadO;
			bBreak=TRUE;
			break;
		}
		dep=m_StProDep;
		PadI1->SeekToDepth(dep);
		PadI2->SeekToDepth(dep);

		k0=0;k=0;
		if(type == REPR_CHAR) 
		{
			while(k0<=RecNum)
			{
				PadI1->Read(3,nps,cV1);
				PadI1->Next();
				PadI2->Read(3,nps,cV2);
				PadI2->Next();
				if(nps==10)
				{
					for(j=1;j<nps+1;j++)
					{
						Value[2*j]=(float)cV1[j-1];
						Value[2*j+1]=(float)cV2[j-1];
					}
					Value[0]=Value[2];
					Value[1]=Value[2];
					Value[22]=Value[21];
					Value[23]=Value[21];
				}
				else
				{
					for(j=0;j<nps;j++)
					{
						Value[2*j]=(float)cV1[j];
						Value[2*j+1]=(float)cV2[j];
					}
				}
				while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
				{
					PadO->Write(3,m_ButtonNum,Value);
					PadO->Next();
					k++;
				}
				k0++;
				dep=m_StProDep+Rlev*k0;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		else if(type == REPR_SHORT)
		{
			while(k0<=RecNum)
			{
				PadI1->Read(3,nps,iV1);
				PadI1->Next();
				PadI2->Read(3,nps,iV2);
				PadI2->Next();
				if(nps==10)
				{
					for(j=1;j<nps+1;j++)
					{
						Value[2*j]=(float)iV1[j-1];
						Value[2*j+1]=(float)iV2[j-1];
					}
					Value[0]=Value[2];
					Value[1]=Value[2];
					Value[22]=Value[21];
					Value[23]=Value[21];
				}
				else
				{
					for(j=0;j<nps;j++)
					{
						Value[2*j]=(float)iV1[j];
						Value[2*j+1]=(float)iV2[j];
					}
				}
				while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
				{
					PadO->Write(3,m_ButtonNum,Value);
					PadO->Next();
					k++;
				}
				k0++;
				dep=m_StProDep+Rlev*k0;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		else
		{
			while(k0<=RecNum)
			{
				PadI1->Read(3,nps,fV1);
				PadI1->Next();
				PadI2->Read(3,nps,fV2);
				PadI2->Next();
				if(nps==10)
				{
					for(j=1;j<nps+1;j++)
					{
						Value[2*j]=(float)fV1[j-1];
						Value[2*j+1]=(float)fV2[j-1];
					}
					Value[0]=Value[2];
					Value[1]=Value[2];
					Value[22]=Value[21];
					Value[23]=Value[21];
				}
				else
				{
					for(j=0;j<nps;j++)
					{
						Value[2*j]=fV1[j];
						Value[2*j+1]=fV2[j];
					}
				}
				while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
				{
					PadO->Write(3,m_ButtonNum,Value);
					PadO->Next();
					k++;
				}
				k0++;
				dep=m_StProDep+Rlev*k0;
				pGuage->SetDepth(dep);
				if(pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		delete PadI1;delete PadI2;delete PadO;
		if(bBreak) break;
	}
	delete Value;delete pGuage;
	if(iV1 != NULL) delete iV1;if(iV2 != NULL) delete iV2;
	if(fV1 != NULL) delete fV1;if(fV2 != NULL) delete fV2;
	if(cV1 != NULL) delete cV1;if(cV2 != NULL) delete cV2;
	m_Rlev=NewRlev;

	return !bBreak;
}
//合并标准深度、加速度、时间生成新阵列曲线
BOOL CBackupData::CreateZACCArray()
{
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"S","mS",REPR_FLOAT,4,2,0,0.f,32767.f,1.f},
							   {"GAZFGPT","none",REPR_FLOAT,4,2,2,0.f,32767.f,1.f}};
	char adi[] = "DATID 0";			

	CFIDIO* pGAZF= NULL;
	pGAZF=new CFIDIO(m_szBufPath+".ZACCGPT");
	if(!pGAZF->Open())
	{
		delete pGAZF;
		return FALSE;
	}

	CFIDIO* pTime= NULL;
	pTime=new CFIDIO(m_szBufPath+".TIMEGPT");
	if(!pTime->Open())
	{
		delete pGAZF;delete pTime;
		return FALSE;
	}
	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=3;
	m_Content[2].nps=3;
	m_Content[2].npw=3;
	FIDINDEX m_Index;
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	CFIDIO* pFID = NULL;
	CString ss;
	ss=m_szBufPath+".GAZFGPT";
	pFID = new CFIDIO(ss,&m_Index);
	if(pFID==NULL)
	{
		delete pGAZF;delete pTime;
		return FALSE;
	}

	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle(" 备份加速度校正曲线...");	
	BOOL bBreak=FALSE;
	float *Value=NULL;
	Value=new float[3];
	unsigned RecNum=(unsigned)((m_EnProDep-m_StProDep)/m_Rlev);
	
	pFID->SeekToDepth(m_StProDep);
	DWORD k=0;
	float dep=m_StProDep;
	while(k<=RecNum)
	{
		Value[0]=dep;
		pGAZF->SeekToDepth(dep);
		pGAZF->Read(2,1,&Value[1]);
		pTime->SeekToDepth(dep);
		pTime->Read(2,1,&Value[2]);

		pFID->Write(3,3,Value);
		pFID->Next();
		k++;
		dep=m_StProDep+m_Rlev*k;
		pGuage->SetDepth(dep);
		if(pGuage->CheckCancelButton()) 
		{
			bBreak=TRUE;
			break;
		}
	}
	delete pFID;
	delete pGAZF;delete pTime;
	delete Value;
	delete pGuage;
	if(bBreak) return FALSE;
	else           return TRUE;
}

//备份增益恢复曲线
BOOL CBackupData::STARGainCopy(float NewRlev/*=0.00254f*/)
{
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"S","m",REPR_FLOAT,4,8,0,0.f,32767.f,1.f},
							   {"GAINGPT","mv",REPR_FLOAT,4,8,8,0.f,32767.f,1.f}};
	char adi[] = "DATID 0";			
	CFIDIO* PadI=new CFIDIO(m_szWellName+"."+m_szGrdVol);
	if(!PadI->Open())
	{
		delete PadI;
		return FALSE;
	}
	float Rlev=PadI->GetDepthLevel();

	CFIDIO* PadGan=new CFIDIO(m_szWellName+"."+m_szPadGan);
	if(!PadGan->Open())
	{
		delete PadI;delete PadGan;
		return FALSE;
	}
	CFIDIO* GrdGan=new CFIDIO(m_szWellName+"."+m_szGrdGan);
	if(!GrdGan->Open())
	{
		delete PadI;delete PadGan;delete GrdGan;
		return FALSE;
	}

	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = NewRlev;
	m_Content[1].nps=m_PadNum+2;
	m_Content[2].nps=m_PadNum+2;
	m_Content[2].npw=m_PadNum+2;
	FIDINDEX m_Index;
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CString szStr;
	szStr=m_szBufPath+".GAINGPT";
	CFIDIO* PadO = new CFIDIO(szStr,&m_Index);
	if(!PadO->Open())
	{
		delete PadI;delete PadO;
		delete PadGan;delete GrdGan;
		return FALSE;
	}

	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle(" 备份增益恢复曲线...");	
	BOOL bBreak=FALSE;
	
	float *Value=new float[m_PadNum+10];
	short *iVal=new short[m_PadNum+1];
	int RecNum=(unsigned)((m_EnProDep-m_StProDep)/Rlev);
	
	int i,k=0,k0=0;
	PadI->SeekToDepth(m_StProDep);
	PadGan->SeekToDepth(m_StProDep);
	GrdGan->SeekToDepth(m_StProDep);
	while(k0<=RecNum)
	{
		PadI->Read(3,m_PadNum,iVal);
		PadI->Next();
		for(i=0;i<m_PadNum;i++)
			Value[i]=(float)iVal[i];

		PadGan->Read(2,1,&Value[m_PadNum]);
		PadGan->Next();

		GrdGan->Read(2,1,&Value[m_PadNum+1]);
		GrdGan->Next();
		while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
		{
			PadO->Write(3,m_PadNum+2,Value);
			PadO->Next();
			k++;
		}
		
		k0++;
		pGuage->SetDepth(m_StProDep+Rlev*k0);
		if(pGuage->CheckCancelButton()) 
		{
			bBreak=TRUE;
			break;
		}
	}
	delete PadI;delete PadO;
	delete PadGan;delete GrdGan;
	delete Value;delete iVal;
	delete pGuage;

	return !bBreak;
}

BOOL CBackupData::CreatRADS(CString Name,float newRlev)
{
	int PadNum=m_PadNum;
	if(m_szSeries.Find("FMI")>-1)
		PadNum=4;
	CLogProcess *pLogIn = new CLogProcess(m_szWellName,NULL);
	INCURVE *PInCurve = new INCURVE[m_CalNum];
	int i;
	for(i=0;i<m_CalNum;i++)
		strcpy(PInCurve[i].name,m_szCalNames[i]);
	float *XX = new float[PadNum];//为了中间使用，对FMI数组定义增大
	pLogIn->InitIn(m_CalNum,PInCurve,(char *)XX);
	pLogIn->InitBuffer();
	float Rlev=pLogIn->DepLevel;
	if(Rlev < 0.0001f)
	{
		AfxMessageBox("采样步长小于 0.0001",MB_OK|MB_ICONERROR);
		delete []XX;
		delete PInCurve;
		delete pLogIn;
		return FALSE;
	}
	
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		               {"S","m",REPR_FLOAT,4,6,0,0.06096f,0.36576f,0.00254f},
						   {"RADSGPT","in",REPR_FLOAT,4,6,6,0.f,255.f,2.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = newRlev;
	m_Content[1].nps=PadNum;
	m_Content[1].rlev = newRlev;
	m_Content[2].nps=PadNum;
	m_Content[2].npw=PadNum; 
	strcpy(m_Content[2].name,Name);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	BOOL bBreak=FALSE;
	CString szStr;
	szStr=m_szBufPath+"."+Name;
	CFIDIO *pFIDW = new CFIDIO(szStr,&m_Index);	
	if(pFIDW->Open())
	{		
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);	
		
		szStr.Format("合成%s阵列数据...",Name);
		pGuage->SetTitle(szStr);
		
		float *OutXX =new float[PadNum];
		float *OutXX1=new float[PadNum];//用于纵向滤波处理
		float *OutXX2=new float[PadNum];
		float *pVal	 =new float[PadNum];//用于插值处理
		int m,k=0,k0=0;
		pFIDW->SeekToTop();
		float dep=m_StProDep;
		float dep0=m_StProDep;
		if(m_szSeries.Find("FMI")>-1)
		{
			pLogIn->Dep=dep;
			pLogIn->DatIn();
			for(i=0;i<m_CalNum;i++)
			{
				OutXX1[i]=XX[i]/2.f;
				OutXX1[i+m_CalNum]=XX[i]/2.f;
			}
			pLogIn->Dep=dep+Rlev;
			pLogIn->DatIn();
			for(i=0;i<m_CalNum;i++)
			{
				OutXX2[i]=XX[i]/2.f;
				OutXX2[i+m_CalNum]=XX[i]/2.f;
			}
			for(i=0;i<PadNum;i++)
				pVal[i]=OutXX1[i];
			while(dep<=m_EnProDep)
			{
				pLogIn->Dep=dep;
				pLogIn->DatIn();
				for(i=0;i<m_CalNum;i++)
				{
					OutXX[i]=XX[i]/2.f;
					OutXX[i+m_CalNum]=XX[i]/2.f;
				}	
				//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
				for(i=0;i<PadNum;i++)
					OutXX[i]=0.212f*OutXX1[i]+0.576f*OutXX2[i]+0.212f*OutXX[i];
				k0++;
				dep=m_StProDep+Rlev*k0;
				pGuage->SetDepth(dep);
				if(pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
				//深度重采样
				m=0;
				while(m_StProDep+k*newRlev<m_StProDep+k0*Rlev)
				{
					for(i=0;i<PadNum;i++)
						XX[i]=pVal[i]+(pVal[i]-OutXX[i])*m*newRlev/(dep-dep0);
					pFIDW->Write(3,PadNum,XX);
					pFIDW->Next();
					k++;m++;
				}
				for(i=0;i<PadNum;i++)
					pVal[i]=OutXX[i];
				dep0=dep;

				for(i=0;i<PadNum;i++)
				{
					OutXX1[i]=OutXX2[i];
					OutXX2[i]=OutXX[i];
				}
			}
		}
		else
		{
			pLogIn->Dep=dep;
			pLogIn->DatIn();
			for(i=0;i<PadNum;i++)
			{
				OutXX1[i]=XX[i];
				pVal[i]=OutXX1[i];
			}
			pLogIn->Dep=dep+Rlev;
			pLogIn->DatIn();
			for(i=0;i<PadNum;i++)
				OutXX2[i]=XX[i];
			while(dep<=m_EnProDep)
			{
				pLogIn->Dep=dep;
				pLogIn->DatIn();
				//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
				for(i=0;i<PadNum;i++)
					OutXX[i]=0.212f*OutXX1[i]+0.576f*OutXX2[i]+0.212f*XX[i];
				k0++;
				dep=m_StProDep+Rlev*k0;
				pGuage->SetDepth(dep);
				if(pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
				//深度重采样
				m=0;
				while(m_StProDep+k*newRlev<m_StProDep+k0*Rlev)
				{
					for(i=0;i<PadNum;i++)
						XX[i]=pVal[i]+(pVal[i]-OutXX[i])*m*newRlev/(dep-dep0);
					pFIDW->Write(3,PadNum,XX);
					pFIDW->Next();
					k++;m++;
				}
				for(i=0;i<PadNum;i++)
					pVal[i]=OutXX[i];
				dep0=dep;
				for(i=0;i<PadNum;i++)
				{
					OutXX1[i]=OutXX2[i];
					OutXX2[i]=OutXX[i];
				}
			}
		}
		pFIDW->Close();
		delete pGuage;
		delete []OutXX;delete []OutXX1;delete []OutXX2;delete []pVal;
	}
	else bBreak=TRUE;
	delete pFIDW;
	delete []XX;delete PInCurve;delete pLogIn;
	return !bBreak;
}

//对数据进行纵向滤波处理
BOOL CBackupData::DataVerFilterPro(CString Name,int nFilter)
{
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		float* fwf=new float[m_PadNum];	
		float* fwf1=new float[m_PadNum];	
		float* fwf2=new float[m_PadNum];
		int Block=m_PadNum*4;
		int RecNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);	
		pGuage->SetTitle(Name+" 数据纵向滤波处理...");
		BOOL bBreak=FALSE;
		int i,j,k;
		for(j=0;j<nFilter;j++)
		{
			fp.Seek (0L,CFile::begin);
			fp.Read (fwf,Block);
			fp.Seek (Block,CFile::begin);
			fp.Read (fwf1,Block);
			k=2;
			while(k<RecNum)
			{
				fp.Seek (k*Block,CFile::begin);
				fp.Read (fwf2,Block);
				//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
				for(i=0;i<m_PadNum;i++)
					fwf[i]=0.212f*fwf[i]+0.576f*fwf1[i]+0.212f*fwf2[i];
				fp.Seek (k*Block-Block,CFile::begin);
				fp.Write (fwf,Block);
				k++;
				for(i=0;i<m_PadNum;i++)
				{
					fwf[i]=fwf1[i];
					fwf1[i]=fwf2[i];
				}
				pGuage->SetDepth(m_StProDep+k*m_Rlev);
				if(pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		if(fwf!=NULL) delete fwf;
		if(fwf1!=NULL) delete fwf1;
		if(fwf2!=NULL) delete fwf2;
		delete pGuage;fp.Close ();
		return !bBreak;
	}
	return FALSE;
}

BOOL CBackupData::BufCurveToWell(CString InName,CString OutName)
{
	BOOL bOK=FALSE;
	CFIDIO* pFIDR = new CFIDIO(m_szBufPath+"."+InName);
	if(pFIDR->Open())
	{
		if(pFIDR->Copy(m_szWellName+"."+OutName)!=-1)
			bOK=TRUE;
	}
	delete pFIDR;
	return bOK;
}

BOOL CBackupData::DipDataCopy(float NewRlev/*=0.00254f*/)
{
	int Num=m_PadNum;
	if(m_szSeries.Find("FMI")>-1)
		Num=4;

	FIDCONTENT ContentW[] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
							 {"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f}};
	ContentW[0].min = m_StProDep;
	ContentW[0].max = m_EnProDep;
	ContentW[0].rlev= NewRlev;
	FIDCONTENT* Content=NULL;
	CFIDIO* pFIDR=NULL;
	CFIDIO* pFIDW = NULL;

	FIDINDEX IndexW;
	char adi[] = "==END==";
	IndexW.adi = adi;

	int i,k0,k,m;
	float Rlev,dep,Val,Val0,df;
	
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);

	CString szIn,szOut;
	BOOL bBreak=FALSE;
	for(i=0;i<Num;i++)
	{
		szIn.Format("%s.%s",m_szWellName,m_szDIPNames[i]);
		pFIDR = new CFIDIO(szIn);
		if(!pFIDR->Open())
		{
			delete pFIDR;
			continue;
		}
		Rlev=pFIDR->GetDepthLevel();
		Content = pFIDR->GetContent(2);

		strcpy(ContentW[1].name,szOut);
		strcpy(ContentW[1].unit,Content->unit);
		ContentW[1].type=Content->type;
		ContentW[1].length=Content->length;
		ContentW[1].nps=Content->nps;
		ContentW[1].npw=Content->npw;
		ContentW[1].min=Content->min;
		ContentW[1].max=Content->max;
		ContentW[1].rlev=NewRlev;

		IndexW.dimension   = 2;
		IndexW.Contentlist = ContentW;

		szOut.Format("DIP%dGPT",i);
		pFIDW= new CFIDIO(m_szBufPath+"."+szOut,&IndexW);
		if(!pFIDW->Open())
		{
			delete pFIDR;delete pFIDW;
			continue;
		}
		
		bBreak=FALSE;
		pGuage->SetTitle(szOut+" 倾角曲线备份...");
		pFIDR->SeekToDepth(m_StProDep);
		pFIDR->Read(2,1,&Val0);
		pFIDR->Next();

		k=0;k0=1;
		pFIDW->SeekToDepth(m_StProDep);
		dep=m_StProDep+Rlev;
		while(dep<m_EnProDep)
		{
			pFIDR->Read(2,1,&Val);
			pFIDR->Next();
			
			df=(Val-Val0)*NewRlev/Rlev;
			//常规曲线进行插值
			m=0;
			while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
			{
				Val=Val0+df*m;
				pFIDW->Write(2,1,&Val);
				pFIDW->Next();
				k++;m++;
			}
			Val0=Val;
			
			k0++;
			dep=m_StProDep+Rlev*k0;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton()) 
			{
				bBreak=TRUE;
				break;
			}
		}
		delete pFIDR;delete pFIDW;
		if(bBreak) break;
	}
	delete pGuage;
	return !bBreak;
}
BOOL CBackupData::SubDataCopy(float NewRlev/*=0.00254f*/)
{
	CString outName[6]={"TIMEGPT","ZACCGPT","DEVGPT",
						"P1AZGPT","RBGPT","DAZGPT"};
	CString inName[6];
	inName[0]=m_szTimeName;
	inName[1]=m_szGAZFName;
	inName[2]=m_szDevName;
	inName[3]=m_szAZ1Name;
	inName[4]=m_szRBName;
	inName[5]=m_szDazName;

	int Num=6;
	FIDCONTENT ContentW[] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
							 {"P1AZ","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f}};
	ContentW[0].min = m_StProDep;
	ContentW[0].max = m_EnProDep;
	ContentW[0].rlev= NewRlev;
	FIDCONTENT* Content=NULL;
	CFIDIO* pFIDR=NULL;
	CFIDIO* pFIDW = NULL;

	FIDINDEX IndexW;
	char adi[] = "==END==";
	IndexW.adi = adi;

	int i,k0,k,m;
	float Rlev,dep,Val,Val0,tmp;
	double df;
	
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);

	CString szIn,szOut;
	BOOL bBreak=FALSE;
	for(i=0;i<Num;i++)
	{
		szIn.Format("%s.%s",m_szWellName,inName[i]);
		pFIDR = new CFIDIO(szIn);
		if(!pFIDR->Open())
		{
			delete pFIDR;
			continue;
		}
		Rlev=pFIDR->GetDepthLevel();
		Content = pFIDR->GetContent(2);

		szOut=outName[i];
		strcpy(ContentW[1].name,szOut);
		strcpy(ContentW[1].unit,Content->unit);
		ContentW[1].type=Content->type;
		ContentW[1].length=Content->length;
		ContentW[1].nps=Content->nps;
		ContentW[1].npw=Content->npw;
		ContentW[1].min=Content->min;
		ContentW[1].max=Content->max;
		ContentW[1].rlev=NewRlev;

		IndexW.dimension   = 2;
		IndexW.Contentlist = ContentW;
		pFIDW= new CFIDIO(m_szBufPath+"."+szOut,&IndexW);
		if(!pFIDW->Open())
		{
			delete pFIDR;delete pFIDW;
			continue;
		}
		
		bBreak=FALSE;
		pGuage->SetTitle(szOut+" 辅助曲线备份...");
		pFIDR->SeekToDepth(m_StProDep);
		pFIDR->Read(2,1,&Val0);
		pFIDR->Next();
		k=0;k0=1;

		dep=m_StProDep;
		while(dep<=m_EnProDep)
		{
			pFIDR->Read(2,1,&Val);
			pFIDR->Next();
			
			//最后3个为方位曲线
			if(Val0==-999.25f||Val0==-9999.f)
				df=0.;
			else if(i>2&&fabs(Val-Val0)>180.)
				df=0.;
			else
				df=(Val-Val0)*NewRlev/Rlev;

			//常规曲线进行插值
			m=0;
			while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
			{
				tmp=(float)(Val0+df*m);
				pFIDW->Write(2,1,&tmp);
				pFIDW->Next();
				k++;m++;
			}
			Val0=Val;
			
			k0++;
			dep=m_StProDep+Rlev*k0;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton()) 
			{
				bBreak=TRUE;
				break;
			}
		}
		delete pFIDR;delete pFIDW;
		if(bBreak) break;
	}
	if(!bBreak)
		BufCurveToWell("P1AZGPT","AZ1-PRO");
	delete pGuage;
	return !bBreak;
}
BOOL CBackupData::CurveReSample(CString InCurveName,CString OutCurveName,float NewRlev,BOOL bAz)
{
	CString FullInName=InCurveName;
	if(InCurveName.Find(":\\")<1)
		FullInName=m_szWellName+"."+InCurveName;
	CFIDIO* pFIDR = new CFIDIO(FullInName);
	if(!pFIDR->Open())
	{
		delete pFIDR;
		return FALSE;
	}
	float Rlev=pFIDR->GetDepthLevel();
	int dim=pFIDR->GetDimenSion();
	
	FIDCONTENT ContentW[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
								{"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f},
								{"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f}};
	ContentW[0].min = m_StProDep;
	ContentW[0].max = m_EnProDep;
	ContentW[0].rlev =NewRlev;
	FIDCONTENT* Content=NULL;
	int i;
	for(i=1;i<dim;i++)
	{
		Content = pFIDR->GetContent(i+1);
		strcpy(ContentW[i].name,OutCurveName);
		strcpy(ContentW[i].unit,Content->unit);
		ContentW[i].type=Content->type;
		ContentW[i].length=Content->length;
		ContentW[i].nps=Content->nps;
		ContentW[i].npw=Content->npw;
		ContentW[i].min=Content->min;
		ContentW[i].max=Content->max;
		ContentW[i].rlev=NewRlev;
	}
	int nps,type;
	short* ibuf = NULL;
	float* fbuf = NULL;
	char* cbuf = NULL;
	if(Content != NULL)
	{
		nps = Content->nps;
		type=Content->type;
		if (type == REPR_FLOAT) 
			fbuf = new float[nps];
		else if (type == REPR_CHAR) 
			cbuf = new char[nps];
		else ibuf = new short[nps];
	}
	FIDINDEX IndexW;
	IndexW.dimension = dim;
	ContentW[0].rlev =NewRlev ;
	IndexW.Contentlist = ContentW;
	char adi[] = "==END==";
  	IndexW.adi = adi;

	CFIDIO* pFIDW = NULL;
	pFIDW= new CFIDIO(m_szBufPath+"."+OutCurveName,&IndexW);
	pFIDW->Open();
	unsigned k0,k,m,n;
	k0=k=0;

	BOOL bBreak=FALSE;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	
	pGuage->SetTitle(OutCurveName+" 曲线数据备份...");
	float dep=m_StProDep;

	//常规曲线进行插值
	pFIDR->SeekToDepth(dep+Rlev);
	pFIDW->SeekToDepth(dep);
	if((dim==2)&&(nps==1)&&(type == REPR_FLOAT))
	{
		double fdata0,fdata,fval;
		pFIDR->Read(2,1,&fdata0);
		pFIDR->Next();
		k0++;
		double df;
		while(dep<=m_EnProDep)
		{
			pFIDR->Read(2,1,&fdata);
			pFIDR->Next();
			k0++;
			if(bAz&&fabs(fdata-fdata0)>180.)
				df=0.;
			else
				df=(fdata-fdata0)*NewRlev/Rlev;
			n=0;
			while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
			{
				fval=fdata0+df*n;
				pFIDW->Write(2,1,&fval);
				pFIDW->Next();
				k++;n++;
			}
			fdata0=fdata;
			dep=m_StProDep+Rlev*k0;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton()) 
			{
				bBreak=TRUE;
				break;
			}
		}
	}
	else
	{
		float *Val=NULL;
		Val=new float[nps];
		float *Val0=NULL;
		Val0=new float[nps];
		float dep0=dep;
		int j;
		if (type == REPR_FLOAT)
		{
			pFIDR->Read(dim,nps,fbuf);
			for(j=0;j<nps;j++)
				Val0[j]=fbuf[j];
		}
		else if (type == REPR_CHAR)
		{
			pFIDR->Read(dim,nps,cbuf);
			for(j=0;j<nps;j++)
				Val0[j]=(float)cbuf[j];
		}
		else 
		{
			pFIDR->Read(dim,nps,ibuf);
			for(j=0;j<nps;j++)
				Val0[j]=(float)ibuf[j];
		}
		while(dep<=m_EnProDep)
		{
			if (type == REPR_FLOAT)
			{
				pFIDR->Read(dim,nps,fbuf);
				for(j=0;j<nps;j++)
					Val[j]=fbuf[j];
			}
			else if (type == REPR_CHAR)
			{
				pFIDR->Read(dim,nps,cbuf);
				for(j=0;j<nps;j++)
					Val[j]=(float)cbuf[j];
			}
			else 
			{
				pFIDR->Read(dim,nps,ibuf);
				for(j=0;j<nps;j++)
					Val[j]=(float)ibuf[j];
			}
			pFIDR->Next();	
			k0++;
			dep=m_StProDep+Rlev*k0;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bBreak=TRUE;
				break;
			}
			m=0;
			while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
			{
				if (type == REPR_FLOAT)
				{
					for(j=0;j<nps;j++)
						fbuf[j]=Val0[j]+(Val[j]-Val0[j])*m*NewRlev/(dep-dep0);
					pFIDW->Write(dim,nps,fbuf);
				}
				else if (type == REPR_CHAR)
				{
					for(j=0;j<nps;j++)
						cbuf[j]=(char)(Val0[j]+(Val[j]-Val0[j])*m*NewRlev/(dep-dep0));
					pFIDW->Write(dim,nps,cbuf);
				}
				else 
				{
					for(j=0;j<nps;j++)
						ibuf[j]=(short)(Val0[j]+(Val[j]-Val0[j])*m*NewRlev/(dep-dep0));
					pFIDW->Write(dim,nps,ibuf);
				}
				pFIDW->Next();
				k++;m++;
			}
			for(j=0;j<nps;j++)
				Val0[j]=Val[j];
			dep0=dep;
		}
		delete Val;delete Val0;
	}
	delete pFIDR;delete pFIDW;
	if (cbuf != NULL)	delete cbuf;
	if (ibuf != NULL)	delete ibuf;
	if (fbuf != NULL)	delete fbuf;
	delete pGuage;
	return !bBreak;
}

//进行方位校正
//同时处理I号极板中间偏移
BOOL CBackupData::AzimuthCorret(CString Name)
{
	CFIDIO* pFIDAZI= new CFIDIO(m_szBufPath+".P1AZGPT");
	if(!pFIDAZI->Open()) 
	{
		delete pFIDAZI; 
		return FALSE;
	}
	BOOL bBreak=FALSE;

	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,Name);
	int Num =m_ButtonNum;
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		int Block=Num*4;	
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		CString ss=m_szSeries+" 方位校正...";
		pGuage->SetTitle(ss);
		pGuage->ShowWindow(SW_SHOW);

		float Azi;int x,dk=0;
		int iPro=m_ButtonNum/2;
		float *OffValue=new float[iPro+1];
		float *Value=new float[2*Num+10];
		
		pFIDAZI->SeekToDepth(m_StProDep);
		int k=0;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);	
			pFIDAZI->Read(2,1,&Azi);
			pFIDAZI->Next();
			if((Azi>0.f)&&(Azi<360.f))
			{
				dk=(int)(Num*1.f*Azi/360.f);
				for(x=Num-1;x>=0;x--)
					Value[x+dk]=Value[x];
				for(x=0;x<dk;x++)
					Value[x]=Value[Num+x];
				//处理I号极板中间偏移
				for(x=0;x<iPro;x++)
					OffValue[x]=Value[x];
				for(x=iPro;x<Num;x++)
					Value[x-iPro]=Value[x];
				for(x=0;x<iPro;x++)
					Value[Num-iPro+x]=OffValue[x];
			}
			fp.Seek (k*Block,CFile::begin);
			fp.Write(Value,Block);	
			k++;
		
			pGuage->SetDepth(m_StProDep+m_Rlev*k);
			if(pGuage->CheckCancelButton()) 
			{
				bBreak=TRUE;
				break;
			}
		}
		delete Value;delete OffValue;
		delete pGuage;
	}
	else bBreak=TRUE;
	fp.Close ();
	delete pFIDAZI;			
	return !bBreak;
}