// MinThickCp.cpp: implementation of the CMinThickCp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "FlowChartDoc.h"
#include "BackupData.h"
#include "MinThickCp.h"
#include "ThickCpDlg.h"
#include "FitCircle.h"
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
IMPLEMENT_SERIAL(CMinThickCp,CFlowNode, 0)
CMinThickCp::CMinThickCp()
{
	m_pDoc=NULL;
}

CMinThickCp::~CMinThickCp()
{

}

CMinThickCp::CMinThickCp(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_HighValue=4.f;
	m_LowValue=-4.f;
	m_BoxLength=60.f;
	m_TubeThick=5.f;
	m_NormalThick=5.f;
}

void CMinThickCp::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_DataNum;
		ar<<m_HighValue;
		ar<<m_LowValue;
		ar<<m_BoxLength;
		ar<<m_TubeThick;
		ar<<m_NormalThick;
	}
	else
	{
		ar>>m_DataNum;
		ar>>m_HighValue;
		ar>>m_LowValue;
		ar>>m_BoxLength;
		ar>>m_TubeThick;
		ar>>m_NormalThick;
	}
}

void CMinThickCp::InitPara(CFlowChartDoc* pDoc)
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

BOOL CMinThickCp::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(!m_pDoc->m_bAunAll)
	{
		CThickCpDlg dlg;
		dlg.m_HighValue=m_HighValue;
		dlg.m_LowValue=m_LowValue;
		dlg.m_BoxLength=m_BoxLength;
		dlg.m_TubeThick=m_TubeThick;
		dlg.m_NormalThick=m_NormalThick;
		if(dlg.DoModal ()!=IDOK)
			return FALSE;
		else
		{
			m_HighValue=dlg.m_HighValue;
			m_LowValue=dlg.m_LowValue;
			m_BoxLength=dlg.m_BoxLength;
			m_TubeThick=dlg.m_TubeThick;
			m_NormalThick=dlg.m_NormalThick;
		}
	}
	m_bResult=ComputeMinThick("DATA00");
	GetCCLDepList();
	return m_bResult;
}

BOOL CMinThickCp::GetCCLDepList()
{
	BOOL flag=FALSE;
	CFIDIO* pFIDIO = new CFIDIO(m_szWellName+".CNTRAD");
	if(pFIDIO->Open())
	{
		float rlev=pFIDIO->GetDepthLevel();
		if (rlev>0.001)
		{
			CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
			pGuage->Create(CWnd::GetActiveWindow());
			pGuage->SetTitle("计算管串接箍深度...");
			pGuage->ShowWindow(SW_SHOW);

			CString filename=m_szWellName.Left(m_szWellName.ReverseFind('\\')+1);
			filename+="HandWork.List";
			CString str;
			CStdioFile File;
			if (File.Open(filename, CFile::modeWrite | CFile::modeCreate| CFile::typeText)) 
			{
				float Val,Val1,Val2;
				float dep=m_StProDep;
				pFIDIO->SeekToDepth((float)dep);
				pFIDIO->Read(2,1,&Val1);
				pFIDIO->Next();
				unsigned k=1;
				int CCLNum=0;
				str="序号,深度,原始角度,原始方位,类型,真角度,真方位\n";
				File.WriteString(str);
				while(dep<m_EnProDep)
				{
					dep=m_StProDep+k*rlev;
					pFIDIO->Read(2,1,&Val2);
					pFIDIO->Next();
					k++;
					if(Val1>5.f)
						Val=Val2-Val1;
					else Val=0.1f;
					Val1=Val2;
					if(Val>m_HighValue)
					{
						CCLNum++;
						str.Format("%d,%.2f,%.2f,%.2f,%s,%.2f,%.2f\n",CCLNum,dep-m_BoxLength/2000.f,0.f,0.f,"管串接箍",0.f,0.f);
						File.WriteString(str);
					}
					else if(Val<m_LowValue)
					{
						CCLNum++;
						str.Format("%d,%.2f,%.2f,%.2f,%s,%.2f,%.2f\n",CCLNum,dep-m_BoxLength/2000.f,0.f,0.f,"管串接箍",0.f,0.f);
						File.WriteString(str);
					}
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton()) 
						break;
				}
				File.Close();
				flag=TRUE;
			}
			delete pGuage;
		}
	}
	delete pFIDIO;
	return flag;
}

