// FidToDLis.cpp: implementation of the CFidToDLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToDLis.h"
#include <math.h>
#include <time.h>
#include "SetDlisParaDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidToDLis::CFidToDLis()
{
	ItemNum=NULL;
}

CFidToDLis::~CFidToDLis()
{
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
}
BYTE CFidToDLis::FidRepCodeToDlis(int RepCode)
{
	BYTE DlisRepC=DLIS_REPC_FSINGL;
	switch(RepCode)
	{
	case FID_REPR_INT:
	case FID_REPR_LONG:
		DlisRepC=DLIS_REPC_SLONG;
		break;
	case FID_REPR_SHORT:
		DlisRepC=DLIS_REPC_SNORM;
		break;
	case FID_REPR_FLOAT:
		DlisRepC=DLIS_REPC_FSINGL;
		break;
	case FID_REPR_DOUBLE:
		DlisRepC=DLIS_REPC_FDOUBL;
		break;
	case FID_REPR_CHAR:
		DlisRepC=DLIS_REPC_SSHORT;
		break;
	default:
		break;
	}
	return DlisRepC;
}
bool CFidToDLis::FidToDlis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo)
{
	m_SourceFile=SourceFile; //Դ�ļ���   
	m_sObjectPath=sObjectPath;  //���Ŀ¼
	m_nCurvesRadio=nCurvesRadio;//���˿���
	m_sProducer=sProducer;
	fp=fo;
	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	FidRead=new CFidRead ;
	DlisWrite=new CDlisWrite ; 
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete FidRead; 
	FidRead=NULL;
	delete DlisWrite; 
	DlisWrite=NULL;

	return bResult;
}


bool CFidToDLis::Transform()
{
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	DlisFileName.Format ("%s\\%s.dlis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	DlisFileName.MakeLower();
	ModiFileName(DlisFileName); //�������������Ƿ��޸��ļ���

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
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)DlisFileName);
	/////////////////////////////////////////////
	SelectCurves();  //ѡ�����߻�����߸���
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!û���������!!!\n\n");
		return false;
	}
	////////////////////////////////////////////
	sInWellPath=MyGetPathName(m_SourceFile); //���뾮·��
	sInWellName=MyGetFileTitle(m_SourceFile); //�õ�����
	sInField=MyGetPathName(sInWellPath); 
	sInField=MyGetFileTitle(sInField); //�õ�������
	GetDlisFDataIndexFromFid(); //ͨ����Ϣ���з���
	SetDlisChannelsFromFid(); //����ͨ����Ϣ
	SetDlisFrameFromFid(); //����DLIS֡��������
	SetAxisFromFid(); //����DLIS���������
	Prt_FdataItemInfo(); //��ӡ֡������Ϣ
	SetStoUnitLab(); //����DLIS�洢��־
	SetFileHeader(); //�����ļ�ͷ��Ϣ
	SetOrigin(); //����Դ��Ϣ
	SetDlisParameter(); //�޸Ļ򲹳����
	DlisWrite->Open(DlisFileName,fp); //��DLIS�ļ�
	DlisWrite->SetTrailLengthFlag(); //��¼β��������
	DlisWrite->WriteStoUnitLab(); //дDLIS�洢��־
	DlisWrite->WriteFileHeader(); //д�ļ�ͷ��Ϣ��¼
	DlisWrite->WriteOrigin(); //дԴ��Ϣ��¼
	DlisWrite->WriteParameter(); //д��̬������¼
	DlisWrite->WriteChannel(); //дͨ����Ϣ��¼
	DlisWrite->WriteFrame(); //д֡��Ϣ��¼
	DlisWrite->WriteAxis(); //д����Ϣ��¼
	FidDataToDlis(); //�������
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	DlisWrite->Close(); //�ر�DLIS�ļ�
	DlisWrite->Prt_Header();
	return !sp_Break_Status;
}

