// FidToLis.cpp: implementation of the CFidToLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToLis.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidToLis::CFidToLis()
{

}

CFidToLis::~CFidToLis()
{
	CurveItem.RemoveAll();
}
bool CFidToLis::FidToLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
{
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
	LisWrite=new CLisWrite ; 
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete FidRead; FidRead=NULL;
	delete LisWrite; LisWrite=NULL;

	return bResult;
}
bool CFidToLis::Transform()
{
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	LisFileName.Format ("%s\\%s.lis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(LisFileName); //�������������Ƿ��޸��ļ���	
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)LisFileName);

	bool bResult=FidRead->ReadFidInfo(m_SourceFile);
	if(!bResult)
	{
		fprintf(fp,"\n!!!!�����ļ����д�!\n\n");
		return false;
	}
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!����û������ʵ��\n\n");
		return false;
	}
	/////////////////////////////////////////////
	SelectCurves();  //ѡ�����߻�����߸���
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!����û������ʵ��\n\n");
		return false;
	}
	////////////////////////////////////////////
	CString sInWellPath=MyGetPathName(m_SourceFile);//FID������������Ŀ¼
	CString sInWellName=MyGetFileTitle(m_SourceFile);//FID�������ݶ�Ӧ����
	LisWrite->Open(LisFileName); //�����LIS�ļ�
	GetLisFileIndexFromFid(); //�õ����߷�����Ϣ

	//Prt_CurveItemIndex(); //��ӡ���߷�����Ϣ

	///////////////////////////////////////////////
	SetReelHeader(); //���þ�ͷ��¼��Ϣ
	LisWrite->WriteRTHT(132); //��ͷ
	LisWrite->WriteRTHT(130); //��ͷ

	//////////////////////////////////////////////
	int NumFile=CurveItem.GetAt(0);
	for(int nf=0;nf<NumFile;nf++)
	{
		SetFileHeader(nf+1); //�����ļ�ͷ����
		LisWrite->WriteFHR(128); //File Header Record
		SetEB(nf);  //���ø�ʽ˵����
		Prt_FileInfo(nf); //����ļ���Ϣ
		LisWrite->WriteEB(); //д��ʽ˵����¼
		WriteDataToLis(nf); //������ݵ�LIS�ļ�
		if(sp_Break_Status) break;
		LisWrite->WriteFHR(129); //File Trailer Record
	}
	fprintf(fp,"----���ֳ� %d ���߼��ļ�----\n\n",NumFile);


	//////////////////////////////////////
	LisWrite->WriteRTHT(131); //��β
	LisWrite->WriteRTHT(133); //��β

	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	LisWrite->Close();
	return !sp_Break_Status;
}
void CFidToLis::GetLisFileIndexFromFid()
{
	CString s,s1;
	const double er=0.00001;
	CWordArray FraSize;
	int *CurveID=new int[FidRead->FidHead.NumLog];
	CWordArray *CurveArray=new CWordArray[FidRead->FidHead.NumLog];
	WORD NumItem=0; //����
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		CurveID[i]=1;
	}
	for(i=0;i<FidRead->FidHead.NumLog;i++)
	{
		int nDim=FidRead->FidChannel[i].NumOfDimension-1;
		if(nDim<2) continue;
		float sdep=FidRead->FidChannel[i].DimInfo[0].Start;
		float rlev=FidRead->FidChannel[i].DimInfo[0].Rlev;
		WORD CurveSize=FidRead->FidChannel[i].DimInfo[nDim].Npw*FidRead->FidChannel[i].DimInfo[nDim].CodeLen;
		for(int j=0;j<NumItem;j++)
		{
			if((FraSize.GetAt(j)+CurveSize)>32767) continue;
			int k=CurveArray[j].GetAt(0);
			float sdep1=FidRead->FidChannel[k].DimInfo[0].Start;
			float rlev1=FidRead->FidChannel[k].DimInfo[0].Rlev;
			if(fabs(sdep-sdep1)<er && fabs(rlev-rlev1)<er)
			{
				CurveArray[j].Add(i);
				CurveID[i]=0;
				FraSize.SetAt(j,FraSize.GetAt(j)+CurveSize);
				break;
			}
		}
		if(CurveID[i]==1)
		{
			CurveArray[NumItem].Add(i);
			FraSize.Add(CurveSize);
			CurveID[i]=0;
			NumItem++;
		}
	}
	//���������߰���ʼ��ȺͲ�������鵽����������
	for(i=0;i<NumItem;i++)
	{
		int k=CurveArray[i].GetAt(0);
		float sdep=FidRead->FidChannel[k].DimInfo[0].Start;
		float rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
		for(int j=0;j<FidRead->FidHead.NumLog;j++)
		{
			if(CurveID[j]==0) continue; //�������ѹ���
			float sdep1=FidRead->FidChannel[j].DimInfo[0].Start;
			float rlev1=FidRead->FidChannel[j].DimInfo[0].Rlev;
			WORD CurveSize=FidRead->FidChannel[i].DimInfo[1].Npw*FidRead->FidChannel[i].DimInfo[1].CodeLen;
			if((FraSize.GetAt(i)+CurveSize)>32767) continue; //���߼��ļ�֡�������㹻��
			if(fabs(sdep-sdep1)<er && fabs(rlev-rlev1)<er)
			{
				FraSize.SetAt(i,FraSize.GetAt(i)+CurveSize);
				CurveArray[i].Add(j);
				CurveID[j]=0;
			}
		}
	}
	//��û�з��벨����ĳ������ߵ������Ϊһ���߼��ļ�
	bool bHasConv=false;
	for(i=0;i<FidRead->FidHead.NumLog;i++)
	{
		if(CurveID[i]==1)
		{
			bHasConv=true;
			break;
		}
	}
	if(bHasConv)
	{
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			if(CurveID[i]==1)
			{
				CurveArray[NumItem].Add(i);
				CurveID[i]=0;
			}
		}
		NumItem++;
	}
	//��������������������
	CurveItem.RemoveAll();
	CurveItem.Add(NumItem);
	for(i=0;i<NumItem;i++)
	{
		int n=CurveArray[i].GetSize();
		CurveItem.Add(n);
		for(int j=0;j<n;j++)
		{
			CurveItem.Add(CurveArray[i].GetAt(j));
		}
	}
	///////////////////////////////////////////////////////
	delete []CurveID; CurveID=NULL;
	FraSize.RemoveAll();
	for(i=0;i<FidRead->FidHead.NumLog;i++)
	{
		CurveArray[i].RemoveAll();
	}
	delete []CurveArray; CurveArray=NULL;
}
void CFidToLis::Prt_CurveItemIndex()
{
	if(CurveItem.GetSize()==0) return;
	int NumFile=CurveItem.GetAt(0);
	int pos=1;
	for(int i=0;i<NumFile;i++)
	{
		fprintf(fp,"\n    --- No. %d Logical File : %d Curves---\n",i+1,CurveItem.GetAt(pos));
		fprintf(fp,"%-3s %-12s %-3s %-9s %-9s %-9s %-8s %-8s\n",
			"No.","Name","Dim","StartDep","EndDep","Rlev","DepUnits","Units");
		for(int j=pos+1;j<pos+1+CurveItem.GetAt(pos);j++)
		{
			int index=CurveItem.GetAt(j);
			int n=j-pos;
			int kDim=FidRead->FidChannel[index].NumOfDimension-1;
			fprintf(fp,"%-3d %-12.32s %-3d %-9G %-9G %-9G %-8.8s %-8.8s\n",
				n,FidRead->FidChannel[index].CurveName,
				kDim,
				FidRead->FidChannel[index].DimInfo[0].Start,
				FidRead->FidChannel[index].DimInfo[0].Stop,
				FidRead->FidChannel[index].DimInfo[0].Rlev,
				FidRead->FidChannel[index].DimInfo[0].Unit,
				FidRead->FidChannel[index].DimInfo[kDim].Unit );
		}
		pos+=CurveItem.GetAt(pos)+1;
	}
	fprintf(fp,"    ----���ֳ�%d���߼��ļ�----\n",NumFile);
}

