// ImageGeoCorrect.cpp: implementation of the CImageGeoCorrect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "ImageGeoCorrect.h"
#include "FlowNode.h"
#include "Gradient.h"
#include "FIDio.h"
#include "Guage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CImageGeoCorrect,CFlowNode, 0)
CImageGeoCorrect::CImageGeoCorrect()
{
	m_pDoc=NULL;
}

CImageGeoCorrect::~CImageGeoCorrect()
{

}

CImageGeoCorrect::CImageGeoCorrect(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
}

void CImageGeoCorrect::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
		ar<<m_szImageType;
	else
		ar>>m_szImageType;
}

BOOL CImageGeoCorrect::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(m_szImageType=="动态图像")
	{
		m_szInName="DATADY";
		m_szOutName="DYNMI";
	}
	else if(m_szImageType=="静态图像")
	{
		m_szInName="DATAST";
		m_szOutName="STAMI";
	}
	else if(m_szImageType=="刻度图像")
	{
		m_szInName="DATARS";
		m_szOutName="STRMI";
	}
	m_bResult=doRun(m_szInName,m_szOutName);
	if(m_bResult)
		OnWellRefresh();
	return m_bResult;
}

void CImageGeoCorrect::InitPara(CFlowChartDoc* pDoc)
{
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		CFlowNode *pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{		
			if(pObj->IsKindOf(RUNTIME_CLASS(CImageCreat)))
			{
				m_szBufPath=((CImageCreat*)pObj)->m_szBufPath;
				m_szWellName=((CImageCreat*)pObj)->m_szWellName;
				m_szSeries=((CImageCreat*)pObj)->m_szSeries;
				m_PadNum=((CImageCreat*)pObj)->m_PadNum; 
				m_ButtonNum=((CImageCreat*)pObj)->m_ButtonNum; 
				m_ButtonMid=((CImageCreat*)pObj)->m_ButtonMid;
				m_DataNum=((CImageCreat*)pObj)->m_DataNum;
				m_CalNum=((CImageCreat*)pObj)->m_CalNum;
				m_StProDep=((CImageCreat*)pObj)->m_StProDep;
				m_EnProDep=((CImageCreat*)pObj)->m_EnProDep;
				m_Rlev=((CImageCreat*)pObj)->m_Rlev;
				m_WinLongth=((CImageCreat*)pObj)->m_WinLongth;          
				m_Bits=((CImageCreat*)pObj)->m_Bits; 
			}
		}
	}
	m_pDoc=pDoc;
}

