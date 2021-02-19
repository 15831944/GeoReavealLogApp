// ComputeRADS.cpp: implementation of the CComputeRADS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "FlowChartDoc.h"
#include "BackupData.h"
#include "ComputeRADS.h"
#include "FitCircle.h"
#include "HoleAnalysis.h"
#include "FIDio.h"
#include "Guage.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CComputeRADS,CFlowNode, 0)
CComputeRADS::CComputeRADS()
{
	m_pDoc=NULL;
	
}

CComputeRADS::~CComputeRADS()
{
	if(m_Value!=NULL)
		delete m_Value;
}

CComputeRADS::CComputeRADS(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_ScaleDep=-9999.f;
	m_ScaleRadius=-9999.f;
	m_ScaleK=1.f;
}

void CComputeRADS::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_DataNum;
		ar<<m_ScaleDep;
		ar<<m_ScaleRadius;
		ar<<m_ScaleK;
	}
	else
	{
		ar>>m_DataNum;
		ar>>m_ScaleDep;
		ar>>m_ScaleRadius;
		ar>>m_ScaleK;
	}
}

void CComputeRADS::InitPara(CFlowChartDoc* pDoc)
{
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		CFlowNode *pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{		
			if(pObj->IsKindOf(RUNTIME_CLASS(CBackupData)))
			{
				m_szBufPath=((CBackupData*)pObj)->m_szBufPath;
				m_szWellName=((CBackupData*)pObj)->m_szWellName;
				m_szSeries=((CBackupData*)pObj)->m_szSeries;
				m_PadNum=((CBackupData*)pObj)->m_PadNum; 
				m_ButtonNum=((CBackupData*)pObj)->m_ButtonNum;	
				m_CalNum=((CBackupData*)pObj)->m_CalNum;
				m_StProDep=((CBackupData*)pObj)->m_StProDep;
				m_EnProDep=((CBackupData*)pObj)->m_EnProDep;
				m_Rlev=((CBackupData*)pObj)->m_Rlev;
				m_Bits=((CBackupData*)pObj)->m_Bits;
				m_ButtonMid=12;
				m_DataNum=m_PadNum*m_ButtonNum;
			}
		}
	}
	m_pDoc=pDoc;
}

BOOL CComputeRADS::Run()
{
	m_bResult=TRUE;
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if((m_szSeries=="CBIL")||(m_szSeries=="CAST")
		    ||(m_szSeries=="UBI")||(m_szSeries=="UBS"))//声成像处理
	{
		if(m_StProDep>m_ScaleDep||m_ScaleDep>m_EnProDep)
		{
			m_ScaleRadius=m_Bits/2.f;
			m_ScaleDep=m_StProDep+(m_EnProDep-m_StProDep)/2.f,
			m_ScaleK=1.f;
		}
		if(!m_pDoc->m_bAunAll)
		{
			CHoleAnalysis dlg;
			CString str;
			str.Format("处理井段：%g--%g",m_StProDep,m_EnProDep);
			dlg.m_DepStr=str;
			dlg.m_szAradFileName=m_szBufPath+".TTGPT";
			dlg.m_Dep=m_ScaleDep;
			dlg.m_ScaleRadius=m_ScaleRadius;
			dlg.m_ScaleK=m_ScaleK;
			if(dlg.DoModal()==IDOK)
			{
				m_ScaleDep=dlg.m_Dep;
				m_ScaleRadius=dlg.m_ScaleRadius;
				m_fRadius=dlg.m_ComputeRadius;
				m_ScaleK=dlg.m_ScaleK;
				ScaleRADS("TTGPT","RADSGPT",FALSE);
			}
		}
		else
			ScaleRADS("TTGPT","RADSGPT",TRUE);
	}
	return m_bResult;
}

