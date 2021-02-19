// GainRegress.cpp: implementation of the CGainRegress class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "FlowChartDoc.h"
#include "BackupData.h"
#include "GainRegress.h"
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
IMPLEMENT_SERIAL(CGainRegress,CFlowNode, 0)
CGainRegress::CGainRegress()
{
	m_pDoc=NULL;
}

CGainRegress::~CGainRegress()
{

}

CGainRegress::CGainRegress(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
}

void CGainRegress::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
}

void CGainRegress::InitPara(CFlowChartDoc* pDoc)
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
				m_szSeries=((CBackupData*)pObj)->m_szSeries;
				m_PadNum=((CBackupData*)pObj)->m_PadNum; 
				m_ButtonNum=((CBackupData*)pObj)->m_ButtonNum;	
				m_StProDep=((CBackupData*)pObj)->m_StProDep;
				m_EnProDep=((CBackupData*)pObj)->m_EnProDep;
				m_Rlev=((CBackupData*)pObj)->m_Rlev;
				m_Rm=((CBackupData*)pObj)->m_Rm;
			}
		}
	}
	m_pDoc=pDoc;
}

BOOL CGainRegress::Run()
{
	m_bResult=TRUE;
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(m_szSeries.Find("STAR")>-1)	
		GainRenew();
	return m_bResult;
}

BOOL CGainRegress::GainRenew()
{
	CString ss;
	CFIDIO* PadI= NULL;
	PadI=new CFIDIO(m_szBufPath+".GAINGPT");
	if(!PadI->Open())
	{
		delete PadI;
		return FALSE;
	}
	int RecordNum;
	char s[256],fn[256];
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	int Block=m_ButtonNum*4;
	int i,j,k;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	
	BOOL bUserBreak=FALSE;
	float dep;
	float *Value=NULL;
	Value=new float[m_ButtonNum+10];
	float *Gain=NULL;
	Gain=new float[m_PadNum+10];
	
	float *Value0=NULL;
	Value0=new float[m_ButtonNum+10];
	float *Gain0=NULL;
	Gain0=new float[m_PadNum+10];
	float A=1.131f;
	float B=4.107f;
	float C=34440.f;
	float D=2.2387f;
	float E=2.f;
	float F=1.78f;
	float G=2.f;
	float H=3.f;
	float kb,kg;
	float temp;
	for(i=0;i<m_PadNum;i++)
	{	
		ss.Format ("%i号极板电扣增益恢复...",i+1);
		pGuage->SetTitle(ss);
		dep=m_StProDep;
		strcpy(s,m_szBufPath);
		ss.Format (".P%iGPT",i);
		strcat(s,ss);
		CFIDIO::GetFileName(s,fn,'D');
		CFile fp;
		if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			k=0;
			dep=m_StProDep;
			while(k<RecordNum)
			{
				PadI->SeekToDepth(dep);
				PadI->Read(3,m_PadNum+2,Gain);
				fp.Seek (k*Block,CFile::begin);
				fp.Read (Value,Block);
				kb=C*D*(float)pow(E,Gain[m_PadNum]);
				kg=F*(float)pow(G,(Gain[m_PadNum+1]-H));	
				for(j=0;j<m_ButtonNum;j++)
				{				
					if(Value[j]!=0.f)
					{
						temp=(Gain[i]*kb)/(Value[j]*kg);
						temp=A/((float)log10(temp)-(float)log10(m_Rm)*(A-1.f)-B);
					//	temp=((float)log10(temp)-(float)log10(m_Rm)*(A-1.f)-B)/A;校正为电阻率
						Value[j]=pow(10.f,temp);
					}
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
			if(bUserBreak) break;
			k=4;
			PadI->SeekToDepth(m_StProDep);
			PadI->Read(3,m_PadNum+2,Gain0);
			while(k<RecordNum-4)
			{
				dep=m_StProDep+m_Rlev*k;
				pGuage->SetDepth(dep);
	
				PadI->SeekToDepth(dep);
				PadI->Read(3,m_PadNum+2,Gain);
				
				if(Gain[0]!=Gain0[0]||Gain[3]!=Gain0[3]
					||Gain[m_PadNum]!=Gain0[m_PadNum]
					||Gain[m_PadNum+1]!=Gain0[m_PadNum+1])
				{
					fp.Seek ((k-2)*Block,CFile::begin);
					fp.Read (Value0,Block);
					fp.Seek ((k+2)*Block,CFile::begin);
					fp.Read (Value,Block);
					for(j=0;j<m_ButtonNum;j++)
						Value[j]=(Value0[j]+Value[j])/2.f;
					fp.Seek (k*Block,CFile::begin);
					fp.Write (Value,Block);
					
					for(j=0;j<m_ButtonNum;j++)
						Value[j]=(Value0[j]+Value[j])/2.f;
					fp.Seek ((k-1)*Block,CFile::begin);
					fp.Write (Value,Block);

					fp.Seek (k*Block,CFile::begin);
					fp.Read (Value0,Block);
					fp.Seek ((k+2)*Block,CFile::begin);
					fp.Read (Value,Block);
					for(j=0;j<m_ButtonNum;j++)
						Value[j]=(Value0[j]+Value[j])/2.f;
					fp.Seek ((k+1)*Block,CFile::begin);
					fp.Write (Value,Block);
				}
			
				for(j=0;j<m_PadNum+2;j++)
					Gain0[j]=Gain[j];
				k++;
				if (pGuage->CheckCancelButton()) 
				{
					bUserBreak=TRUE;
					break;
				}
			}
		}
		else bUserBreak=TRUE;
		fp.Close ();
		if(bUserBreak) break;
	}
	delete pGuage;delete Value;
	delete Gain;delete PadI;
	delete Gain0;delete Value0;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}