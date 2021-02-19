// PORCompute.cpp: implementation of the CPORCompute class.CPORCompute
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "flowchart.h"
#include "PORCompute.h"
#include "FlowNode.h"
#include "FlowChartDoc.h"
#include "ImageCreate.h"
#include "PORComputDlg.h"
#include "FIDio.h"
#include "Guage.h"
#include "Clustering.h"
#include "LogProcess.h"
#include "FitCircle.h"

#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CPORCompute,CFlowNode, 0)
CPORCompute::CPORCompute()
{
	m_pDoc=NULL;
	m_SpcNum=30;
}

CPORCompute::~CPORCompute()
{

}

CPORCompute::CPORCompute(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_bResistance=FALSE;
	m_bSpectFilter=TRUE;
	m_PORMax=30.f;
	m_XM=2.f;
	m_XN=2.f;
	m_XA=1.f;
	m_XB=1.f;
	m_szExtPorWell="";
	m_szExtPorName="";
	m_StatWin=0.254f;     //1英尺
	m_StatRlev=0.0254f;   //1英寸
	m_MinStat=1.f;
	m_SpcNum=30;
}

void CPORCompute::Serialize(CArchive& ar)
{
	m_Ver=1.5f;
	CFlowNode::Serialize(ar);	
	if(ar.IsStoring())
	{
		ar<<m_bSpectFilter;
		ar<<m_StatWin;
		ar<<m_StatRlev;
		ar<<m_XM;
		ar<<m_XN;
		ar<<m_XA;
		ar<<m_XB;
		ar<<m_MinStat;
		ar<<m_PORMax;
		ar<<m_szExtPorWell;
		ar<<m_szExtPorName;	
		ar<<m_bResistance;
		ar<<m_Rm;
	}
	else
	{
		ar>>m_bSpectFilter;
		ar>>m_StatWin;
		ar>>m_StatRlev;
		ar>>m_XM;
		ar>>m_XN;
		ar>>m_XA;
		ar>>m_XB;
		ar>>m_MinStat;
		ar>>m_PORMax;
		ar>>m_szExtPorWell;
		ar>>m_szExtPorName;
		ar>>m_bResistance;
		ar>>m_Rm;
	}
}

BOOL CPORCompute::Curve_Filter(int nFilter)
{
	CString InName[6]={"POR1","POR2","POR3","PORV1","PORV2","PORV3"};
	float f_Sdep,f_Edep,f_Rlev;
	BOOL bRun=TRUE;
	float fdat[10];
	CString Path;
	//定义进度指示器
	CGuage* pGuage = new CGuage(0.f,1000.f);
	pGuage->Create(AfxGetMainWnd ());
	pGuage->ShowWindow(SW_SHOW);
	int i,j,k,m;
	for(j=0;j<6;j++)
	{
		Path=m_szWellName+"."+InName[j];
		CFIDIO* pFIDtemp = NULL;
		pFIDtemp = new CFIDIO(Path);
		if(pFIDtemp->Open())
		{
			pGuage->SetTitle(InName[j]+" 曲线滤波处理...");
			f_Sdep = pFIDtemp->GetStartDepth();
			f_Edep = pFIDtemp->GetEndDepth();
			f_Rlev=pFIDtemp->GetDepthLevel();
			pGuage->SetDepthRange(f_Sdep,f_Edep);
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
					dep=f_Sdep+m*f_Rlev;
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton())
					{
						bRun=FALSE;
						break;
					}
					for(i=0;i<4;i++) 
						fdat[i]=fdat[i+1];
					pFIDtemp->SeekToDepth(dep);
					pFIDtemp->Read(2,1,&data);
					fdat[4]=data;
					m++;
				}
				if(!bRun) break;
			}	
		}
		delete pFIDtemp;
		if(!bRun) break;
	}
	delete pGuage;
	return bRun;
}

