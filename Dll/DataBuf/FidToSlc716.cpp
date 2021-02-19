// FidToSlc716.cpp: implementation of the CFidToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToSlc716.h"
#include "ModiConvDlg.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidToSlc716::CFidToSlc716()
{

}

CFidToSlc716::~CFidToSlc716()
{

}
bool CFidToSlc716::FidToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //Դ�ļ���   
	m_sObjectPath=sObjectPath;  //���Ŀ¼
	m_nCurvesRadio=nCurvesRadio;//���˿���
	m_nCurvesRadio=nCurvesRadio;//���˿���
	m_FixRlev=FixRlev;
	m_nConvEncode=nConvEncode;
	fp=fo;

	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	CurveIndex.RemoveAll();
	FidRead=new CFidRead;
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
	delete FidRead; FidRead=NULL;
	delete pGuage;  //ɾ��������
	return bResult;
}
bool CFidToSlc716::Transform()
{
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���

	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);

	bool bResult=FidRead->ReadFidInfo(m_SourceFile);
	if(!bResult)
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
		return false;
	}
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!����û������ʵ��\n\n");
		return false;
	}

	//////////////
	CString sInWellPath=MyGetPathName(m_SourceFile);//FID������������Ŀ¼
	CString sInWellName=MyGetFileTitle(m_SourceFile);//FID������������Ŀ¼
	Slc716W->ResetHead();
	GetSlc716HeadFromFid();
	SetStartStopRlev();
	if((Slc716W->Slc716Head.Endep>Slc716W->Slc716Head.Stdep) &&(Slc716W->Slc716Head.NumLog>0))
	{
		if(Slc716W->Open(sOutFileName))
		{
			Slc716W->NumPoint=int((Slc716W->Slc716Head.Endep-Slc716W->Slc716Head.Stdep)/Slc716W->Slc716Head.Rlev+0.5)+1;
			Slc716W->NumBlock=(Slc716W->NumPoint-1)/64+1;
			Slc716W->ClearHead(); //������ļ�ͷ
			Slc716W->WriteHead(); //���ͷ��Ϣ
			pGuage->SetRange(0,Slc716W->Slc716Head.NumLog);
			for(int i=0;i<Slc716W->Slc716Head.NumLog;i++)
			{
				pGuage->SetPos(i+1);  //���ý�������ǰֵ
				if (pGuage->CheckCancelButton())
				{
					sp_Break_Status=true;
					break;
				}
				float Start1=FidRead->FidChannel[CurveIndex[i]].DimInfo[0].Start;
				float Stop1=FidRead->FidChannel[CurveIndex[i]].DimInfo[0].Stop;
				float Rlev1=FidRead->FidChannel[CurveIndex[i]].DimInfo[0].Rlev;
				int Num1=int((Stop1-Start1)/Rlev1+0.5)+1;
				float *buf1=new float[Num1];
				float *buf2=new float[Slc716W->NumPoint];
				CString sCurveName;
				sCurveName.Format("%-.32s",FidRead->FidChannel[CurveIndex[i]].CurveName);
				FidRead->ReadGenData(sCurveName,Num1,buf1);
				FloatResample(Start1,Stop1,Rlev1,buf1,Slc716W->Slc716Head.Stdep,Slc716W->Slc716Head.Endep,Slc716W->Slc716Head.Rlev,buf2);
				Slc716W->WriteChannel(i,buf2);
				delete []buf1;	buf1=NULL;
				delete []buf2;	buf2=NULL;
			}
			Slc716W->Close();
		}
	}
	return !sp_Break_Status;
}