//作井眼几何校正
//按钻头尺寸计算井眼周长
//根据极板数和每一极板电扣数及仪器横向分辩率计算盲区折算的电扣数
BOOL CImageGeoCorrect::GeometryCorrect(CString InName,CString OutName)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	} 
	
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	CString ss=m_szImageType+"几何校正...";
	pGuage->SetTitle(ss);
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"AZIM","DEG",REPR_FLOAT,4,6,0,0.f,360.f,0.f},
							   {"STAMI","1/ohm",REPR_FLOAT,4,144,144,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	//横向分辩率0.1英寸=2.54mm
	float Hor=2.54f;
//	if(m_szSeries.Find ("STARV")>-1)
//		Hor=3.556f;
	int Space=(int)(m_Bits*3.1416f/Hor);
	int	AllSpace=1;
	int Num,WriteNum=0;
	int Button=m_ButtonNum;//副极板电扣数
	int Off=2;//主副极板间预留盲区宽度
	if((Space<m_DataNum)&&m_szSeries.Find("FMI")>-1)
	{
		//主副极板间预留盲区为6
		Off=0;
		//副极板少写6个电扣的数据，余下的算盲区
		Button=m_ButtonNum-6;
		WriteNum=m_ButtonNum*4+Button*4+Off*4;
		Space=(Space-WriteNum)/4;
		Num=WriteNum+Space*4;
	}
	else
	{
		AllSpace=Space-m_DataNum;
		Space=(Space-m_DataNum)/m_PadNum;
		Num=m_DataNum+m_PadNum*Space;
	}

	int m_ImgNum=Num;
	m_Content[1].nps=Num;                              //图像点数 
	m_Content[1].min=Hor;                              //横向分辩率 mm
	m_Content[1].max=m_Bits*3.1416f;                   //按钻头直径计算井身周长 mm
	m_Content[1].rlev = m_Bits;                        //钻头直径 mm

	m_Content[2].nps=Num;
	m_Content[2].npw=Num;                              //总电扣数
	m_Content[2].min=(float)m_PadNum;                  //极板数
	m_Content[2].max=(float)m_ButtonNum;               //每个极板电扣数
	m_Content[2].rlev =(float)Space;                   //电扣间距(折算为电扣数)
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	
	ss=m_szWellName+"."+OutName;
	CFIDIO* pFID= NULL;
	pFID = new CFIDIO(ss);
	if(pFID->Open())
		pFID->Delete();
	delete pFID;

	CFIDIO* pFIDW = NULL;
	float *InValue,*OutValue;	
	int i,j,k,PerSpace,OffPoint;
	pFIDW = new CFIDIO(ss,&m_Index);
	if(pFIDW->Open())
	{				
		i=0;
		float SumCal,*Cal;
		CFIDIO* pCal = new CFIDIO(m_szBufPath+".RADSGPT");
		if(pCal->Open())
		{
			InValue=new float[m_DataNum+10];
			OutValue=new float[Num+10];
			Cal=new float[m_PadNum+1];
			pCal->SeekToDepth(m_StProDep);
			pFIDI->SeekToDepth(m_StProDep);
			pFIDW->SeekToDepth(m_StProDep);
			float FMICal[8];
			while(i<RecordNum)
			{
				for(j=0;j<=Num;j++)
					OutValue[j]=-9999.f;
				SumCal=0;
				if(m_szSeries.Find("FMI_8PAD")>-1)
				{
					pCal->Read(3,4,FMICal);
					for(j=0;j<4;j++) 
					{
						Cal[j*2]=FMICal[j];
						Cal[j*2+1]=1.f;
						SumCal+=Cal[j*2];
						SumCal+=1.f;
					}
				}
				else
				{
					pCal->Read(3,m_PadNum,Cal);
					for(j=0;j<m_PadNum;j++) 
						SumCal+=Cal[j];
				}
				pFIDI->Read(3,m_DataNum,InValue);
				for(j=0;j<m_ButtonNum;j++) 
					OutValue[j]=InValue[j];
				if(WriteNum>1)
				{
					OffPoint=m_ButtonNum+Space;
					for(k=1;k<m_PadNum;k++)
					{
						if(k%2==1)
						{
							for(j=0;j<Button;j++) 
								OutValue[OffPoint+j]=InValue[m_ButtonNum*k+j];
							//主副极板间留Off个电扣宽度的盲区
							OffPoint+=Button+Off;	
						}
						else
						{
							for(j=0;j<m_ButtonNum;j++) 
								OutValue[OffPoint+j]=InValue[m_ButtonNum*k+j];
							//主副极板间留Off个电扣宽度的盲区
							OffPoint+=m_ButtonNum+Space;
						}
					}
				}
				else if(SumCal>4.f&&Space>1)
				{	
					OffPoint=0;
					for(k=1;k<m_PadNum;k++)
					{
						PerSpace=(int)(Cal[k]/SumCal*AllSpace)+m_ButtonNum;
						OffPoint+=PerSpace;
						for(j=0;j<m_ButtonNum;j++) 
							OutValue[OffPoint+j]=InValue[m_ButtonNum*k+j];
					}
				}
				else
				{
					PerSpace=Space+m_ButtonNum;
					for(k=1;k<m_PadNum;k++)
					{	
						for(j=0;j<m_ButtonNum;j++) 
							OutValue[PerSpace*k+j]=InValue[m_ButtonNum*k+j];
					}
				}	
				pFIDW->Write(3,Num,OutValue);
				pFIDW->Next();pCal->Next();pFIDI->Next();
				i++;
				dep=m_StProDep+m_Rlev*i;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton()) 
				{
					bUserBreak=TRUE;
					break;
				}
			}	
			delete InValue;delete OutValue;delete Cal;
		}
		else bUserBreak=TRUE;
		delete pCal;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

//同时处理I号极板中间偏移
BOOL CImageGeoCorrect::AzimuthCorret(CString Path,CString Name)
{
	CFIDIO* pFIDAZI=NULL;
	pFIDAZI = new CFIDIO(m_szBufPath+".P1AZGPT");
	if(!pFIDAZI->Open()) 
		{delete pFIDAZI; return FALSE;}
	pFIDAZI->SeekToDepth(m_StProDep);	
	BOOL bUserBreak=FALSE;

	char s[256],fn[256];
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO *pFIDIO = new CFIDIO(s);
	if (!pFIDIO->Open()) 
		{delete pFIDIO;return FALSE;}		 
	FIDCONTENT* Content = pFIDIO->GetContent(3);
	int Num = Content->nps;
	delete pFIDIO;

	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		int Block=Num*4;	
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		CString ss=Name+" 曲线方位校正...";
		pGuage->SetTitle(ss);
		pGuage->ShowWindow(SW_SHOW);
		float dep=m_StProDep;

		float Azi;int x,dk=0;
		int iPro=m_ButtonNum/2;
		float *OffValue;//处理I号极板中间偏移
		OffValue=new float[iPro+1];
		float *Value=new float[2*Num+10];
		int k=0;
		pFIDAZI->SeekToDepth(m_StProDep);
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);	
			pFIDAZI->SeekToDepth(dep);
			pFIDAZI->Read(2,1,&Azi);
			if((Azi>=0.f)&&(Azi<=360.f))
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
			fp.Write (Value,Block);	
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete Value;delete OffValue;
		delete pGuage;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	delete pFIDAZI;			
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CImageGeoCorrect::doRun(CString InName,CString OutName)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	} 
	
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	CString szStr=m_szImageType+OutName+" 几何校正...";
	pGuage->SetTitle(szStr);
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,6,0,0.f,0.f,0.f},
							   {"STAMI","1/ohm",REPR_FLOAT,4,144,144,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	//横向分辩率0.1英寸=2.54mm
	
	float Hor=2.54f;
	if(m_szSeries.Find ("STARV")>-1)
		Hor=3.556f;
	int Space=(int)(m_Bits*3.1416f/Hor);
	int	AllSpace=1;
	int Num,WriteNum=0;
	int Button=m_ButtonNum;//副极板电扣数
	int Off=2;//主副极板间预留盲区宽度
	if((Space<m_DataNum)&&m_szSeries.Find("FMI")>-1)
	{
		//主副极板间预留盲区为6
		Off=0;
		//副极板少写6个电扣的数据，余下的算盲区
		Button=m_ButtonNum-6;
		WriteNum=m_ButtonNum*4+Button*4+Off*4;
		Space=(Space-WriteNum)/4;
		Num=WriteNum+Space*4;
	}
	else
	{
		AllSpace=Space-m_DataNum;
		Space=(Space-m_DataNum)/m_PadNum;
		Num=m_DataNum+m_PadNum*Space;
	}

	int m_ImgNum=Num;
	m_Content[1].nps=Num;                              //图像点数 
	m_Content[1].min=Hor;                              //横向分辩率 mm
	m_Content[1].max=m_Bits*3.1416f;                   //按钻头直径计算井身周长 mm
	m_Content[1].rlev = m_Bits;                        //钻头直径 mm

	m_Content[2].nps=Num;
	m_Content[2].npw=Num;                              //总电扣数
	m_Content[2].min=(float)m_PadNum;                  //极板数
	m_Content[2].max=(float)m_ButtonNum;               //每个极板电扣数
	m_Content[2].rlev =(float)Space;                   //电扣间距(折算为电扣数)
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	
	szStr=m_szWellName+"."+OutName;
	CFIDIO* pFID= new CFIDIO(szStr);
	if(pFID->Open())
		pFID->Delete();
	delete pFID;

	CFIDIO* pFIDW = NULL;
	float *InValue,*OutValue;	
	int i,j,k,PerSpace,OffPoint;
	pFIDW = new CFIDIO(szStr,&m_Index);
	if(pFIDW->Open())
	{				
		i=0;
		float SumCal,*Cal;
		CFIDIO* pCal = new CFIDIO(m_szBufPath+".RADSGPT");
		if(pCal->Open())
		{
			InValue=new float[m_DataNum+10];
			OutValue=new float[Num+10];
			Cal=new float[m_PadNum+1];
			pCal->SeekToDepth(m_StProDep);
			pFIDI->SeekToDepth(m_StProDep);
			pFIDW->SeekToDepth(m_StProDep);
			float FMICal[8];
			while(i<RecordNum)
			{
				for(j=0;j<=Num;j++)
					OutValue[j]=-9999.f;
				SumCal=0;
				if(m_szSeries.Find("FMI_8PAD")>-1)
				{
					pCal->Read(3,4,FMICal);
					for(j=0;j<4;j++) 
					{
						Cal[j*2]=FMICal[j];
						Cal[j*2+1]=1.f;
						SumCal+=Cal[j*2];
						SumCal+=1.f;
					}
				}
				else
				{
					pCal->Read(3,m_PadNum,Cal);
					for(j=0;j<m_PadNum;j++) 
						SumCal+=Cal[j];
				}
				pFIDI->Read(3,m_DataNum,InValue);
				for(j=0;j<m_ButtonNum;j++) 
					OutValue[j]=InValue[j];
				if(WriteNum>1)
				{
					OffPoint=m_ButtonNum+Space;
					for(k=1;k<m_PadNum;k++)
					{
						if(k%2==1)
						{
							for(j=0;j<Button;j++) 
								OutValue[OffPoint+j]=InValue[m_ButtonNum*k+j];
							//主副极板间留Off个电扣宽度的盲区
							OffPoint+=Button+Off;	
						}
						else
						{
							for(j=0;j<m_ButtonNum;j++) 
								OutValue[OffPoint+j]=InValue[m_ButtonNum*k+j];
							//主副极板间留Off个电扣宽度的盲区
							OffPoint+=m_ButtonNum+Space;
						}
					}
				}
				else if(SumCal>4.f&&Space>1)
				{	
					OffPoint=0;
					for(k=1;k<m_PadNum;k++)
					{
						PerSpace=(int)(Cal[k]/SumCal*AllSpace)+m_ButtonNum;
						OffPoint+=PerSpace;
						for(j=0;j<m_ButtonNum;j++) 
							OutValue[OffPoint+j]=InValue[m_ButtonNum*k+j];
					}
				}
				else
				{
					PerSpace=Space+m_ButtonNum;
					for(k=1;k<m_PadNum;k++)
					{	
						for(j=0;j<m_ButtonNum;j++) 
							OutValue[PerSpace*k+j]=InValue[m_ButtonNum*k+j];
					}
				}	
				pFIDW->Write(3,Num,OutValue);
				pFIDW->Next();pCal->Next();pFIDI->Next();
				i++;
				dep=m_StProDep+m_Rlev*i;
				pGuage->SetDepth(dep);
				if(pGuage->CheckCancelButton()) 
				{
					bUserBreak=TRUE;
					break;
				}
			}	
			delete InValue;delete OutValue;delete Cal;
		}
		else bUserBreak=TRUE;
		delete pCal;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	if(bUserBreak) 
	{
		delete pGuage;
		return FALSE;
	}
	CFIDIO* pFIDAZI=NULL;
	pFIDAZI = new CFIDIO(m_szBufPath+".P1AZGPT");
	if(!pFIDAZI->Open()) 
		{delete pFIDAZI; return FALSE;}
	pFIDAZI->SeekToDepth(m_StProDep);	

	char s[256],fn[256];
	strcpy(s,szStr);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		int Block=Num*4;	
		szStr=m_szImageType+OutName+" 方位校正...";
		pGuage->SetTitle(szStr);
		dep=m_StProDep;

		float Azi;int x,dk=0;
		int iPro=m_ButtonNum/2;
		float *OffValue=new float[iPro+1];//处理I号极板中间偏移
		float *Value=new float[2*Num+10];
		int k=0;
		pFIDAZI->SeekToDepth(m_StProDep);
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);	
			pFIDAZI->SeekToDepth(dep);
			pFIDAZI->Read(2,1,&Azi);
			if((Azi>=0.f)&&(Azi<=360.f))
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
			fp.Write (Value,Block);	
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete Value;delete OffValue;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	delete pFIDAZI;	

	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;
}