void CFidToDLis::SelectCurves()
{
	CString s;
	bool *CurveID=new bool[FidRead->FidHead.NumLog];
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		CurveID[i]=false;
	}
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
	//���ݿ������þ����Ƿ���н���ѡ��
	if(m_nCurvesRadio==3)
	{
		SelCurveName.RemoveAll();
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=255; //������߳���
		ReCurNamDlg.m_nMaxLog=99999; //���������
		for(int i=0;i<FidRead->FidHead.NumLog;i++)
		{
			int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			ReCurNamDlg.m_Units.Add(FidRead->FidChannel[i].DimInfo[NumDim].Unit);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			sCurveName.Remove(' ');
			ReCurNamDlg.m_ObjectName.Add(sCurveName);//Ԥ����������
			ReCurNamDlg.m_NumDim.Add(NumDim);
		}
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			//ԭ����
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			//������
			s=ReCurNamDlg.m_ObjectName.GetAt(i);
			s.MakeUpper();
			NewCurveName.Add(s);
			//��λ
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));

		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
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
		//û�н�������
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			sCurveName.Format("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.MakeUpper();
			NewCurveName.Add(sCurveName);
			int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
			NewUnits.Add(FidRead->FidChannel[i].DimInfo[NumDim].Unit);
		}
	}
}

void CFidToDLis::GetDlisFDataIndexFromFid()
{
	CString s;
	typedef struct tagCurve
	{
		float Start;
		float Stop;
		float Rlev;
	}_Curve;
	const double er=0.00001;
	NumItem=0; //����
	if(FidRead->FidHead.NumLog<1) return;
	_Curve *Curve=new _Curve[FidRead->FidHead.NumLog];
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
	ItemNum=new int[FidRead->FidHead.NumLog];
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		float Start=FidRead->FidChannel[i].DimInfo[0].Start;
		float Stop=FidRead->FidChannel[i].DimInfo[0].Stop;
		float Rlev=FidRead->FidChannel[i].DimInfo[0].Rlev;
		bool bFind=false;
		for(int j=0;j<NumItem;j++)
		{
			if(fabs(Start-Curve[j].Start)<er && fabs(Stop-Curve[j].Stop)<er	&&  fabs(Rlev-Curve[j].Rlev)<er)
			{
				bFind=true;
				ItemNum[i]=j;
				break;
			}
		}
		if(!bFind)
		{
			Curve[NumItem].Start=Start;
			Curve[NumItem].Stop=Stop;
			Curve[NumItem].Rlev=Rlev;
			ItemNum[i]=NumItem;
			NumItem++;
		}
	}
	delete []Curve; Curve=NULL;
}
void CFidToDLis::Prt_FdataItemInfo()
{
	for(int i=0;i<NumItem;i++)
	{
		fprintf(fp,"��%2d��: ",i+1);
		int nc=0;
		for(int j=0;j<FidRead->FidHead.NumLog;j++)
		{
			if(ItemNum[j]==i)
			{
				fprintf(fp," %-4.32s",FidRead->FidChannel[j].CurveName);
				nc++;
				if(nc%10==0)
				{
					fprintf(fp,"\n%-8s","");
				}
			}
		}
		fprintf(fp,"\n");
	}
}
void CFidToDLis::SetStoUnitLab()
{
	char str[256];
//	DlisWrite->MaxPhyLen=58;//����¼����
	memcpy(DlisWrite->StoUnitLab.SeqNum,"   1",4); //�������
	memcpy(DlisWrite->StoUnitLab.DLISVer,"V1.00",5); //�汾��
	memcpy(DlisWrite->StoUnitLab.Stru,"RECORD",6); //�洢���ṹ
	sprintf(str,"%5d",DlisWrite->MaxPhyLen); 
	memcpy(DlisWrite->StoUnitLab.MaxRecLen,str,5); 
	sprintf(str,"%-60.60s","FID to DLIS Conversion"); //�洢����־
	memcpy(DlisWrite->StoUnitLab.StoSetIden,str,60);
}
void CFidToDLis::SetFileHeader()
{
	sprintf(DlisWrite->FileHeader.SequenceNumber,"%10.10s","1");//���к�
	sprintf(DlisWrite->FileHeader.ID,"%-65.65s","FID.001");//ID��
}
void CFidToDLis::SetOrigin()
{
	CString s;
	strcpy(DlisWrite->Origin.FileID,"FID.001");//FILE-ID
	strcpy(DlisWrite->Origin.FileSetName,"FID-TO-DLIS-CONVERSION");//FILE-SET-NAME
	strcpy(DlisWrite->Origin.FileSetNumber,"1");//FILE-SET-NUMBER
	strcpy(DlisWrite->Origin.FileNumber,"1"); //FILE-NUMBER
	strcpy(DlisWrite->Origin.FileType,"RE-FMT"); //FILE-TYPE
	strcpy(DlisWrite->Origin.Product,"GeoReveal "); //PRODUCT
	strcpy(DlisWrite->Origin.Version,"1.0"); //VERSION
	strcpy(DlisWrite->Origin.Programs,"FID-TO-DLIS"); //PROGRAMS
	CTime t=CTime::GetCurrentTime(); //ȡ��ǰϵͳ���ں�ʱ��
	sprintf(DlisWrite->Origin.CreationTime,"YEAR=%d MONTH=%d DAY=%d HOUR=%d MINUTE=%d SECOND=%d",
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),
		t.GetSecond()); //CREATION-TIME
	strcpy(DlisWrite->Origin.OrderNumber,""); //ORDER-NUMBER
	strcpy(DlisWrite->Origin.DescentNumber,""); //DESCENT-NUMBER
	strcpy(DlisWrite->Origin.RunNumber,""); //RUN-NUMBER
	strcpy(DlisWrite->Origin.WellID,""); //WELL-ID
	strcpy(DlisWrite->Origin.WellName,FidRead->FidHead.Well); //WELL-NAME
	strcpy(DlisWrite->Origin.FieldName,FidRead->FidHead.Oil_Field); //FIELD-NAME
	s=m_sProducer;
	s.TrimLeft();
	s.TrimRight();
	int sl=(m_sProducer).GetLength();
	if(sl>6)
	{
		strcpy(DlisWrite->Origin.ProducerName,s.Left(sl-6));//PRODUCER-NAME
		strcpy(DlisWrite->Origin.ProducerCode,s.Mid(sl-4,3));//PRODUCER-CODE
		strcpy(DlisWrite->Origin.NameSpaceName,""); //NAME-SPACE-NAME
	}
	else
	{
		strcpy(DlisWrite->Origin.ProducerName,"Schlumberger"); 
		strcpy(DlisWrite->Origin.ProducerCode,"440"); 
		strcpy(DlisWrite->Origin.NameSpaceName,"SLB"); //NAME-SPACE-NAME
	}
	strcpy(DlisWrite->Origin.Company,"SiChuan Petroleum Administration"); //COMPANY
	strcpy(DlisWrite->Origin.NameSpaceVersion,"0"); //NAME-SPACE-VERSION
}

