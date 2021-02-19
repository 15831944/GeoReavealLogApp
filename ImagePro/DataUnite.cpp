// DataUnite.cpp: implementation of the CDataUnite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "FlowChartDoc.h"
#include "BackupData.h"
#include "DataUnite.h"
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
IMPLEMENT_SERIAL(CDataUnite,CFlowNode, 0)
CDataUnite::CDataUnite()
{
	m_pDoc=NULL;
}

CDataUnite::~CDataUnite()
{

}

CDataUnite::CDataUnite(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
}

void CDataUnite::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
		ar<<m_DataNum;
	else
		ar>>m_DataNum;
}

void CDataUnite::InitPara(CFlowChartDoc* pDoc)
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
				m_Rm=((CBackupData*)pObj)->m_Rm;
				m_ButtonMid=12;
			}
		}
	}
	m_pDoc=pDoc;
}


BOOL CDataUnite::DataCopy(CString InName,CString OutName)
{
	BOOL bOK=FALSE;
	CFIDIO* pFIDR = new CFIDIO(m_szBufPath+"."+InName);
	if(pFIDR->Open())
	{
		if(pFIDR->Copy(m_szBufPath+"."+OutName)!=-1)
			bOK=TRUE;
	}
	delete pFIDR;
	return bOK;
}

BOOL CDataUnite::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(m_szSeries=="FMI_8PAD")
		m_bResult=FMI8UnitData();
	else if(m_szSeries=="FMI_4PAD")
		m_bResult=FMI4UnitData();
	else if(m_szSeries=="EMI")
		m_bResult=EMIUnitData();
	else if(m_szSeries.Find("STAR")>-1)
		m_bResult=StarIIUnitData();
	else if(m_szSeries=="CMI")
		m_bResult=CMIUnitData();
	else
		m_bResult=TRUE;
	return m_bResult;
}

