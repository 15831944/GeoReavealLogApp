// DLisToLis.cpp: implementation of the CDlisToLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "_Comext.h"
#include "DLisToLis.h"
#include "DlisCurDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlisToLis::CDlisToLis()
{

}

CDlisToLis::~CDlisToLis()
{

}
bool CDlisToLis::DlisToLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	
	DlisRead=new CDlisRead ;
	LisWrite=new CLisWrite ; //�����
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete LisWrite; LisWrite=NULL;
	delete DlisRead; DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDlisToLis::Transform()
{
	CString s;
	if(!IsDLIS(m_SourceFile))
	{
		fprintf(fp,"%s is not DLIS Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ������ļ���
	LisFileName.Format ("%s\\%s.lis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	LisFileName.MakeLower();
	ModiFileName(LisFileName); //�������������Ƿ��޸��ļ���

	if(!DlisRead->Open(m_SourceFile,fp)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
	}
	else
	{
		fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
		fprintf(fp,"ToObject: %s\n",(const char*)LisFileName);
		NumLisLog=0;
		LisWrite->Open(LisFileName); //�����LIS�ļ�
		SetReelHeader(); //���þ�ͷ��¼��Ϣ
		LisWrite->WriteRTHT(132); //��ͷ
		LisWrite->WriteRTHT(130); //��ͷ

		//���ý�����ΪһС�Σ�Ϊ���Ӿ�
		pGuage->SetRange(0,100);
		pGuage->SetPos(3);
		//�������DLIS�ļ���ȡ���߼��ļ��������߼��ļ���λ��
		int NumLogFile=DlisRead->GetDlisLogicalFileNumber();

		pGuage->m_nTextMode=1; //�����ı���ʾģʽΪ�Զ���
		pGuage->m_sTextInfo.Format ("��ʼת��"); //���ı�����
		pGuage->SetRange(0,100);
		pGuage->SetPos(1);
		for(int nf=0;nf<NumLogFile;nf++)
		{
			pGuage->m_sTextInfo.Format ("ɨ���߼��ļ���Ϣ"); //���ı�����
			pGuage->SetPos(nf*100/NumLogFile);
			DlisRead->ScanDlisOneLogicalFile(nf);
			/////////////////////////////////////////////////////////
			bool bResult=LogicalToLis(nf);
			if(sp_Break_Status) break; //�ж�
		}
		pGuage->m_nTextMode=0;
		//////////////////////////////////////
		LisWrite->WriteRTHT(131); //��β
		LisWrite->WriteRTHT(133); //��β
		LisWrite->Close();//�ر�����ļ�
		DlisRead->Close(); //�ر������ļ�
		if(NumLisLog>0)
		{
			fprintf(fp,"\n");
			fprintf(fp,"    From: %s\n",m_SourceFile);
			fprintf(fp,"      To: %s\n",LisFileName);
			fprintf(fp," Include: %d Logical File\n\n",NumLisLog);
		}
		else
		{
			remove(LisFileName);
			return false;
		}
	}
	return !sp_Break_Status;
}

bool CDlisToLis::LogicalToLis(int nf)
{
	CString s;
	/////////////////////////////////////////////////////////////////
	SetCurvesFromDlis();
	if(NewCurveName.GetSize()==0) return false;
	pGuage->m_nTextMode=1; //�����ı���ʾģʽΪ�Զ���
	pGuage->m_sTextInfo.Format ("��ȡDLIS���ݵ���ʱ�ļ�"); //���ı�����
	pGuage->SetRange(0,100);
	pGuage->SetPos(1);
	DlisRead->ReadLogicalFile(nf); //DLIS���ݶ�����֡Ϊ��λ���ļ�
	//���ý�������ΧΪ����������
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		LisWrite->NumLog=GetNumLog(i);
		if(LisWrite->NumLog==0)
		{
			continue;//��֡������Ҫת��
		}
		NumLisLog++;
		fprintf(fp,"\n====Write No. %d LIS Logical File From No. %d DLIS Logical File\n",NumLisLog,nf+1);
		SetFileHeader(); //�����ļ�ͷ����
		SetEB(i);  //���ø�ʽ˵����
		int NumPoint=DlisRead->Frame[i].noFrame;
		float sdep,edep,rlev;
		rlev=float(fabs(DlisRead->Frame[i].Index_Step));
		if(DlisRead->Frame[i].Index_Start<DlisRead->Frame[i].Index_Stop)
		{
			sdep=DlisRead->Frame[i].Index_Start;
			edep=DlisRead->Frame[i].Index_Stop;
		}
		else
		{
			sdep=DlisRead->Frame[i].Index_Stop;
			edep=DlisRead->Frame[i].Index_Start;
		}
		LisWrite->StDep=sdep; //�߼��ļ���ʼ���
		LisWrite->EnDep=edep; //�߼��ļ��������
		LisWrite->Rlev=rlev;  //�߼��ļ��������
		///////////////////////////////////////////////////////////////
		LisWrite->WriteFHR(128); //File Header Record
		LisWrite->WriteEB(); //д��ʽ˵����¼
		LisWrite->StartWriteData(); //��ʼ�����ǰLIS�߼��ļ�����
		////////////////////////////////////////////////////////////////
		int NumLog=-1;
		pGuage->SetRange(0,100);
		pGuage->SetPos(1);
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(sp_Break_Status) break;
			if(!DlisRead->Frame[i].bDecode[j]) continue;
			///////////////////////////////////////////////////
			NumLog++;
			int index=DlisRead->Frame[i].ChannelID[j];
			int Count=DlisRead->Channel[index].Sample; //һ���������������
			if(Count==1)
			{
				pGuage->m_sTextInfo.Format ("Curve: %s",DlisRead->Channel[index].Name); //���ı�����
			}
			else
			{
				pGuage->m_sTextInfo.Format ("Waveform: %s",DlisRead->Channel[index].Name); //���ı�����
			}
			pGuage->SetPos((NumLog+2)*100/LisWrite->NumLog);  //���ý�������ǰֵ
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			/////////////////////////////////////////////////////////////////////
			fprintf(fp,"====TRANSFORM: %-8s  START=%8G  STOP=%8G  SPACING=%8G  COUNT=%5d\n",
				DlisRead->Channel[index].Name,sdep,edep,rlev,Count);
			fflush(fp);
			if(DlisRead->Channel[index].Sample==1)
			{
				//��������
				float *buf=new float[NumPoint];
				DlisRead->ReadCurve(i,j,NumPoint,buf);
				LisWrite->WriteCurveData(NumLog,sdep,edep,rlev,NumPoint,buf);
				delete []buf; buf=NULL;
			}
			else if(DlisRead->Channel[index].NumDim==1)
			{
				float *buf=new float[Count];
				for(int k=0;k<Count;k++)
				{
					buf[k]=0;
				}
				pGuage->SetRange(0,100);
				pGuage->SetPos(1);
				for(k=0;k<NumPoint;k++)
				{
					pGuage->SetPos((k+1)*100/NumPoint);
					if (pGuage->CheckCancelButton())
						sp_Break_Status=true;
					if(sp_Break_Status) break;
					float DepFram=DlisRead->Frame[i].Index_Start+k*DlisRead->Frame[i].Index_Step;
					DlisRead->ReadWaveArray(i,j,DepFram,buf);
					LisWrite->WriteWaveData(NumLog,DepFram,Count,buf);
				}
				delete []buf; buf=NULL;
			}
		}
		//���ý�������ϢΪ��ʾ
		pGuage->m_sTextInfo="��ʱ�ļ�д��LIS�ļ�";
		pGuage->SetPos(10);
		LisWrite->StopWriteData();//���������ǰLIS�߼��ļ�����
		pGuage->SetPos(100);
		LisWrite->WriteFHR(129); //File Trailer Record
	}
	pGuage->m_nTextMode=0;
	pGuage->m_sTextInfo.Empty();
	return true;
}
void CDlisToLis::SetFileHeader()
{
	sprintf(LisWrite->FileHead.Name,"%-6s.%03d","LIDT",NumLisLog);
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
int CDlisToLis::GetNumLog(int nFrame)
{
	int NumLog=0;
	for(int i=0;i<DlisRead->Frame[nFrame].NumCurve;i++)
	{
		if(DlisRead->Frame[nFrame].bDecode[i])
		{
			NumLog++;
		}
	}
	return NumLog;
}
void CDlisToLis::SetEB(int nFrame)
{ 
	CString s;
	if(LisWrite->Dsbr)
	{
		delete []LisWrite->Dsbr; LisWrite->Dsbr=NULL;
	}
	//ȡ��ǰ�߼��ļ�������
	LisWrite->NumLog=GetNumLog(nFrame);
	if(LisWrite->NumLog==0)
	{
		return;
	}
	//������ת����������
	int NumLogT=0;
	for(int nItem=0;nItem<nFrame;nItem++)
	{
		for(int i=0;i<DlisRead->Frame[nItem].NumCurve;i++)
		{
			if(DlisRead->Frame[nItem].bDecode[i])
			{
				NumLogT++;
			}
		}
	}
	char str[255];
	LisWrite->Rlev=float(fabs(DlisRead->Frame[nFrame].Index_Step));
	LisWrite->Dsbr=new DataSpeBlockRec[LisWrite->NumLog];
	LisWrite->EnVa.DaFraSiz=0;
	int noi=-1;
	for(int i=0;i<DlisRead->Frame[nFrame].NumCurve;i++)
	{
		if(!DlisRead->Frame[nFrame].bDecode[i]) continue;
		int k=NumLogT;
		NumLogT++;
		noi++;
		int index=DlisRead->Frame[nFrame].ChannelID[i];
		sprintf(str,"%-4.4s",(const char*)(NewCurveName.GetAt(k)));
		memcpy(LisWrite->Dsbr[noi].Mnemon,str,4); //������
		memcpy(LisWrite->Dsbr[noi].ServID,"SNAM  ",6); //������
		memset(LisWrite->Dsbr[noi].ServOrd,' ',8); //�����
		int kDim=1;
		if(DlisRead->Channel[index].Sample>1)
		{
			kDim=2;
		}
		//���ߵ�λ��������ߵ�λ���ȳ���4�ֽ��Һ��С�.��ʱɾ��,����ȡǰ��4�ֽ�
		s.Format("%s",DlisRead->Channel[index].Units); 
		if(s.GetLength()>4)
		{
			s.Remove('.');
		}
		s.MakeUpper();
		sprintf(str,"%-4.4s",s);
		s=str;
		if(s.CompareNoCase("none")==0)
		{
			memcpy(LisWrite->Dsbr[noi].Units,"    ",4); //������λ
		}
		else
		{
			memcpy(LisWrite->Dsbr[noi].Units,str,4); //������λ
		}
		
		LisWrite->Dsbr[noi].APICode=0; //API����,����0�Ϳɽ����Ϊ4����:�⾮���͡��������͡����ߵȼ�����������
		LisWrite->Dsbr[noi].FileNumber=NumLisLog; //�ļ���,ָ�״λ������ʱ���ļ���
		LisWrite->Dsbr[noi].Nul=0; //���ֽڣ�ֵΪ0
		LisWrite->Dsbr[noi].ProLvl=0; //����ˮƽ(����0)����������1û��
		LisWrite->Dsbr[noi].Samples=1; //��������
		if(LisWrite->Dsbr[noi].Samples<1)
		{
			LisWrite->Dsbr[noi].Samples=1;
		}
		LisWrite->Dsbr[noi].RepC=PcCodeToLis(DlisRead->Channel[index].PcCode); //���ݴ���
		LisWrite->Dsbr[noi].Size=DlisRead->Channel[index].Dimension[0]*LisWrite->GetCodeLength(LisWrite->Dsbr[noi].RepC); //�����ܳ��ȣ�ָһ֡����ռ���ֽ���
		for(int j=0;j<5;j++)
		{
			LisWrite->Dsbr[noi].ProInd[j]=0; //����ˮƽָʾ��(����1),��������0û��
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

void CDlisToLis::SetCurvesFromDlis()
{
	CString s;
	CString sCurveName;
	//////////////////////////////////////////////////////////////
	bool bResult=DlisCurvesFilter();//��������ѡ�񿪹�(m_nCurvesRadio)�����߽��й���
	if(bResult)
		//Ҫ���н�������
		SelectChannelFromWin();
	else
	{//���ؽ��н�������
		NewCurveName.RemoveAll();
		for(int i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					int k=DlisRead->Frame[i].ChannelID[j];
					NewCurveName.Add(DlisRead->Channel[k].Name);
				}
			}
		}
	}
}

bool CDlisToLis::DlisCurvesFilter()
{
	CString s;
	CStringArray sCurve;
	bool bFilter=false;
	int i;
	//����ѡ�񿪹ؽ��й���
	switch(m_nCurvesRadio)
	{
	case 0: //��������		
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				int k=DlisRead->Frame[i].ChannelID[j];
				if(DlisRead->Channel[k].NumDim>1)
				{
					continue; //ֻ��ת�������������
				}
				CString sCurveName;
				bool bResult=false;
				sCurveName.Format ("%s",DlisRead->Channel[k].Name);
				for(int k1=0;k1<sp_FilterCurves.GetSize();k1++)
				{
					if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(k1))==0)
					{
						bResult=true;
						break;
					}
				}
				DlisRead->Frame[i].bDecode[j]=bResult;			
			}
		}
		break;
	case 2: //��������
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				int k=DlisRead->Frame[i].ChannelID[j];
				if(DlisRead->Channel[k].Sample==1)
				{
					DlisRead->Frame[i].bDecode[j]=true;
				}
				else
				{
					DlisRead->Frame[i].bDecode[j]=false;
				}
			}
		}
		break;
	case 1: //ȫ������
	case 3: //����ѡ��
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				int k=DlisRead->Frame[i].ChannelID[j];
				if(DlisRead->Channel[k].NumDim==1)
				{
					DlisRead->Frame[i].bDecode[j]=true;
				}
				else
				{
					DlisRead->Frame[i].bDecode[j]=false;
				}
			}
		}
		break;
	default:
		break;
	}
	//�����֡���ݣ�����Ӧ֡��������Ϊ������
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		if(DlisRead->Frame[i].noFrame>0) continue;
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	//����֡��DIRECTION�����ж��Ƿ����������ߣ�������ɢ���ݣ���ʱû�ж�����д���
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		//�����֡���߲������������������(����֡������������Ϊ������)
		if(DlisRead->Frame[i].bDiscrete)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				DlisRead->Frame[i].bDecode[j]=false;
			}
		}
	}
	//������������Ϊ������
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		DlisRead->Frame[i].bDecode[0]=false;
	}
	//�����Ƿ���н�������
	switch(m_nCurvesRadio)
	{
	case 3: //��������
		bFilter=true;
		break;
	default: //���������������ж��Ƿ�Ҫ���и����Ȳ���
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					int k=DlisRead->Frame[i].ChannelID[j];
					//�жϳ����Ƿ����Ҫ��
					if(strlen(DlisRead->Channel[k].Name)>4)
					{
						bFilter=true;
						break;
					}
				}
			}
			if(bFilter) break;
		}
		break;
	}
	sCurve.RemoveAll();

	return bFilter;
}
BYTE CDlisToLis::PcCodeToLis(BYTE PcCode)
{
	BYTE LisCode=LIS_REPC_FLOAT32;
	switch(PcCode)
	{
	case  PC_REPR_FLOAT: //IEEE�����ȸ����� 
	case  PC_REPR_DOUBLE: //IEE˫���ȸ�����
	case  PC_REPR_UINT: //�޷�������
	case  PC_REPR_ULONG: //�޷��ų�������
		LisCode=LIS_REPC_FLOAT32;
		break;
	case  PC_REPR_SHORT://�з��Ŷ�������
	case  PC_REPR_UCHAR: //�޷����ֽ���
		LisCode=LIS_REPC_INT16;
		break;
	case  PC_REPR_INT: //�з�������
	case  PC_REPR_LONG: //�з��ų�������
	case  PC_REPR_USHORT: //�޷��Ŷ�������
		LisCode=LIS_REPC_INT32;
		break;
	case  PC_REPR_CHAR: //�з����ֽ���
		LisCode=LIS_REPC_INT8;
		break;
	case  PC_REPR_STRING: //�ַ���
		LisCode=LIS_REPC_ASCII;
		break;
	default:
		LisCode=LIS_REPC_FLOAT32;
		break;
	}
	return LisCode;
}

