// ImageCreat.cpp: implementation of the CImageCreat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "ImageCreate.h"
#include "FlowNode.h"
#include "FlowChartDoc.h"
#include "DataUnite.h"
#include "BackupData.h"
#include "ImageCreatDlg.h"
#include "FIDio.h"
#include "Guage.h"
#include "Clustering.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CImageCreat,CFlowNode, 0)
CImageCreat::CImageCreat()
{
	m_pDoc=NULL;
}

CImageCreat::~CImageCreat()
{

}

void CImageCreat::OnWellRefresh() 
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

CImageCreat::CImageCreat(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_bFilter=TRUE;
	m_bBalance=FALSE;  
	m_nClass=8.f;                        //�����СƵ��ֵ8-512
	m_PadMin=0.f;						 //������СƵ��ֵ1-30
	m_WinLongth = 0.3048f;               //1Ӣ��
	m_bLogScal=FALSE;
	m_bRepair=FALSE;
	m_radius=0.15f;
	m_bPadPro=TRUE;
	m_bButPro=TRUE;
	m_bCompose=FALSE;					 //ͼ��ϳ�	
}

BOOL CImageCreat::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();

	if(!DataCopy("DATAPS","DATAST"))
		return FALSE;
	m_bResult=TRUE;
	CString Name="DATAST";
	if(!m_pDoc->m_bAunAll)
	{			
		CImageCreatDlg Dlg(this);
		if(Dlg.DoModal()!=IDOK)
			return FALSE;
	}
	//��̬�̶ȴ���
	m_bResult=DataFreqScale(m_szBufPath,"DATAST",m_PadMin);
	if(m_bResult&&m_bButPro)		//��۾���
		m_bResult=AutoButBalancePro("DATAST");
	if(m_bResult&&m_bPadPro)		//�������
		m_bResult=AutoPadBalancePro("DATAST");
	//if(m_bResult)					//һ��ͼ��ϳ�
	//	m_bResult=AllDataExtentPro(m_szBufPath,"DATAST");
	if(m_bResult&&m_bCompose)		//����ͼ��ϳ�
		m_bResult=ImageCompose(m_szBufPath,"DATAST");
	return m_bResult;
}
/*
 *����Զ��̶�
 *�Զ���Ѱÿ����۵������Сֵ
 *��ÿ����۴���̶ȴ���
 *��ÿ����۹�һ��Ϊ0--1024�����Է�Χ
*/
BOOL CImageCreat::DataFreqScale(CString Path,CString Name,float PadMin)
{	
	char s[256],fn[256];
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{	
		int RecNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);	
		float *Value=new float[m_DataNum];
		int Block=m_DataNum*4;
		
		//�������ָʾ��
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());	
		pGuage->SetTitle(Name+" ��̬�̶�(���ݲ�ѯ)...");
		pGuage->ShowWindow(SW_SHOW);

		int i,j,k,NNN,pos;
		int gNum=2048;//��ֱ��ͼ�������
		int **freq=new int *[m_DataNum];
		for(i=0;i<m_DataNum;i++)
			freq[i]=new int[gNum];
		float *Min=new float[m_DataNum];
		float *Max=new float[m_DataNum];
		int Maxfq;
		float delta,data;
		for(i=0;i<m_DataNum;i++)
		{
			Min[i]=FLT_MAX;
			Max[i]=FLT_MIN;
		}
		//Ѱ��ȫ����ÿ����������Сֵ
		fp.Seek(0L,CFile::begin);
		for(k=0;k<RecNum;k++)
		{
			fp.Read(Value,Block);
			for(i=0;i<m_DataNum;i++)
			{
				if(Value[i]>Max[i]) 
					Max[i]=Value[i];
				if(Value[i]<Min[i])
					Min[i]=Value[i];
			}
			pGuage->SetDepth(m_StProDep+m_Rlev*k);
		}

		for(i=0;i<m_DataNum;i++)
		{
			if(Min[i]<0.f)
				Min[i]=0.f;
			if(Max[i]>1024.f)
				Max[i]=1024.f;
		}
		//��ȫ����ÿ����۵�Ƶ��ֱ��ͼ
		for(j=0;j<m_DataNum;j++)
			for(i=0;i<gNum;i++)
				freq[j][i]=0;
		pGuage->SetTitle(Name+" ��̬�̶�(Ƶ��ͳ��)...");
		fp.Seek(0L,CFile::begin);
		for(k=0;k<RecNum;k++)
		{
			fp.Read(Value,Block);
			for(j=0;j<m_DataNum;j++)
			{
				pos=(int)((Value[j]-Min[j])/(Max[j]-Min[j])*gNum);
				if(pos>-1&&pos<gNum) 
					freq[j][pos]++;
			}
			pGuage->SetDepth(m_StProDep+m_Rlev*k);
		}
		//����ÿ����۵������Сֵ
		for(j=0;j<m_DataNum;j++)
		{
			Maxfq=1;
			for(i=0;i<gNum;i++)
				if(freq[j][i]>Maxfq)
					Maxfq=freq[j][i];

			delta=Max[j]-Min[j];
			data=Min[j];
			for(i=0;i<gNum;i++)
				freq[j][i]=freq[j][i]*100/Maxfq;
			//Ѱ�����ֵ
			NNN=0;//����3����������
			for(i=gNum-1;i>0;i--)
			{
				if(freq[j][i]>PadMin)
					NNN++;
				else
					NNN=0;
				if(NNN>2)
				{
					Max[j]=data+i*delta/gNum;
					break;
				}
			}
			//Ѱ����Сֵ
			NNN=0;
			for(i=0;i<gNum;i++)
			{
				if(freq[j][i]>PadMin)
					NNN++;
				else
					NNN=0;
				if(NNN>2)
				{
					Min[j]=data+i*delta/gNum;
					break;
				}
			}
		}
		//�����е�ۿ̶�ֵȡ���ڼ��ƽ��ֵ
		for(j=1;j<m_DataNum;j++)
		{
			if(Max[j]<=Min[j])
				Max[j]=Min[j]+1024.f;
			else
			{
				Max[j]=(Max[j]+Max[j-1])/2.f;
				Min[j]=(Min[j]+Min[j-1])/2.f;
			}
		}
		//����1�ŵ�ۿ̶�ֵ
		if(Max[0]<=Min[0])
			Max[0]=Min[0]+1024.f;
		else
		{
			Max[0]=(Max[0]+Max[m_DataNum-1])/2.f;
			Min[0]=(Min[0]+Min[m_DataNum-1])/2.f;
		}
		//��ÿ����۽������¿̶�
		pGuage->SetTitle(Name+" ��̬�̶�(���ݴ���)...");
		fp.Seek(0L,CFile::begin);
		for(k=0;k<RecNum;k++)
		{
			fp.Read(Value,Block);
			for(j=0;j<m_DataNum;j++)
			{
				if(Value[j]<Min[j])
					Value[j]=Min[j];
				if(Value[j]>Max[j])
					Value[j]=Max[j];
				Value[j]=(Value[j]-Min[j])/(Max[j]-Min[j])*1024.f;
			}
			pGuage->SetDepth(m_StProDep+m_Rlev*k);
			fp.Seek(k*Block,CFile::begin);
			fp.Write(Value,Block);
		}
		
		delete Value;
		for(i=0;i<m_DataNum;i++)
			delete freq[i];
		delete freq;
		delete Max;delete Min;delete pGuage;
		fp.Close ();
		return TRUE;
	}
	return FALSE;
}
//�����Զ�����
BOOL CImageCreat::AutoPadBalancePro(CString Name)
{
	int PadNum=m_PadNum;
	int ButtonNum=m_ButtonNum;
	int RecNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	int nWin=120,nSav=1;//3Ӣ��
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	BOOL bBreak=FALSE;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int Block=m_DataNum*nWin*4;
		float *Value=new float[m_DataNum*nWin];

		int i,j,k,m;
		//�����׼ƫ��
		float AllQ,AllAver,tmp;
		float *pPadQ=new float[PadNum];
		float *pPadAver=new float[PadNum];

		//�������ָʾ��
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());	
		pGuage->SetTitle(Name+" ������⴦��...");
		pGuage->ShowWindow(SW_SHOW);

		m=0;
		int nWin0=nWin;
		while(m<RecNum-nWin)
		{
			//�������һ������
			if(m+nWin>=RecNum)
				nWin=RecNum-m-1;

			fp.Seek(m*m_DataNum*4L,CFile::begin);
			fp.Read(Value,Block);
			for(i=0;i<PadNum;i++)
			{
				pPadAver[i]=0.f;
				for(k=0;k<nWin;k++)
					for(j=0;j<ButtonNum;j++)
						pPadAver[i]+=Value[k*m_DataNum+i*ButtonNum+j];
			}
			//����ÿ�������ֵ
			for(i=0;i<PadNum;i++)
				pPadAver[i]/=ButtonNum*nWin;
			
			//�������м����ֵ
			AllAver=0.f;
			for(i=0;i<PadNum;i++)
				AllAver+=pPadAver[i];
			AllAver/=PadNum;
			
			///////////////////////////////////////////////////////////////////////////////
			//�������м����׼��
			AllQ=0.f;
			for(k=0;k<nWin;k++)
				for(i=0;i<m_DataNum;i++)
				{
					tmp=(Value[k*m_DataNum+i]-AllAver)*(Value[k*m_DataNum+i]-AllAver);
					AllQ+=tmp;
				}		
			AllQ=sqrt(AllQ/(m_DataNum*nWin));

			///////////////////////////////////////////////////////////////////////////////
			//����ÿ���ļ����׼��
			for(i=0;i<PadNum;i++)
			{
				pPadQ[i]=0.f;
				for(k=0;k<nWin;k++)
					for(j=0;j<ButtonNum;j++)
					{
						tmp=(Value[k*m_DataNum+i*ButtonNum+j]-pPadAver[i])*(Value[k*m_DataNum+i*ButtonNum+j]-pPadAver[i]);
						pPadQ[i]+=tmp;
					}
			}
			for(i=0;i<PadNum;i++)
				pPadQ[i]=sqrt(pPadQ[i]/(ButtonNum*nWin));
				
			//����Զ����⴦��
			///////////////////////////////////////////////////////////////////////////////////////
			
			for(i=0;i<PadNum;i++)
			{
				if(pPadQ[i]>0.f)
				{
					for(k=0;k<nWin;k++)
						for(j=0;j<ButtonNum;j++)
							Value[k*m_DataNum+i*ButtonNum+j]=(AllQ/pPadQ[i])*(Value[k*m_DataNum+i*ButtonNum+j]-pPadAver[i])+AllAver;
				}
			}
			if(nWin0>nWin)
			{
				fp.Seek(m_DataNum*m*4L,CFile::begin);
				fp.Write(Value,Block);
				break;
			}
			fp.Seek(m_DataNum*m*4L,CFile::begin);
			fp.Write(Value,m_DataNum*nSav*4);

			m+=nSav;
			pGuage->SetDepth(m_StProDep+m_Rlev*m);
			if(pGuage->CheckCancelButton())
			{
				bBreak=TRUE;
				break;
			}
		}
		delete pGuage;delete Value;
		delete pPadQ;delete pPadAver;
	}
	else bBreak=TRUE;
	fp.Close ();
	return !bBreak;
}
//����Զ�����
//���ÿ��������й淶������
//�ܱ�׼��Ϊ��һ����
//�ֲ���׼��Ϊ��һ���
BOOL CImageCreat::AutoButBalancePro(CString Name)
{
	int PadNum=m_PadNum;
	int ButNum=m_ButtonNum;
	int RecNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	int nWin=120,nSav=1;//3Ӣ��
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	BOOL bBreak=FALSE;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		DWORD Block=m_DataNum*nWin*4;
		float *Value=new float[m_DataNum*nWin];

		int i,k,m,n;
		//��۱�׼ƫ���ֵ
		float AllQ,AllAver,tmp;
		//�����׼ƫ���ֵ
		float *pButQ=new float[ButNum];
		float *pButAver=new float[ButNum];
		
		//�������ָʾ��
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());	
		pGuage->SetTitle(Name+" ��۾��⴦��...");
		pGuage->ShowWindow(SW_SHOW);
		//
		//�����ۼ����ֵ��׼ƫ��
		///////////////////////////////////////////////////////////////////////////////
		//
		m=0;
		int nWin0=nWin;
		while(m<RecNum)
		{
			//�������һ������
			if(m+nWin>=RecNum)
				nWin=RecNum-m-1;

			fp.Seek(m*m_DataNum*4L,CFile::begin);
			fp.Read(Value,Block);
			for(n=0;n<PadNum;n++)
			{
				//����1������ÿ����۴����ڵĺ�ֵ
				for(i=0;i<ButNum;i++)
				{
					k=0;
					pButAver[i]=0.f;
					for(k=0;k<nWin;k++)
						pButAver[i]+=Value[k*m_DataNum+n*ButNum+i];
				}
				//����ÿ����۾�ֵ
				for(i=0;i<ButNum;i++)
					pButAver[i]/=nWin;
				
				//���㼫���ֵ
				AllAver=0.f;
				for(i=0;i<ButNum;i++)
					AllAver+=pButAver[i];
				AllAver/=ButNum;
				
				///////////////////////////////////////////////////////////////////////////////
				//�����׼ƫ��
				//����1���������е��ƫ��ƽ����
				AllQ=0.f;
				for(k=0;k<nWin;k++)
					for(i=0;i<ButNum;i++)
					{
						tmp=(Value[k*m_DataNum+n*ButNum+i]-AllAver)*(Value[k*m_DataNum+n*ButNum+i]-AllAver);
						AllQ+=tmp;	
					}
				//���㼫���׼��
				AllQ=sqrt(AllQ/(ButNum*nWin));
				//����ÿ����۴����ڵ�ƫ��ƽ����
				for(i=0;i<ButNum;i++)
				{
					pButQ[i]=0.f;
					for(k=0;k<nWin;k++)
					{
						tmp=(Value[k*m_DataNum+n*ButNum+i]-pButAver[i])*(Value[k*m_DataNum+n*ButNum+i]-pButAver[i]);
						pButQ[i]+=tmp;			
					}
				}
				//�����۱�׼��
				for(i=0;i<ButNum;i++)
					pButQ[i]=sqrt(pButQ[i]/nWin);
					
				//����Զ����⴦��
				///////////////////////////////////////////////////////////////////////////////////////
				for(i=0;i<ButNum;i++)
				{
					if(pButQ[i]>0.f)
					{
						for(k=0;k<nWin;k++)
							Value[k*m_DataNum+n*ButNum+i]=(AllQ/pButQ[i])*(Value[k*m_DataNum+n*ButNum+i]-pButAver[i])+AllAver;
					}
				}
			}
			if(nWin0>nWin)
			{
				fp.Seek(m_DataNum*m*4L,CFile::begin);
				fp.Write(Value,Block);
				break;
			}
			fp.Seek(m_DataNum*m*4L,CFile::begin);
			fp.Write(Value,m_DataNum*nSav*4);

			m+=nSav;
			pGuage->SetDepth(m_StProDep+m_Rlev*m);
			if(pGuage->CheckCancelButton())
			{
				bBreak=TRUE;
				break;
			}
		}
		
		delete pGuage;delete Value;
		delete pButQ;delete pButAver;
	}
	else bBreak=TRUE;
	fp.Close ();
	return !bBreak;
}