void CFidToSlc716::GetSlc716HeadFromFid()
{
	CString s,sCurveName;
	CStringArray SelCurveName,NewCurveName;
	Slc716W->Slc716Head.Spcpr=64;
	Slc716W->Slc716Head.Ecc=0;
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!����û������ʵ��\n\n");
		return;
	}
	s.Format ("%-.80s",FidRead->FidHead.Well);
	s.TrimLeft();
	s.TrimRight();
	if(s.IsEmpty())
		sprintf(Slc716W->Slc716Head.WellName,"%-80.80s","NoneWellName");
	else
		sprintf(Slc716W->Slc716Head.WellName,"%-80.80s",(const char*)s);
	s.Format ("%-.80s",FidRead->FidHead.Company);
	s.TrimLeft();
	s.TrimRight();
	if(s.IsEmpty())
		sprintf(Slc716W->Slc716Head.CompanyName,"%-80.80s","Si Chuan Logging Company");
	else
		sprintf(Slc716W->Slc716Head.CompanyName,"%-80.80s",(const char*)s);
	bool bHasLenName=false; //�Ƿ��г���������
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		if(FidRead->FidChannel[i].NumOfDimension != 2)
			continue; //ֻҪ��������
		bool bOk=false;
		sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
		sCurveName.TrimLeft();
		sCurveName.TrimRight();
		if(m_nCurvesRadio==0)
		{
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					bOk=true;
					break;
				}
			}
		}
		else
		{
			bOk=true;
		}
		if(!bOk) continue;
		SelCurveName.Add(sCurveName);
		if(sCurveName.GetLength()>4)
		{	//���������ȴ���SLC716�ļ�����������Ҫ��
			bHasLenName=true;
		}
	}

	//�Գ���������������
	if(bHasLenName || m_nCurvesRadio==3 || CurveIndex.GetSize()>128 || SelCurveName.GetSize()>128)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLog=128;
		ReCurNamDlg.m_nMaxLen=4;
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
			ReCurNamDlg.m_ObjectName.Add(SelCurveName.GetAt(i));
			ReCurNamDlg.m_Units.Add("");
			ReCurNamDlg.m_NumDim.Add(1);
		}
		SelCurveName.RemoveAll();
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
		ReCurNamDlg.m_NumDim.RemoveAll();
	}
	else
	{
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			NewCurveName.Add(SelCurveName.GetAt(i));
		}
	}
	//������������������
	CurveIndex.RemoveAll();
	Slc716W->Slc716Head.NumLog=SelCurveName.GetSize();
	for(i=0;i<SelCurveName.GetSize();i++)
	{
		for(int j=0;j<FidRead->FidHead.NumLog;j++)
		{
			sCurveName.Format("%-.32s",FidRead->FidChannel[j].CurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
			{
				CurveIndex.Add(j);
				sprintf(Slc716W->Slc716Head.CurveName[i],"%-4.4s",(const char*)NewCurveName.GetAt(i));
			}
		}
	}
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	//�������ߵ���ֹ��ȡ����������ֵ
	for(i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		if(i==0)
		{
			Slc716W->Slc716Head.Stdep=FidRead->FidChannel[k].DimInfo[0].Start;
			Slc716W->Slc716Head.Endep=FidRead->FidChannel[k].DimInfo[0].Stop;
			Slc716W->Slc716Head.Rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
		}
		else
		{
			if(Slc716W->Slc716Head.Stdep>FidRead->FidChannel[k].DimInfo[0].Start)
			{
				Slc716W->Slc716Head.Stdep=FidRead->FidChannel[k].DimInfo[0].Start;
			}
			if(Slc716W->Slc716Head.Endep<FidRead->FidChannel[k].DimInfo[0].Stop)
			{
				Slc716W->Slc716Head.Endep=FidRead->FidChannel[k].DimInfo[0].Stop;
			}
			if(Slc716W->Slc716Head.Rlev>FidRead->FidChannel[k].DimInfo[0].Rlev)
			{
				Slc716W->Slc716Head.Rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
			}
		}
	}
}
void CFidToSlc716::SetStartStopRlev()
{
	CString s;
	int nDep;
	CModiConvDlg ModiConvDlg;
	switch(m_nConvEncode)
	{
	case 1: //������ȼ����ʵ�����ֹ��ȼ��������
		nDep=int(Slc716W->Slc716Head.Stdep/m_FixRlev+0.95);
		Slc716W->Slc716Head.Stdep=nDep*m_FixRlev;
		nDep=int(Slc716W->Slc716Head.Endep/m_FixRlev);
		Slc716W->Slc716Head.Endep=nDep*m_FixRlev;
		Slc716W->Slc716Head.Rlev=m_FixRlev;
		break;
	case 2: //�˹�ָ����ֹ��ȼ��������
		ModiConvDlg.m_OldStart=Slc716W->Slc716Head.Stdep;
		ModiConvDlg.m_OldStop=Slc716W->Slc716Head.Endep;
		ModiConvDlg.m_OldRlev=Slc716W->Slc716Head.Rlev;
		if(ModiConvDlg.DoModal()==IDOK)
		{
			Slc716W->Slc716Head.Stdep=ModiConvDlg.m_NewStart;
			Slc716W->Slc716Head.Endep=ModiConvDlg.m_NewStop;
			Slc716W->Slc716Head.Rlev=ModiConvDlg.m_NewRlev;
		}
		break;
	default://��������ָ��������С�����������󾮶�
		break;
	}
}