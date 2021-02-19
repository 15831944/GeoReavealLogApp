// WisToDLis.cpp: implementation of the CWisToDLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "WisToDLis.h"
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

CWisToDLis::CWisToDLis()
{
	ItemNum=NULL;
}

CWisToDLis::~CWisToDLis()
{
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
}

bool CWisToDLis::WisToDLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo)
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

	WisRead=new CWisRead ;
	DlisWrite=new CDlisWrite ; 
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete WisRead; WisRead=NULL;
	delete DlisWrite; DlisWrite=NULL;

	return bResult;
}


bool CWisToDLis::Transform()
{
	if(!IsWIS(m_SourceFile))
	{
		fprintf(fp,"%s is not WIS Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	DlisFileName.Format ("%s\\%s.dlis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	DlisFileName.MakeLower();
	ModiFileName(DlisFileName); //�������������Ƿ��޸��ļ���

	if(!WisRead->Open(m_SourceFile))
	{
		return false;
	}
	WisRead->Open(m_SourceFile);
	WisRead->GetHead();
	WisRead->GetObjectEntry();
	if(WisRead->WisHead.ObjectNumber<1)
	{
		return false;
	}

	SelectCurves();//ѡ�����߻�����߸���
	if(CurveIndex.GetSize()==0)
	{
		WisRead->Close();
		fprintf(fp,"\n!!!!û���������!!!\n\n");
		return false;
	}
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)DlisFileName);
	////////////////////////////////////////////
	GetDlisFDataIndexFromWis(); //��WISͨ����Ϣ���з���
	Prt_FdataItemInfo(); //��ӡ֡������Ϣ
	SetDlisChannelsFromWis(); //����WIS��Ϣ����ͨ����Ϣ
	SetDlisFrameFromWis(); //����DLIS֡��������
	SetAxisFromWis(); //����DLIS���������
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
	WisDataToDlis(); //�������
	DlisWrite->Close(); //�ر�DLIS�ļ�
	DlisWrite->Prt_Header();
	CurveIndex.RemoveAll(); //Դ����������
	SelCurveName.RemoveAll(); //ѡ����������
	NewCurveName.RemoveAll(); //����������
	NewUnits.RemoveAll(); ////�µ�λ����
	return !sp_Break_Status;
}
BYTE CWisToDLis::WisRepCodeToDlis(int RepCode)
{
	BYTE DlisRepC=DLIS_REPC_FSINGL;
	switch(RepCode)
	{
	case WIS_REPR_INT://4�ֽ��з�������
	case WIS_REPR_LONG:   //	4�ֽ��з��ų�������
		DlisRepC=DLIS_REPC_SLONG;
		break;
	case WIS_REPR_SHORT:  //	2�ֽ��з��Ŷ�������
		DlisRepC=DLIS_REPC_SNORM;
		break;
	case WIS_REPR_FLOAT:  //	4�ֽ�IEEE�����ȸ�����
		DlisRepC=DLIS_REPC_FSINGL;
		break;
	case WIS_REPR_DOUBLE: //	8�ֽ�IEE˫���ȸ�����
		DlisRepC=DLIS_REPC_FDOUBL;
		break;
	case WIS_REPR_CHAR:   //	1�ֽ��з����ֽ���
		DlisRepC=DLIS_REPC_SSHORT;
		break;
	case WIS_REPR_UCHAR:  //	1�ֽ��޷����ֽ���
		DlisRepC=DLIS_REPC_USHORT;
		break;
	case WIS_REPR_USHORT: //	2�ֽ��޷��Ŷ�������
		DlisRepC=DLIS_REPC_UNORM;
		break;
	case WIS_REPR_UINT:   //	4�ֽ��޷�������
	case WIS_REPR_ULONG:  //	4�ֽ��޷��ų�������
		DlisRepC=DLIS_REPC_ULONG;
		break;
	default:
		break;
	}
	return DlisRepC;
}
void CWisToDLis::SelectCurves()
{
	CString s;
	if(WisRead->WisHead.ObjectNumber<1)
		return;
	bool *CurveID=new bool[WisRead->WisHead.ObjectNumber];
	for(int i=0;i<WisRead->WisHead.ObjectNumber;i++)
	{
		CurveID[i]=false;
	}
	CString sCurveName;
	switch(m_nCurvesRadio)
	{
	case 0://���߹���	
		for(i=0;i<WisRead->WisHead.ObjectNumber;i++)
		{
			if(WisRead->GetCurveDim(i)<1) continue; //��������ͨ��
			bool bOk=false;
			sCurveName.Format ("%-.15s",WisRead->m_ObjectEntry[i].Name);
			sCurveName.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					bOk=true;
					break;
				}
			}
			CurveID[i]=bOk;
		}
		break;
	case 2://��������
		for(i=0;i<WisRead->WisHead.ObjectNumber;i++)
		{
			if(WisRead->GetCurveDim(i)==1)
			{
				CurveID[i]=true;
			}
		}
		break;
	default://����ѡ���ȫ������
		for(i=0;i<WisRead->WisHead.ObjectNumber;i++)
		{
			if(WisRead->GetCurveDim(i)>0)
			{
				CurveID[i]=true;
			}
		}
		break;
	}
	//////////////////////////////////////////////
	SelCurveName.RemoveAll(); //ѡ����������
	NewCurveName.RemoveAll(); //����������
	NewUnits.RemoveAll(); ////�µ�λ����
	CurveIndex.RemoveAll(); //Դ����������
	for(i=0;i<WisRead->WisHead.ObjectNumber;i++)
	{
		if(!CurveID[i]) continue;
		s.Format("%-.15s",WisRead->m_ObjectEntry[i].Name);
		SelCurveName.Add(s);
		s.TrimRight();
		s.TrimLeft();
		NewCurveName.Add(s);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[i].Name);
		s.Format ("%-.7s",WisRead->WisChannel.Unit);
		s.TrimRight();
		if(s.CompareNoCase("none")==0)
		{
			NewUnits.Add("");
		}
		else
		{
			NewUnits.Add(s);
		}
		CurveIndex.Add(i);
	}
	delete []CurveID; CurveID=NULL;
	///////////////////////////////////////////

	//���ݿ������þ����Ƿ���н���ѡ��
	if(m_nCurvesRadio==3)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=255; //������߳���
		ReCurNamDlg.m_nMaxLog=99999; //���������
		for(int i=0;i<CurveIndex.GetSize();i++)
		{
			int index=CurveIndex.GetAt(i);
			WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
			int NumDim=WisRead->WisChannel.NumOfDimension;
			ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
			ReCurNamDlg.m_Units.Add(NewUnits.GetAt(i));
			ReCurNamDlg.m_ObjectName.Add(NewCurveName.GetAt(i));//Ԥ����������
			ReCurNamDlg.m_NumDim.Add(NumDim);
		}
		SelCurveName.RemoveAll();
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));//ԭ����
			s=ReCurNamDlg.m_ObjectName.GetAt(i);//������
			s.MakeUpper();
			NewCurveName.Add(s);
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));//��λ
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_NumDim.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
		//��������
		CWordArray NewIndex;
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			for(int j=0;j<CurveIndex.GetSize();j++)
			{
				int index=CurveIndex.GetAt(j);
				s.Format ("%-.15s",WisRead->m_ObjectEntry[index].Name);
				if(s.CompareNoCase(SelCurveName.GetAt(i))==0)
				{
					NewIndex.Add(index);
					break;
				}
			}
		}
		//������ֵ����
		CurveIndex.RemoveAll();
		for(i=0;i<NewIndex.GetSize();i++)
		{
			CurveIndex.Add(NewIndex.GetAt(i));
		}
		NewIndex.RemoveAll();
	}
}