void CImageCreat::InitPara(CFlowChartDoc* pDoc)
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
				m_Rm=((CBackupData*)pObj)->m_Rm;
				m_ButtonMid=12;
				if(m_szSeries=="EMI")
					m_ButtonMid=13;
				m_DataNum=m_PadNum*m_ButtonNum;
			}
		}
	}
	m_pDoc=pDoc;
	if(m_szSeries=="CBIL"||m_szSeries=="CAST"
		||m_szSeries=="UBI"||m_szSeries=="UBS"||m_szSeries=="AMK2000")
		m_bLogScal=FALSE;
	else
		m_bLogScal=TRUE;
}

void CImageCreat::Serialize(CArchive& ar)
{
	m_Ver=1.1f;
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_bFilter;		//δʹ��
		ar<<m_bBalance;		//δʹ��
		ar<<m_nClass;
		ar<<m_PadMin;
		ar<<m_WinLongth;	//δʹ��
		ar<<m_bCompose;
		ar<<m_bPadPro;
		ar<<m_radius;
		ar<<m_bButPro;
	}
	else
	{
		ar>>m_bFilter;
		ar>>m_bBalance;  
		ar>>m_nClass;
		ar>>m_PadMin;
		ar>>m_WinLongth;
		ar>>m_bCompose;
		ar>>m_bPadPro;
		ar>>m_radius;
		if(m_Ver>1.f)
			ar>>m_bButPro;
	}
}
//ͼ���޸�����
void CImageCreat::DataRepair(CString Path,CString Name)
{
	float *fwf0,*fwf1,*fwf2;
	int j,k,RecordNum;
	char s[256],fn[256];
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');	
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		fwf0=new float[m_DataNum];
		fwf1=new float[m_DataNum];
		fwf2=new float[m_DataNum];
		int Block=m_DataNum*4;
		
		//�������ָʾ��
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle(Name+" ͼ���޸�...");
		float correction,ix,iy,ixx,ixy,iyy;
		
		k=0;
		fp.Seek (k*Block,CFile::begin);
		fp.Read (fwf0,Block);
		k++;
		fp.Seek (k*Block,CFile::begin);
		fp.Read (fwf1,Block);
		k++;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (fwf2,Block);
			for(j=1;j<m_DataNum-1;j++)
			{
				ix= (fwf1[j+1]-fwf1[j-1])/2.f;
				iy= (fwf2[j]-fwf0[j])/2.f;
				ixx= fwf1[j+1] - 2.f * fwf1[j] + fwf1[j-1];
				iyy= fwf2[j] - 2.f * fwf1[j]+ fwf0[j];
				ixy=(fwf2[j+1] + fwf0[j-1]-fwf2[j-1] - fwf0[j+1])/4.f;
				correction = ((1.f+iy*iy)*ixx - ix*iy*ixy + (1.f+ix*ix)*iyy)/(1.f+ix*ix+iy*iy);
				fwf1[j]=fwf1[j]+ m_radius * correction + 0.5f;
			}
			fp.Seek ((k-2)*Block,CFile::begin);
			fp.Write (fwf1,Block);
			memcpy(fwf0,fwf1,Block);
			memcpy(fwf1,fwf2,Block);
			k++;
			pGuage->SetDepth(m_StProDep+m_Rlev*k);
			if(pGuage->CheckCancelButton()) 
				break;
		}
		delete pGuage;
		delete fwf0;delete fwf1;delete fwf2;
	}
	fp.Close ();
}