void CDlisToLis::SetReelHeader()
{
	sprintf(LisWrite->TapeHead.SerName,"%-6.6s","LIDT");
	sprintf(LisWrite->TapeHead.Nul1,"%-6.6s"," ");
	struct tm *newtime;
	long ltime;
	time( &ltime );
	newtime = gmtime( &ltime );
	sprintf(LisWrite->TapeHead.Date,"%02d/%02d/%02d",newtime->tm_mon+1,newtime->tm_mday,newtime->tm_year%100);
	sprintf(LisWrite->TapeHead.Nul2,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.OriginData,"%-4.4s","DLIS");	
	sprintf(LisWrite->TapeHead.Nul3,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.Name,"%-8.8s"," ");
	sprintf(LisWrite->TapeHead.Nul4,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.ConnectNum,"%-2.2s","01");	
	sprintf(LisWrite->TapeHead.Nul5,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.PreConName,"%-8.8s"," ");
	sprintf(LisWrite->TapeHead.Nul6,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.Comments,"%-74s","SICHUAN LOGGING SERVICES , LIS DATA");
}

void CDlisToLis::SelectChannelFromWin()
{
	CString s;
	CDlisCurDlg DlisCurDlg;
	NewCurveName.RemoveAll();
	int NumLog=0;
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				NumLog++;
				int k=DlisRead->Frame[i].ChannelID[j];
				CString sNewName=DlisRead->Channel[k].Name; //������
				DlisRead->GetNewName(sNewName,i,j,DlisRead->Channel[k].CopyNum,
					DlisRead->Channel[k].Origin,sp_bAddFrameNo,
					sp_bAddOrigin,sp_bAddCopyNum);
				DlisCurDlg.m_ObjectName.Add(sNewName); //���������;
				DlisCurDlg.m_SourceName.Add(DlisRead->Channel[k].Name); ////����������
				s.Format ("%u",DlisRead->Channel[k].Origin);
				DlisCurDlg.m_Origin.Add(s); //ͨ����Դ
				s.Format ("%u",DlisRead->Channel[k].CopyNum);
				DlisCurDlg.m_CopyNum.Add(s); //ͨ���п�����
				s.Format ("%.1f-%.1f",DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
				DlisCurDlg.m_IndexInfo.Add(s); //��ֹ����
				s.Format ("%.5f",DlisRead->Frame[i].Index_Step);
				DlisCurDlg.m_StepInfo.Add(s); //�������
				s.Format ("%d",i);
				DlisCurDlg.m_FrameNum.Add(s); //֡��
				CString sValue="[";
				for(int k1=0;k1<DlisRead->Channel[k].NumDim;k1++)
				{
					if(k1>0)
					{
						sValue+=",";
					}
					s.Format ("%d",DlisRead->Channel[k].Dimension[k1]);
					sValue+=s;
				}
				sValue+="]";
				DlisCurDlg.m_Dimension.Add(sValue); //ά����
			}
		}
	}
	if(NumLog==0) 
		return ; //û������
	DlisCurDlg.m_nMaxLen=4;
	DlisCurDlg.m_nMaxLog=1000;
	DlisCurDlg.m_NumLog=NumLog;
	DlisCurDlg.m_sModuleName="DLISTOLIS";
	DlisCurDlg.nTransMode=m_nCurvesRadio;
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.DoModal();
	//////////////////////////////////////////////////////////////////
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	for(i=0;i<DlisCurDlg.m_ObjectName.GetSize();i++)
	{
		CString sFrame=DlisCurDlg.m_FrameNum.GetAt(i);
	
		int iFrame=atol((const char*)sFrame);
		DWORD Origin=atol(DlisCurDlg.m_Origin.GetAt(i));
		BYTE CopyNum=BYTE(atol(DlisCurDlg.m_CopyNum.GetAt(i)));
		CString sCurveName=	DlisCurDlg.m_SourceName.GetAt(i);

		for(int j=0;j<DlisRead->Frame[iFrame].NumCurve;j++)
		{
			int k=DlisRead->Frame[iFrame].ChannelID[j];
			if(DlisRead->Channel[k].Origin != Origin) continue;
			if(DlisRead->Channel[k].CopyNum != CopyNum) continue;
			if(sCurveName.Compare(DlisRead->Channel[k].Name)==0)
			{
				DlisRead->Frame[iFrame].bDecode[j]=true;
				NewCurveName.Add(DlisCurDlg.m_ObjectName.GetAt(i));
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.m_ObjectName.RemoveAll(); //���������
	DlisCurDlg.m_SourceName.RemoveAll(); //����������
	DlisCurDlg.m_IndexInfo.RemoveAll(); //��ֹ����
	DlisCurDlg.m_StepInfo.RemoveAll(); //�������
	DlisCurDlg.m_FrameNum.RemoveAll(); //֡������֡����Ŵ��棩
	DlisCurDlg.m_Dimension.RemoveAll(); //ά����
	DlisCurDlg.m_Origin.RemoveAll(); //ͨ����Դ
	DlisCurDlg.m_CopyNum.RemoveAll(); //ͨ���п�����
}
