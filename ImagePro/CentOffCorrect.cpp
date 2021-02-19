// CentOffCorrect.cpp: implementation of the CCentOffCorrect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "FlowChartDoc.h"
#include "BackupData.h"
#include "CentOffCorrect.h"
#include "FitCircle.h"
#include "FIDio.h"
#include "LogProcess.h"
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
IMPLEMENT_SERIAL(CCentOffCorrect,CFlowNode, 0)
CCentOffCorrect::CCentOffCorrect()
{
	m_pDoc=NULL;
}

CCentOffCorrect::~CCentOffCorrect()
{
	if(m_Value!=NULL)
		delete m_Value;
}

CCentOffCorrect::CCentOffCorrect(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
}

void CCentOffCorrect::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
		ar<<m_DataNum;
	else
		ar>>m_DataNum;
}

void CCentOffCorrect::InitPara(CFlowChartDoc* pDoc)
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
				m_ButtonMid=12;
				m_DataNum=m_PadNum*m_ButtonNum;
			}
		}
	}
	m_pDoc=pDoc;
}

BOOL CCentOffCorrect::Run()
{
	m_bResult=TRUE;
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if((m_szSeries=="CBIL")||(m_szSeries=="CAST")
		||(m_szSeries=="UBI")||(m_szSeries=="UBS"))//声成像处理
	{
		m_bResult=MITCorrect(m_szBufPath,"AMPGPT",FALSE);
		m_bResult=MITCorrect(m_szBufPath,"TTGPT",FALSE);
		m_bResult=MITCorrect(m_szBufPath,"DATA00",TRUE);
	}
	if(m_bResult) OnWellRefresh();

	return m_bResult;
}

BOOL CCentOffCorrect::MITCorrect(CString Path,CString InName,BOOL bWrite)
{
	char s[256],fn[256];
	strcpy(s,Path);
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
		pGuage->SetTitle(InName+" 曲线偏心校正...");
		pGuage->ShowWindow(SW_SHOW);
		float dep=m_StProDep;;
		BOOL bUserBreak=FALSE;
	
		int k=0;
		int Block=m_DataNum*4;
		m_Value=new float[m_DataNum+10];
		if(!bWrite)
		{
			while(k<RecordNum)
			{
				fp.Seek (k*Block,CFile::begin);
				fp.Read (m_Value,Block);
				//最小二乘法拟合圆对m_Value校正
				FitCirclePro();
			//  校正后的井径写回缓冲区
				fp.Seek (k*Block,CFile::begin);
				fp.Write(m_Value,Block);
				k++;
				dep=m_StProDep+m_Rlev*k;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton()) 
				{
					bUserBreak=TRUE;
					break;
				}
			}	
		}
		else
		{
			CLogProcess *pLog=NULL;
			pLog = new CLogProcess(m_szWellName,NULL);
			pLog->StartDep=m_StProDep;
			pLog->EndDep=m_EnProDep;
			pLog->DepLevel=m_Rlev;
			OUTCURVE *POutCurve = new OUTCURVE[4];
			//拟合圆半径、偏心距、偏心角、偏心方位
			strcpy(POutCurve[0].name,"CNTRAD");
			strcpy(POutCurve[1].name,"CNTOFF");
			strcpy(POutCurve[2].name,"CNTDEV");
			strcpy(POutCurve[3].name,"CNTDAZ");
			float *XX=new float[4];
			pLog->InitOut(4,POutCurve,(char *)XX);
			pLog->InitBuffer();
			float Off;
			while(k<RecordNum)
			{
				fp.Seek (k*Block,CFile::begin);
				fp.Read (m_Value,Block);
				//最小二乘法拟合圆对m_Value校正
				FitCirclePro();
			//  校正后的井径写回缓冲区
				fp.Seek (k*Block,CFile::begin);
				fp.Write(m_Value,Block);
				k++;
				dep=m_StProDep+m_Rlev*k;

				//将拟合圆半径、偏心距、偏心角、偏心方位写入曲线
				Off=sqrt(fX*fX+fY*fY);
				if(m_fRadius>0.001f&&Off>0.001f)
				{
					XX[0]=m_fRadius;
					XX[1]=m_CenOff;
					XX[2]=(float)(atan(m_CenOff/m_fRadius)*57.29577957855);
					XX[3]=(float)(asin(fabs(fX/Off))*57.29577957855);					
					if(fY<0)
					{
						if(fX>0.f)
							XX[3]*=2.f;
						else
							XX[3]+=180.f;
					}
					pLog->Dep=dep;
					pLog->DatOut();
				}
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton()) 
				{
					bUserBreak=TRUE;
					break;
				}
			}
			delete pLog;	
			delete POutCurve;
			delete XX;
		}
		delete m_Value;m_Value=NULL;
		fp.Close ();
		delete pGuage;
	}
	else return FALSE;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

void CCentOffCorrect::FitCirclePro()
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

	fScale=55000.f/(MaxValue-MinValue);
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

		fX = (float)FitCircle.m_fCenterX;
		fY = (float)FitCircle.m_fCenterY;
		for(i=0;i<m_DataNum;i++)
		   m_Value[i]=(float)(sqrt((points[i].x-fX)*(points[i].x-fX)+(points[i].y-fY)*(points[i].y-fY))/fScale);
		m_fRadius=(float)(FitCircle.m_fRadius/fScale);
		m_CenOff=(float)(sqrt(fX*fX+fY*fY)/fScale);
	}
	delete points;
}

void CCentOffCorrect::OnWellRefresh() 
{
	// TODO: Add your command handler code here
	CString WellName=m_szWellName;
	char TargetCurveName[256];
	strcpy(TargetCurveName,WellName);
	strcat(TargetCurveName,".FID");
	if(!RefurbishFID(TargetCurveName))
	{
		CString refurbishmsg="Can't Refurbish file ";
		refurbishmsg+=TargetCurveName;
		AfxMessageBox(refurbishmsg);
	}
}