void CFidToLis::SetReelHeader()
{
	sprintf(LisWrite->TapeHead.SerName,"%-6.6s","LIDT");
	sprintf(LisWrite->TapeHead.Nul1,"%-6.6s"," ");
	struct tm *newtime;
	long ltime;
	time( &ltime );
	newtime = gmtime( &ltime );
	sprintf(LisWrite->TapeHead.Date,"%02d/%02d/%02d",newtime->tm_mon+1,newtime->tm_mday,newtime->tm_year%100);
	sprintf(LisWrite->TapeHead.Nul2,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.OriginData,"%-4.4s","FID");	
	sprintf(LisWrite->TapeHead.Nul3,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.Name,"%-8.8s"," ");
	sprintf(LisWrite->TapeHead.Nul4,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.ConnectNum,"%-2.2s","01");	
	sprintf(LisWrite->TapeHead.Nul5,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.PreConName,"%-8.8s"," ");
	sprintf(LisWrite->TapeHead.Nul6,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.Comments,"%-74s","SICHUAN LOGGING SERVICES , LIS DATA");
}
void CFidToLis::SetFileHeader(int FileNo)
{
	sprintf(LisWrite->FileHead.Name,"%-6s.%03d","LIDT",FileNo);
	sprintf(LisWrite->FileHead.Nul1,"%-2.2s"," ");
	sprintf(LisWrite->FileHead.SubSerName,"%-6s","LIDT");
	sprintf(LisWrite->FileHead.Version,"%-8s","V 1.00");


	struct tm *newtime;
	long ltime;
	_tzset();
	time( &ltime );
	newtime = localtime( &ltime );
	sprintf(LisWrite->FileHead.Date,"%02d/%02d/%02d",newtime->tm_mon+1,newtime->tm_mday,newtime->tm_year%100);
	LisWrite->FileHead.Nul2=32;
	sprintf(LisWrite->FileHead.MaxPRL,"%5d",MAX_PHYRECORD_LENGTH);
	sprintf(LisWrite->FileHead.Nul3,"%-2s"," ");
	sprintf(LisWrite->FileHead.FileType,"%-2s","LO");
	sprintf(LisWrite->FileHead.Nul4,"%-2s","  ");
	sprintf(LisWrite->FileHead.PreConName,"%-10s"," ");
}