BOOL CImageCreat::ImageCompose(CString Path,CString Name)
{	
	BOOL bBreak=FALSE;

	char s[256],fn[256];
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//�������ָʾ��
		CGuage* pGuage = new CGuage(0.f,m_DataNum*1.f);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle(Name+" ����ͼ��ϳ�...");

		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		float *Value=new float[m_DataNum*RecordNum];
		//��ȡ������������
		fp.Seek (0L,CFile::begin);
		fp.Read (Value,m_DataNum*RecordNum*4);

		//Ѱ��ȫ����ÿ����������Сֵ
		int i,j,k;
		float Min,Max,delta;

		// K-Means������ȡÿ����������Сֵ
		int kmk=(int)m_nClass;		//��������
		int kmp=2;					//���ݵ�ά��
		float **xMeans=new float *[kmp];
		for(i=0;i<kmp;i++)
			xMeans[i]=new float[RecordNum];

		float **cMeans = new float *[kmp];
		for(i=0;i<kmp;i++)
			cMeans[i]=new float[kmk];

		int *zMeans = new int[RecordNum];
		float *pWork = new float[RecordNum];

		for(j=0;j<RecordNum;j++)
			xMeans[1][j]=1.f;
		for(k=0;k<m_DataNum;k++)
		{
			for(j=0;j<RecordNum;j++)
				xMeans[0][j]=Value[m_DataNum*j+k];
			if(Kmeans(xMeans,kmp,RecordNum, kmk, cMeans, zMeans, pWork)==0)
			{
				//����δ�ܾ�������ݵ�
				Min=FLT_MAX;
				Max=FLT_MIN;
				for(j=0;j<kmk;j++)
				{
					if(cMeans[0][j]>Max) 
						Max=cMeans[0][j];
					if(cMeans[0][j]<Min)
						Min=cMeans[0][j];
				}
				
				if(Max>Min)
				{
					delta=Max-Min;
					Min-=delta/kmk;
					Max+=delta/kmk;
					delta=Max-Min;
					for(j=0;j<RecordNum;j++)
						Value[m_DataNum*j+k]=(Value[m_DataNum*j+k]-Min)/delta*1024.f;
				}
			}
			pGuage->SetDepth(k*1.f);
			if(pGuage->CheckCancelButton()) 
			{
				bBreak=TRUE;
				break;
			}
		}
		//����������������
		if(!bBreak)
		{
			fp.Seek (0L,CFile::begin);
			fp.Write(Value,m_DataNum*RecordNum*4);
		}

		for(i=0;i<kmp;i++)
			delete xMeans[i];
		delete xMeans;
		for(i=0;i<kmp;i++)
			delete cMeans[i];
		delete cMeans;
		delete zMeans;
		delete pWork;

		delete Value;
		delete pGuage;
	}
	else bBreak=TRUE;

	fp.Close ();
	return !bBreak;
}
//
//BOOL CImageCreat::CopyCurve(CString InName,CString OutName)
//{
//	BOOL bOK=FALSE;
//	CFIDIO* pFIDR = new CFIDIO(m_szBufPath+"."+InName);
//	if(pFIDR->Open())
//	{
//		if(pFIDR->Copy(m_szBufPath+"."+OutName)!=-1)
//			bOK=TRUE;
//	}
//	delete pFIDR;
//	return bOK;
//}

