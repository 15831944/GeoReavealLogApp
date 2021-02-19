// Skc98ToFid.cpp: implementation of the CSkc98ToFid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Skc98ToFid.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkc98ToFid::CSkc98ToFid()
{

}

CSkc98ToFid::~CSkc98ToFid()
{

}
bool CSkc98ToFid::Skc98ToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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

	Skc98R=new CSkc98R ;
	FidWrite=new CFidWrite ; //�����
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	//���ת�������Ϣ
	if(bResult)
		FidWrite->PrintHead(fp);

	delete pGuage;  //ɾ��������
	delete FidWrite; FidWrite=NULL;
	delete Skc98R;	Skc98R=NULL;

	CurveIndex.RemoveAll();
	return bResult;
}
bool CSkc98ToFid::Transform()
{
	CString s;
	if(!IsSKC98(m_SourceFile))
	{
		fprintf(fp,"%s is not SKC98 Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ�����������·��
	CString sFileTitle=MyGetFileTitle(m_SourceFile);
	CString sOutWellPath=m_sObjectPath+"\\#"+sFileTitle;
	ModiFileName(sOutWellPath); //�������������Ƿ��޸��ļ���
	sFileTitle=MyGetFileTitle(sOutWellPath);
	CString sWellName=sFileTitle.Mid(1); //ȥ����#��
	FidWrite->SetFidFileName(m_sObjectPath,sWellName);

	if(!Skc98R->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
		return FALSE; 
	}
	Skc98R->ReadHead();
	SetFidHeadFromSkc98Head();
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
		int index=CurveIndex.GetAt(i);
		SetFidChannelFromSkc98(i,index); //������FIDͨ����Ϣ
		int NumPoint=Skc98R->NumPoint; //һ�����ߵĲ�������
		if(IsConvCurve(index))
		{	//��������
			fprintf(fp,"====Transform Generic Curve: %s(%s)\n",SelCurveName.GetAt(i),NewCurveName.GetAt(i),NewUnits.GetAt(i));
			int NumR=NumPoint*(Skc98R->Skc98Head.CurveTypes[index]/4);
			float *buf=new float[NumR];
			Skc98R->ReadChannel(index,buf);
			FidWrite->WriteGenData(i,NumR,buf);
			delete []buf; buf=NULL;
		}
		else
		{	//��������
			fprintf(fp,"====Transform Waveform Curve: %s(%s)\n",SelCurveName.GetAt(i),NewCurveName.GetAt(i),NewUnits.GetAt(i));
			if(FidWrite->OpenWave(i))
			{
				int VdlSamp=Skc98R->Skc98Head.CurveTypes[index]/2;
				float Start=FidWrite->FidChannel[i].DimInfo[0].Start;
				float Step=FidWrite->FidChannel[i].DimInfo[0].Rlev;
				short *buf=new short[VdlSamp];
				for(int j=0;j<VdlSamp;j++)
				{
					buf[j]=0;
				}
				for(j=0;j<NumPoint;j++)
				{
					float depth=Start+j*Step;
					Skc98R->ReadVDL(index,depth,buf);
					FidWrite->WriteWave(depth,buf);
				}
				delete []buf; buf=NULL;
				FidWrite->CloseWave();
			}
		}
	}
	Skc98R->Close();
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)FidWrite->m_sFidFile);
	//�����FID�ļ�����������������
	FidWrite->FefurbishFidInfo();
	//////////////////
	return !sp_Break_Status;
}
bool CSkc98ToFid::IsConvCurve(int index)
{
	if(Skc98R->Skc98Head.CurveTypes[index]==4)
	{
		return true;
	}
	CString s;
	s.Format ("%-4.4s",Skc98R->Skc98Head.CurveName[index]);
	s.TrimRight();
	if(s=="VDL"  || s=="WAVE" || s.Left(2)=="WF")
	{
		return false;
	}
	return true;
}
void CSkc98ToFid::SetFidHeadFromSkc98Head()
{
	CString s,sCurveName;
	sprintf(FidWrite->FidHead.Oil_Field,"%-.80s","");
	sprintf(FidWrite->FidHead.Region,"%-.50s",Skc98R->Skc98Head.AreaName);
	sprintf(FidWrite->FidHead.Company,"%-.72s",Skc98R->Skc98Head.CompanyName);
	sprintf(FidWrite->FidHead.Well,"%-.72s",Skc98R->Skc98Head.WellName);
	sprintf(FidWrite->FidHead.X_Coordinate,"%-.40s","");
	sprintf(FidWrite->FidHead.Y_Coordinate,"%-.40s","");
	sprintf(FidWrite->FidHead.Exformation,"%-.40s","");
	FidWrite->FidHead.NumLog=0;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	if(m_nCurvesRadio==0)
	{	//����
		for(int i=0;i<Skc98R->Skc98Head.NumLog;i++)
		{
			sCurveName.Format("%-.4s",Skc98R->Skc98Head.CurveName[i]);
			sCurveName.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					CurveIndex.Add(i);
					SelCurveName.Add(sCurveName);
					sCurveName.Remove('.');
					NewCurveName.Add(sCurveName);
					NewUnits.Add("");
					break;
				}
			}
		}
	}
	else if(m_nCurvesRadio==1)
	{	//ȫ��
		for(int i=0;i<Skc98R->Skc98Head.NumLog;i++)
		{
			CurveIndex.Add(i);
			sCurveName.Format("%-.4s",Skc98R->Skc98Head.CurveName[i]);
			sCurveName.TrimRight();
			SelCurveName.Add(sCurveName);
			sCurveName.Remove('.');
			NewCurveName.Add(sCurveName);
			NewUnits.Add("");
		}
	}
	else if(m_nCurvesRadio==2)
	{	//����
		for(int i=0;i<Skc98R->Skc98Head.NumLog;i++)
		{
			if(!IsConvCurve(i)) continue; //���ǳ�������
			CurveIndex.Add(i);
			sCurveName.Format("%-.4s",Skc98R->Skc98Head.CurveName[i]);
			sCurveName.TrimRight();
			SelCurveName.Add(sCurveName);
			sCurveName.Remove('.');
			NewCurveName.Add(sCurveName);
			NewUnits.Add("");
		}
	}
	else if(m_nCurvesRadio==3)
	{	//����
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=15;
		ReCurNamDlg.m_nMaxLog=10000;
		for(int i=0;i<Skc98R->Skc98Head.NumLog;i++)
		{
			s.Format("%-.4s",Skc98R->Skc98Head.CurveName[i]);
			s.TrimRight();
			ReCurNamDlg.m_SourceName.Add(s);
			s.Remove('.');
			ReCurNamDlg.m_ObjectName.Add(s);
			ReCurNamDlg.m_Units.Add("");
			int nDim=1; //���ߵ�ά��
			if(!IsConvCurve(i)) nDim=2;
			ReCurNamDlg.m_NumDim.Add(nDim);
		}
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
			for(int j=0;j<Skc98R->Skc98Head.NumLog;j++)
			{
				sCurveName.Format("%-.4s",Skc98R->Skc98Head.CurveName[j]);
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

void CSkc98ToFid::SetFidChannelFromSkc98(int IndexFid,int IndexSkc98)
{
	CString s;
	//ά��
	if(IsConvCurve(IndexSkc98))
	{
		FidWrite->FidChannel[IndexFid].NumOfDimension=2;
	}
	else
	{
		FidWrite->FidChannel[IndexFid].NumOfDimension=3;
	}

	//����������λ�ж����ߵĲ����������ͣ�ʱ�����ȣ�
	//��һά:û�п���ʱ��������������ΪSKC98��־λ��ʱ��������
	strcpy(FidWrite->FidChannel[IndexFid].DimInfo[0].Name,"Dep");
	strcpy(FidWrite->FidChannel[IndexFid].DimInfo[0].Unit,"m");
	FidWrite->FidChannel[IndexFid].DimInfo[0].RepCode=FID_REPR_FLOAT;
	FidWrite->FidChannel[IndexFid].DimInfo[0].CodeLen=4;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Nps=1;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Npw=0;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Start=Skc98R->Skc98Head.Stdep;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Stop=Skc98R->Skc98Head.Endep;
	FidWrite->FidChannel[IndexFid].DimInfo[0].Rlev=float(fabs(Skc98R->Skc98Head.Rlev));
	if(IsConvCurve(IndexSkc98))
	{
		int NumY=int(fabs(Skc98R->Skc98Head.Rlev/Skc98R->yStep[IndexSkc98])+0.5);
		if(NumY>1)
		{
			FidWrite->FidChannel[IndexFid].DimInfo[0].Rlev/=NumY;
		}
	}
	if(Skc98R->Skc98Head.Stdep<Skc98R->Skc98Head.Endep)
	{
		FidWrite->FidChannel[IndexFid].DimInfo[0].Start=Skc98R->Skc98Head.Stdep;
		FidWrite->FidChannel[IndexFid].DimInfo[0].Stop=Skc98R->Skc98Head.Endep;
	}
	else
	{
		FidWrite->FidChannel[IndexFid].DimInfo[0].Start=Skc98R->Skc98Head.Endep;
		FidWrite->FidChannel[IndexFid].DimInfo[0].Stop=Skc98R->Skc98Head.Stdep;
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
		int VdlSamp=Skc98R->Skc98Head.CurveTypes[IndexSkc98]/2;
		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[1].Name,"T");
		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[1].Unit,"us");
		FidWrite->FidChannel[IndexFid].DimInfo[1].RepCode=FID_REPR_FLOAT;
		FidWrite->FidChannel[IndexFid].DimInfo[1].CodeLen=4;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Nps=VdlSamp;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Npw=0;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Start=0;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Rlev=2;
		FidWrite->FidChannel[IndexFid].DimInfo[1].Stop=FidWrite->FidChannel[IndexFid].DimInfo[1].Start+VdlSamp*FidWrite->FidChannel[IndexFid].DimInfo[1].Rlev;

		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[2].Name,NewCurveName.GetAt(IndexFid));
		strcpy(FidWrite->FidChannel[IndexFid].DimInfo[2].Unit,NewUnits.GetAt(IndexFid));
		FidWrite->FidChannel[IndexFid].DimInfo[2].RepCode=FID_REPR_SHORT;
		FidWrite->FidChannel[IndexFid].DimInfo[2].CodeLen=2;
		FidWrite->FidChannel[IndexFid].DimInfo[2].Nps=VdlSamp;
		FidWrite->FidChannel[IndexFid].DimInfo[2].Npw=VdlSamp;
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
