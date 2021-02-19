// Skc92ToSlc716.cpp: implementation of the CSkc92ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Skc92ToSlc716.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkc92ToSlc716::CSkc92ToSlc716()
{

}

CSkc92ToSlc716::~CSkc92ToSlc716()
{

}
bool CSkc92ToSlc716::Skc92ToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	Skc92R=new CSkc92R;
	Slc716W=new CSlc716W;
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
		Slc716W->PrintHead(fp);

	////////////////////////////////////
	CurveIndex.RemoveAll();
	delete Slc716W;	Slc716W=NULL;
	delete Skc92R; Skc92R=NULL;
	delete pGuage;  //ɾ��������
	return bResult;
}
bool CSkc92ToSlc716::Transform()
{
	if(!IsSKC92(m_SourceFile))
	{
		fprintf(fp,"%s is not SKC92 Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	//����SKC92�ļ�����չ��һ��Ϊһ���ַ��������ļ�������һ�£�
	//��������ļ���������Ӧ��������չ��
	CString sTempSource=MyGetFileName(m_SourceFile);
	if(MyGetFileExt(m_SourceFile).GetLength()==1)
	{
		sTempSource.Replace('.','_');
	}
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,sTempSource);
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���
	
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);

	if(!Skc92R->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
		return false; 
	}
	Skc92R->ReadHead();
	if(Slc716W->Open(sOutFileName))
	{
		//ͷ��Ϣ����
		Skc92HeadToSlc716Head();
		Slc716W->NumPoint=Skc92R->NumPoint; //һ�����ߵĲ�������
		Slc716W->NumBlock=(Slc716W->NumPoint-1)/64+1; //���ݼ�¼��
		Slc716W->ClearHead(); //������ļ�ͷ
		int nBufSize=Skc92R->NumPoint;

		//�����Ƿ���CCL����Ҫ���������Ӧ�������������
		if(Skc92R->bCCLFour)
		{
			Slc716W->Slc716Head.Rlev*=0.25; //CBL�⾮�Ҵ���CCL
			nBufSize*=4; //����нӹ����ߣ���������Ϊ4��
		}
		Slc716W->WriteHead(); //���ͷ��Ϣ

		nBufSize=((nBufSize-1)/64+1)*64;
		pGuage->SetRange(0,Slc716W->Slc716Head.NumLog);
		float *buf=new float[nBufSize]; //һ�����ߵĿռ�
		for(int i=0;i<nBufSize;i++)buf[i]=-999.25; 
		for(i=0;i<Slc716W->Slc716Head.NumLog;i++)
		{
			pGuage->SetPos(i+1);  //���ý�������ǰֵ
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}

			Skc92R->ReadChannel(CurveIndex.GetAt(i),buf);
			if(Skc92R->bCCLFour)
			{
				if(Skc92R->CurveTypes[CurveIndex.GetAt(i)]==4)
				{
					//����CCL����ʱ���������߽��в�ֵ
					for(int j=Skc92R->NumPoint-1;j>0;j--)
					{
						buf[4*j]=buf[j]; //������ֵ���¶�λ
					}
					//��ֵ����
					for(j=0;j<Skc92R->NumPoint-1;j++)
					{
						for(int k=1;k<4;k++)
						{
							buf[4*j+k]=buf[4*j]+k*0.25f*(buf[4*j+4]-buf[4*j]);
						}
					}
				}
			}
			Slc716W->WriteChannel(i,buf);
		}

		Slc716W->Close();
		delete []buf; buf=0;
	}
	Skc92R->Close(); //�ر������ļ�

	return !sp_Break_Status;
}
void CSkc92ToSlc716::Skc92HeadToSlc716Head()
{
	CString s;
	//ͷ��Ϣ����
	sprintf(Slc716W->Slc716Head.WellName,"%-80.80s",Skc92R->Skc92Head.WellName);
	sprintf(Slc716W->Slc716Head.CompanyName,"%-80.80s",Skc92R->Skc92Head.CompanyName);
	if(Skc92R->Skc92Head.Stdep<Skc92R->Skc92Head.Endep)
	{
		Slc716W->Slc716Head.Stdep=Skc92R->Skc92Head.Stdep;
		Slc716W->Slc716Head.Endep=Skc92R->Skc92Head.Endep;
	}
	else
	{
		Slc716W->Slc716Head.Stdep=Skc92R->Skc92Head.Endep;
		Slc716W->Slc716Head.Endep=Skc92R->Skc92Head.Stdep;
	}
	Slc716W->Slc716Head.Rlev=float(fabs(Skc92R->Skc92Head.Rlev));
	Slc716W->Slc716Head.Spcpr=64;
	Slc716W->Slc716Head.Ecc=0;
	Slc716W->Slc716Head.NumLog=0; //�����������Ϊ0��Ȼ��ӹ��˱��в���
	CurveIndex.RemoveAll();	
	for(int i=0;i<Skc92R->Skc92Head.NumLog;i++)
	{
		s.Format ("%-4.4s",Skc92R->Skc92Head.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		if(s.CompareNoCase("VDL")==0 || s.CompareNoCase("WAVE")==0)
		{
			continue; //SKC92�еĲ������߲�����SLC716��ʵ��
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
			sprintf(Slc716W->Slc716Head.CurveName[Slc716W->Slc716Head.NumLog],"%-4.4s",Skc92R->Skc92Head.CurveName[i]);
			Slc716W->Slc716Head.NumLog++;
		}
	}
}