BOOL CMinThickCp::ComputeMinThick(CString InName)
{
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	BOOL bUserBreak=FALSE;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//初始输出管串壁厚损失量 MINTHICK
		FIDCONTENT Content[2] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
 								{"MINTHICK","mm",REPR_FLOAT,4,1,1,0.f,500.f,0.f}};
		char adi[] = "DATID 0";	
		Content[0].min = m_StProDep;
		Content[0].max = m_EnProDep;
		Content[0].rlev = m_Rlev;                       

		FIDINDEX Index;	
		Index.Contentlist = Content;
		Index.dimension = 2;
		Index.adi = adi;
		CFIDIO* pFIDth = NULL;
		pFIDth = new CFIDIO(m_szWellName+".MINTHICK",&Index);
		if(!pFIDth->Open())
		{
			fp.Close ();
			delete pFIDth;return FALSE;
		}

		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->SetTitle("计算管串壁厚最大损失量...");
		pGuage->ShowWindow(SW_SHOW);
		float dep=m_StProDep;
		BOOL bUserBreak=FALSE;
		int i,k=0,Block=m_DataNum*4;
		m_Value=new float[m_DataNum+10];
		float Min,MinThick;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (m_Value,Block);
			//最小二乘法拟合圆对m_Value校正
			FitCirclePro();
			MinThick=0.f;
			for(i=0;i<m_DataNum;i++)
			{
				Min=m_Value[i]-m_fRadius;
				if(MinThick<Min)
					MinThick=Min;
			}
			pFIDth->Write(2,1,&MinThick);
			pFIDth->Next();
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}	
		delete m_Value;
		delete pFIDth;delete pGuage;
		fp.Close ();
	}
	else return FALSE;

	if(bUserBreak) return FALSE;
	else           return TRUE;
}

void CMinThickCp::FitCirclePro()
{
	CPoint *points=NULL;
	points=new CPoint[m_DataNum+10];
	CFitCircle FitCircle(m_DataNum);//最小二乘法拟合圆

	float MaxValue=4.f;
	int i;
	for(i=0;i<m_DataNum;i++)
	{
		if(m_Value[i]<4.f) continue;
		if(m_Value[i]>MaxValue) 
			MaxValue=m_Value[i];
	}
	int pk=(int)(MaxValue/6.f);
	float Max=pk*6.f;
	if(Max<MaxValue)
		Max+=6.f;
	MaxValue=Max;
	float MinValue=Max*-1.f;

	CPoint pt(300,300);
	double fX,fY;
	double fScale=55000./(MaxValue-MinValue);
	int nFitNum=0;
	for(i=0;i<m_DataNum;i++)
	{
		if(m_Value[i]>2.f)
		{
			points[nFitNum].x=(int)pt.x+(int)(sin(6.2831853*i/m_DataNum)*fScale*m_Value[i]);
			points[nFitNum].y=(int)pt.y-(int)(cos(6.2831853*i/m_DataNum)*fScale*m_Value[i]);
			FitCircle.m_points[nFitNum].x=points[nFitNum].x;
			FitCircle.m_points[nFitNum].y=points[nFitNum].y;
			nFitNum++;
		}
	}
	if(nFitNum>3)
	{
		FitCircle.m_nNum=nFitNum;
		FitCircle.DoCircleFit();

		fX = FitCircle.m_fCenterX;
		fY = FitCircle.m_fCenterY;
		for(i=0;i<m_DataNum;i++)
		   m_Value[i]=(float)(sqrt((points[i].x-fX)*(points[i].x-fX)+(points[i].y-fY)*(points[i].y-fY))/fScale);
		m_fRadius = (float)(FitCircle.m_fRadius/fScale);
	}
	delete points;
}
