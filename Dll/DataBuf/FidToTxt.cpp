// FidToTxt.cpp: implementation of the CFidToTxt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToTxt.h"
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

CFidToTxt::CFidToTxt()
{

}

CFidToTxt::~CFidToTxt()
{

}
bool CFidToTxt::FidToTxt(CString SourceFile,CString sObjectPath,int nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo)
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
	TxtW=new CFwdTxtW;
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
	{
		TxtW->Prt_Header(fp);
	}

	////////////////////////////////////
	CurveIndex.RemoveAll();
	delete TxtW;	TxtW=NULL;
	delete FidRead; FidRead=NULL;
	delete pGuage;  //ɾ��������
	return bResult;
}
bool CFidToTxt::Transform()
{
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	sOutFileName.Format ("%s\\%s.txt",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���
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
	TxtW->ResetHead();
	GetTxtHeadFromFid();
	if((TxtW->TxtHd.Endep<=TxtW->TxtHd.Stdep) || (TxtW->TxtHd.NumLog<1))
	{
		fprintf(fp,"\n!!!!û��Ҫ���������\n");
		return false;
	}
	SetStartStopRlev();
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);
	if(TxtW->Open(sOutFileName))
	{
		TxtW->TxtHd.NumPoint=int((TxtW->TxtHd.Endep-TxtW->TxtHd.Stdep)/TxtW->TxtHd.Rlev+0.5)+1;
		TxtW->WriteHead(); //���ͷ��Ϣ
		pGuage->SetRange(0,TxtW->TxtHd.NumLog);
		for(int i=0;i<TxtW->TxtHd.NumLog;i++)
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
			float *buf2=new float[TxtW->TxtHd.NumPoint];
			CString sCurveName;
			sCurveName.Format("%-.32s",FidRead->FidChannel[CurveIndex[i]].CurveName);
			FidRead->ReadGenData(sCurveName,Num1,buf1);
			FloatResample(Start1,Stop1,Rlev1,buf1,TxtW->TxtHd.Stdep,TxtW->TxtHd.Endep,TxtW->TxtHd.Rlev,buf2);
			TxtW->WriteChannel(i,buf2);
			delete []buf1;	buf1=NULL;
			delete []buf2;	buf2=NULL;
		}
		TxtW->Close();
	}
	return !sp_Break_Status;
}

void CFidToTxt::GetTxtHeadFromFid()
{
	CString s,sCurveName;
	CStringArray SelCurveName,NewCurveName;
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!����û������ʵ��\n\n");
		return;
	}
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
		if(sCurveName.GetLength()>15)
		{	//���������ȴ���TXT�ļ�����������Ҫ��
			bHasLenName=true;
		}
	}

	//�Գ���������������
	if(bHasLenName || m_nCurvesRadio==3 || CurveIndex.GetSize()>128 || SelCurveName.GetSize()>128)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLog=128;
		ReCurNamDlg.m_nMaxLen=15;
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
	TxtW->TxtHd.NumLog=SelCurveName.GetSize();
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
				strcpy(TxtW->TxtHd.CurveName[i],(const char*)NewCurveName.GetAt(i));
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
			TxtW->TxtHd.Stdep=FidRead->FidChannel[k].DimInfo[0].Start;
			TxtW->TxtHd.Endep=FidRead->FidChannel[k].DimInfo[0].Stop;
			TxtW->TxtHd.Rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
		}
		else
		{
			if(TxtW->TxtHd.Stdep>FidRead->FidChannel[k].DimInfo[0].Start)
			{
				TxtW->TxtHd.Stdep=FidRead->FidChannel[k].DimInfo[0].Start;
			}
			if(TxtW->TxtHd.Endep<FidRead->FidChannel[k].DimInfo[0].Stop)
			{
				TxtW->TxtHd.Endep=FidRead->FidChannel[k].DimInfo[0].Stop;
			}
			if(TxtW->TxtHd.Rlev>FidRead->FidChannel[k].DimInfo[0].Rlev)
			{
				TxtW->TxtHd.Rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
			}
		}
	}
}
void CFidToTxt::SetStartStopRlev()
{
	CString s;
	int nDep;
	CModiConvDlg ModiConvDlg;
	switch(m_nConvEncode)
	{
	case 1: //������ȼ����ʵ�����ֹ��ȼ��������
		nDep=int(TxtW->TxtHd.Stdep/m_FixRlev+0.95);
		TxtW->TxtHd.Stdep=nDep*m_FixRlev;
		nDep=int(TxtW->TxtHd.Endep/m_FixRlev);
		TxtW->TxtHd.Endep=nDep*m_FixRlev;
		TxtW->TxtHd.Rlev=m_FixRlev;
		break;
	case 2: //�˹�ָ����ֹ��ȼ��������
		ModiConvDlg.m_OldStart=TxtW->TxtHd.Stdep;
		ModiConvDlg.m_OldStop=TxtW->TxtHd.Endep;
		ModiConvDlg.m_OldRlev=TxtW->TxtHd.Rlev;
		if(ModiConvDlg.DoModal()==IDOK)
		{
			TxtW->TxtHd.Stdep=ModiConvDlg.m_NewStart;
			TxtW->TxtHd.Endep=ModiConvDlg.m_NewStop;
			TxtW->TxtHd.Rlev=ModiConvDlg.m_NewRlev;
		}
		break;
	default://��������ָ��������С�����������󾮶�
		break;
	}
}