int CFidToLis::GetLogicalCurveIndex(int nf)
{
	if(CurveItem.GetSize()==0) return -1;
	int NumFile=CurveItem.GetAt(0);
	if(NumFile==0) return 0;
	if(nf>=NumFile) return 0;
	int StartPos=1;
	for(int i=0;i<nf;i++)
	{
		StartPos+=CurveItem.GetAt(StartPos)+1;
	}
	int NumLog=CurveItem.GetAt(StartPos); //���߼��ļ���������
	//�߼��ļ���������
	CurveIndex.RemoveAll();
	for(i=0;i<NumLog;i++)
	{
		CurveIndex.Add(CurveItem.GetAt(StartPos+i+1));
	}
	return NumLog;
}
unsigned char CFidToLis::FidRepCodeToLis(unsigned char FidRepCode)
{
	unsigned char LisRepCode;
	switch(FidRepCode)
	{
	case FID_REPR_INT:
	case FID_REPR_LONG:
		LisRepCode=73;
		break;
	case FID_REPR_SHORT:
		LisRepCode=79;
		break;
	case FID_REPR_FLOAT:
	case FID_REPR_DOUBLE:
		LisRepCode=68;
		break;
	case FID_REPR_CHAR:
		LisRepCode=56;
		break;
	default:
		LisRepCode=56;
		break;
	}
	return LisRepCode;
}
void CFidToLis::SetEB(int nf)
{ 
	CString s;
	if(LisWrite->Dsbr)
	{
		delete []LisWrite->Dsbr; LisWrite->Dsbr=NULL;
	}
	//�ҳ����߼��ļ���������ʼλ��
	LisWrite->NumLog=GetLogicalCurveIndex(nf);
	//�ҳ���С�����������
	float MinRlev=float(fabs(FidRead->FidChannel[CurveIndex.GetAt(0)].DimInfo[0].Rlev));
	float MaxRlev=MinRlev;
	char str[255];
	for(int i=1;i<LisWrite->NumLog;i++)
	{
		int k=CurveIndex.GetAt(i);
		if(MinRlev>fabs(FidRead->FidChannel[k].DimInfo[0].Rlev))
		{
			MinRlev=float(fabs(FidRead->FidChannel[k].DimInfo[0].Rlev));
		}
		if(MaxRlev<fabs(FidRead->FidChannel[k].DimInfo[0].Rlev))
		{
			MaxRlev=float(fabs(FidRead->FidChannel[k].DimInfo[0].Rlev));
		}
	}
	//ȷ��֡�������
	int num=int(MaxRlev/MinRlev);
	if(num>127) num=127; //������ֻ��ռһ���ֽ�,���Ϊ255
	LisWrite->Rlev=num*MinRlev;
	LisWrite->Dsbr=new DataSpeBlockRec[LisWrite->NumLog];
	LisWrite->EnVa.DaFraSiz=0;
	for(i=0;i<LisWrite->NumLog;i++)
	{
		int k=CurveIndex.GetAt(i);
		sprintf(str,"%-4.4s",(const char*)(NewCurveName.GetAt(k)));
		memcpy(LisWrite->Dsbr[i].Mnemon,str,4); //������
		memcpy(LisWrite->Dsbr[i].ServID,"SNAM  ",6); //������
		memcpy(LisWrite->Dsbr[i].ServOrd,"        ",8);  //�����
		int kDim=FidRead->FidChannel[k].NumOfDimension-1;
		
		//sprintf(str,"%-4.4s",FidRead->FidChannel[k].DimInfo[kDim].Unit);
		sprintf(str,"%-4.4s",NewUnits.GetAt(k));
		s=str;
		if(s.CompareNoCase("none")==0)
		{
			memcpy(LisWrite->Dsbr[i].Units,"    ",4); //������λ
		}
		else
		{
			memcpy(LisWrite->Dsbr[i].Units,str,4); //������λ
		}
		
		LisWrite->Dsbr[i].APICode=0; //API����,����0�Ϳɽ����Ϊ4����:�⾮���͡��������͡����ߵȼ�����������
		LisWrite->Dsbr[i].FileNumber=nf+1; //�ļ���,ָ�״λ������ʱ���ļ���
		LisWrite->Dsbr[i].Nul=0; //���ֽڣ�ֵΪ0
		LisWrite->Dsbr[i].ProLvl=0; //����ˮƽ(����0)����������1û��
		LisWrite->Dsbr[i].Samples=int((LisWrite->Rlev+0.5*FidRead->FidChannel[k].DimInfo[0].Rlev)/FidRead->FidChannel[k].DimInfo[0].Rlev); //��������
		if(LisWrite->Dsbr[i].Samples<1)
		{
			LisWrite->Dsbr[i].Samples=1;
		}
		LisWrite->Dsbr[i].RepC=FidRepCodeToLis(FidRead->FidChannel[k].DimInfo[kDim].RepCode); //���ݴ���
		LisWrite->Dsbr[i].Size=FidRead->FidChannel[k].DimInfo[kDim].Npw*LisWrite->Dsbr[i].Samples*LisWrite->GetCodeLength(LisWrite->Dsbr[i].RepC); //�����ܳ��ȣ�ָһ֡����ռ���ֽ���
		for(int j=0;j<5;j++)
		{
			LisWrite->Dsbr[i].ProInd[j]=0; //����ˮƽָʾ��(����1),��������0û��
		}
	}
	/////////////////////////////////////////////////////////////
	LisWrite->EnVa.DaRecTyp=0; //1:���ݼ�¼���߼�����
	LisWrite->EnVa.DSBTyp=0; //2:����˵��������
	//3:���ݼ�¼��ÿ֡���ȣ��ֽڣ�
	LisWrite->EnVa.DaFraSiz=0;
	for(i=0;i<LisWrite->NumLog;i++)
	{
		LisWrite->EnVa.DaFraSiz+=LisWrite->Dsbr[i].Size;
	}
	LisWrite->EnVa.UDFlag=255; //4:�ϲ�/�²��־,1Ϊ�ϲ� 
	LisWrite->EnVa.DepUnitFlag=255; //5:��ȵ�λ��־,1ΪӢ�ߣ�255Ϊ��,0Ϊʱ��
	LisWrite->EnVa.DaRefP=0; //6:���ݲο��㵽����������Ծ���
	memcpy(LisWrite->EnVa.DaRefUnt,"M   ",4); //7:���ݲο��㵽����������Ծ��뵥λ
	LisWrite->EnVa.FraSpace=LisWrite->Rlev; //8:֮֡��Ĳ������
	memcpy(LisWrite->EnVa.FraUnt,"M   ",4); //9:�������ĵ�λ
	LisWrite->EnVa.MaxFraRec=1; //һ���߼���¼��������֡��
	for(i=0;i<127;i++)
	{	//���127֡
		if(LisWrite->EnVa.MaxFraRec>=127) break;
		if((LisWrite->EnVa.MaxFraRec+1)*LisWrite->EnVa.DaFraSiz > 1014)break;
		LisWrite->EnVa.MaxFraRec++;
	}
	LisWrite->EnVa.AbsVal=-999.25;	//12:ȱʡֵ
	LisWrite->EnVa.DepMod=1; //13:��ȼ�¼��ʽ��־
	memcpy(LisWrite->EnVa.DepUnt,"M   ",4);	//14:���ֵ������λ
	LisWrite->EnVa.DepRpc=68;	//15:���ֵ��Ϣ����(��ȷ�ʽΪ1ʱ)
	LisWrite->EnVa.DSBSTyp=1;	//16:����˵���������ͺ�
}
void CFidToLis::WriteDataToLis(int nf)
{
	LisWrite->NumLog=CurveIndex.GetSize();//�߼��ļ�������
	for(int i=0;i<LisWrite->NumLog;i++)
	{
		int k=CurveIndex.GetAt(i);
		float sdep=FidRead->FidChannel[k].DimInfo[0].Start;
		float edep=FidRead->FidChannel[k].DimInfo[0].Stop;
		if(i==0)
		{
			LisWrite->StDep=sdep; //�߼��ļ���ʼ���
			LisWrite->EnDep=edep; //�߼��ļ��������
		}
		else
		{
			if(LisWrite->StDep>sdep)LisWrite->StDep=sdep;
			if(LisWrite->EnDep<edep)LisWrite->EnDep=edep;
		}
	}
	CString sCurveName;
	LisWrite->StartWriteData();

	pGuage->SetRange(0,LisWrite->NumLog);
	for(i=0;i<LisWrite->NumLog;i++)
	{
		pGuage->SetPos(i+1);
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int k=CurveIndex.GetAt(i); //FID������
		sCurveName.Format("%-.32s",FidRead->FidChannel[k].CurveName);
		float sdep=FidRead->FidChannel[k].DimInfo[0].Start;
		float edep=FidRead->FidChannel[k].DimInfo[0].Stop;
		float rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
		int NumR=int((edep-sdep)/rlev+0.5)+1;
		if(FidRead->FidChannel[k].NumOfDimension==2)
		{
			float *buf=new float[NumR];
			FidRead->ReadGenData(sCurveName,NumR,buf);
			LisWrite->WriteCurveData(i,sdep,edep,rlev,NumR,buf);
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
					LisWrite->WriteWaveData(i,depth,FidRead->FidChannel[k].DimInfo[2].Npw,buf);
				}
				FidRead->CloseWave();
				delete []buf;
				buf=NULL;
			}
		}
	}
	LisWrite->StopWriteData();
}