//将进行电扣深度校正和极板深度校正后的数据P1GPT、P2GPT、P3GPT...
//合并为条阵列曲线DATA
BOOL CDataUnite::EMIUnitData()
{
	CString PName[10];int n;
	for(n=0;n<m_PadNum;n++)
		PName[n].Format (".P%iGPT",n);
	CFIDIO* pFIDI1 = new CFIDIO(m_szBufPath+PName[0]);
	if(!pFIDI1->Open())
	{
		AfxMessageBox(PName[0]+" 曲线打开失败    ");
		delete pFIDI1;return FALSE;
	}
	FIDCONTENT* Cont =pFIDI1->GetContent(2);
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_FLOAT,4,6,0,0.f,0.f,0.f},
							   {"DATA00","mmho",REPR_FLOAT,4,144,144,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;

	m_DataNum=m_PadNum*m_ButtonNum;
	m_Content[1].nps=m_DataNum;                   //总电扣数
	m_Content[1].min=(float)m_PadNum;   
	m_Content[1].max=(float)m_ButtonNum;
	m_Content[1].rlev =2.54f;  
	
	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                   
	m_Content[2].min=Cont->min;                  
	m_Content[2].max=Cont->max;               
	strcpy(m_Content[2].unit,Cont->unit); 
	m_Content[2].rlev =2.54f;                     //电扣间距(折算为电扣数)

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CString ss=m_szBufPath+".DATA00";
	CFIDIO* pFIDW = new CFIDIO(ss,&m_Index);
	pFIDW->Close();
	delete pFIDW;
	CFIDIO* pFIDI2 = new CFIDIO(m_szBufPath+PName[1]);
	if(!pFIDI2->Open())
	{
		AfxMessageBox(PName[1]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;
		return FALSE;
	}
	CFIDIO* pFIDI3 = new CFIDIO(m_szBufPath+PName[2]);
	if(!pFIDI3->Open())
	{
		AfxMessageBox(PName[2]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		return FALSE;
	}
	CFIDIO* pFIDI4 = new CFIDIO(m_szBufPath+PName[3]);
	if(!pFIDI4->Open())
	{
		AfxMessageBox(PName[3]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;
		return FALSE;
	}
	CFIDIO* pFIDI5 = new CFIDIO(m_szBufPath+PName[4]);
	if(!pFIDI5->Open())
	{
		AfxMessageBox(PName[4]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;
		return FALSE;
	}
	CFIDIO* pFIDI6 = new CFIDIO(m_szBufPath+PName[5]);
	if(!pFIDI6->Open())
	{
		AfxMessageBox(PName[5]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;delete pFIDI6;
		return FALSE;
	}
	float *Value=new float[m_DataNum+10];
	int i,j;
	pFIDW = new CFIDIO(ss);
	if(pFIDW->Open())
	{	
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->SetTitle("极板数据合并...");
		pGuage->ShowWindow(SW_SHOW);	
		float dep=m_StProDep;

		pFIDW->SeekToTop();
		pFIDI1->SeekToDepth(m_StProDep);
		pFIDI2->SeekToDepth(m_StProDep);
		pFIDI3->SeekToDepth(m_StProDep);
		pFIDI4->SeekToDepth(m_StProDep);
		pFIDI5->SeekToDepth(m_StProDep);
		pFIDI6->SeekToDepth(m_StProDep);		
		float *V1,*V2,*V3,*V4,*V5,*V6;
		V1=new float[m_ButtonNum];
		V2=new float[m_ButtonNum];
		V3=new float[m_ButtonNum];
		V4=new float[m_ButtonNum];
		V5=new float[m_ButtonNum];
		V6=new float[m_ButtonNum];
		i=0;
		while(i<RecordNum)
		{		
			pFIDI1->Read(3,m_ButtonNum,V1);
			pFIDI2->Read(3,m_ButtonNum,V2);
			pFIDI3->Read(3,m_ButtonNum,V3);
			pFIDI4->Read(3,m_ButtonNum,V4);
			pFIDI5->Read(3,m_ButtonNum,V5);
			pFIDI6->Read(3,m_ButtonNum,V6);
/*
			for(j=m_ButtonMid;j<m_ButtonNum;j++) 
				Value[j-m_ButtonMid]=V1[j];
			for(j=0;j<m_ButtonMid;j++) 
				Value[j+m_ButtonMid+1]=V1[j];
			for(j=0;j<m_ButtonNum;j++) 
				V1[m_ButtonNum-1-j]=Value[j];
			for(j=0;j<m_ButtonNum;j++) 
				Value[j]=V1[j];

			for(j=m_ButtonMid;j<m_ButtonNum;j++) 
				V1[j-m_ButtonMid]=V2[j];
			for(j=0;j<m_ButtonMid;j++) 
				V1[j+m_ButtonMid+1]=V2[j];
			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum+j]=V1[j];
			
			for(j=m_ButtonMid;j<25;j++)
				V1[j-m_ButtonMid]=V3[j];
			for(j=0;j<m_ButtonMid;j++) 
				V1[j+m_ButtonMid+1]=V3[j];
			for(j=0;j<m_ButtonNum;j++) 
				V3[m_ButtonNum-1-j]=V1[j];
			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum*2+j]=V3[j];
			
			for(j=m_ButtonMid;j<m_ButtonNum;j++) 
				V1[j-m_ButtonMid]=V4[j];
			for(j=0;j<m_ButtonMid;j++) 
				V1[j+m_ButtonMid+1]=V4[j];
			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum*3+j]=V1[j];
			
			for(j=m_ButtonMid;j<m_ButtonNum;j++) 
				V1[j-m_ButtonMid]=V5[j];
			for(j=0;j<m_ButtonMid;j++)
				V1[j+m_ButtonMid+1]=V5[j];
			for(j=0;j<m_ButtonNum;j++) 
				V5[m_ButtonNum-1-j]=V1[j];
			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum*4+j]=V5[j];

			for(j=m_ButtonMid;j<m_ButtonNum;j++) 
				V1[j-m_ButtonMid]=V6[j];
			for(j=0;j<m_ButtonMid;j++) 
				V1[j+m_ButtonMid+1]=V6[j];
			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum*5+j]=V1[j];
*/

			for(j=0;j<m_ButtonNum;j++) 
				Value[j]=V1[m_ButtonNum-1-j];

			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum+j]=V2[j];

			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum*2+j]=V3[m_ButtonNum-1-j];

			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum*3+j]=V4[j];

			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum*4+j]=V5[m_ButtonNum-1-j];

			for(j=0;j<m_ButtonNum;j++) 
				Value[m_ButtonNum*5+j]=V6[j];

			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			pFIDI1->Next();pFIDI2->Next();pFIDI3->Next();
			pFIDI4->Next();pFIDI5->Next();pFIDI6->Next();
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) break;

		}
		if(V1) delete V1;
		if(V2) delete V2;
		if(V3) delete V3;
		if(V4) delete V4;
		if(V5) delete V5;
		if(V6) delete V6;
		delete pGuage;
	}
	delete Value;delete pFIDW;
	delete pFIDI1;delete pFIDI2;delete pFIDI3;
	delete pFIDI4;delete pFIDI5;delete pFIDI6;
	return TRUE;
}