void CWisToDLis::GetDlisFDataIndexFromWis()
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
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
		float Start=WisRead->WisChannel.DimInfo[0].StartVal;
		float Rlev=WisRead->WisChannel.DimInfo[0].Delta;
		float Stop=Start+(WisRead->WisChannel.DimInfo[0].Samples-1)*Rlev;
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
void CWisToDLis::Prt_FdataItemInfo()
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
void CWisToDLis::SetDlisChannelsFromWis()
{
	CString s;
	DlisWrite->SetFrameNumber(NumItem);
	DlisWrite->SetChannelNumber(CurveIndex.GetSize());
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
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
		DlisWrite->Channel[i].RepCode=WisRepCodeToDlis(WisRead->WisChannel.RepCode);
		if(WisRead->WisChannel.NumOfDimension==1)
		{
			DlisWrite->Channel[i].NumDim=1; //�������
			DlisWrite->Channel[i].Dimension[0]=1; //άԪ��
			DlisWrite->Channel[i].Dimension[1]=0; //άԪ��
			DlisWrite->Channel[i].Dimension[2]=0; //άԪ��
		}
		else 
		{
			DlisWrite->Channel[i].NumDim=WisRead->WisChannel.NumOfDimension-1; //����
			for(int j=0;j<DlisWrite->Channel[i].NumDim;j++)
			{
				int k=DlisWrite->Channel[i].NumDim-j;
				DlisWrite->Channel[i].Dimension[j]=WisRead->WisChannel.DimInfo[k].Samples;
			}
		}
	}
}
void CWisToDLis::SetDlisFrameFromWis()
{
	CString s;
	CString IndexUnits;
	int index=CurveIndex.GetAt(0); //��һ��������ߵ�����
	WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);

	IndexUnits.Format ("%-.7s",WisRead->WisChannel.DimInfo[0].Unit);
	IndexUnits.TrimRight();
	if(IndexUnits.CompareNoCase("feet")==0 || IndexUnits.CompareNoCase("ft")==0)
	{
		IndexUnits="ft";
	}
	else
	{
		IndexUnits="m";
	}
	for(int i=0;i<NumItem;i++)
	{
		DlisWrite->Frame[i].Origin=1;//Դ(����)
		DlisWrite->Frame[i].CopyNum=0;//������(����)
		sprintf(DlisWrite->Frame[i].Name,"B%d",i+1);//֡��(������)
		strcpy(DlisWrite->Frame[i].Index_Type,"BOREHOLE-DEPTH"); //��������
		strcpy(DlisWrite->Frame[i].Direction,"INCREASING"); //�⾮����
		strcpy(DlisWrite->Frame[i].IndexUnits,IndexUnits); //������λ 
		int NumCurve=0; //��֡������
		for(int k=0;k<CurveIndex.GetSize();k++)
		{
			if(ItemNum[k]==i)
			{
				int index=CurveIndex.GetAt(k);
				WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
				DlisWrite->Frame[i].Index_Start=WisRead->WisChannel.DimInfo[0].StartVal; //������Сֵ
				DlisWrite->Frame[i].Index_Stop=WisRead->WisChannel.DimInfo[0].StartVal+(WisRead->WisChannel.DimInfo[0].Samples-1)*WisRead->WisChannel.DimInfo[0].Delta; //�������ֵ
				DlisWrite->Frame[i].Index_Step=WisRead->WisChannel.DimInfo[0].Delta; //����
				DlisWrite->Frame[i].ChannelID[NumCurve]=k;
				NumCurve++;
			}
		}
		DlisWrite->Frame[i].NumCurve=NumCurve; //��֡������
	}
}
void CWisToDLis::SetAxisFromWis()
{
	CString s;
	int NumAxis=0;
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
		int NumDim=WisRead->WisChannel.NumOfDimension;
		NumAxis+=(NumDim-1);
	}
	fprintf(fp,"====NumAxis=%d\n",NumAxis);
	fflush(fp);
	if(NumAxis==0) return;
	DlisWrite->SetDlisAxisNumber(NumAxis);
	int nAx=0;
	for(i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i); //WIS����������
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
		int NumDim=WisRead->WisChannel.NumOfDimension;
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
		if(WisRead->WisChannel.DimInfo[kDim].Delta>0)
		{
			strcpy(DlisWrite->Axis[nAx]. AxisID,"MICRO_TIME");//��ID��:C=1,R=IDENT
		}
		else
		{
			strcpy(DlisWrite->Axis[nAx]. AxisID,"SENSOR");//��ID��:C=1,R=IDENT
		}
		DlisWrite->Axis[nAx].Coordinates[0]=WisRead->WisChannel.DimInfo[kDim].StartVal; //����
		DlisWrite->Axis[nAx].Spacing=WisRead->WisChannel.DimInfo[kDim].Delta; //���
		s.Format ("%-.7s",WisRead->WisChannel.DimInfo[kDim].Unit);
		s.TrimRight();
		strcpy(DlisWrite->Axis[nAx].Units,s); //���굥λ
		nAx++;
		//�ڶ�ά
		if(DlisWrite->Channel[i].nAxis==2)
		{
			DlisWrite->Axis[nAx].Origin=1; //Դ
			DlisWrite->Axis[nAx].CopyNum=0; //������
			DlisWrite->Axis[nAx].NumC=1; //������
			DlisWrite->Channel[i].Axis[1].Origin=1; //�᣺R=OBNAME	
			DlisWrite->Channel[i].Axis[1].CopyNum=0; //�᣺R=OBNAME	
			sprintf(DlisWrite->Axis[nAx].Name,"%d",1000+nAx); //��������Ӧ�ڶ�����
			strcpy(DlisWrite->Channel[i].Axis[1].Name,DlisWrite->Axis[nAx].Name); //�᣺R=OBNAME	
			strcpy(DlisWrite->Axis[nAx]. AxisID,"SENSOR");//��ID��:C=1,R=IDENT
			DlisWrite->Axis[nAx].Coordinates[0]=WisRead->WisChannel.DimInfo[1].StartVal; //����
			DlisWrite->Axis[nAx].Spacing=WisRead->WisChannel.DimInfo[1].Delta; //���
			s.Format ("%-.7s",WisRead->WisChannel.DimInfo[1].Unit);
			s.TrimRight();
			strcpy(DlisWrite->Axis[nAx].Units,s); //���굥λ
			nAx++;
		}
	}
}
void CWisToDLis::SetStoUnitLab()
{
	char str[256];
	memcpy(DlisWrite->StoUnitLab.SeqNum,"   1",4); //�������
	memcpy(DlisWrite->StoUnitLab.DLISVer,"V1.00",5); //�汾��
	memcpy(DlisWrite->StoUnitLab.Stru,"RECORD",6); //�洢���ṹ
	sprintf(str,"%5d",DlisWrite->MaxPhyLen); 
	memcpy(DlisWrite->StoUnitLab.MaxRecLen,str,5); 
	sprintf(str,"%-60.60s","WIS to DLIS Conversion"); //�洢����־
	memcpy(DlisWrite->StoUnitLab.StoSetIden,str,60);
}
void CWisToDLis::SetFileHeader()
{
	sprintf(DlisWrite->FileHeader.SequenceNumber,"%10.10s","1");//���к�
	sprintf(DlisWrite->FileHeader.ID,"%-65.65s","WIS.001");//ID��
}
void CWisToDLis::SetOrigin()
{
	CString s;
	strcpy(DlisWrite->Origin.FileID,"WIS.001");//FILE-ID
	strcpy(DlisWrite->Origin.FileSetName,"WIS-TO-DLIS-CONVERSION");//FILE-SET-NAME
	strcpy(DlisWrite->Origin.FileSetNumber,"1");//FILE-SET-NUMBER
	strcpy(DlisWrite->Origin.FileNumber,"1"); //FILE-NUMBER
	strcpy(DlisWrite->Origin.FileType,"RE-FMT"); //FILE-TYPE
	strcpy(DlisWrite->Origin.Product,"GeoReveal"); //PRODUCT
	strcpy(DlisWrite->Origin.Version,"1.0"); //VERSION
	strcpy(DlisWrite->Origin.Programs,"WIS-TO-DLIS"); //PROGRAMS
	CTime t=CTime::GetCurrentTime(); //ȡ��ǰϵͳ���ں�ʱ��
	sprintf(DlisWrite->Origin.CreationTime,"YEAR=%d MONTH=%d DAY=%d HOUR=%d MINUTE=%d SECOND=%d",
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),
		t.GetSecond()); //CREATION-TIME
	strcpy(DlisWrite->Origin.OrderNumber,""); //ORDER-NUMBER
	strcpy(DlisWrite->Origin.DescentNumber,""); //DESCENT-NUMBER
	strcpy(DlisWrite->Origin.RunNumber,""); //RUN-NUMBER
	strcpy(DlisWrite->Origin.WellID,""); //WELL-ID
	strcpy(DlisWrite->Origin.WellName,""); //WELL-NAME
	strcpy(DlisWrite->Origin.FieldName,""); //FIELD-NAME
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