void CFidToLis::SelectCurves()
{
	CString s;
	bool *CurveID=new bool[FidRead->FidHead.NumLog];
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
		CurveID[i]=false;
	CString sCurveName;
	switch(m_nCurvesRadio)
	{
	case 0://���߹���
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					bOk=true;
					break;
				}
			}
			if(bOk)
			{
				CurveID[i]=true;
			}
		}
		break;
	case 2://��������
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			if(FidRead->FidChannel[i].NumOfDimension==2)
			{
				CurveID[i]=true;
			}
		}
		break;
	default://����ѡ���ȫ������
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			CurveID[i]=true;
		}
		break;
	}
	for(i=FidRead->FidHead.NumLog-1;i>=0;i--)
	{
		if(!CurveID[i])
		{
			for(int j=i;j<FidRead->FidHead.NumLog-1;j++)
			{
				FidRead->FidChannel[j]=FidRead->FidChannel[j+1];
			}
			FidRead->FidHead.NumLog--;
		}
	}
	delete []CurveID; CurveID=NULL;
	///////////////////////////////////////////
	//���ݿ������ü����������Ⱦ����Ƿ���н���ѡ��
	bool bRename=false;
	if(m_nCurvesRadio==3)
	{//����ѡ��
		bRename=true;
	}
	else
	{//�ж����߳����Ƿ񳬹�LISҪ��
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			sCurveName.Format("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.TrimRight();
			if(sCurveName.GetLength()>4)
			{
				bRename=true;
				break;
			}
		}
	}
	if(bRename)
	{
		SelCurveName.RemoveAll();
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=4;
		ReCurNamDlg.m_nMaxLog=1024;
		for(int i=0;i<FidRead->FidHead.NumLog;i++)
		{
			int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			ReCurNamDlg.m_ObjectName.Add(sCurveName);
			ReCurNamDlg.m_Units.Add(FidRead->FidChannel[i].DimInfo[NumDim].Unit);
			ReCurNamDlg.m_NumDim.Add(NumDim);
		}
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();

		CurveID=new bool[FidRead->FidHead.NumLog];
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			CurveID[i]=false;
		}
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			for(int j=0;j<SelCurveName.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(SelCurveName.GetAt(j))==0)
				{
					CurveID[i]=true;
				}
			}
		}
		for(i=FidRead->FidHead.NumLog-1;i>=0;i--)
		{
			if(!CurveID[i])
			{
				for(int j=i;j<FidRead->FidHead.NumLog-1;j++)
				{
					FidRead->FidChannel[j]=FidRead->FidChannel[j+1];
				}
				FidRead->FidHead.NumLog--;
			}
		}
		delete []CurveID; CurveID=NULL;
		SelCurveName.RemoveAll();
	}
	else 
	{
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
			sCurveName.Format("%-.4s",FidRead->FidChannel[i].CurveName);
			NewCurveName.Add(sCurveName);
			NewUnits.Add(FidRead->FidChannel[i].DimInfo[NumDim].Unit);
		}
	}
}
void CFidToLis::Prt_FileInfo(int nf)
{
	fprintf(fp,"    --- �� %d �� �߼��ļ�,���� %d ������ ---\n",nf+1,LisWrite->NumLog);
	fprintf(fp,"%-3s %-12s %-4s %-3s %-9s %-9s %-9s %-3s %-4s %-4s %-5s %-8s\n",
			"No.","FID","LIS","Dim","StartDep","EndDep","Rlev",
			"Sam","Size","Code","Index","Units");
	for(int i=0;i<LisWrite->NumLog;i++)
	{
		int k=CurveIndex.GetAt(i);
		int nDim=FidRead->FidChannel[k].NumOfDimension-1;
		fprintf(fp,"%-3d",i+1);
		fprintf(fp," %-12.32s",FidRead->FidChannel[k].CurveName);
		fprintf(fp," %-4.4s",LisWrite->Dsbr[i].Mnemon);
		fprintf(fp," %-3d",nDim);
		fprintf(fp," %-9G",FidRead->FidChannel[k].DimInfo[0].Start);
		fprintf(fp," %-9G",FidRead->FidChannel[k].DimInfo[0].Stop);
		fprintf(fp," %-9G",FidRead->FidChannel[k].DimInfo[0].Rlev);
		fprintf(fp," %-3d",LisWrite->Dsbr[i].Samples);
		fprintf(fp," %-4u",LisWrite->Dsbr[i].Size);
		fprintf(fp," %-4u",LisWrite->Dsbr[i].RepC);
		fprintf(fp," %-5s",FidRead->FidChannel[k].DimInfo[0].Unit);
		fprintf(fp," %-8s",NewUnits.GetAt(i));
		fprintf(fp,"\n");
	}
}
