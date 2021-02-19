// CurveSplit.cpp: implementation of the CCurveSplit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CurveSplit.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCurveSplit::CCurveSplit()
{
	FidRead=NULL;
	FidWrite=NULL;
}

CCurveSplit::~CCurveSplit()
{
	if(FidRead)
	{
		delete FidRead;
		FidRead=NULL;
	}
	if(FidWrite)
	{
		delete FidWrite; FidWrite=NULL;
	}
}
bool CCurveSplit::CurveSplit(CString FidFile,CString sCurveName)
{
	CString s;
	FidRead=new CFidRead;
	///////////////////////////////////////////
	bool bResult=FidRead->ReadFidInfo(FidFile);
	if(!bResult || FidRead->FidHead.NumLog<=0) return false;
	sWellName=MyGetFileTitle(FidFile);//FID�������ݶ�Ӧ����
	Index=FidRead->GetIndexFromCurveName(sCurveName);
	int NumDim=FidRead->FidChannel[Index].NumOfDimension;
	if(NumDim<3) 
	{
		delete FidRead;	FidRead=NULL;
		return false; //��������
	}
	float Start=FidRead->FidChannel[Index].DimInfo[0].Start;
	float Stop=FidRead->FidChannel[Index].DimInfo[0].Stop;
	float Rlev=FidRead->FidChannel[Index].DimInfo[0].Rlev;
	int NumPoint=int((Stop-Start)/Rlev+0.5)+1;
	
	int CountIn=1; //��������һ�������
	for(int i=0;i<(NumDim-1);i++)
	{
		CountIn*=FidRead->FidChannel[Index].DimInfo[i].Nps;
	}
	//�������һ��ĳ���
	int BlockSizeIn=CountIn*FidRead->FidChannel[Index].DimInfo[NumDim-1].CodeLen;

	int CountOut=1; //�������һ�������
	for(i=0;i<(NumDim-2);i++)
	{
		CountOut*=FidRead->FidChannel[Index].DimInfo[i].Nps;
	}
	//�������һ��ĳ���
	int BlockSizeOut=CountOut*FidRead->FidChannel[Index].DimInfo[NumDim-1].CodeLen;
	
	///////////////////////////////////////////////
	if(FidWrite)
	{
		delete FidWrite; FidWrite=NULL;
	}
	FidWrite=new CFidWrite;
	//�������Ŀ¼�������ļ���ȷ�����������·��
	CString sWellName=MyGetFileTitle(FidFile);
	sPathName=MyGetPathName(FidFile);
	sPathName=MyGetPathName(sPathName);
	FidWrite->SetFidFileName(sPathName,sWellName);

	FidWrite->FidHead=FidRead->FidHead;
	FidWrite->FidHead.NumLog=FidRead->FidChannel[Index].DimInfo[NumDim-2].Nps;
	//����������(FidWrite->FidHead.NumLog)���ٿռ�
	FidWrite->InitFidChannel(); 
	char CurFor[10]; //�����������ʽ
	int nx=int(log10(double(FidWrite->FidHead.NumLog)))+1;
	sprintf(CurFor,"%%s[%%0%dd]",nx);
	for(i=0;i<FidWrite->FidHead.NumLog;i++)
	{
		sprintf(FidWrite->FidChannel[i].CurveName,CurFor,FidRead->FidChannel[Index].CurveName,i+1);
		FidWrite->FidChannel[i].NumOfDimension=NumDim-1;
		for(int j=0;j<NumDim-1;j++)
		{
			int n=j;
			if(n==(NumDim-2))
			{
				n=NumDim-1; //���������ڶ�ά
			}
			sprintf(FidWrite->FidChannel[i].DimInfo[j].Name,"%-.16s",FidRead->FidChannel[Index].DimInfo[n].Name);
			sprintf(FidWrite->FidChannel[i].DimInfo[j].Unit,"%-.16s",FidRead->FidChannel[Index].DimInfo[n].Unit);
			FidWrite->FidChannel[i].DimInfo[j].RepCode=FidRead->FidChannel[Index].DimInfo[n].RepCode;
			FidWrite->FidChannel[i].DimInfo[j].CodeLen=FidRead->FidChannel[Index].DimInfo[n].CodeLen;
			FidWrite->FidChannel[i].DimInfo[j].Nps=FidRead->FidChannel[Index].DimInfo[n].Nps;
			FidWrite->FidChannel[i].DimInfo[j].Npw=FidRead->FidChannel[Index].DimInfo[n].Npw;
			FidWrite->FidChannel[i].DimInfo[j].Start=FidRead->FidChannel[Index].DimInfo[n].Start;
			FidWrite->FidChannel[i].DimInfo[j].Stop=FidRead->FidChannel[Index].DimInfo[n].Stop;
			FidWrite->FidChannel[i].DimInfo[j].Rlev=FidRead->FidChannel[Index].DimInfo[n].Rlev;
		}
		FidWrite->FidChannel[i].DimInfo[NumDim-2].Nps=FidRead->FidChannel[Index].DimInfo[NumDim-3].Nps;
		FidWrite->FidChannel[i].DimInfo[NumDim-2].Npw=FidWrite->FidChannel[i].DimInfo[NumDim-2].Nps;
		strcpy(FidWrite->FidChannel[i].DimInfo[NumDim-2].Name,FidWrite->FidChannel[i].CurveName);
		//���ڳ���������ߣ���һά���������޸�
		if(FidWrite->FidChannel[i].NumOfDimension<=3)
		{
			strcpy(FidWrite->FidChannel[i].DimInfo[0].Name,"DEP"); 
		} 
		//�Կ��������ߣ��ڶ�ά�����޸�
		if(FidWrite->FidChannel[i].NumOfDimension==3)
		{
			strcpy(FidWrite->FidChannel[i].DimInfo[1].Name,"T");
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CFile fIn;
	CString sInData;
	sInData.Format("%s\\#%s\\D%s.%s",sPathName,sWellName,sWellName,sCurveName);
	fIn.Open(sInData,CFile::modeRead|CFile::shareDenyNone);
	BYTE *buf=new BYTE[BlockSizeOut];
	DWORD nfs=fIn.GetLength(); //Դ�ļ�����
	for(i=0;i<FidWrite->FidHead.NumLog;i++)
	{
		CString sCurve;
		sCurve.Format("%s",FidWrite->FidChannel[i].CurveName);
		FidWrite->WriteIndexFile(i); //д��������
		//�������
		CFile fOut;
		CString sOutData;
		sOutData.Format("%s\\#%s\\D%s.%s",sPathName,sWellName,sWellName,sCurve);
		fOut.Open(sOutData,CFile::modeCreate|CFile::modeWrite);
		int fs=BlockSizeOut*NumPoint; //����ļ�����
		fOut.SetLength(fs); //��������ļ�����
		fOut.SeekToBegin(); //���õ��ļ���ʼλ��
		//���������߶�ȡ���ݣ��������Ͳ��䣬�����ַ�ֱ�Ӷ�д
		for(int Rec=0;Rec<NumPoint;Rec++)
		{
			DWORD pos=Rec*BlockSizeIn+i*BlockSizeOut;
			if(pos>=nfs) break;
			fIn.Seek(Rec*BlockSizeIn+i*BlockSizeOut,CFile::begin);
			fIn.Read(buf,BlockSizeOut);
			fOut.Write(buf,BlockSizeOut);
		}
		fOut.Close();
	}
	fIn.Close();
	delete []buf; buf=NULL;
	//ˢ��FID�ļ�
	FidWrite->FefurbishFidInfo();
	///////////////////////////////////////////
	delete FidWrite; FidWrite=NULL;
	delete FidRead;	FidRead=NULL;
	return true;
}
