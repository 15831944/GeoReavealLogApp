// BitToSlc716.cpp: implementation of the CBitToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_Comext.h"
#include "BitToSlc716.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitToSlc716::CBitToSlc716()
{

}

CBitToSlc716::~CBitToSlc716()
{

}
bool CBitToSlc716::BitToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	BitR=new CBitR;
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
	delete BitR; BitR=NULL;
	delete pGuage;  //ɾ��������
	return bResult;
}
bool CBitToSlc716::Transform()
{
	if(!IsBIT(m_SourceFile))
	{
		fprintf(fp,"%s is not BIT Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���

	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);

	if(!BitR->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
		return FALSE; 
	}
	BitR->ReadHead();
	if(Slc716W->Open(sOutFileName))
	{
		//int *nIndex=new int[BitR->BitHead.NumLog];
		//ͷ��Ϣ����
		BitHeadToSlc716Head();
		if(BitR->bWaveform && m_nCurvesRadio)
		{
			Slc716W->Slc716Head.NumLog-=4; //���ٲ���������
		}
		Slc716W->NumPoint=BitR->NumPoint; //һ�����ߵĲ�������
		Slc716W->NumBlock=(Slc716W->NumPoint-1)/64+1; //���ݼ�¼��
		Slc716W->ClearHead(); //������ļ�ͷ
		Slc716W->WriteHead(); //���ͷ��Ϣ
		int nBufSize=BitR->NumBlock*int(BitR->BitHead.Spcpr);
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

			BitR->ReadChannel(CurveIndex.GetAt(i),buf);
			Slc716W->WriteChannel(i,buf);
		}
		Slc716W->Close();
		delete []buf; buf=0;
	}
	BitR->Close(); //�ر������ļ�

	return !sp_Break_Status;
}
void CBitToSlc716::BitHeadToSlc716Head()
{
	CString s;
	sprintf(Slc716W->Slc716Head.WellName,"%-80.72s",BitR->BitHead.WellName);
	sprintf(Slc716W->Slc716Head.CompanyName,"%-80.72s",BitR->BitHead.CompanyName);
	if(BitR->BitHead.Stdep<BitR->BitHead.Endep)
	{
		Slc716W->Slc716Head.Stdep=BitR->BitHead.Stdep;
		Slc716W->Slc716Head.Endep=BitR->BitHead.Endep;
	}
	else
	{
		Slc716W->Slc716Head.Stdep=BitR->BitHead.Endep;
		Slc716W->Slc716Head.Endep=BitR->BitHead.Stdep;
	}
	Slc716W->Slc716Head.Rlev=float(fabs(BitR->BitHead.Rlev));
	Slc716W->Slc716Head.Spcpr=64;
	Slc716W->Slc716Head.Ecc=0;
	Slc716W->Slc716Head.NumLog=BitR->BitHead.NumLog;
	CurveIndex.RemoveAll();
	for(int i=0;i<BitR->BitHead.NumLog;i++)
		sprintf(Slc716W->Slc716Head.CurveName[i],"%-4.4s",BitR->BitHead.CurveName[i]);
	if(m_nCurvesRadio==0)
	{
		Slc716W->Slc716Head.NumLog=0; //�����������Ϊ0��Ȼ��ӹ��˱��в���
		for(int i=0;i<BitR->BitHead.NumLog;i++)
		{
			s.Format ("%-4.4s",BitR->BitHead.CurveName[i]);
			s.TrimLeft();
			s.TrimRight();
			if(s.Left(2)=="WF") continue; //BIT�еĲ������߲�����SLC716��ʵ��
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(s.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					CurveIndex.Add(i);
					sprintf(Slc716W->Slc716Head.CurveName[Slc716W->Slc716Head.NumLog],"%-4.4s",BitR->BitHead.CurveName[i]);
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
			CurveIndex.Add(i);
		}
	}
}