BOOL CComputeRADS::ScaleRADS(CString InName,CString OutName,BOOL bFit)
{
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	BOOL bUserBreak=FALSE;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->SetTitle(InName+" 管串井径刻度...");
		pGuage->ShowWindow(SW_SHOW);
		float dep=m_StProDep;;
		BOOL bUserBreak=FALSE;
		FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    							{"PAD","none",REPR_SHORT,4,6,0,0.f,0.f,0.f},
								{"RADSGPT","mm",REPR_FLOAT,4,144,144,0.f,0.f,0.f}};
		m_Content[0].min = m_StProDep;
		m_Content[0].max = m_EnProDep;
		m_Content[0].rlev = m_Rlev;
		m_Content[1].nps=m_DataNum;                        
		m_Content[1].min=5.f;                            
		m_Content[1].max=300.f;                   
		m_Content[1].rlev = m_Bits;                        

		m_Content[2].nps=m_DataNum;
		m_Content[2].npw=m_DataNum;                        
		m_Content[2].min=(float)m_PadNum;                  
		m_Content[2].max=(float)m_ButtonNum; 
		strcpy(m_Content[2].name,OutName);
		m_Content[2].rlev =0.f;                            

		FIDINDEX m_Index;	
		m_Index.Contentlist = m_Content;
		m_Index.dimension = 3;
		m_Index.adi = "DATID 0";

		int i,k,Block=m_DataNum*4;
		CFIDIO* pFIDW = NULL;
		pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
		if(pFIDW->Open())
		{	
			//备份刻度后的半径曲线到原井目
			CFIDIO* pFIDW0= NULL;
			pFIDW0 = new CFIDIO(m_szWellName+".RADS",&m_Index);
			if(m_Value!=NULL)
				delete m_Value;
			m_Value=new float[m_DataNum+10];
			if(bFit)
			{
				k=(int)((m_ScaleDep-m_StProDep)/m_Rlev);
				fp.Seek (k*Block,CFile::begin);
				fp.Read (m_Value,Block);
				//最小二乘法拟合圆对m_Value校正
				FitCirclePro();
				m_ScaleK=m_ScaleRadius/m_fRadius;
			}
			k=0;
			pFIDW->SeekToDepth(m_StProDep);
			pFIDW->SeekToDepth(m_StProDep);
			while(k<RecordNum)
			{
				fp.Seek (k*Block,CFile::begin);
				fp.Read (m_Value,Block);
				
				for(i=0;i<m_DataNum;i++)
					m_Value[i]*=m_ScaleK;
				pFIDW->Write(3,m_DataNum,m_Value);
				pFIDW->Next();
			
				pFIDW0->Write(3,m_DataNum,m_Value);
				pFIDW0->Next();
				k++;
				dep=m_StProDep+m_Rlev*k;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton()) 
				{
					bUserBreak=TRUE;
					break;
				}
			}	
			delete m_Value;delete pFIDW0;
		}
		else bUserBreak=TRUE;
		delete pFIDW;delete pGuage;
		fp.Close ();
	}
	else return FALSE;

	if(bUserBreak) return FALSE;
	else           return TRUE;
}

void CComputeRADS::FitCirclePro()
{
	CPoint *points=NULL;
	points=new CPoint[m_DataNum+10];
	CFitCircle FitCircle(m_DataNum);//最小二乘法拟合圆

	float MaxValue=0.f;
	int i;
	for(i=0;i<m_DataNum;i++)
	{
		if(m_Value[i]<0.f) continue;
		if(m_Value[i]>MaxValue) 
			MaxValue=m_Value[i];
	}
	float MinValue=MaxValue*-1.f;

	float fScale=55000.f/(MaxValue-MinValue);
	int nFitNum=0;
	for(i=0;i<m_DataNum;i++)
	{
		if(m_Value[i]<0.0125f||m_Value[i]==32767.f) 
				continue;
		points[nFitNum].x=(int)(sin(6.2831853*i/m_DataNum)*fScale*m_Value[i]);
		points[nFitNum].y=(int)(cos(6.2831853*i/m_DataNum)*fScale*m_Value[i]);
		FitCircle.m_points[nFitNum].x=points[nFitNum].x;
		FitCircle.m_points[nFitNum].y=points[nFitNum].y;
		nFitNum++;
	}
	if(nFitNum>3)
	{
		FitCircle.m_nNum=nFitNum;
		FitCircle.DoCircleFit();
		m_fRadius = (float)(FitCircle.m_fRadius/fScale);
	}
	delete points;
}
