// BitToFid.cpp: implementation of the CBitToFid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_Comext.h"
#include "BitToFid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitToFid::CBitToFid()
{

}

CBitToFid::~CBitToFid()
{

}
bool CBitToFid::BitToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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

	BitR=new CBitR ;
	FidWrite=new CFidWrite ; //�����
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	//���ת�������Ϣ
	if(bResult)
	{
		FidWrite->PrintHead(fp);
	}

	delete pGuage;  //ɾ��������
	delete FidWrite; FidWrite=NULL;
	delete BitR;	BitR=NULL;

	CurveIndex.RemoveAll();
	return bResult;
}

bool CBitToFid::Transform()
{
	CString s;
	if(!IsBIT(m_SourceFile))
	{
		fprintf(fp,"%s is not BIT Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ�����������·��
	CString sFileTitle=MyGetFileTitle(m_SourceFile);
	CString sOutWellPath=m_sObjectPath+"\\#"+sFileTitle;
	ModiFileName(sOutWellPath); //�������������Ƿ��޸��ļ���
	sFileTitle=MyGetFileTitle(sOutWellPath);
	CString sWellName=sFileTitle.Mid(1); //ȥ����#��
	FidWrite->SetFidFileName(m_sObjectPath,sWellName);

	if(!BitR->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n\n!!!!�����ļ���ʧ��!\n\n");
		return FALSE; 
	}
	BitR->ReadHead();
	SetFidHeadFromBitHead();
	FidWrite->WriteFidInfo();
	pGuage->SetRange(0,CurveIndex.GetSize());
	for(int i=0;i<CurveIndex.GetSize();i++)
	{	//��������
		pGuage->SetPos(i+1);  //���ý�������ǰֵ
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int index=CurveIndex.GetAt(i); //��ѡ�������еõ�������
		SetFidChannelFromBit(i,index); //����FIDͨ����Ϣ
		int NumPoint=BitR->NumPoint; //һ�����ߵĲ�������
		if(index<BitR->NumConvLog)
		{	//��������
			fprintf(fp,"====Transform Generic Curve: %s(%s)\n",SelCurveName.GetAt(i),NewCurveName.GetAt(i),NewUnits.GetAt(i));
			float *buf=new float[NumPoint];
			BitR->ReadChannel(index,buf);
			FidWrite->WriteGenData(i,NumPoint,buf);
			delete []buf; buf=NULL;
		}
		else
		{	//��������
			fprintf(fp,"====Transform Waveform Curve: %s(%s)\n",SelCurveName.GetAt(i),NewCurveName.GetAt(i),NewUnits.GetAt(i));
			if(FidWrite->OpenWave(i))
			{
				int nWave=index-BitR->NumConvLog;
				float Start=FidWrite->FidChannel[i].DimInfo[0].Start;
				float Step=FidWrite->FidChannel[i].DimInfo[0].Rlev;
				char *buf=new char[960];
				memset(buf,'\0',960);
				for(int j=0;j<NumPoint;j++)
				{
					float depth=Start+j*Step;
					BitR->ReadWaveform(nWave,depth,buf);
					FidWrite->WriteWave(depth,buf);
				}
				delete []buf; buf=NULL;
				FidWrite->CloseWave();
			}
		}
	}
	BitR->Close();
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)FidWrite->m_sFidFile);
	//�����FID�ļ�����������������
	FidWrite->FefurbishFidInfo();
	//////////////////
	return !sp_Break_Status;
}
void CBitToFid::SetFidHeadFromBitHead()
{
	CString s,sCurveName;
	sprintf(FidWrite->FidHead.Oil_Field,"%-80.80s","");
	sprintf(FidWrite->FidHead.Region,"%-32.32s","");
	sprintf(FidWrite->FidHead.Company,"%-72.72s",BitR->BitHead.CompanyName);
	sprintf(FidWrite->FidHead.Well,"%-72.72s",BitR->BitHead.WellName);
	sprintf(FidWrite->FidHead.X_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Y_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Exformation,"%-40.40s","");
	FidWrite->FidHead.NumLog=0;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	if(m_nCurvesRadio==0)
	{
		for(int i=0;i<BitR->BitHead.NumLog;i++)
		{
			sCurveName.Format("%-.4s",BitR->BitHead.CurveName[i]);
			sCurveName.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					CurveIndex.Add(i);
					SelCurveName.Add(sCurveName);
					NewCurveName.Add(sCurveName);
					NewUnits.Add("");
					break;
				}
			}
		}
	}
	else if(m_nCurvesRadio==1)
	{
		for(int i=0;i<BitR->BitHead.NumLog;i++)
		{
			CurveIndex.Add(i);
			sCurveName.Format("%-.4s",BitR->BitHead.CurveName[i]);
			sCurveName.TrimRight();
			SelCurveName.Add(sCurveName);
			NewCurveName.Add(sCurveName);
			NewUnits.Add("");
		}
	}
	else if(m_nCurvesRadio==2)
	{
		for(int i=0;i<BitR->NumConvLog;i++)
		{
			CurveIndex.Add(i);
			sCurveName.Format("%-.4s",BitR->BitHead.CurveName[i]);
			sCurveName.TrimRight();
			SelCurveName.Add(sCurveName);
			NewCurveName.Add(sCurveName);
			NewUnits.Add("");
		}
	}
	else if(m_nCurvesRadio==3)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=15;
		ReCurNamDlg.m_nMaxLog=10000;
		for(int i=0;i<BitR->BitHead.NumLog;i++)
		{
			s.Format("%-.4s",BitR->BitHead.CurveName[i]);
			s.TrimRight();
			ReCurNamDlg.m_SourceName.Add(s);
			ReCurNamDlg.m_ObjectName.Add(s);
			ReCurNamDlg.m_Units.Add("");
			int nDim=1; //���ߵ�ά��
			if(i>=BitR->NumConvLog) nDim=2;
			ReCurNamDlg.m_NumDim.Add(nDim);
		}
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
			for(int j=0;j<BitR->BitHead.NumLog;j++)
			{
				sCurveName.Format("%-.4s",BitR->BitHead.CurveName[j]);
				sCurveName.TrimRight();
				if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
				{
					CurveIndex.Add(j);
					break;
				}
			}
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
		ReCurNamDlg.m_NumDim.RemoveAll();
	}

	FidWrite->FidHead.NumLog=CurveIndex.GetSize();
	//����������(FidWrite->FidHead.NumLog)���ٿռ�
	FidWrite->InitFidChannel();
	//�õ�ÿ�����ߵ�������Ϣ
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		sprintf(FidWrite->FidChannel[i].CurveName,"%-.32s",NewCurveName.GetAt(i));
	}
}