void CWisToDLis::SetDlisParameter()
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

void CWisToDLis::WisDataToDlis()
{
	CString s;
	int NumLog=-1; //�������������
	pGuage->m_nTextMode=1;
	for(int nItem=0;nItem<NumItem;nItem++)
	{
		if(sp_Break_Status) break;
		int NumCurve=DlisWrite->Frame[nItem].NumCurve;
		float Start=DlisWrite->Frame[nItem].Index_Start;
		float Stop=DlisWrite->Frame[nItem].Index_Stop;
		float Step=DlisWrite->Frame[nItem].Index_Step;
		int NumPoint=int((Stop-Start)/Step+0.5)+1;
		DlisWrite->StartWriteData(nItem);
		pGuage->SetRange(0,100);
		for(int i=0;i<NumCurve;i++)
		{
			if(sp_Break_Status) break;
			NumLog++; //�����������
			//index������DLISͨ��������
			int index=DlisWrite->Frame[nItem].ChannelID[i]; 
			int IndexWis=CurveIndex.GetAt(index); //������WIS������
			WisRead->OpenChannel(WisRead->m_ObjectEntry[IndexWis].Name);
			s=SelCurveName.GetAt(index)+"("+NewCurveName.GetAt(index)+")";
			s.Remove(' ');
			fprintf(fp,"====Write: %-4d %-20s %5d\n",NumLog,s,DlisWrite->Channel[index].Sample);
			pGuage->m_sTextInfo=SelCurveName.GetAt(index);
			pGuage->SetPos((i+1)*100/NumCurve);
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			/////////////////////////////////////////////////////////////////
			if(WisRead->WisChannel.NumOfDimension==1)
			{
				float depth=WisRead->WisChannel.DimInfo[0].StartVal;
				float *buf=new float[NumPoint];
				WisRead->ReadCurve(IndexWis,depth,NumPoint,buf);
				DlisWrite->WriteCurveData(nItem,i,NumPoint,buf);
				delete []buf; buf=NULL;
			}
			else
			{
				int Count=DlisWrite->Channel[index].Sample;
				float *buf=new float[Count];
				for(int j=0;j<Count;j++)
				{
					buf[j]=0;
				}
				pGuage->SetRange(0,100);
				for(j=0;j<NumPoint;j++)
				{
					pGuage->SetPos(((j+1)*100)/NumPoint);
					if (pGuage->CheckCancelButton())
					{
						sp_Break_Status=true;
						break;
					}
					float depth=Start+j*Step;
					WisRead->ReadWaveArray(IndexWis,depth,buf);
					DlisWrite->WriteArrayData(nItem,i,j,Count,buf);
				}
				delete []buf ; buf=NULL;
			}
		}
		pGuage->m_sTextInfo="����ʱ�ļ�д��DLIS�ļ�";
		pGuage->SetPos(75);
		DlisWrite->StopWriteData(nItem);
	}
}
