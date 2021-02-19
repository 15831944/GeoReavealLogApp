// LisToDlis.cpp: implementation of the CLisToDlis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "LisToDlis.h"
#include <math.h>
#include "SetDlisParaDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLisToDlis::CLisToDlis()
{
	ItemNum=NULL;
}

CLisToDlis::~CLisToDlis()
{
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
	
}
bool CLisToDlis::LisToDlis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo)
{
	CString s;
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
	
	LisRead=new CLisRead ;
	DlisWrite=new CDlisWrite ; //�����
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete DlisWrite; DlisWrite=NULL;
	delete LisRead;	LisRead=NULL;
	
	CurveIndex.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	return bResult;
}
bool CLisToDlis::Transform()
{
	CString s;
	if(!IsLIS(m_SourceFile))
	{
		fprintf(fp,"%s is not LIS Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ������ļ���
	DlisFileName.Format ("%s\\%s.dlis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	DlisFileName.MakeLower();
	ModiFileName(DlisFileName); //�������������Ƿ��޸��ļ���

	if(!LisRead->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
	}
	else
	{
		fprintf(fp,"Source: %s\n",m_SourceFile);
		fprintf(fp,"Object: %s\n",DlisFileName);
		SetStoUnitLab(); //����DLIS�洢��־
		DlisWrite->Open(DlisFileName,fp); //��DLIS�ļ�
		DlisWrite->SetTrailLengthFlag(); //��¼β��������
		DlisWrite->WriteStoUnitLab(); //дDLIS�洢��־
		LisRead->GetTapeProducer(sProducerName,OriginOfdata);
		int NumFile=0;
		while(1)
		{
			if(!LisRead->ScanLisOneLogicalFile(fp))break;
			if(LisRead->NumLog>0)
			{
				NumFile++;
				LisRead->SetLisPos(LisRead->LisLogicalFileStartPos);
				bool bResult=LogicalToDlis(NumFile);
				if(sp_Break_Status) break;
				//���ת�������Ϣ
				if(bResult)
					DlisWrite->Prt_Header();
			}
			//LIS���ļ���һ���߼��ļ�λ��
			LisRead->SetLisPos(LisRead->LisLogicalFileStopPos);
		}
		DlisWrite->Close(); //�ر�DLIS�ļ�
		LisRead->Close(); //�ر������ļ�
	}
	return !sp_Break_Status;
}
bool CLisToDlis::LogicalToDlis(int NumFile)
{
	CString s;
	SelectCurves();
	if(CurveIndex.GetSize()==0) return false;
	GetDlisFDataIndexFromLis();
	Prt_FdataItemInfo();
	SetDlisChannelsFromLis();
	SetDlisFrameFromLis();
	SetAxisFromLis();
	SetFileHeader(NumFile);
	SetOrigin(NumFile);
	if(NumFile==1)
	{
		SetDlisParameter();
	}
	DlisWrite->WriteFileHeader(); //д�ļ�ͷ��Ϣ��¼
	DlisWrite->WriteOrigin(); //дԴ��Ϣ��¼
	DlisWrite->WriteParameter(); //д��̬������¼
	DlisWrite->WriteChannel(); //дͨ����Ϣ��¼
	DlisWrite->WriteFrame(); //д֡��Ϣ��¼
	DlisWrite->WriteAxis(); //д����Ϣ��¼
	LisDataToDlis(); //�������
	return true;
}
void CLisToDlis::LisDataToDlis()
{
	fflush(fp);
	CString s;
	LisRead->ReadLogicalFile(); //LIS���ݶ���һ����ʱ�ļ�
	for(int nItem=0;nItem<NumItem;nItem++)
	{
		if(sp_Break_Status) break;
		int NumCurve=DlisWrite->Frame[nItem].NumCurve;
		float Start=DlisWrite->Frame[nItem].Index_Start;
		float Stop=DlisWrite->Frame[nItem].Index_Stop;
		float Step=DlisWrite->Frame[nItem].Index_Step;
		DlisWrite->StartWriteData(nItem);
		pGuage->SetRange(0,NumCurve);
		for(int i=0;i<NumCurve;i++)
		{
			if(sp_Break_Status) break;
			pGuage->SetPos(i+1);
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			//index������DLISͨ��������
			int index=DlisWrite->Frame[nItem].ChannelID[i]; 
			int IndexLis=CurveIndex.GetAt(index); //������BIT������
			s=SelCurveName.GetAt(index)+"("+NewCurveName.GetAt(index)+")";
			s.Remove(' ');
			/////////////////////////////////////////////////////////////////
			if(LisRead->Curve[IndexLis].NumOfDimension==1)
			{
				int NumR=LisRead->NumPoint*LisRead->Curve[IndexLis].Samples;
				fprintf(fp,"====Transform Curve: %-10s %5d\n",s,DlisWrite->Channel[index].Sample);

				float *buf=new float[NumR];
				LisRead->ReadCurve(IndexLis,NumR,buf);
				DlisWrite->WriteCurveData(nItem,i,NumR,buf);
				delete []buf; buf=NULL;
			}
			else
			{
				int Count=LisRead->Curve[IndexLis].Samples*LisRead->Curve[IndexLis].NumX;
				fprintf(fp,"====Transform Waveform: %-10s %5d\n",s,Count);
				float *buf=new float[Count];
				for(int j=0;j<Count;j++)
				{
					buf[j]=0;
				}
				bool bDownLogMode=true; //�ϲ���²⿪��
				if(LisRead->StDep>LisRead->EnDep)
				{
					bDownLogMode=false;
				}

				pGuage->SetRange(0,LisRead->NumPoint);
				for(j=0;j<LisRead->NumPoint;j++)
				{
					pGuage->SetPos(j+1);
					if (pGuage->CheckCancelButton())
					{
						sp_Break_Status=true;
						break;
					}
					float DepFram=LisRead->StDep+j*LisRead->Rlev;
					LisRead->ReadWave(IndexLis,DepFram,buf);
					int NumX=LisRead->Curve[IndexLis].NumX;
					int NumY=LisRead->Curve[IndexLis].Samples;
					for(int k=0;k<NumY;k++)
					{
						int noFrame=j*NumY+k;
						if(!bDownLogMode)
						{	//�ϲ�ģʽ
							noFrame=LisRead->NumPoint*NumY-1-noFrame;
						}
						DlisWrite->WriteArrayData(nItem,i,noFrame,NumX,&buf[k*NumX]);
					}
				}
				delete []buf ; buf=NULL;
			}
		}
		DlisWrite->StopWriteData(nItem);
	}
}

void CLisToDlis::SelectCurves()
{
	CString s;
	CurveIndex.RemoveAll();
	CString sCurveName;
	if(m_nCurvesRadio==3)
	{	//����ѡ��
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=15; //��������󳤶�
		ReCurNamDlg.m_nMaxLog=512; //DLIS�ļ��������������
		ReCurNamDlg.m_NumLog=LisRead->NumLog; //ʵ��������
		for(int i=0;i<LisRead->NumLog;i++)
		{
			sCurveName.Format ("%-.4s",LisRead->Curve[i].Name);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			ReCurNamDlg.m_ObjectName.Add(sCurveName);
			s.Format ("%-4.4s",LisRead->Curve[i].Units);
			s.TrimRight();
			ReCurNamDlg.m_Units.Add(s);
			ReCurNamDlg.m_NumDim.Add(LisRead->Curve[i].NumOfDimension);
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
			for(int j=0;j<LisRead->NumLog;j++)
			{
				sCurveName.Format ("%-.4s",LisRead->Curve[j].Name);
				sCurveName.TrimLeft();
				sCurveName.TrimRight();
				if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
				{
					CurveIndex.Add(j);
					break;
				}
			}
		}
	}
	else
	{
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		for(int i=0;i<LisRead->NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.4s",LisRead->Curve[i].Name);
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
			else if(m_nCurvesRadio==2)
			{
				if(LisRead->Curve[i].NumX==1)
				{
					bOk=true;
				}
			}
			else if(m_nCurvesRadio==1)
			{
				bOk=true;
			}
			//////////////////////////////
			if(bOk)
			{
				CurveIndex.Add(i);
				s.Format("%-.4s",LisRead->Curve[i].Name);
				SelCurveName.Add(s);
				s.TrimRight();
				NewCurveName.Add(s);
				s.Format ("%-.4s",LisRead->Curve[i].Units);
				s.TrimRight();
				NewUnits.Add(s);
			}
		}
	}
}
void CLisToDlis::SetStoUnitLab()
{
	char str[256];
	memcpy(DlisWrite->StoUnitLab.SeqNum,"   1",4); //�������
	memcpy(DlisWrite->StoUnitLab.DLISVer,"V1.00",5); //�汾��
	memcpy(DlisWrite->StoUnitLab.Stru,"RECORD",6); //�洢���ṹ
	sprintf(str,"%5d",DlisWrite->MaxPhyLen); 
	memcpy(DlisWrite->StoUnitLab.MaxRecLen,str,5); 
	sprintf(str,"%-60.60s","LIS to DLIS Conversion"); //�洢����־
	memcpy(DlisWrite->StoUnitLab.StoSetIden,str,60);
}
void CLisToDlis::GetDlisFDataIndexFromLis()
{
	CString s;
	typedef struct tagCurve
	{
		float Start;
		float Stop;
		float Rlev;
	}_Curve;
	const double er=0.001;
	NumItem=0; //����
	if(CurveIndex.GetSize()==0) return;
	_Curve *Curve=new _Curve[CurveIndex.GetSize()];
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
	ItemNum=new int[CurveIndex.GetSize()];
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		float Start=LisRead->StDep;
		float Rlev=LisRead->Rlev/LisRead->Curve[index].Samples;
		float Stop=LisRead->EnDep;
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
void CLisToDlis::Prt_FdataItemInfo()
{
	for(int i=0;i<NumItem;i++)
	{
		fprintf(fp,"��%2d��: ",i+1);
		int nc=0;
		for(int j=0;j<CurveIndex.GetSize();j++)
		{
			if(ItemNum[j]==i)
			{
				fprintf(fp," %-8.8s",NewCurveName.GetAt(j));
				nc++;
				if(nc%5==0)
				{
					fprintf(fp,"\n%-8s","");
				}
			}
		}
		fprintf(fp,"\n");
	}
}
void CLisToDlis::SetDlisChannelsFromLis()
{
	CString s;
	DlisWrite->SetFrameNumber(NumItem);
	DlisWrite->SetChannelNumber(CurveIndex.GetSize());
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		DlisWrite->Channel[i].Origin=1;  //����O=1,C=0
		DlisWrite->Channel[i].CopyNum=0; //����O=1,C=0
		strcpy(DlisWrite->Channel[i].Name,NewCurveName.GetAt(i));

		strcpy(DlisWrite->Channel[i].LongName,"");
		strcpy(DlisWrite->Channel[i].Properties,"");
		strcpy(DlisWrite->Channel[i].Units,NewUnits.GetAt(i));
		if(stricmp(DlisWrite->Channel[i].Units,"none")==0)
		{
			strcpy(DlisWrite->Channel[i].Units,"");
		}
		if(LisRead->Curve[index].NumOfDimension==1)
		{
			DlisWrite->Channel[i].RepCode=DLIS_REPC_FSINGL;
			DlisWrite->Channel[i].NumDim=1; //�������
			DlisWrite->Channel[i].Dimension[0]=1; //άԪ��
			DlisWrite->Channel[i].Dimension[1]=0; //άԪ��
			DlisWrite->Channel[i].Dimension[2]=0; //άԪ��
		}
		else
		{
			DlisWrite->Channel[i].RepCode=FidRepCToDlis(LisRead->Curve[index].iRepC);
			DlisWrite->Channel[i].NumDim=1; //����
			DlisWrite->Channel[i].Dimension[0]=LisRead->Curve[index].NumX;//άԪ��
			DlisWrite->Channel[i].Dimension[1]=0; //άԪ��
			DlisWrite->Channel[i].Dimension[2]=0; //άԪ��
		}
	}
}
BYTE CLisToDlis::FidRepCToDlis(BYTE FidRepC)
{
	BYTE DlisRepC=DLIS_REPC_FSINGL;
	switch(FidRepC)
	{
	case LIS_REPC_FLOAT16:
	case LIS_REPC_FLOAT32LOW:
	case LIS_REPC_FLOAT32:
	case LIS_REPC_FLOAT32FIX:
		DlisRepC=DLIS_REPC_FSINGL;
		break;
	case LIS_REPC_INT8:
		DlisRepC=DLIS_REPC_SSHORT;
		break;
	case LIS_REPC_UINT8:
		DlisRepC=DLIS_REPC_USHORT;
		break;
	case LIS_REPC_INT32:
		DlisRepC=DLIS_REPC_SLONG;
		break;
	case LIS_REPC_INT16:
		DlisRepC=DLIS_REPC_SNORM;
		break;
	default:
		DlisRepC=DLIS_REPC_SSHORT;
	}
	return DlisRepC;
}
void CLisToDlis::SetDlisFrameFromLis()
{
	CString s;
	for(int i=0;i<NumItem;i++)
	{
		DlisWrite->Frame[i].Origin=1;//Դ(����)
		DlisWrite->Frame[i].CopyNum=0;//������(����)
		sprintf(DlisWrite->Frame[i].Name,"B%d",i+1);//֡��(������)
		strcpy(DlisWrite->Frame[i].Index_Type,"BOREHOLE-DEPTH"); //��������
		strcpy(DlisWrite->Frame[i].Direction,"INCREASING"); //�⾮����
		strcpy(DlisWrite->Frame[i].IndexUnits,"m"); //������λ 
		int NumCurve=0; //��֡������
		for(int k=0;k<CurveIndex.GetSize();k++)
		{
			if(ItemNum[k]==i)
			{
				int index=CurveIndex.GetAt(k);
				float Start=LisRead->StDep;
				float Stop=LisRead->EnDep;
				float Rlev=float(fabs(LisRead->Rlev/LisRead->Curve[index].Samples));
				if(Start<Stop)
				{
					DlisWrite->Frame[i].Index_Start=Start; //������Сֵ
					DlisWrite->Frame[i].Index_Stop=Stop+(LisRead->Curve[index].Samples-1)*Rlev; //�������ֵ
				}
				else
				{
					DlisWrite->Frame[i].Index_Start=Stop; //������Сֵ
					DlisWrite->Frame[i].Index_Stop=Start+(LisRead->Curve[index].Samples-1)*Rlev; //�������ֵ
				}
				DlisWrite->Frame[i].Index_Step=Rlev; //����
				DlisWrite->Frame[i].ChannelID[NumCurve]=k;
				NumCurve++;
			}
		}
		DlisWrite->Frame[i].NumCurve=NumCurve; //��֡������
	}
}
void CLisToDlis::SetAxisFromLis()
{
	CString s;
	int NumAxis=0;
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		if(LisRead->Curve[index].NumX>1)
		{
			NumAxis+=1;
		}
	}
	fprintf(fp,"====NumAxis=%d\n",NumAxis);
	fflush(fp);
	if(NumAxis==0) return;
	DlisWrite->SetDlisAxisNumber(NumAxis);
	int nAx=0;
	for(i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i); //LIS����������
		int NumDim=1;
		if(LisRead->Curve[index].NumX>1)
		{
			NumDim=2;
		}
		DlisWrite->Channel[i].nAxis=NumDim-1; //������ά��
		if(DlisWrite->Channel[i].nAxis==0) continue;
		DlisWrite->Axis[nAx].NumC=0; //������
		//��һά
		int kDim=NumDim-1;
		DlisWrite->Axis[nAx].Origin=1; //Դ
		DlisWrite->Axis[nAx].CopyNum=0; //������
		DlisWrite->Axis[nAx].NumC=1; //������
		DlisWrite->Channel[i].Axis[0].Origin=1; //�᣺R=OBNAME	
		DlisWrite->Channel[i].Axis[0].CopyNum=0; //�᣺R=OBNAME	
		sprintf(DlisWrite->Axis[nAx].Name,"%d",1000+nAx); //��������Ӧ�ڶ�����
		strcpy(DlisWrite->Channel[i].Axis[0].Name,DlisWrite->Axis[nAx].Name); //�᣺R=OBNAME
		s.Format ("%-.4s",LisRead->Curve[index].Name);
		if(s.Left(2)==_T("WF"))
		{
			if(sProducerName=="SCHLUMBERGER")
			{
				strcpy(DlisWrite->Axis[nAx]. AxisID,"MICRO_TIME");//��ID��:C=1,R=IDENT
				DlisWrite->Axis[nAx].Coordinates[0]=200; //����
				DlisWrite->Axis[nAx].Spacing=5; //���
				strcpy(DlisWrite->Axis[nAx].Units,"us"); //���굥λ
			}
			else if(sProducerName=="HALLIBURTON")
			{
				strcpy(DlisWrite->Axis[nAx]. AxisID,"MICRO_TIME");//��ID��:C=1,R=IDENT
				DlisWrite->Axis[nAx].Coordinates[0]=0; //����
				DlisWrite->Axis[nAx].Spacing=4; //���
				strcpy(DlisWrite->Axis[nAx].Units,"us"); //���굥λ
			}
			else
			{
				strcpy(DlisWrite->Axis[nAx]. AxisID,"MICRO_TIME");//��ID��:C=1,R=IDENT
				DlisWrite->Axis[nAx].Coordinates[0]=0; //����
				DlisWrite->Axis[nAx].Spacing=1; //���
				strcpy(DlisWrite->Axis[nAx].Units,"us"); //���굥λ
			}
		}
		else
		{
			strcpy(DlisWrite->Axis[nAx]. AxisID,"SENSOR");//��ID��:C=1,R=IDENT
			DlisWrite->Axis[nAx].Coordinates[0]=1; //����
			DlisWrite->Axis[nAx].Spacing=1; //���
			strcpy(DlisWrite->Axis[nAx].Units,"m"); //���굥λ
		}
		nAx++;
	}
}
void CLisToDlis::SetFileHeader(int NumFile)
{
	char str[256];
	sprintf(DlisWrite->FileHeader.SequenceNumber,"%10d",NumFile);//���к�
	sprintf(str,"LIS.%03d",NumFile);
	sprintf(DlisWrite->FileHeader.ID,"%-65.65s",str);//ID��
}
void CLisToDlis::SetOrigin(int NumFile)
{
	CString s;
	char str[256];
	sprintf(str,"LIS.%03d",NumFile);
	memcpy(DlisWrite->Origin.FileID,str,6);//FILE-ID
	strcpy(DlisWrite->Origin.FileSetName,"BIT-TO-DLIS-CONVERSION");//FILE-SET-NAME
	sprintf(DlisWrite->Origin.FileSetNumber,"%d",NumFile);//FILE-SET-NUMBER
	sprintf(DlisWrite->Origin.FileNumber,"%d",NumFile); //FILE-NUMBER
	strcpy(DlisWrite->Origin.FileType,"RE-FMT"); //FILE-TYPE
	strcpy(DlisWrite->Origin.Product,"GeoReveal "); //PRODUCT
	strcpy(DlisWrite->Origin.Version,"1.0"); //VERSION
	strcpy(DlisWrite->Origin.Programs,"BIT-TO-DLIS"); //PROGRAMS
	CTime t=CTime::GetCurrentTime(); //ȡ��ǰϵͳ���ں�ʱ��
	sprintf(DlisWrite->Origin.CreationTime,"YEAR=%d MONTH=%d DAY=%d HOUR=%d MINUTE=%d SECOND=%d",
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),
		t.GetSecond()); //CREATION-TIME
	strcpy(DlisWrite->Origin.OrderNumber,""); //ORDER-NUMBER
	strcpy(DlisWrite->Origin.DescentNumber,""); //DESCENT-NUMBER
	strcpy(DlisWrite->Origin.RunNumber,""); //RUN-NUMBER
	strcpy(DlisWrite->Origin.WellID,""); //WELL-ID
	if(NumFile==1)
	{	//�״ε���ʱ����
		strcpy(DlisWrite->Origin.WellName,""); //WELL-NAME
		strcpy(DlisWrite->Origin.FieldName,""); //FIELD-NAME
		strcpy(DlisWrite->Origin.Company,""); //COMPANY
	}
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
	strcpy(DlisWrite->Origin.NameSpaceVersion,"0"); //NAME-SPACE-VERSION
}
void CLisToDlis::SetDlisParameter()
{
	CString sInWellName=MyGetFileTitle(m_SourceFile); //�õ��ľ���
	CString sInField=MyGetPathName(m_SourceFile); 
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