BOOL CPORCompute::Run()
{
	m_bResult=TRUE;
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(!m_pDoc->m_bAunAll)
	{
		CPORComputDlg dlg;
		dlg.m_StatWin=m_StatWin;
		dlg.m_StatRlev=m_StatRlev;
		dlg.m_Rm=m_Rm;
		dlg.m_XM=m_XM;
		dlg.m_XA=m_XA;
		dlg.m_XN=m_XN;
		dlg.m_XB=m_XB;
		dlg.m_PORMax=m_PORMax;
		dlg.m_szExtPorWell=m_szExtPorWell;
		dlg.m_szExtPorName=m_szExtPorName;
		dlg.m_MinStat=m_MinStat;
		dlg.m_bSpectFilter=m_bSpectFilter;
		dlg.m_bResistance=m_bResistance;
		if(dlg.DoModal ()==IDOK)
		{
			m_StatWin=dlg.m_StatWin;
			m_StatRlev=dlg.m_StatRlev;
			float StatWin0=m_StatWin;
			float StatRlev0=m_StatRlev;
			m_Rm=dlg.m_Rm;
			m_XM=dlg.m_XM;
			m_XA=dlg.m_XA;
			m_XN=dlg.m_XN;
			m_XB=dlg.m_XB;
			m_PORMax=dlg.m_PORMax;
			m_bResistance=dlg.m_bResistance;
			m_szExtPorWell=dlg.m_szExtPorWell;
			m_szExtPorName=dlg.m_szExtPorName;
			m_MinStat=dlg.m_MinStat;
			m_bSpectFilter=dlg.m_bSpectFilter;
			m_bResult=CreatSpect("RBUT","PORSPC");
			if(m_bResult)
				m_bResult=StaticSectionPor("PORSPC");
			//	m_bResult=StaticKmeansPor("PORSPC");
			//	m_bResult=StaticPor("PORSPC");
			if(m_bResult)
			{
				if(m_bSpectFilter)
					Curve_Filter(1);
				OnWellRefresh();
			}
			m_StatWin=StatWin0;
			m_StatRlev=StatRlev0;
		}
	}
	return TRUE;
}

void CPORCompute::InitPara(CFlowChartDoc* pDoc)
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

BOOL CPORCompute::CreatStacData(CString InName,CString OutName)
{
	if(!DataCopy(InName,OutName)) return FALSE;
	int i,k,RecordNum;
	char s[256],fn[256];
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');	
	BOOL bUserBreak=FALSE;
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int Block=m_DataNum*4;
		
		//定义进度指示器
		float dep=m_StProDep;
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle(OutName+" 消除图像本底...");

		CPoint *points=NULL;
		points=new CPoint[m_DataNum];
		CFitCircle FitCircle(m_DataNum);//最小二乘法拟合圆
		float fX,fY,Radius,fScale;
		int nFitNum;
		
		float *fwf=NULL;
		fwf=new float[m_DataNum];
		k=0;
		fp.Seek (k*Block,CFile::begin);
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (fwf,Block);	
			//用拟合圆的方法求取图像底值，消除图像本底
			nFitNum=0;
			fScale=1.f;
			for(i=0;i<m_DataNum;i++)
			{
				if(fwf[i]>fScale) 
					fScale=fwf[i];
			}
			fScale=30000.f/fScale;
			for(i=0;i<m_DataNum;i++)
			{
				points[nFitNum].x=(int)(sin(6.2831853*i/m_DataNum)*fScale*fwf[i]);
				points[nFitNum].y=(int)(cos(6.2831853*i/m_DataNum)*fScale*fwf[i]);
				FitCircle.m_points[nFitNum].x=points[nFitNum].x;
				FitCircle.m_points[nFitNum].y=points[nFitNum].y;
				nFitNum++;
			}
			if(nFitNum>3)
			{
				FitCircle.m_nNum=nFitNum;
				FitCircle.DoCircleFit();
				fX = (float)FitCircle.m_fCenterX;
				fY = (float)FitCircle.m_fCenterY;
				Radius=(float)FitCircle.m_fRadius/fScale;
				for(i=0;i<m_DataNum;i++)
				{
					fwf[i]=(float)(sqrt((points[i].x-fX)*(points[i].x-fX)+
							(points[i].y-fY)*(points[i].y-fY)))/fScale;
					fwf[i]-=Radius;
				}
			}
			fp.Seek (k*Block,CFile::begin);
			fp.Write (fwf,Block);
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) break;
		}
		delete points;
		delete fwf;
		delete pGuage;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	if(bUserBreak) return FALSE;
	else return TRUE;
}