void CFidToDLis::SetDlisParameter()
{
	sInWellName=MyGetFileTitle(m_SourceFile); //�õ��ľ���
	sInField=MyGetPathName(m_SourceFile); 
	sInField=MyGetFileTitle(sInField); //�õ���������
	CString s;
	BOOL b1=IsAsciiString(DlisWrite->Origin.Company);
	if(!b1)
	{
		strcpy(DlisWrite->Origin.Company,"SiChuan Petroleum Administration"); //COMPANY
	}
	BOOL b2=IsAsciiString(DlisWrite->Origin.FieldName);	
	if(!b2)
	{
		strcpy(DlisWrite->Origin.FieldName,sInField); //�õ�������
	}
	BOOL b3=IsAsciiString(DlisWrite->Origin.WellName);
	if(!b3)
	{
		strcpy(DlisWrite->Origin.WellName,sInWellName); //�õ�����
	}
	if(sp_bAllIgnore) return; //�����޸�

/*	BOOL bModify=b1&b2&b3;
	if(!bModify)
	{
		CSetDlisParaDlg SetDlisParaDlg;
		SetDlisParaDlg.m_FileName=m_SourceFile;
		SetDlisParaDlg.m_Company=DlisWrite->Origin.Company;
		SetDlisParaDlg.m_Field=DlisWrite->Origin.FieldName;
		SetDlisParaDlg.m_Well=DlisWrite->Origin.WellName;
		SetDlisParaDlg.m_Field.Remove('[');
		SetDlisParaDlg.m_Field.Remove(']');
		SetDlisParaDlg.m_Well.Remove('[');
		SetDlisParaDlg.m_Well.Remove(']');
		SetDlisParaDlg.m_DefaultVal=DlisWrite->Origin.DefaultVal;
		SetDlisParaDlg.DoModal();
		//������ȷ������ȡ��������ȡֵ,ȷ��Ϊ�˹�������ȡ��������Ϊȱʡֵ
		strcpy(DlisWrite->Origin.Company,(const char*)SetDlisParaDlg.m_Company);
		strcpy(DlisWrite->Origin.FieldName,(const char*)SetDlisParaDlg.m_Field);
		strcpy(DlisWrite->Origin.WellName,(const char*)SetDlisParaDlg.m_Well);
		DlisWrite->Origin.DefaultVal=SetDlisParaDlg.m_DefaultVal;
		sp_bAllIgnore=SetDlisParaDlg.m_bAllIgnore;
	}*/
}
void CFidToDLis::SetDlisChannelsFromFid()
{
	CString s;
	DlisWrite->SetFrameNumber(NumItem);
	DlisWrite->SetChannelNumber(FidRead->FidHead.NumLog);
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		DlisWrite->Channel[i].Origin=1;  //����O=1,C=0
		DlisWrite->Channel[i].CopyNum=0; //����O=1,C=0
		int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
		strcpy(DlisWrite->Channel[i].Name,NewCurveName.GetAt(i));

		strcpy(DlisWrite->Channel[i].LongName,"");
		strcpy(DlisWrite->Channel[i].Properties,"");
		strcpy(DlisWrite->Channel[i].Units,NewUnits.GetAt(i));
		if(stricmp(DlisWrite->Channel[i].Units,"none")==0)
		{
			strcpy(DlisWrite->Channel[i].Units,"");
		}
		DlisWrite->Channel[i].RepCode=FidRepCodeToDlis(FidRead->FidChannel[i].DimInfo[NumDim].RepCode);
		if(NumDim==1)
		{
			DlisWrite->Channel[i].NumDim=1; //ά��
			DlisWrite->Channel[i].Dimension[0]=1; //άԪ��
		}
		else if(NumDim==2)
		{
			DlisWrite->Channel[i].NumDim=1; //ά��
			DlisWrite->Channel[i].Dimension[0]=FidRead->FidChannel[i].DimInfo[1].Nps; //άԪ��
		}
		else if(NumDim==3)
		{
			DlisWrite->Channel[i].NumDim=2; //ά��
			DlisWrite->Channel[i].Dimension[0]=FidRead->FidChannel[i].DimInfo[1].Nps; //άԪ��
			DlisWrite->Channel[i].Dimension[1]=FidRead->FidChannel[i].DimInfo[2].Nps; //άԪ��
		}
	}
}
void CFidToDLis::SetDlisFrameFromFid()
{
	CString s;
	for(int i=0;i<NumItem;i++)
	{
		DlisWrite->Frame[i].Origin=1;//Դ(����)
		DlisWrite->Frame[i].CopyNum=0;//������(����)
		sprintf(DlisWrite->Frame[i].Name,"B%d",i+1);//֡��(������)
		strcpy(DlisWrite->Frame[i].Index_Type,"BOREHOLE-DEPTH"); //��������
		strcpy(DlisWrite->Frame[i].Direction,"INCREASING"); //�⾮����
		strcpy(DlisWrite->Frame[i].IndexUnits,FidRead->FidChannel[i].DimInfo[0].Unit); //������λ 
		int NumCurve=0; //��֡������
		for(int k=0;k<FidRead->FidHead.NumLog;k++)
		{
			if(ItemNum[k]==i)
			{
				DlisWrite->Frame[i].Index_Start=FidRead->FidChannel[k].DimInfo[0].Start; //������Сֵ
				DlisWrite->Frame[i].Index_Stop=FidRead->FidChannel[k].DimInfo[0].Stop; //�������ֵ
				DlisWrite->Frame[i].Index_Step=FidRead->FidChannel[k].DimInfo[0].Rlev; //����
				DlisWrite->Frame[i].ChannelID[NumCurve]=k;
				NumCurve++;
			}
		}
		DlisWrite->Frame[i].NumCurve=NumCurve; //��֡������
	}
}
void CFidToDLis::SetAxisFromFid()
{
	CString s;
	int NumAxis=0;
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
		if(NumDim<2) continue;
		for(int j=1;j<NumDim;j++)
		{
			if(fabs(FidRead->FidChannel[i].DimInfo[j].Rlev)>0)
			{
				NumAxis++;
			}
		}
	}
	if(NumAxis==0) return;
	DlisWrite->SetDlisAxisNumber(NumAxis);
	int nAx=0;
	for(i=0;i<FidRead->FidHead.NumLog;i++)
	{
		int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
		DlisWrite->Channel[i].nAxis=NumDim-1; //������ά��
		if(NumDim<2) continue;
		DlisWrite->Axis[nAx].NumC=0; //������
		for(int j=1;j<NumDim;j++)
		{
			if(fabs(FidRead->FidChannel[i].DimInfo[j].Rlev)>0)
			{
				DlisWrite->Axis[nAx].Origin=1; //Դ
				DlisWrite->Axis[nAx].CopyNum=0; //������
				sprintf(DlisWrite->Axis[nAx].Name,"%d",1000+nAx); //��������Ӧ�ڶ�����
				s.Format("%s",FidRead->FidChannel[i].DimInfo[j].Name);
				if(s.CompareNoCase("T")==0 || s.CompareNoCase("TIME")==0)
				{
					strcpy(DlisWrite->Axis[nAx]. AxisID,"MICRO_TIME");//��ID��:C=1,R=IDENT
				}
				else
				{
					strcpy(DlisWrite->Axis[nAx].AxisID,"SENSOR_OFFSET");//��ID��:C=1,R=IDENT
				}
				DlisWrite->Axis[nAx].NumC=1; //������
				DlisWrite->Axis[nAx].Coordinates[0]=FidRead->FidChannel[i].DimInfo[j].Start; //����
				DlisWrite->Axis[nAx].Spacing=FidRead->FidChannel[i].DimInfo[j].Rlev; //���
				strcpy(DlisWrite->Axis[nAx].Units,FidRead->FidChannel[i].DimInfo[j].Unit); //���굥λ
				DlisWrite->Channel[i].Axis[j-1].Origin=1; //�᣺R=OBNAME	
				DlisWrite->Channel[i].Axis[j-1].CopyNum=0; //�᣺R=OBNAME	
				strcpy(DlisWrite->Channel[i].Axis[j-1].Name,DlisWrite->Axis[nAx].Name); //�᣺R=OBNAME	
				nAx++;
			}
			else
			{
				//��������Ϊ0,���ø�����Ϊ��
				DlisWrite->Channel[i].Axis[j-1].Origin=1; //�᣺R=OBNAME
				DlisWrite->Channel[i].Axis[j-1].CopyNum=0; //�᣺R=OBNAME
				strcpy(DlisWrite->Channel[i].Axis[j-1].Name,""); //�᣺R=OBNAME
			}
		}
	}
}
void CFidToDLis::FidDataToDlis()
{
	CString s;
	for(int nItem=0;nItem<NumItem;nItem++)
	{
		if(sp_Break_Status) break;
		int NumCurve=DlisWrite->Frame[nItem].NumCurve;
		float Start=DlisWrite->Frame[nItem].Index_Start;
		float Stop=DlisWrite->Frame[nItem].Index_Stop;
		float Step=DlisWrite->Frame[nItem].Index_Step;
		int NumR=int((Stop-Start)/Step+0.5)+1;
		DlisWrite->StartWriteData(nItem);
		pGuage->SetRange(0,NumCurve);
		for(int i=0;i<NumCurve;i++)
		{
			pGuage->SetPos(i+1);
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}

			int index=DlisWrite->Frame[nItem].ChannelID[i];
			CString sCurveName;
			sCurveName.Format("%-.32s",FidRead->FidChannel[index].CurveName);
			if(FidRead->FidChannel[index].NumOfDimension==2)
			{
				float *buf=new float[NumR];
				FidRead->ReadGenData(sCurveName,NumR,buf);
				for(int k=0;k<NumR;k++)
				{
					if(fabs(buf[k]+9999.0)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
					if(fabs(buf[k]+999.25)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
				}
				DlisWrite->WriteCurveData(nItem,i,NumR,buf);
				delete []buf; buf=NULL;
			}
			else
			{
				FidRead->OpenArray(sCurveName);
				int Count=DlisWrite->Channel[index].Sample;
				float *vFloat=new float[Count];
				for(int j=0;j<NumR;j++)
				{
					float depth=Start+j*Step;
					if(!FidRead->ReadArray(depth,vFloat)) break;
					DlisWrite->WriteArrayData(nItem,i,j,Count,vFloat);
				}
				FidRead->CloseArray();
				delete []vFloat ; vFloat=NULL;
			}
		}
		DlisWrite->StopWriteData(nItem);
	}
}
