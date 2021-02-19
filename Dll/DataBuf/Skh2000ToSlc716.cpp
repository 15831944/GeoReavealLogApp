// Skh2000ToSlc716.cpp: implementation of the CSkh2000ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Skh2000ToSlc716.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkh2000ToSlc716::CSkh2000ToSlc716()
{

}

CSkh2000ToSlc716::~CSkh2000ToSlc716()
{

}
bool CSkh2000ToSlc716::Skh2000ToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	Skh2000R=new CSkh2000R;
	Slc716W=new CSlc716W;
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
	{
		Slc716W->PrintHead(fp);
	}

	////////////////////////////////////
	CurveIndex.RemoveAll();
	delete Slc716W;	Slc716W=NULL;
	delete Skh2000R; Skh2000R=NULL;
	delete pGuage;  //ɾ��������
	return bResult;
}
bool CSkh2000ToSlc716::Transform()
{
	if(!IsSKH2000(m_SourceFile))
	{
		fprintf(fp,"%s is not SKH2000 Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���

	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);

	if(!Skh2000R->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
		return FALSE; //������SKH2000
	}
	//������ǳ����ļ����򷵻�
	if(Skh2000R->bIsWAVE)
	{
		Skh2000R->Close(); //�ر������ļ�
		return false;
	}

	Skh2000R->ReadHead(); //��SKH2000ͷ

	if(Slc716W->Open(sOutFileName))
	{
		Skh2000ToSlc716Head();
		Slc716W->NumPoint=Skh2000R->NumPoint; //һ�����ߵĲ�������
		Slc716W->NumBlock=Skh2000R->NumBlock; //���ݼ�¼��
		Slc716W->ClearHead(); //������ļ�ͷ
		Slc716W->WriteHead(); //���ͷ��Ϣ
		pGuage->SetRange(0,Slc716W->Slc716Head.NumLog);
		float *buf=new float[Skh2000R->NumBlock*int(Skh2000R->Skh2000Head.Spcpr+0.5)]; //һ�����ߵĿռ�
		for(int i=0;i<Slc716W->Slc716Head.NumLog;i++)
		{
			pGuage->SetPos(i+1);  //���ý�������ǰֵ
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			Skh2000R->ReadChannel(CurveIndex.GetAt(i),buf);
			Slc716W->WriteChannel(i,buf);
		}
		delete []buf; buf=0;
		Slc716W->Close();
	}
	Skh2000R->Close(); //�ر������ļ�

	return !sp_Break_Status;
}
void CSkh2000ToSlc716::Skh2000ToSlc716Head()
{
	CString s;
	sprintf(Slc716W->Slc716Head.CompanyName,"%-80.40s",Skh2000R->Skh2000Head.CompanyName);
	sprintf(Slc716W->Slc716Head.WellName,"%-80.40s",Skh2000R->Skh2000Head.WellName);
	if(Skh2000R->Skh2000Head.Stdep>Skh2000R->Skh2000Head.Endep)
	{
		Slc716W->Slc716Head.Stdep=Skh2000R->Skh2000Head.Endep;
		Slc716W->Slc716Head.Endep=Skh2000R->Skh2000Head.Stdep;
		Slc716W->Slc716Head.Rlev=float(fabs(Skh2000R->Skh2000Head.Rlev));
	}
	else
	{
		Slc716W->Slc716Head.Stdep=Skh2000R->Skh2000Head.Stdep;
		Slc716W->Slc716Head.Endep=Skh2000R->Skh2000Head.Endep;
		Slc716W->Slc716Head.Rlev=Skh2000R->Skh2000Head.Rlev;
	}
	Slc716W->Slc716Head.Spcpr=64;
	Slc716W->Slc716Head.Ecc=Skh2000R->Skh2000Head.Ecc;
	Slc716W->Slc716Head.NumLog=Skh2000R->Skh2000Head.NumLog;
	Slc716W->Slc716Head.Iz=0;
	CurveIndex.RemoveAll();
	if(m_nCurvesRadio==0)
	{
		Slc716W->Slc716Head.NumLog=0; //�����������Ϊ0��Ȼ��ӹ��˱��в���
		for(int i=0;i<Skh2000R->Skh2000Head.NumLog;i++)
		{
			s.Format ("%-4.4s",Skh2000R->Skh2000Head.CurveName[i]);
			s.TrimLeft();
			s.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(s.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					CurveIndex.Add(i);
					sprintf(Slc716W->Slc716Head.CurveName[Slc716W->Slc716Head.NumLog],"%-4.4s",Skh2000R->Skh2000Head.CurveName[i]);
					Slc716W->Slc716Head.NumLog++;
					break;
				}
			}
		}
	}
	else
	{
		for(int i=0;i<Slc716W->Slc716Head.NumLog;i++)
		{
			sprintf(Slc716W->Slc716Head.CurveName[i],"%-4.4s",Skh2000R->Skh2000Head.CurveName[i]);
			CurveIndex.Add(i);
		}
	}
}