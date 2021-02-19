// ImageAzCorrect.cpp: implementation of the CImageAzCorrect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "ImageAzCorrect.h"
#include "FlowNode.h"
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
IMPLEMENT_SERIAL(CImageAzCorrect,CFlowNode, 0)
CImageAzCorrect::CImageAzCorrect()
{
	m_pDoc=NULL;
}

CImageAzCorrect::~CImageAzCorrect()
{

}

CImageAzCorrect::CImageAzCorrect(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
}

void CImageAzCorrect::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
		ar<<m_szImageType;
	else
		ar>>m_szImageType;
}

BOOL CImageAzCorrect::Run()
{
	m_bResult=TRUE;
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	CString InName;
	if(m_szImageType=="ACIMG")
	{
		InName="AMPIMG";
		AzimuthCorret(InName);
		InName="TTIMG";
		AzimuthCorret(InName);
		return TRUE;
	}
	if(m_szImageType=="动态图像")
		InName="DYNMI";
	else if(m_szImageType=="静态图像")
		InName="STAMI";
	else 
		InName="ROIMG";
	AzimuthCorret(InName);
	return TRUE;
}

void CImageAzCorrect::InitPara(CFlowChartDoc* pDoc)
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

//进行方位校正
//同时处理I号极板中间偏移
BOOL CImageAzCorrect::AzimuthCorret(CString Name)
{
	CFIDIO* pFIDAZI=NULL;
	pFIDAZI = new CFIDIO(m_szBufPath+".P1AZGPT");
	if(!pFIDAZI->Open()) 
	{
		delete pFIDAZI; 
		return FALSE;
	}
	BOOL bUserBreak=FALSE;

	char s[256],fn[256];
	strcpy(s,m_szWellName);
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
		CString ss=Name+"图像方位校正...";
		pGuage->SetTitle(ss);
		pGuage->ShowWindow(SW_SHOW);
		float dep=m_StProDep;

		float Azi;int x,dk=0;
		int iPro=m_ButtonNum/2;
		float *OffValue;//处理I号极板中间偏移
		OffValue=new float[iPro+1];
		float *Value;
		Value=new float[2*Num+10];
		int k=0;
		pFIDAZI->SeekToDepth(m_StProDep);
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);	
			pFIDAZI->Read(2,1,&Azi);
			pFIDAZI->Next();
			if(Azi<0.f) Azi=0.f;
			if(Azi>360.f) Azi=360.f;
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