//将进行电扣深度校正和极板深度校正后的数据P1GPT、P2GPT、P3GPT...
//合并为条阵列曲线DATA
BOOL CDataUnite::StarIIUnitData()
{
	CString PName[10];int n;
	for(n=0;n<m_PadNum;n++)
		PName[n].Format (".P%iGPT",n);
	CFIDIO* pFIDI1 = new CFIDIO(m_szBufPath+PName[0]);
	if(!pFIDI1->Open())
	{
		AfxMessageBox(PName[0]+" 曲线打开失败    ");
		delete pFIDI1;return FALSE;
	}
	FIDCONTENT* Cont =pFIDI1->GetContent(2);
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_FLOAT,4,6,0,0.f,0.f,0.f},
							   {"DATA00","1/ohm",REPR_FLOAT,4,144,144,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;

	m_DataNum=m_PadNum*m_ButtonNum;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min= (float)m_PadNum;                         
	m_Content[1].max=(float)m_ButtonNum; 
	m_Content[1].rlev =2.54f;                                  

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;           
	m_Content[2].min=Cont->min;                  
	m_Content[2].max=Cont->max;               
	strcpy(m_Content[2].unit,Cont->unit);   
	m_Content[2].rlev =2.54f;               

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CString ss=m_szBufPath+".DATA00";
	CFIDIO* pFIDW = new CFIDIO(ss,&m_Index);
	pFIDW->Close();
	delete pFIDW;
	CFIDIO* pFIDI2 = new CFIDIO(m_szBufPath+PName[1]);
	if(!pFIDI2->Open())
	{
		AfxMessageBox(PName[1]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;
		return FALSE;
	}
	CFIDIO* pFIDI3 = new CFIDIO(m_szBufPath+PName[2]);
	if(!pFIDI3->Open())
	{
		AfxMessageBox(PName[2]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		return FALSE;
	}
	CFIDIO* pFIDI4 = new CFIDIO(m_szBufPath+PName[3]);
	if(!pFIDI4->Open())
	{
		AfxMessageBox(PName[3]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;
		return FALSE;
	}
	CFIDIO* pFIDI5 = new CFIDIO(m_szBufPath+PName[4]);
	if(!pFIDI5->Open())
	{
		AfxMessageBox(PName[4]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;
		return FALSE;
	}
	CFIDIO* pFIDI6 = new CFIDIO(m_szBufPath+PName[5]);
	if(!pFIDI6->Open())
	{
		AfxMessageBox(PName[5]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;delete pFIDI6;
		return FALSE;
	}
	float *Value=new float[m_DataNum+10];
	int i,j;
	pFIDW = new CFIDIO(ss);
	if(pFIDW->Open())
	{	
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->SetTitle("极板数据合并...");
		pGuage->ShowWindow(SW_SHOW);	
		float dep=m_StProDep;

		pFIDW->SeekToTop();
		pFIDI1->SeekToDepth(m_StProDep);
		pFIDI2->SeekToDepth(m_StProDep);
		pFIDI3->SeekToDepth(m_StProDep);
		pFIDI4->SeekToDepth(m_StProDep);
		pFIDI5->SeekToDepth(m_StProDep);
		pFIDI6->SeekToDepth(m_StProDep);		
		float *V1,*V2,*V3,*V4,*V5,*V6;
		V1=new float[m_ButtonNum];
		V2=new float[m_ButtonNum];
		V3=new float[m_ButtonNum];
		V4=new float[m_ButtonNum];
		V5=new float[m_ButtonNum];
		V6=new float[m_ButtonNum];
		i=0;
		while(i<RecordNum)
		{		
			pFIDI1->Read(3,m_ButtonNum,V1);pFIDI2->Read(3,m_ButtonNum,V2);
			pFIDI3->Read(3,m_ButtonNum,V3);pFIDI4->Read(3,m_ButtonNum,V4);
			pFIDI5->Read(3,m_ButtonNum,V5);pFIDI6->Read(3,m_ButtonNum,V6);
			for(j=0;j<m_ButtonNum;j++) Value[j]=V1[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum+j]=V2[j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*2+j]=V3[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*3+j]=V4[j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*4+j]=V5[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*5+j]=V6[j];
			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			pFIDI1->Next();pFIDI2->Next();pFIDI3->Next();
			pFIDI4->Next();pFIDI5->Next();pFIDI6->Next();
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) break;
		}
		if(V1) delete V1;
		if(V2) delete V2;
		if(V3) delete V3;
		if(V4) delete V4;
		if(V5) delete V5;
		if(V6) delete V6;
		delete pGuage;
	}
	delete Value;delete pFIDW;
	delete pFIDI1;delete pFIDI2;delete pFIDI3;
	delete pFIDI4;delete pFIDI5;delete pFIDI6;
	return TRUE;
}

BOOL CDataUnite::FMI4UnitData()
{
	CString PName[10];int n;
	for(n=0;n<m_PadNum;n++)
		PName[n].Format (".P%iGPT",n);
	CFIDIO* pFIDI1 = new CFIDIO(m_szBufPath+PName[0]);
	if(!pFIDI1->Open())
	{
		AfxMessageBox(PName[0]+" 曲线打开失败    ");
		delete pFIDI1;return FALSE;
	}
	FIDCONTENT* Cont =pFIDI1->GetContent(2);
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_FLOAT,4,6,0,0.f,0.f,0.f},
							   {"DATA00","mmho",REPR_FLOAT,4,144,144,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;

	m_DataNum=m_PadNum*m_ButtonNum;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min= (float)m_PadNum;                         
	m_Content[1].max=(float)m_ButtonNum; 
	m_Content[1].rlev =2.54f;                                  

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;           
	m_Content[2].min=Cont->min;                  
	m_Content[2].max=Cont->max;               
	strcpy(m_Content[2].unit,Cont->unit); 
	m_Content[2].rlev =2.54f;               

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CString ss=m_szBufPath+".DATA00";
	CFIDIO* pFIDW = new CFIDIO(ss,&m_Index);
	pFIDW->Close();
	delete pFIDW;
	CFIDIO* pFIDI2 = new CFIDIO(m_szBufPath+PName[1]);
	if(!pFIDI2->Open())
	{
		AfxMessageBox(PName[1]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;
		return FALSE;
	}
	CFIDIO* pFIDI3 = new CFIDIO(m_szBufPath+PName[2]);
	if(!pFIDI3->Open())
	{
		AfxMessageBox(PName[2]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		return FALSE;
	}
	CFIDIO* pFIDI4 = new CFIDIO(m_szBufPath+PName[3]);
	if(!pFIDI4->Open())
	{
		AfxMessageBox(PName[3]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;
		return FALSE;
	}
	float *Value=new float[m_DataNum+10];
	int i,j;
	pFIDW = new CFIDIO(ss);
	if(pFIDW->Open())
	{		
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->SetTitle("极板数据合并...");
		pGuage->ShowWindow(SW_SHOW);	
		float dep=m_StProDep;

		pFIDW->SeekToTop();
		pFIDI1->SeekToDepth(m_StProDep);
		pFIDI2->SeekToDepth(m_StProDep);
		pFIDI3->SeekToDepth(m_StProDep);
		pFIDI4->SeekToDepth(m_StProDep);
		float V1[24],V2[24],V3[24],V4[24];
		i=0;
		while(i<RecordNum)
		{		
			pFIDI1->Read(3,m_ButtonNum,V1);pFIDI2->Read(3,m_ButtonNum,V2);
			pFIDI3->Read(3,m_ButtonNum,V3);pFIDI4->Read(3,m_ButtonNum,V4);
			for(j=0;j<m_ButtonNum;j++) Value[j]=V1[23-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum+j]=V2[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*2+j]=V3[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*3+j]=V4[m_ButtonNum-1-j];
			
			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			pFIDI1->Next();pFIDI2->Next();
			pFIDI3->Next();pFIDI4->Next();
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) break;
		}
		delete pGuage;
	}
	delete Value;delete pFIDW;
	delete pFIDI1;delete pFIDI2;delete pFIDI3;delete pFIDI4;
	return TRUE;
}

BOOL CDataUnite::FMI8UnitData()
{
	CString PName[10];int n;
	for(n=0;n<m_PadNum;n++)
		PName[n].Format (".P%iGPT",n);
	CFIDIO* pFIDI1 = new CFIDIO(m_szBufPath+PName[0]);
	if(!pFIDI1->Open())
	{
		AfxMessageBox(PName[0]+" 曲线打开失败    ");
		delete pFIDI1;return FALSE;
	}
	FIDCONTENT* Cont =pFIDI1->GetContent(2);
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_FLOAT,4,6,0,0.f,0.f,0.f},
							   {"DATA00","mmho",REPR_FLOAT,4,144,144,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;

	m_DataNum=m_PadNum*m_ButtonNum;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min= (float)m_PadNum;                         
	m_Content[1].max=(float)m_ButtonNum; 
	m_Content[1].rlev =2.54f;                                  

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;           
	m_Content[2].min=Cont->min;                  
	m_Content[2].max=Cont->max;               
	strcpy(m_Content[2].unit,Cont->unit);  
	m_Content[2].rlev =2.54f;   

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CString ss=m_szBufPath+".DATA00";
	CFIDIO* pFIDW = new CFIDIO(ss,&m_Index);
	pFIDW->Close();
	delete pFIDW;
	CFIDIO* pFIDI2 = new CFIDIO(m_szBufPath+PName[1]);
	if(!pFIDI2->Open())
	{
		AfxMessageBox(PName[1]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;
		return FALSE;
	}
	CFIDIO* pFIDI3 = new CFIDIO(m_szBufPath+PName[2]);
	if(!pFIDI3->Open())
	{
		AfxMessageBox(PName[2]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		return FALSE;
	}
	CFIDIO* pFIDI4 = new CFIDIO(m_szBufPath+PName[3]);
	if(!pFIDI4->Open())
	{
		AfxMessageBox(PName[3]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;
		return FALSE;
	}
	CFIDIO* pFIDI5 = new CFIDIO(m_szBufPath+PName[4]);
	if(!pFIDI5->Open())
	{
		AfxMessageBox(PName[4]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;
		return FALSE;
	}
	CFIDIO* pFIDI6 = new CFIDIO(m_szBufPath+PName[5]);
	if(!pFIDI6->Open())
	{
		AfxMessageBox(PName[5]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;delete pFIDI6;
		return FALSE;
	}
	CFIDIO* pFIDI7 = new CFIDIO(m_szBufPath+PName[6]);
	if(!pFIDI7->Open())
	{
		AfxMessageBox(PName[6]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;delete pFIDI6;delete pFIDI7;
		return FALSE;
	}
	CFIDIO* pFIDI8 = new CFIDIO(m_szBufPath+PName[7]);
	if(!pFIDI8->Open())
	{
		AfxMessageBox(PName[7]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;delete pFIDI6;delete pFIDI7;delete pFIDI8;
		return FALSE;
	}
	float *Value=new float[m_DataNum+10];
	int i,j;
	pFIDW = new CFIDIO(ss);
	if(pFIDW->Open())
	{		
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->SetTitle("极板数据合并...");
		pGuage->ShowWindow(SW_SHOW);	
		float dep=m_StProDep;

		pFIDW->SeekToTop();
		pFIDI1->SeekToDepth(m_StProDep);
		pFIDI2->SeekToDepth(m_StProDep);
		pFIDI3->SeekToDepth(m_StProDep);
		pFIDI4->SeekToDepth(m_StProDep);
		pFIDI5->SeekToDepth(m_StProDep);
		pFIDI6->SeekToDepth(m_StProDep);
		pFIDI7->SeekToDepth(m_StProDep);
		pFIDI8->SeekToDepth(m_StProDep);		
		float V1[24],V2[24],V3[24],V4[24],V5[24],V6[24],V7[24],V8[24];
		i=0;
		while(i<RecordNum)
		{		
			pFIDI1->Read(3,m_ButtonNum,V1);pFIDI2->Read(3,m_ButtonNum,V2);
			pFIDI3->Read(3,m_ButtonNum,V3);pFIDI4->Read(3,m_ButtonNum,V4);
			pFIDI5->Read(3,m_ButtonNum,V5);pFIDI6->Read(3,m_ButtonNum,V6);
			pFIDI7->Read(3,m_ButtonNum,V7);pFIDI8->Read(3,m_ButtonNum,V8);
			for(j=0;j<m_ButtonNum;j++) Value[j]=V1[23-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum+j]=V2[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*2+j]=V3[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*3+j]=V4[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*4+j]=V5[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*5+j]=V6[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*6+j]=V7[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*7+j]=V8[m_ButtonNum-1-j];			
			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			pFIDI1->Next();pFIDI2->Next();pFIDI3->Next();
			pFIDI4->Next();pFIDI5->Next();pFIDI6->Next();
			pFIDI7->Next();pFIDI8->Next();
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) break;
		}
		delete pGuage;
	}
	delete Value;delete pFIDW;
	delete pFIDI1;delete pFIDI2;delete pFIDI3;
	delete pFIDI4;delete pFIDI5;delete pFIDI6;delete pFIDI7;delete pFIDI8;
	return TRUE;
}

BOOL CDataUnite::CMIUnitData()
{
	CString PName[10];int n;
	for(n=0;n<m_PadNum;n++)
		PName[n].Format (".P%iGPT",n);
	CFIDIO* pFIDI1 = new CFIDIO(m_szBufPath+PName[0]);
	if(!pFIDI1->Open())
	{
		AfxMessageBox(PName[0]+" 曲线打开失败    ");
		delete pFIDI1;return FALSE;
	}
	FIDCONTENT* Cont =pFIDI1->GetContent(2);
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_FLOAT,4,6,0,0.f,0.f,0.f},
							   {"DATA00","mmho",REPR_FLOAT,4,144,144,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;

	m_DataNum=m_PadNum*m_ButtonNum;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min= (float)m_PadNum;                         
	m_Content[1].max=(float)m_ButtonNum; 
	m_Content[1].rlev =2.54f;                                  

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;           
	m_Content[2].min=Cont->min;                  
	m_Content[2].max=Cont->max;               
	strcpy(m_Content[2].unit,Cont->unit);  
	m_Content[2].rlev =2.54f;   

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CString ss=m_szBufPath+".DATA00";
	CFIDIO* pFIDW = new CFIDIO(ss,&m_Index);
	pFIDW->Close();
	delete pFIDW;
	CFIDIO* pFIDI2 = new CFIDIO(m_szBufPath+PName[1]);
	if(!pFIDI2->Open())
	{
		AfxMessageBox(PName[1]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;
		return FALSE;
	}
	CFIDIO* pFIDI3 = new CFIDIO(m_szBufPath+PName[2]);
	if(!pFIDI3->Open())
	{
		AfxMessageBox(PName[2]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		return FALSE;
	}
	CFIDIO* pFIDI4 = new CFIDIO(m_szBufPath+PName[3]);
	if(!pFIDI4->Open())
	{
		AfxMessageBox(PName[3]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;
		return FALSE;
	}
	CFIDIO* pFIDI5 = new CFIDIO(m_szBufPath+PName[4]);
	if(!pFIDI5->Open())
	{
		AfxMessageBox(PName[4]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;
		return FALSE;
	}
	CFIDIO* pFIDI6 = new CFIDIO(m_szBufPath+PName[5]);
	if(!pFIDI6->Open())
	{
		AfxMessageBox(PName[5]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;delete pFIDI6;
		return FALSE;
	}
	CFIDIO* pFIDI7 = new CFIDIO(m_szBufPath+PName[6]);
	if(!pFIDI7->Open())
	{
		AfxMessageBox(PName[6]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;delete pFIDI6;delete pFIDI7;
		return FALSE;
	}
	CFIDIO* pFIDI8 = new CFIDIO(m_szBufPath+PName[7]);
	if(!pFIDI8->Open())
	{
		AfxMessageBox(PName[7]+" 曲线打开失败    ");
		delete pFIDI1;delete pFIDI2;delete pFIDI3;
		delete pFIDI4;delete pFIDI5;delete pFIDI6;delete pFIDI7;delete pFIDI8;
		return FALSE;
	}
	float *Value=new float[m_DataNum+10];
	int i,j;
	pFIDW = new CFIDIO(ss);
	if(pFIDW->Open())
	{		
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->SetTitle("极板数据合并...");
		pGuage->ShowWindow(SW_SHOW);	
		float dep=m_StProDep;

		pFIDW->SeekToTop();
		pFIDI1->SeekToDepth(m_StProDep);
		pFIDI2->SeekToDepth(m_StProDep);
		pFIDI3->SeekToDepth(m_StProDep);
		pFIDI4->SeekToDepth(m_StProDep);
		pFIDI5->SeekToDepth(m_StProDep);
		pFIDI6->SeekToDepth(m_StProDep);
		pFIDI7->SeekToDepth(m_StProDep);
		pFIDI8->SeekToDepth(m_StProDep);		
		float V1[24],V2[24],V3[24],V4[24],V5[24],V6[24],V7[24],V8[24];
		i=0;
		while(i<RecordNum)
		{		
			pFIDI1->Read(3,m_ButtonNum,V1);pFIDI2->Read(3,m_ButtonNum,V2);
			pFIDI3->Read(3,m_ButtonNum,V3);pFIDI4->Read(3,m_ButtonNum,V4);
			pFIDI5->Read(3,m_ButtonNum,V5);pFIDI6->Read(3,m_ButtonNum,V6);
			pFIDI7->Read(3,m_ButtonNum,V7);pFIDI8->Read(3,m_ButtonNum,V8);
			for(j=0;j<m_ButtonNum;j++) Value[j]=V1[23-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum+j]=V2[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*2+j]=V3[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*3+j]=V4[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*4+j]=V5[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*5+j]=V6[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*6+j]=V7[m_ButtonNum-1-j];
			for(j=0;j<m_ButtonNum;j++) Value[m_ButtonNum*7+j]=V8[m_ButtonNum-1-j];			
			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			pFIDI1->Next();pFIDI2->Next();pFIDI3->Next();
			pFIDI4->Next();pFIDI5->Next();pFIDI6->Next();
			pFIDI7->Next();pFIDI8->Next();
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) break;
		}
		delete pGuage;
	}
	delete Value;delete pFIDW;
	delete pFIDI1;delete pFIDI2;delete pFIDI3;
	delete pFIDI4;delete pFIDI5;delete pFIDI6;delete pFIDI7;delete pFIDI8;
	return TRUE;
}

