// Skc98ToSlc716.cpp: implementation of the CSkc98ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Skc98ToSlc716.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkc98ToSlc716::CSkc98ToSlc716()
{

}

CSkc98ToSlc716::~CSkc98ToSlc716()
{

}
bool CSkc98ToSlc716::Skc98ToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //Դ�ļ���   
	m_sObjectPath=sObjectPath;  //���Ŀ¼
	m_nCurvesRadio=nCurvesRadio;//���˿���
	fp=fo;

	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	CurveIndex.RemoveAll();
	Skc98R=new CSkc98R;
	Slc716W=new CSlc716W;
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
		Slc716W->PrintHead(fp);

	////////////////////////////////////
	CurveIndex.RemoveAll();
	delete Slc716W;	Slc716W=NULL;
	delete Skc98R; Skc98R=NULL;
	delete pGuage;  //ɾ��������
	return bResult;
}
bool CSkc98ToSlc716::Transform()
{
	if(!IsSKC98(m_SourceFile))
	{
		fprintf(fp,"%s is not SKC98 Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���
	
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);

	if(!Skc98R->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
		return FALSE; 
	}
	Skc98R->ReadHead();

	if(Slc716W->Open(sOutFileName))
	{
		//ͷ��Ϣ����
		Skc98ToSlc716Head();
		int nMaxSamples=1; //����������������
		for(int i=0;i<Slc716W->Slc716Head.NumLog;i++)
		{
			if(nMaxSamples < Skc98R->Skc98Head.CurveTypes[CurveIndex.GetAt(i)]/4)
			{
				nMaxSamples=Skc98R->Skc98Head.CurveTypes[CurveIndex.GetAt(i)]/4;
			}
		}
		Slc716W->NumPoint=Skc98R->NumPoint; //һ�����ߵĲ�������
		Slc716W->NumBlock=(Slc716W->NumPoint-1)/64+1; //���ݼ�¼��
		Slc716W->ClearHead(); //������ļ�ͷ

		if(nMaxSamples>1)
		{
			Slc716W->Slc716Head.Rlev/=nMaxSamples;
		}
		Slc716W->WriteHead(); //���ͷ��Ϣ
		pGuage->SetRange(0,Slc716W->Slc716Head.NumLog);
		int nBufSize=Skc98R->NumPoint*nMaxSamples;
		nBufSize=((nBufSize-1)/64+1)*64;
		float *buf=new float[nBufSize]; //һ�����ߵĿռ�
		for(i=0;i<Slc716W->Slc716Head.NumLog;i++)
		{
			pGuage->SetPos(i+1);  //���ý�������ǰֵ
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}

			for(int j=0;j<nBufSize;j++) buf[j]=-999.25;
			Skc98R->ReadChannel(CurveIndex.GetAt(i),buf);
			//�ز���
			int NumX=Skc98R->NumPoint*Skc98R->Skc98Head.CurveTypes[CurveIndex.GetAt(i)]/4;
			int NumY=Skc98R->NumPoint*nMaxSamples;
			if(NumX != NumY)
			{
				FloatResample(buf,NumX,NumY);
			}
			Slc716W->WriteChannel(i,buf);
		}
		Slc716W->Close();
		delete []buf; buf=0;
	}
	Skc98R->Close(); //�ر������ļ�

	return !sp_Break_Status;
}
void CSkc98ToSlc716::Skc98ToSlc716Head()
{
	CString s;
	sprintf(Slc716W->Slc716Head.WellName,"%-80.80s",Skc98R->Skc98Head.WellName);
	sprintf(Slc716W->Slc716Head.CompanyName,"%-80.80s",Skc98R->Skc98Head.CompanyName);
	if(Skc98R->Skc98Head.Stdep<Skc98R->Skc98Head.Endep)
	{
		Slc716W->Slc716Head.Stdep=Skc98R->Skc98Head.Stdep;
		Slc716W->Slc716Head.Endep=Skc98R->Skc98Head.Endep;
	}
	else
	{
		Slc716W->Slc716Head.Stdep=Skc98R->Skc98Head.Endep;
		Slc716W->Slc716Head.Endep=Skc98R->Skc98Head.Stdep;
	}
	Slc716W->Slc716Head.Rlev=float(fabs(Skc98R->Skc98Head.Rlev));
	Slc716W->Slc716Head.Spcpr=64;
	Slc716W->Slc716Head.Ecc=0;
	//�����������Ϊ0��Ȼ��ӹ��˱��в���
	CurveIndex.RemoveAll();
	Slc716W->Slc716Head.NumLog=0;
	for(int i=0;i<Skc98R->Skc98Head.NumLog;i++)
	{
		s.Format ("%-4.4s",Skc98R->Skc98Head.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		if(s.CompareNoCase("VDL")==0 || s.CompareNoCase("WAVE")==0)
		{
			continue; //SKC98�еĲ������߲�����SLC716��ʵ��
		}
		bool bYes=false; //�Ƿ�Ҫ������
		if(m_nCurvesRadio==0)
		{
			//���ù����ļ�ѡ������
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(s.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					bYes=true;
					break;
				}
			}
		}
		else
		{
			bYes=true; //ѡ���������߻򳣹�����
		}
		if(bYes)
		{
			CurveIndex.Add(i);
			sprintf(Slc716W->Slc716Head.CurveName[Slc716W->Slc716Head.NumLog],"%-4.4s",Skc98R->Skc98Head.CurveName[i]);
			Slc716W->Slc716Head.NumLog++;
		}
	}
}