BOOL CPORCompute::CreatSpect(CString InName,CString OutName)
{
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle(OutName+" 生成孔隙度频谱...");
	BOOL bUserBreak=FALSE;
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

	CString PorName=m_szWellName+"."+OutName;
	CFIDIO* pFID = new CFIDIO(PorName,&Index);
	pFID->Open();
	pFID->Close();
	delete pFID;

	char s[256],fn[256];
	strcpy(s,m_szWellName);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile Infp;
	if(Infp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		strcpy(s,PorName);
		CFIDIO::GetFileName(s,fn,'D');
		CFile Outfp;
		if(Outfp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			int Block=(m_DataNum+1)*4;
			int SpctBlock=m_SpcNum*4;
			float *Value=NULL;
			Value=new float[(m_DataNum+1)*iWin];

			int i,j,k,k1,pos,Num,m;
			float *pFreq=new float[m_SpcNum];
			Num=iWin*m_DataNum/2;

			CString szExtPorName=m_szExtPorWell+"."+m_szExtPorName;
			CString PORstr=szExtPorName.Left(szExtPorName.ReverseFind('\\')+1);
			PORstr+=szExtPorName.Mid(szExtPorName.ReverseFind('\\')+1);		
			
			BOOL bPOR=FALSE;
			float dep,dep0,por0,tmp,ave;
			float SPOR,fXM=1.f/m_XM;
			CFIDIO* pFIDPOR=NULL;
			pFIDPOR = new CFIDIO(szExtPorName);
			if(pFIDPOR->Open()) 
				bPOR=TRUE;

			CString LLsStr=m_szBufPath+".RMFGPT";

			float fLLs;
			BOOL bLLs=FALSE;	
			CFIDIO* pLLs=NULL;
			pLLs = new CFIDIO(LLsStr);
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
					pFIDPOR->SeekToDepth(dep0);
					pFIDPOR->Read(2,1,&por0);
				}
				//读取浅侧向值
				fLLs=m_Rm;
				if(bLLs)
				{
					pLLs->SeekToDepth(dep0);
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
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton())
				{
					bUserBreak=TRUE;
					break;
				}
			}
			delete Value;
			delete pFIDPOR;delete pLLs;
			delete pFreq;
		}
		Outfp.Close ();
	}
	else bUserBreak=TRUE;
	Infp.Close ();
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CPORCompute::StaticPor(CString InName)
{
	 int iWin=(int)(m_StatWin/m_Rlev);
	if(iWin<2) iWin=2;
	int Num=iWin*m_DataNum/2;

	float StatRlev=0.125f;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle("区间孔隙度统计...");
	BOOL bUserBreak=FALSE;

	char s[256],fn[256];
	strcpy(s,m_szWellName);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile Infp;
	if(Infp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		CLogProcess *pLog;
		//输出曲线信息
		pLog = new CLogProcess(m_szWellName,NULL);
		pLog->StartDep=m_StProDep;
		pLog->EndDep=m_EnProDep;
		pLog->DepLevel=StatRlev;
		//初始化输出
		OUTCURVE *pOutCurve = NULL;
		pOutCurve = new OUTCURVE[8];
		strcpy(pOutCurve[0].name,"POR1");  // POR1 最小孔隙度
		strcpy(pOutCurve[1].name,"POR2");  // POR2 最大频率孔隙度
		strcpy(pOutCurve[2].name,"POR3");  // POR3 最大孔隙度

		strcpy(pOutCurve[3].name,"POR12"); // POR12原生孔隙度
		strcpy(pOutCurve[4].name,"POR23"); // POR23次生孔隙度
        strcpy(pOutCurve[5].name,"POR13"); // POR13总孔隙度
        
		strcpy(pOutCurve[6].name,"PORV2"); // PORV2原生累加孔隙度
		strcpy(pOutCurve[7].name,"PORV3"); // PORV3累加总孔隙度

		float *OutXX;
		OutXX = new float[8];
		pLog->InitOut(8,pOutCurve,(char *)OutXX);	
		pLog->InitBuffer();

		int Block=m_SpcNum*4;
		float Minfreq,Pra;
		float *freq=new float[m_SpcNum];
		float Scale=m_PORMax/m_SpcNum;
		int i,k,m,MinPos,MaxPos,MaxFrePos;
		float por1,por2,por3,por12,por13;
		float dep,temp;
		int N,Nx;
		k=0;
		dep=m_StProDep;
		while(dep<m_EnProDep)
		{
			m=(int)((dep-m_StProDep)/m_StatRlev);
			Infp.Seek (m*Block,CFile::begin);
			Infp.Read (freq,Block);

			//计算最小孔隙度位置
			for(i=1;i<m_SpcNum;i++)
				if(freq[i]>m_MinStat)
					break;
			if(i>m_SpcNum-5)
			{
				OutXX[0]=0.f;OutXX[1]=0.f;OutXX[2]=0.f;
				OutXX[3]=0.f;OutXX[4]=0.f;OutXX[5]=0.f;
				OutXX[6]=0.f;OutXX[7]=0.f;

				dep=m_StProDep+StatRlev*k;
				pLog->Dep=dep;
				pLog->DatOut();
				pGuage->SetDepth(dep);
				k++;
				continue;
			}
			Minfreq=freq[i];
			temp=freq[i];
			MinPos=i;
			MaxPos=i;

			por1=MinPos*Scale;
			por2=por1;
			por3=por1;
			por12=por1;
			por13=por1;

			//得到最大孔隙度 最大频率位置
			for(i=MinPos;i<m_SpcNum-5;i++)
			{
				if(freq[i]>Minfreq)
				{
					por3=i*Scale;
					MaxPos=i;
					if(temp<freq[i])
					{
						temp=freq[i];
						por2=i*Scale;
						MaxFrePos=i;
					}
				}
			}
			
			if(MaxPos-MinPos<5)
			{
				por13=0.f;
				por12=0.f;
			}
			else
			{
				por13=0.f;N=0;
				for(i=MinPos;i<MaxPos;i++)
				{
					Nx=(int)(Num*freq[i]/100.f);
					temp=Nx*i*Scale;
					por13+=temp;
					N+=Nx;
				}
				por13/=N;
				if(MaxFrePos-MinPos<2)
					por12=0.f;
				else
				{
					por12=0.f;N=0;
					for(i=MinPos;i<MaxFrePos;i++)
					{
						Nx=(int)(Num*freq[i]/100.f);
						temp=Nx*i*Scale;
						por12+=temp;
						N+=Nx;
					}
					por12/=N;
				}
			}

			OutXX[0]=por1;OutXX[1]=por2;OutXX[2]=por3;
			OutXX[3]=por12;OutXX[4]=por13-por12;OutXX[5]=por13;
			OutXX[6]=por1+por12;OutXX[7]=por1+por13;

			Pra=por3/OutXX[7];
			for(i=3;i<8;i++)
				OutXX[i]*=Pra;
			dep=m_StProDep+StatRlev*k;
			pLog->Dep=dep;
			pLog->DatOut();
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
			k++;
		}
		delete OutXX;
		delete pOutCurve;
		delete pLog;
		delete freq;
	}
	else bUserBreak=TRUE;
	Infp.Close ();
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CPORCompute::StaticSectionPor(CString InName)
{
	//打开外部孔隙度曲线
	CFIDIO* pPor=NULL;
	CString szExtPorName=m_szExtPorWell+"."+m_szExtPorName;
	pPor = new CFIDIO(szExtPorName);
	if(!pPor->Open()) 
	{
		delete pPor;
		return FALSE;
	}
	CFIDIO* pSpc=NULL;
	CString szSpcName=m_szWellName+"."+InName;
	pSpc= new CFIDIO(szSpcName);
	if(!pSpc->Open()) 
	{
		delete pPor;
		delete pSpc;
		return FALSE;
	}

	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle("区间孔隙度统计...");
	BOOL bUserBreak=FALSE;

	//输出曲线信息
	CLogProcess *pLog = new CLogProcess(m_szWellName,NULL);
	pLog->StartDep=m_StProDep;
	pLog->EndDep=m_EnProDep;
	pLog->DepLevel=m_StatRlev;
	//初始化输出
	OUTCURVE *pOutCurve = NULL;
	pOutCurve = new OUTCURVE[6];
	strcpy(pOutCurve[0].name,"POR1");  // POR1 分区位置1
	strcpy(pOutCurve[1].name,"POR2");  // POR2 分区位置2
	strcpy(pOutCurve[2].name,"POR3");  // POR3 分区位置3

	strcpy(pOutCurve[3].name,"PORV1"); // PORV1 累加原生孔隙 
	strcpy(pOutCurve[4].name,"PORV2"); // PORV2 累加次生孔隙
	strcpy(pOutCurve[5].name,"PORV3"); // PORV3 累加缝洞孔隙

	float *OutXX= new float[6];
	pLog->InitOut(6,pOutCurve,(char *)OutXX);	
	pLog->InitBuffer();

	int Block=m_SpcNum*4;
	float *pFreq=new float[m_SpcNum];
	float por,dep,tmp;
	int i,k=0;
	int p0,p1,p2;

	int nSect=3;
	dep=m_StProDep;
	while(dep<m_EnProDep)
	{
		pPor->SeekToDepth(dep);
		pPor->Read(2,1,&por);

		pSpc->SeekToDepth(dep);
		pSpc->Read(3,m_SpcNum,pFreq);

		for(i=0;i<6;i++)
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
		if(p2>0)
		{
			OutXX[0]=p0*m_PORMax/m_SpcNum;
			OutXX[1]=p1*m_PORMax/m_SpcNum;
			OutXX[2]=p2*m_PORMax/m_SpcNum;
			OutXX[3]=p0*1.f;
			OutXX[4]=p1*1.f;
			OutXX[5]=p2*1.f;
			//外部孔隙度刻度
			tmp=OutXX[5];

			for(i=3;i<6;i++)
				OutXX[i]=OutXX[i]*por/tmp;
		}
		dep=m_StProDep+m_StatRlev*k;
		pLog->Dep=dep;
		pLog->DatOut();
		pGuage->SetDepth(dep);
		if(pGuage->CheckCancelButton())
		{
			bUserBreak=TRUE;
			break;
		}
		k++;
	}
	delete OutXX;
	delete pOutCurve;
	delete pLog;
	delete pFreq;
	delete pPor;
	delete pSpc;
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CPORCompute::StaticKmeansPor(CString InName)
{
	//打开外部孔隙度曲线
	CFIDIO* pPor=NULL;
	CString szExtPorName=m_szExtPorWell+"."+m_szExtPorName;
	pPor = new CFIDIO(szExtPorName);
	if(!pPor->Open()) 
	{
		delete pPor;
		return FALSE;
	}

	CFIDIO* pSpc=NULL;
	CString szSpcName=m_szWellName+"."+InName;
	pSpc= new CFIDIO(szSpcName);
	if(!pSpc->Open()) 
	{
		delete pPor;
		delete pSpc;
		return FALSE;
	}

	int nSect=3;						//分类数
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle("区间孔隙度统计...");
	BOOL bUserBreak=FALSE;

	//输出曲线信息
	CLogProcess *pLog = new CLogProcess(m_szWellName,NULL);
	pLog->StartDep=m_StProDep;
	pLog->EndDep=m_EnProDep;
	pLog->DepLevel=m_StatRlev;
	//初始化输出
	OUTCURVE *pOutCurve= new OUTCURVE[nSect*2];
	int i;
	CString szStr;
	for(i=0;i<nSect;i++)
	{
		szStr.Format("POR%d",i+1);
		strcpy(pOutCurve[i].name,szStr);  // 孔隙度
		szStr.Format("PORV%d",i+1);
		strcpy(pOutCurve[nSect+i].name,szStr);  // 孔隙度
	}
	float *OutXX = new float[nSect*2];
	pLog->InitOut(nSect*2,pOutCurve,(char *)OutXX);	
	pLog->InitBuffer();

	int Block=m_SpcNum*4;
	float *pFreq=new float[m_SpcNum];
	
	//用于Kmeans聚类
	float **pData=new float *[2];		//二维样本
	pData[0]= new float[m_SpcNum];
	pData[1]= new float[m_SpcNum];
	float **pCenter=new float *[2];		//类中点
	pCenter[0]=new float[nSect];
	pCenter[1]=new float[nSect];
	int *pZin=new int[m_SpcNum];		//样本点所属类
	float *pWork=new float[m_SpcNum];

	float por,tmp,tmp0;
	int j,m,Num,k=0;
	float dep=m_StProDep;
	while(dep<m_EnProDep)
	{
		pPor->SeekToDepth(dep);
		pPor->Read(2,1,&por);

		pSpc->SeekToDepth(dep);
		pSpc->Read(3,m_SpcNum,pFreq);

		for(i=0;i<nSect*2;i++)
			OutXX[i]=0.f;
		Num=0;
		for(i=0;i<m_SpcNum-5;i++)
		{
			if(pFreq[i]>m_MinStat)
			{
				pData[0][Num]=pFreq[i];
				pData[1][Num]=i*1.f;
				Num++;
			}
		}
		if(Num>nSect)
		{
			if(kmeans(pData,2,Num,nSect,pCenter,pZin, pWork)==0)
			{
				tmp0=-1.f;
				tmp=m_SpcNum*1.f;

				for(j=0;j<nSect;j++)
				{
					m=0;
					for(i=0;i<nSect;i++)
					{
						if(tmp>pCenter[1][i]&&pCenter[1][i]>tmp0)
						{
							tmp=pCenter[1][i];
							m=i;
						}
					}
					tmp0=tmp;tmp=m_SpcNum*1.f;
					/*OutXX[j]=pCenter[1][m]*m_PORMax/m_SpcNum;
					for(i=0;i<Num;i++)
					{
						if(pZin[i]==m)
							OutXX[j+nSect]+=pData[0][i];
					}*/
					OutXX[j]=pCenter[1][m];
				}
				/*for(i=1;i<nSect;i++)
					OutXX[i+nSect]+=OutXX[i-1+nSect];*/
				for(i=0;i<nSect;i++)
				{
					OutXX[i+nSect]=OutXX[i];
					OutXX[i]=OutXX[i]*m_PORMax/m_SpcNum;
				}
				//外部孔隙度刻度
				tmp=OutXX[nSect*2-1];
				if(tmp>0.f)
				{
					for(i=0;i<nSect;i++)
						OutXX[i+nSect]=OutXX[i+nSect]*por/tmp;
				}
			}
		}
		dep=m_StProDep+m_StatRlev*k;
		pLog->Dep=dep;
		pLog->DatOut();
		pGuage->SetDepth(dep);
		if(pGuage->CheckCancelButton())
		{
			bUserBreak=TRUE;
			break;
		}
		k++;
	}
	delete OutXX;
	delete pOutCurve;
	delete pLog;
	delete pFreq;

	//用于Kmeans聚类
	delete pData[0];delete pData[1];delete []pData;
	delete pCenter[0];delete pCenter[1];delete []pCenter;
	delete pZin;
	delete pWork;
	
	delete pPor;
	delete pSpc;
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

/***********************************************************************
* kmpp - K-Means++
* Traditional data clustering with a special initialization
* by Andy Allinger, 2017, released to the public domain
* This program may be used by any person for any purpose
*
* Origin:
*     Hugo Steinhaus, 1956
*
* Refer to:
*     "kmeans++: the advantages of careful seeding"
*     David Arthur and Sergei Vassilvitskii
*     Proceedings of the eighteenth annual ACM-SIAM symposium
*       on Discrete algorithms, 2007
*
*___Variable___Type______I/O_______Description__________________________
*   x[p][n]    float     In        Data points
*   p          int       In        Dimension of the data
*   n          int       In        Number of points
*   k          int       In        # clusters
*   c[p][k]    float     Out       Center points of clusters
*   z[n]       int       Out       What cluster a point is in
*   work[n]    float     Neither
*
* Return value: 0 no errors
*               1 empty cluster
*               2 max iterations exceeded
*               3 k not in valid range
***********************************************************************/
int CPORCompute::kmeans (float **x, int p, int n, int k, float **c, int *z, float *work)
{
/* Local variables */
	int h, i, j, l;
	float u, d, d2;
	int i1, l0, l1;
	float frm, tot, best;
	bool change;

/* constants */
	const int ITER=1000;
	const float BIG=1e33f;

/* Begin. */
	if (k < 1 || k > n) return 3;      /* k out of bounds */
	for (i = 0; i < n; ++i) z[i] = 0;  /* clear Z */
	frm = (float)(RAND_MAX);

/* Initial centers */
	for (i = 0; i < n; ++i) work[i] = 1e33f;
	u = (float)(rand()) / frm;
	i1 = __min((int)(u * (float)(n)) + 1, n);      /* choose first center at random */
	for (j = 0; j < p; ++j) c[j][0] = x[j][i1];     /* copy */
	for (l = 1; l < k; ++l) {        /* initialize other centers */
		tot = 0.f;
		for (i = 0; i < n; ++i) {            /* measure from each point */
			best = work[i];
			d2 = 0.f;			/* to prior center */
			for (j = 0; j < p; ++j) {       /* Squared Euclidean distance */
				d = x[j][i] - c[j][l-1];
				d2 += d * d;
				if (d2 >= best) break;     /* needless to add to d2 */
			} /* next dimension */
			if (d2 < best) best = d2;       /* shortest squared distance? */
			work[i] = best;
			tot += best;                         /* cumulative squared distance */
		} /* next data point */

/* Choose center with probability proportional to its squared distance
     from existing centers. */
		u = (float)(rand()) / frm;
		u *= tot;                     /* uniform at random over cumulative distance */
		tot = 0.f;
		for (i = 0; i < n; ++i) {
			i1 = i;
			tot += work[i];
			if (tot > u) break;
		} /* next i */
		for (j = 0; j < p; ++j) c[j][l] = x[j][i1];        /* assign center */
	} /* next center to initialize */

/* Main loop */
	for (h = 0; h < ITER; ++h) {
		change = false;

/* Find nearest center for each point */
		for (i = 0; i < n; ++i) {
			l0 = z[i] - 1;
			l1 = 0;
			best = BIG;
			for (l = 0; l < k; ++l) {
				d2 = 0.f;
				for (j = 0; j < p; ++j) {
					d = x[j][i] - c[j][l];
					d2 += d * d;
					if (d2 >= best) break;
				} /* next j */

				if (d2 < best) {
					best = d2;
					l1 = l;
				}
			} /* next L */

			if (l0 != l1) {
				z[i] = l1 + 1;         /* reassign point */
				change = true;
			}
		} /* next i */
		if (! change) return 0;          /* success */

/* Find cluster centers */
		for (l = 0; l < k; ++l) work[l] = 0.f;   /* zero population */
		for (j = 0; j < p; ++j) {                /* zero centers */
			for (l = 0; l < k; ++l) {
				c[j][l] = 0.f;
			}
		}

		for (i = 0; i < n; ++i) {
			l = z[i] - 1;
			work[l] += 1.f;             /* count */
			for (j = 0; j < p; ++j) c[j][l] += x[j][i];    /* add */
		}

		for (l = 0; l < k; ++l) {
			if (work[l] < .5f) return 1;  /* empty cluster check */
			u = 1.f / work[l];
			for (j = 0; j < p; ++j) c[j][l] *= u; /* multiplication is faster than division*/
		}
	} /* next h */
	return 2;  /* too many iterations */
} /* end of file kmpp.c */