void CBitToFid::SetFidChannelFromBit(int IndexFid,int IndexBit)
{
	CString s;
	//ά��
	if(IndexBit<BitR->NumConvLog)
	{
		FidWrite->FidChannel[IndexFid].NumOfDimension=2;
	}
	else
	{
		FidWrite->FidChannel[IndexFid].NumOfDimension=3;
	}

	//����������λ�ж����ߵĲ����������ͣ�ʱ�����ȣ�
	//��һά
	sprintf(FidWrite->FidChannel[IndexFid].DimInfo[0].Name,"%-.8s","Dep");
	sprintf(FidWrite->FidChannel[IndexFid].DimInfo[0].Unit,"%-.8s","m");

	FidWrite->FidChannel[IndexFid].DimInfo[0].RepCode=FID_REPR_FLOAT;
	FidWrite->FidChannel[IndexFid].DimInfo[0].CodeLen=4;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Nps=1;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Npw=0;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Start=BitR->BitHead.Stdep;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Stop=BitR->BitHead.Endep;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Rlev=float(fabs(BitR->BitHead.Rlev));
	if(BitR->BitHead.Stdep<BitR->BitHead.Endep)
	{
		FidWrite->FidChannel[IndexFid].DimInfo[0].Start=BitR->BitHead.Stdep;
		FidWrite->FidChannel[IndexFid].DimInfo[0].Stop=BitR->BitHead.Endep;
	}
	else
	{
		FidWrite->FidChannel[IndexFid].DimInfo[0].Start=BitR->BitHead.Endep;
		FidWrite->FidChannel[IndexFid].DimInfo[0].Stop=BitR->BitHead.Stdep;
	}


	if(FidWrite->FidChannel[IndexFid].NumOfDimension==2)
	{
		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[1].Name,NewCurveName.GetAt(IndexFid));
		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[1].Unit,NewUnits.GetAt(IndexFid));
		FidWrite->FidChannel[IndexFid].DimInfo[1].RepCode=FID_REPR_FLOAT;
		FidWrite->FidChannel[IndexFid].DimInfo[1].CodeLen=FID_REPR_FLOAT;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Nps=1;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Npw=1;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Start=0;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Stop=100;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Rlev=0;
	}
	else
	{
		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[1].Name,"T");
		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[1].Unit,"us");
		FidWrite->FidChannel[IndexFid].DimInfo[1].RepCode=FID_REPR_FLOAT;
		FidWrite->FidChannel[IndexFid].DimInfo[1].CodeLen=4;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Nps=BitR->BitHead.NumSample;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Npw=0;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Start=0;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Rlev=BitR->BitHead.RlevTime;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Stop=float(BitR->BitHead.NumSample*BitR->BitHead.RlevTime);

		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[2].Name,NewCurveName.GetAt(IndexFid));
		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[2].Unit,NewUnits.GetAt(IndexFid));
		FidWrite->FidChannel[IndexFid].DimInfo[2].RepCode=FID_REPR_CHAR;
		FidWrite->FidChannel[IndexFid].DimInfo[2].CodeLen=1;
		FidWrite->FidChannel[IndexFid].DimInfo[2].Nps=BitR->BitHead.NumSample;
		FidWrite->FidChannel[IndexFid].DimInfo[2].Npw=BitR->BitHead.NumSample;
		FidWrite->FidChannel[IndexFid].DimInfo[2].Start=0;
		FidWrite->FidChannel[IndexFid].DimInfo[2].Stop=100;
		FidWrite->FidChannel[IndexFid].DimInfo[2].Rlev=0;
	}
	//������λ������
	for(int i=0;i<FidWrite->FidChannel[IndexFid].NumOfDimension;i++)
	{
		s.Format ("%-.8s",FidWrite->FidChannel[IndexFid].DimInfo[i].Unit);
		s.TrimRight();
		if(s.IsEmpty())
		{
			sprintf(FidWrite->FidChannel[IndexFid].DimInfo[i].Unit,"%-.8s","none");
		}
	}
}
