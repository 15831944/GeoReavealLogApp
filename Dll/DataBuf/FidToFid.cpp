// FidToFid.cpp: implementation of the CFidToFid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToFid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidToFid::CFidToFid()
{

}

CFidToFid::~CFidToFid()
{

}
bool CFidToFid::FidToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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

	FidRead=new CFidRead ;
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
	delete FidRead;	FidRead=NULL;

	CurveIndex.RemoveAll();
	NewUnits.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();

	return bResult;
}
bool CFidToFid::Transform()
{
	CString s;
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ�����������·��
	CString sFileTitle=MyGetFileTitle(m_SourceFile);
	CString sOutWellPath=m_sObjectPath+"\\#"+sFileTitle;
	ModiFileName(sOutWellPath); //�������������Ƿ��޸��ļ���
	sFileTitle=MyGetFileTitle(sOutWellPath);
	CString sWellName=sFileTitle.Mid(1); //ȥ����#��
	FidWrite->SetFidFileName(m_sObjectPath,sWellName);

	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)FidWrite->m_sFidFile);

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
	SetFidHeadFromFidHead();
	FidWrite->WriteFidInfo();
	pGuage->SetRange(0,CurveIndex.GetSize());
	CString sCurveName;
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		pGuage->SetPos(i+1);  //���ý�������ǰֵ
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		sCurveName=SelCurveName.GetAt(i);
		sCurveName.TrimRight();
		if(FidWrite->FidChannel[i].NumOfDimension==2)
		{
			int NumR=int((FidWrite->FidChannel[i].DimInfo[0].Stop-FidWrite->FidChannel[i].DimInfo[0].Start)/FidWrite->FidChannel[i].DimInfo[0].Rlev+0.5)+1;
			float *buf=new float[NumR];
			FidRead->ReadGenData(sCurveName,NumR,buf);
			FidWrite->WriteGenData(i,NumR,buf);
			delete []buf;
			buf=NULL;
		}
		else if(FidWrite->FidChannel[i].NumOfDimension==3)
		{
			if(FidRead->OpenWave(sCurveName))
			{
				if(FidWrite->OpenWave(i))
				{
					float *buf=new float[FidWrite->FidChannel[i].DimInfo[2].Npw];
					int Num=int((FidWrite->FidChannel[i].DimInfo[0].Stop-FidWrite->FidChannel[i].DimInfo[0].Start)/FidWrite->FidChannel[i].DimInfo[0].Rlev+0.5)+1;
					for(int j=0;j<Num;j++)
					{
						float depth=FidWrite->FidChannel[i].DimInfo[0].Start+j*FidWrite->FidChannel[i].DimInfo[0].Rlev;
						if(!FidRead->ReadWave(depth,buf))
						{
							for(int k=0;k<FidWrite->FidChannel[i].DimInfo[2].Npw;k++)
							{
								buf[k]=0;
							}
						}
						FidWrite->WriteWave(depth,buf);
					}
					FidRead->CloseWave();
					delete []buf;
					buf=NULL;
				}
				FidWrite->CloseWave();
			}
		}
	}
	return !sp_Break_Status;
}
void CFidToFid::SetFidHeadFromFidHead()
{
	CString s;
	sprintf(FidWrite->FidHead.Oil_Field,"%-80.80s",FidRead->FidHead.Oil_Field);
	sprintf(FidWrite->FidHead.Region,"%-80.80s",FidRead->FidHead.Region);
	sprintf(FidWrite->FidHead.Company,"%-80.80s",FidRead->FidHead.Company);
	sprintf(FidWrite->FidHead.Well,"%-80.80s",FidRead->FidHead.Well);
	sprintf(FidWrite->FidHead.X_Coordinate,"%-40.40s",FidRead->FidHead.X_Coordinate);
	sprintf(FidWrite->FidHead.Y_Coordinate,"%-40.40s",FidRead->FidHead.Y_Coordinate);
	sprintf(FidWrite->FidHead.Exformation,"%-80.80s",FidRead->FidHead.Exformation);
	FidWrite->FidHead.NumLog=0;
	CurveIndex.RemoveAll();
	if(m_nCurvesRadio<3)
	{
		for(int i=0;i<FidRead->FidHead.NumLog;i++)
		{
			bool bOk=false;
			CString sCurveName;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			if(m_nCurvesRadio==0)
			{	//����
				for(int j=0;j<sp_FilterCurves.GetSize();j++)
				{
					if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
					{
						bOk=true;
						break;
					}
				}
			}
			else if(m_nCurvesRadio==1)
			{	//����
				bOk=true;
			}
			else if(m_nCurvesRadio==2)
			{	//����
				if(FidRead->FidChannel[i].NumOfDimension>2)
				{
					bOk=false;
				}
				else
				{
					bOk=true;
				}
			}
			if(bOk)
			{
				CurveIndex.Add(i);
				SelCurveName.Add(sCurveName);
				NewCurveName.Add(sCurveName);
				int nDim=FidRead->FidChannel[i].NumOfDimension-1;
				NewUnits.Add(FidRead->FidChannel[i].DimInfo[nDim].Unit);
			}
		}
	}
	else
	{	//����
		CurveIndex.RemoveAll();
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=32; //��������󳤶�
		ReCurNamDlg.m_nMaxLog=512; //FID�ļ��������������
		ReCurNamDlg.m_NumLog=FidRead->FidHead.NumLog; //ʵ��������
		for(int i=0;i<FidRead->FidHead.NumLog;i++)
		{
			CString sCurveName;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			ReCurNamDlg.m_ObjectName.Add(sCurveName);
			int nDim=FidRead->FidChannel[i].NumOfDimension-1;
			s.Format ("%s",FidRead->FidChannel[i].DimInfo[nDim].Unit);
			ReCurNamDlg.m_Units.Add(s);
			ReCurNamDlg.m_NumDim.Add(nDim);
		}
		int Result=ReCurNamDlg.DoModal();
		SelCurveName.RemoveAll();
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			for(int j=0;j<FidRead->FidHead.NumLog;j++)
			{
				CString sCurveName;
				sCurveName.Format ("%-.32ss",FidRead->FidChannel[j].CurveName);
				if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
				{
					CurveIndex.Add(j);
					break;
				}
			}
		}		
	}

	FidWrite->FidHead.NumLog=CurveIndex.GetSize();

	//����������(FidWrite->FidHead.NumLog)���ٿռ�
	FidWrite->InitFidChannel(); 
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		sprintf(FidWrite->FidChannel[i].CurveName,"%s",NewCurveName.GetAt(i));
		FidWrite->FidChannel[i].NumOfDimension=FidRead->FidChannel[k].NumOfDimension;
		for(int j=0;j<FidRead->FidChannel[k].NumOfDimension;j++)
		{
			//����ά����
			sprintf(FidWrite->FidChannel[i].DimInfo[j].Name,"%-.32s",FidRead->FidChannel[k].DimInfo[j].Name);
			sprintf(FidWrite->FidChannel[i].DimInfo[j].Unit,"%-.32s",FidRead->FidChannel[k].DimInfo[j].Unit);
			if(j==(FidRead->FidChannel[k].NumOfDimension-1))
			{
				sprintf(FidWrite->FidChannel[i].DimInfo[j].Unit,"%s",NewUnits.GetAt(i));
			}
			FidWrite->FidChannel[i].DimInfo[j].RepCode=FidRead->FidChannel[k].DimInfo[j].RepCode;
			FidWrite->FidChannel[i].DimInfo[j].CodeLen=FidRead->FidChannel[k].DimInfo[j].CodeLen;
			FidWrite->FidChannel[i].DimInfo[j].Nps=FidRead->FidChannel[k].DimInfo[j].Nps;
			FidWrite->FidChannel[i].DimInfo[j].Npw=FidRead->FidChannel[k].DimInfo[j].Npw;
			FidWrite->FidChannel[i].DimInfo[j].Start=FidRead->FidChannel[k].DimInfo[j].Start;
			FidWrite->FidChannel[i].DimInfo[j].Stop=FidRead->FidChannel[k].DimInfo[j].Stop;
			FidWrite->FidChannel[i].DimInfo[j].Rlev=FidRead->FidChannel[k].DimInfo[j].Rlev;
		}
	}
}
