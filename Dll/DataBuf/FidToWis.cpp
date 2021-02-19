// FidToWis.cpp: implementation of the CFidToWis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToWis.h"
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidToWis::CFidToWis()
{
	WisWrite=NULL;

}

CFidToWis::~CFidToWis()
{

}
bool CFidToWis::FidToWis(CString SourceFile,CString ObjectPath,int nCurvesRadio,FILE *fo)
{
	m_SourceFile=SourceFile; //Դ�ļ���
	m_sObjectPath=ObjectPath; //���Ŀ¼
	m_nCurvesRadio=nCurvesRadio; //���˿���
	fp=fo;
	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);
	
	FidRead=new CFidRead;
	WisWrite=new CWisWrite;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
	{
		WisWrite->PrintHead(fp);
		fprintf(fp,"%d Curve:\n",NewCurveName.GetSize());
		for(int i=0;i<NewCurveName.GetSize();i++)
		{
			if(i>0 && i%5==0)
			{
				fprintf(fp,"\n");
			}
			fprintf(fp," %-10.15s",(const char*)(NewCurveName.GetAt(i)));
		}
		fprintf(fp,"\n");
		fprintf(fp,"---- End Head information ---\n\n");
	}

	delete WisWrite; WisWrite=NULL;
	delete FidRead; FidRead=NULL;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	delete pGuage;  //ɾ��������
	return true;
}
bool CFidToWis::Transform()
{
	CString s;
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ������ļ���
	CString Filetitle=MyGetFileTitle(m_SourceFile);
	if(sp_bDirectory)
	{
		WisFileName.Format ("%s\\#%s\\%s.wis",m_sObjectPath,Filetitle,Filetitle);
		CString WellPath=m_sObjectPath+"\\#"+Filetitle;
		_mkdir(WellPath);
	}
	else
	{
		WisFileName.Format ("%s\\%s.wis",m_sObjectPath,Filetitle);
	}
	ModiFileName(WisFileName); //�������������Ƿ��޸��ļ���
	
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)WisFileName);

	bool bResult=FidRead->ReadFidInfo(m_SourceFile);
	if(!bResult)
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
		return false;
	}
//****************************************************************
	//����FID�ļ�ֻ��һ����ά�����壬���Ե����ڶ�ά����ʱ��������Ӧ����
	FidRead->CleanArrayCurve(); //�����������
//****************************************************************

	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!����û������ʵ��\n\n");
		return false;
	}
	//////////////
	CString sInWellPath=MyGetPathName(m_SourceFile);//FID������������Ŀ¼
	CString sInWellName=MyGetFileTitle(m_SourceFile);//FID�������ݶ�Ӧ����
	WisWrite->Open(WisFileName);
	GetWisObjectIndexFromFid();
	//���������¼����������
	WisWrite->InitWisHead(CurveIndex.GetSize());
	CString sCurveName;
	WisWrite->m_ObjectEntry=NULL; //�������
	WisWrite->WisChannel=NULL; //ͨ����Ϣ
	WisWrite->m_ObjectEntry=new WIS_OBJECT_ENTRY;
	WisWrite->WisChannel=new WIS_CHANNEL;
	WisWrite->InitWisObjectEntry();
	WisWrite->InitWisChannel();
	///////////////////////////
	pGuage->SetRange(0,CurveIndex.GetSize());
	for(int index=0;index<CurveIndex.GetSize();index++)
	{
		pGuage->SetPos(index+1);  //���ý�������ǰֵ
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int k=CurveIndex.GetAt(index);
		SetWisChannelFromFidHead(k,index); //����ͨ������
		WisWrite->WriteChannel(); //дͨ����Ϣ��WIS�ļ�
		sCurveName.Format("%-.32s",FidRead->FidChannel[k].CurveName);
		int NumR=int((FidRead->FidChannel[k].DimInfo[0].Stop-FidRead->FidChannel[k].DimInfo[0].Start)/FidRead->FidChannel[k].DimInfo[0].Rlev+0.5)+1;
		if(FidRead->FidChannel[k].NumOfDimension==2)
		{
			float *buf=new float[NumR];
			FidRead->ReadGenData(sCurveName,NumR,buf);
			//д�����������ݵ�WIS�ļ�
			WisWrite->WriteGendata(NumR,buf);
			delete []buf; buf=NULL;
		}
		else if(FidRead->FidChannel[k].NumOfDimension==3)
		{
			if(FidRead->OpenWave(sCurveName))
			{
				float *buf=new float[FidRead->FidChannel[k].DimInfo[2].Npw];
				for(int j=0;j<NumR;j++)
				{
					float depth=FidRead->FidChannel[k].DimInfo[0].Start+j*FidRead->FidChannel[k].DimInfo[0].Rlev;
					if(!FidRead->ReadWave(depth,buf))
					{
						for(int k1=0;k1<FidRead->FidChannel[k].DimInfo[2].Npw;k1++)
						{
							buf[k1]=0;
						}
					}
					//���һ����ȵ�Ĳ�������
					WisWrite->WriteWave(depth,buf);
				}
				FidRead->CloseWave();
				delete []buf;
				buf=NULL;
			}
		}
	}
	WisWrite->Close();
	
	////////////////////////////////
	delete WisWrite->m_ObjectEntry; 
	WisWrite->m_ObjectEntry=NULL;
	delete WisWrite->WisChannel; 
	WisWrite->WisChannel=NULL;
	
	return !sp_Break_Status;
}
void CFidToWis::GetWisObjectIndexFromFid()
{
	CString s,sCurveName;
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!����û������ʵ��\n\n");
		return;
	}
	CurveIndex.RemoveAll();
	WisWrite->WisHead.ObjectNumber=0;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	bool bHasLongName=false; //���ڳ�������
	if(m_nCurvesRadio<3)
	{
		for(int i=0;i<FidRead->FidHead.NumLog;i++)
		{
			if(m_nCurvesRadio==2)
			{
				if(FidRead->FidChannel[i].NumOfDimension != 2)
				{
					continue; //ֻҪ��������
				}
			}
			bool bOk=false;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
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
			else
			{
				bOk=true;
			}
			if(!bOk) continue;
			CurveIndex.Add(i);
			SelCurveName.Add(sCurveName);
			NewCurveName.Add(sCurveName);
			int nDim=FidRead->FidChannel[i].NumOfDimension-1;
			NewUnits.Add(FidRead->FidChannel[i].DimInfo[nDim].Unit);
			if(sCurveName.GetLength()>15)
			{
				bHasLongName=true;
			}
		}
	}
	if(m_nCurvesRadio==3 || bHasLongName)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=15; //��������󳤶�
		ReCurNamDlg.m_nMaxLog=2048; //XTF�ļ��������������
		if(m_nCurvesRadio==3)
		{
			ReCurNamDlg.m_NumLog=FidRead->FidHead.NumLog; //ʵ��������
			for(int i=0;i<FidRead->FidHead.NumLog;i++)
			{
				int nDim=FidRead->FidChannel[i].NumOfDimension-1;
				sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
				sCurveName.TrimLeft();
				sCurveName.TrimRight();
				ReCurNamDlg.m_SourceName.Add(sCurveName);
				ReCurNamDlg.m_ObjectName.Add(sCurveName);
				ReCurNamDlg.m_Units.Add(FidRead->FidChannel[i].DimInfo[nDim].Unit);
				ReCurNamDlg.m_NumDim.Add(nDim);
			}
		}
		else
		{
			ReCurNamDlg.m_NumLog=SelCurveName.GetSize(); //ʵ��������
			for(int i=0;i<SelCurveName.GetSize();i++)
			{
				ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
				ReCurNamDlg.m_ObjectName.Add(SelCurveName.GetAt(i));
				int k=GetCurveIndex(SelCurveName.GetAt(i));
				int nDim=FidRead->FidChannel[k].NumOfDimension-1;
				ReCurNamDlg.m_NumDim.Add(nDim);
				ReCurNamDlg.m_Units.Add(FidRead->FidChannel[k].DimInfo[nDim].Unit);
			}
			//���ԭ����������
			SelCurveName.RemoveAll();
			NewCurveName.RemoveAll();
			NewUnits.RemoveAll();
			CurveIndex.RemoveAll();
		}
		int Result=ReCurNamDlg.DoModal();
		for(int i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
		ReCurNamDlg.m_NumDim.RemoveAll();

		for(i=0;i<SelCurveName.GetSize();i++)
		{
			int k=GetCurveIndex(SelCurveName.GetAt(i));
			CurveIndex.Add(k);
		}
	}
}
int CFidToWis::GetCurveIndex(CString SelCurve)
{
	SelCurve.TrimRight();
	int index=-1;
	CString sCurveName;
	for(int j=0;j<FidRead->FidHead.NumLog;j++)
	{
		sCurveName.Format ("%-.32s",FidRead->FidChannel[j].CurveName);
		sCurveName.TrimLeft();
		sCurveName.TrimRight();
		if(sCurveName.CompareNoCase(SelCurve)==0)
		{
			index=j;
			break;
		}
	}
	return index;
}
void CFidToWis::SetWisChannelFromFidHead(int IndexFid,int IndexWis)
{
	//��������(����������WIS�����������)
	sprintf(WisWrite->m_ObjectEntry->Name,"%-.15s",NewCurveName.GetAt(IndexWis));
	WisWrite->m_ObjectEntry->Attribute=1;
	WisWrite->m_ObjectEntry->SubAttribute=FidRead->FidChannel[IndexFid].NumOfDimension-1;
	
	int kDim=FidRead->FidChannel[IndexFid].NumOfDimension-1; //WIS�ļ�ͨ��ά��
	//ͨ������
	sprintf(WisWrite->WisChannel->Unit,"%-.7s",NewUnits.GetAt(IndexWis));
	sprintf(WisWrite->WisChannel->AliasName,"%-.15s",FidRead->FidChannel[IndexFid].CurveName);
	sprintf(WisWrite->WisChannel->AliasUnit,"%-.15s",WisWrite->WisChannel->Unit);
	
	WisWrite->WisChannel->RepCode=FidRead->FidChannel[IndexFid].DimInfo[kDim].RepCode;
	WisWrite->WisChannel->CodeLen=FidRead->FidChannel[IndexFid].DimInfo[kDim].CodeLen;
	WisWrite->WisChannel->MinVal=FidRead->FidChannel[IndexFid].DimInfo[kDim].Start;
	WisWrite->WisChannel->MaxVal=FidRead->FidChannel[IndexFid].DimInfo[kDim].Stop;
	WisWrite->WisChannel->Reserved=0;
	WisWrite->WisChannel->NumOfDimension=kDim;
	
	DWORD NumPoint=DWORD((FidRead->FidChannel[IndexFid].DimInfo[0].Stop-FidRead->FidChannel[IndexFid].DimInfo[0].Start)/FidRead->FidChannel[IndexFid].DimInfo[0].Rlev+0.5)+1;
	//����һά��Ϣ
	sprintf(WisWrite->WisChannel->DimInfo[0].Name,"%-.7s",FidRead->FidChannel[IndexFid].DimInfo[0].Name);
	sprintf(WisWrite->WisChannel->DimInfo[0].Unit,"%-.7s",FidRead->FidChannel[IndexFid].DimInfo[0].Unit);
	sprintf(WisWrite->WisChannel->DimInfo[0].AliasName,"%-.16s",FidRead->FidChannel[IndexFid].DimInfo[0].Name);
	WisWrite->WisChannel->DimInfo[0].StartVal=FidRead->FidChannel[IndexFid].DimInfo[0].Start;
	WisWrite->WisChannel->DimInfo[0].Delta=FidRead->FidChannel[IndexFid].DimInfo[0].Rlev;
	WisWrite->WisChannel->DimInfo[0].Samples=NumPoint;
	WisWrite->WisChannel->DimInfo[0].MaxSamples=WisWrite->WisChannel->DimInfo[0].Samples;
	WisWrite->WisChannel->DimInfo[0].Size=FidRead->FidChannel[IndexFid].DimInfo[kDim].Npw*FidRead->FidChannel[IndexFid].DimInfo[kDim].CodeLen;
	WisWrite->WisChannel->DimInfo[0].RepCode=FidRead->FidChannel[IndexFid].DimInfo[0].RepCode;
	WisWrite->WisChannel->DimInfo[0].Reserved=0;
	
	//���ö�ά��Ϣ
	if(WisWrite->WisChannel->NumOfDimension==2)
	{
		sprintf(WisWrite->WisChannel->DimInfo[1].Name,"%-.7s",FidRead->FidChannel[IndexFid].DimInfo[1].Name);
		sprintf(WisWrite->WisChannel->DimInfo[1].Unit,"%-.7s",FidRead->FidChannel[IndexFid].DimInfo[1].Unit);
		sprintf(WisWrite->WisChannel->DimInfo[1].AliasName,"%-.15s",FidRead->FidChannel[IndexFid].DimInfo[1].Name);
		WisWrite->WisChannel->DimInfo[1].StartVal=FidRead->FidChannel[IndexFid].DimInfo[1].Start;
		WisWrite->WisChannel->DimInfo[1].Delta=FidRead->FidChannel[IndexFid].DimInfo[1].Rlev;
		WisWrite->WisChannel->DimInfo[1].Samples=FidRead->FidChannel[IndexFid].DimInfo[kDim].Npw;
		WisWrite->WisChannel->DimInfo[1].MaxSamples=WisWrite->WisChannel->DimInfo[1].Samples;
		WisWrite->WisChannel->DimInfo[1].Size=FidRead->FidChannel[IndexFid].DimInfo[kDim].CodeLen;
		WisWrite->WisChannel->DimInfo[1].RepCode=FidRead->FidChannel[IndexFid].DimInfo[1].RepCode;
		WisWrite->WisChannel->DimInfo[1].Reserved=0;
	}
}