//������չ����
BOOL CImageCreat::AllDataExtentPro(CString Path,CString Name)
{
	int RecordNum;
	char s[256],fn[256];
	CFIDIO* pFIDI = new CFIDIO(Path+"."+Name);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}
	FIDCONTENT* Content=NULL;
	Content = pFIDI->GetContent(3);
	int DataNum=Content->nps;
	delete pFIDI;

	int Block=DataNum*4;
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bBreak=FALSE;
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//�������ָʾ��
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle(Name+" һ��ͼ��ϳ�(���ݲ�ѯ)...");
		float dep=m_StProDep;

		float *Value=new float[DataNum+10];
		int i,k,QMax,p;

		int freq[101];
		//Ѱ�Ҵ��ڷ�Χ�ڵ�Ƶ�����λ�õ�ֵ
		for(i=0;i<100;i++)
			freq[i]=0;
		k=0;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);
			for(i=0;i<DataNum;i++)
			{
				p=(int)(Value[i]/1024.f*100.f);
				if(p>-1&&p<100)
					freq[p]++;
			}
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton()) 
			{
				bBreak=TRUE;
				break;
			}
		}
		if(!bBreak)
		{
			QMax=0;p=0;
			for(i=0;i<100;i++)
			{
				if(freq[i]>QMax)
				{
					QMax=freq[i];
					p=i;
				}
			}
			for(i=0;i<100;i++)
				freq[i]=freq[i]/QMax*100;

			float Min=0.f,Max=0.f,xxx;
			if(p>97)
			{
				//Ѱ����Сֵ
				for(i=p;i>0;i--)
					if(freq[i]>0)
					{
						Min=i*10.24f;
						Max=1024.f;
					}
			}
			else if(p<3)
			{
				//Ѱ�����ֵ
				for(i=p;i<100;i++)
					if(freq[i]>0)
					{
						Max=i*10.24f;
						Min=0.f;
					}
			}
			else
			{
				//Ѱ�����ֵ
				for(i=p;i<100;i++)
				{
					if(freq[i]>0)
						Max=i*10.24f;
				}
				//Ѱ����Сֵ
				for(i=p;i>0;i--)
				{
					if(freq[i]>0)
						Min=i*10.24f;
				}
			}
			if(Max>0.f&&Max>Min)
			{
				xxx=Max-Min;
				k=0;
				pGuage->SetTitle(Name+" һ��ͼ��ϳ�(���ݴ���)...");
				while(k<RecordNum)
				{
					fp.Seek (k*Block,CFile::begin);
					fp.Read (Value,Block);
						
					for(i=0;i<DataNum;i++)
						Value[i]=(Value[i]-Min)/xxx*1024.f;
					fp.Seek (k*Block,CFile::begin);
					fp.Write (Value,Block);
					k++;
					dep=m_StProDep+m_Rlev*k;
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton()) 
					{
						bBreak=TRUE;
						break;
					}
				}
			}
		}
		delete Value;delete pGuage;
	}
	else bBreak=TRUE;
	fp.Close ();
	return !bBreak;
}
//�����ݽ����˲�����
BOOL CImageCreat::ImageFilterPro(CString Path,CString Name)
{	
	char s[256],fn[256];
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int RecNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		//�������ָʾ��
		CGuage* pGuage = new CGuage(0.f,m_DataNum*1.f+RecNum);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle(Name+" ͼ��ƽ���˲�...");
		float *Value=new float[m_DataNum*RecNum];
		//��ȡ������������
		fp.Seek (0L,CFile::begin);
		fp.Read (Value,m_DataNum*RecNum*4);

		//�����˲�
		int j,k;
		for(k=0;k<m_DataNum;k++)
		{
			for(j=1;j<RecNum-1;j++)
				Value[m_DataNum*j+k]=0.212f*Value[m_DataNum*(j-1)+k]+0.576f*Value[m_DataNum*j+k]+0.212f*Value[m_DataNum*(j+1)+k];
			pGuage->SetDepth(k*1.f);
		}
		//�����˲�
		for(j=0;j<RecNum;j++)
		{
			for(k=1;k<m_DataNum-1;k++)
				Value[m_DataNum*j+k]=0.212f*Value[m_DataNum*j+k-1]+0.576f*Value[m_DataNum*j+k]+0.212f*Value[m_DataNum*j+k+1];
			pGuage->SetDepth(m_DataNum*1.f+j);
		}
		//����������������
		fp.Seek (0L,CFile::begin);
		fp.Write(Value,m_DataNum*RecNum*4);
		delete Value; delete pGuage;
		fp.Close ();
		return TRUE;
	}	
	return FALSE;
}