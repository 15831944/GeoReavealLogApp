// XtfToDLis.cpp: implementation of the CXtfToDLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "XtfToDLis.h"
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

CXtfToDLis::CXtfToDLis()
{
	ItemNum=NULL;
}

CXtfToDLis::~CXtfToDLis()
{
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
}
BYTE CXtfToDLis::XtfRepCodeToDlis(int RepCode)
{
	BYTE DlisRepC=DLIS_REPC_FSINGL;
	switch(RepCode)
	{
	case XTF_REPR_BIT://λֵ
	case XTF_REPR_CHAR://�з����ֽ���
		DlisRepC=DLIS_REPC_SSHORT;
		break;
	case XTF_REPR_INT2://�з��Ŷ�������
		DlisRepC=DLIS_REPC_SNORM;
		break;
	case XTF_REPR_UCHAR://�޷����ֽ�
		DlisRepC=DLIS_REPC_USHORT;
		break;
	case XTF_REPR_FLOAT://IEEE�����ȸ�����	
		DlisRepC=DLIS_REPC_FSINGL;
		break;
	case XTF_REPR_DOUBLE://˫���ȸ���
		DlisRepC=DLIS_REPC_FDOUBL;
		break;
	case XTF_REPR_UINT2://�޷��Ŷ�������
		DlisRepC=DLIS_REPC_UNORM;
		break;
	case XTF_REPR_INT4://������
		DlisRepC=DLIS_REPC_SLONG;
		break;
	case XTF_REPR_UINT4://�޷��ų�������
		DlisRepC=DLIS_REPC_ULONG;
		break;
	default:
		break;
	}
	return DlisRepC;
}
bool CXtfToDLis::XtfToDLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo)
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

	XtfRead=new CXtfRead ;
	DlisWrite=new CDlisWrite ; 
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete XtfRead; XtfRead=NULL;
	delete DlisWrite; DlisWrite=NULL;

	return bResult;
}


bool CXtfToDLis::Transform()
{
	if(!IsXTF(m_SourceFile))
	{
		fprintf(fp,"%s is not XTF Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	DlisFileName.Format ("%s\\%s.dlis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	DlisFileName.MakeLower();
	ModiFileName(DlisFileName); //�������������Ƿ��޸��ļ���


	if(!XtfRead->Open(m_SourceFile))
		return false;
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)DlisFileName);
	XtfRead->GetHead();
	SelectCurves();//ѡ�����߻�����߸���
	if(CurveIndex.GetSize()==0)
	{
		XtfRead->Close();
		fprintf(fp,"\n!!!!û���������!!!\n\n");
		return false;
	}
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)DlisFileName);
	////////////////////////////////////////////
	GetDlisFDataIndexFromXtf(); //��XTFͨ����Ϣ���з���
	Prt_FdataItemInfo(); //��ӡ֡������Ϣ
	SetDlisChannelsFromXtf(); //����XTF��Ϣ����ͨ����Ϣ
	SetDlisFrameFromXtf(); //����DLIS֡��������
	SetAxisFromXtf(); //����DLIS���������
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
	XtfDataToDlis(); //�������
	CurveIndex.RemoveAll(); //Դ����������
	SelCurveName.RemoveAll(); //ѡ����������
	NewCurveName.RemoveAll(); //����������
	NewUnits.RemoveAll(); ////�µ�λ����
	DlisWrite->Close(); //�ر�DLIS�ļ�
	DlisWrite->Prt_Header();
	return !sp_Break_Status;
}

void CXtfToDLis::SelectCurves()
{
	CString s;
	bool *CurveID=new bool[XtfRead->XtfHead.isnumcv];
	for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
	{
		CurveID[i]=false;
	}
	CString sCurveName;
	switch(m_nCurvesRadio)
	{
	case 0://���߹���
		for(i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.8s",XtfRead->XtfHead.CurveName[i]);
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
		for(i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			if(XtfRead->XtfHead.CurveAttrib[i].CurType==1)
			{
				CurveID[i]=true;
			}
		}
		break;
	default://����ѡ���ȫ������
		for(i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			CurveID[i]=true;
		}
		break;
	}
	//////////////////////////////////////////////
	SelCurveName.RemoveAll(); //ѡ����������
	NewCurveName.RemoveAll(); //����������
	NewUnits.RemoveAll(); ////�µ�λ����
	CurveIndex.RemoveAll(); //Դ����������
	for(i=0;i<XtfRead->XtfHead.isnumcv;i++)
	{
		if(!CurveID[i]) continue;
		s.Format("%-8.8s",XtfRead->XtfHead.CurveName[i]);
		SelCurveName.Add(s);
		s.TrimRight();
		NewCurveName.Add(s);
		s.Format("%-8.8s",XtfRead->XtfHead.Units[i]);
		s.TrimRight();
		NewUnits.Add(s);
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
			int NumDim=XtfRead->XtfHead.CurveAttrib[index].CurType;
			ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
			ReCurNamDlg.m_Units.Add(NewUnits.GetAt(i));
			ReCurNamDlg.m_ObjectName.Add(NewCurveName.GetAt(i));//Ԥ����������
			ReCurNamDlg.m_NumDim.Add(NumDim);
		}
		SelCurveName.RemoveAll();
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		CurveIndex.RemoveAll();
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
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			for(int j=0;j<XtfRead->XtfHead.isnumcv;j++)
			{
				s.Format ("%-8.8s",XtfRead->XtfHead.CurveName[j]);
				if(s.CompareNoCase(SelCurveName.GetAt(i))==0)
				{
					CurveIndex.Add(j);
					break;
				}
			}
		}
	}
	//////////////////////////////////////////////
}

void CXtfToDLis::GetDlisFDataIndexFromXtf()
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
		float Start=XtfRead->XtfHead.Sdep[index];
		float Stop=XtfRead->XtfHead.Edep[index];
		float Rlev=XtfRead->XtfHead.Level[index];
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
void CXtfToDLis::Prt_FdataItemInfo()
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
void CXtfToDLis::SetStoUnitLab()
{
	char str[256];
	memcpy(DlisWrite->StoUnitLab.SeqNum,"   1",4); //�������
	memcpy(DlisWrite->StoUnitLab.DLISVer,"V1.00",5); //�汾��
	memcpy(DlisWrite->StoUnitLab.Stru,"RECORD",6); //�洢���ṹ
	sprintf(str,"%5d",DlisWrite->MaxPhyLen); 
	memcpy(DlisWrite->StoUnitLab.MaxRecLen,str,5); 
	sprintf(str,"%-60.60s","XTF to DLIS Conversion"); //�洢����־
	memcpy(DlisWrite->StoUnitLab.StoSetIden,str,60);
}
void CXtfToDLis::SetFileHeader()
{
	sprintf(DlisWrite->FileHeader.SequenceNumber,"%10.10s","1");//���к�
	sprintf(DlisWrite->FileHeader.ID,"%-65.65s","XTF.001");//ID��
}
void CXtfToDLis::SetOrigin()
{
	CString s;
	strcpy(DlisWrite->Origin.FileID,"XTF.001");//FILE-ID
	strcpy(DlisWrite->Origin.FileSetName,"XTF-TO-DLIS-CONVERSION");//FILE-SET-NAME
	strcpy(DlisWrite->Origin.FileSetNumber,"1");//FILE-SET-NUMBER
	strcpy(DlisWrite->Origin.FileNumber,"1"); //FILE-NUMBER
	strcpy(DlisWrite->Origin.FileType,"RE-FMT"); //FILE-TYPE
	strcpy(DlisWrite->Origin.Product,"GeoReveal"); //PRODUCT
	strcpy(DlisWrite->Origin.Version,"1.0"); //VERSION
	strcpy(DlisWrite->Origin.Programs,"XTF-TO-DLIS"); //PROGRAMS
	CTime t=CTime::GetCurrentTime(); //ȡ��ǰϵͳ���ں�ʱ��
	sprintf(DlisWrite->Origin.CreationTime,"YEAR=%d MONTH=%d DAY=%d HOUR=%d MINUTE=%d SECOND=%d",
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),
		t.GetSecond()); //CREATION-TIME
	strcpy(DlisWrite->Origin.OrderNumber,""); //ORDER-NUMBER
	strcpy(DlisWrite->Origin.DescentNumber,""); //DESCENT-NUMBER
	strcpy(DlisWrite->Origin.RunNumber,""); //RUN-NUMBER
	strcpy(DlisWrite->Origin.WellID,""); //WELL-ID
	s.Format ("%-80.s",XtfRead->XtfHead.Wellname);
	s.TrimLeft();
	s.TrimRight();
	strcpy(DlisWrite->Origin.WellName,s); //WELL-NAME
	s.Format ("%-80.s",XtfRead->XtfHead.Fieldname);
	s.TrimLeft();
	s.TrimRight();
	strcpy(DlisWrite->Origin.FieldName,s); //FIELD-NAME
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
	s.Format ("%-.80s",XtfRead->XtfHead.Companyname);
	s.TrimRight();
	strcpy(DlisWrite->Origin.Company,s); //COMPANY
	strcpy(DlisWrite->Origin.NameSpaceVersion,"0"); //NAME-SPACE-VERSION
}

void CXtfToDLis::SetDlisParameter()
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
void CXtfToDLis::SetDlisChannelsFromXtf()
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
		DlisWrite->Channel[i].RepCode=XtfRepCodeToDlis(XtfRead->XtfHead.CurveAttrib[index].RepCode);
		DlisWrite->Channel[i].NumDim=XtfRead->XtfHead.ndimension[index]; //ά��
		DlisWrite->Channel[i].Dimension[0]=XtfRead->XtfHead.idimen1[index]; //άԪ��
		DlisWrite->Channel[i].Dimension[1]=XtfRead->XtfHead.idimen2[index]; //άԪ��
		DlisWrite->Channel[i].Dimension[2]=XtfRead->XtfHead.idimen3[index]; //άԪ��
	}
}
void CXtfToDLis::SetDlisFrameFromXtf()
{
	CString s;
	CString IndexUnits;
	IndexUnits.Format ("%-8.8s",XtfRead->XtfHead.IndexUnit);
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
				DlisWrite->Frame[i].Index_Start=XtfRead->XtfHead.Sdep[index]; //������Сֵ
				DlisWrite->Frame[i].Index_Stop=XtfRead->XtfHead.Edep[index]; //�������ֵ
				DlisWrite->Frame[i].Index_Step=XtfRead->XtfHead.Level[index]; //����
				DlisWrite->Frame[i].ChannelID[NumCurve]=k;
				NumCurve++;
			}
		}
		DlisWrite->Frame[i].NumCurve=NumCurve; //��֡������
	}
}
void CXtfToDLis::SetAxisFromXtf()
{
	CString s;
	int NumAxis=0;
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		//CString sCurve=SelCurveName.GetAt(i);
		//sCurve.TrimRight();
		int NumDim=XtfRead->XtfHead.CurveAttrib[index].CurType;
		NumAxis+=(NumDim-1);
	}
	fprintf(fp,"====NumAxis=%d\n",NumAxis);
	fflush(fp);
	if(NumAxis==0) return;
	DlisWrite->SetDlisAxisNumber(NumAxis);
	int nAx=0;
	for(i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i); //XTF����������
		int NumDim=XtfRead->XtfHead.CurveAttrib[index].CurType;
		DlisWrite->Channel[i].nAxis=NumDim-1; //������ά��
		if(DlisWrite->Channel[i].nAxis==0) continue;
		DlisWrite->Axis[nAx].NumC=0; //������
		CString sCurve=SelCurveName.GetAt(i);
		sCurve.TrimRight();
		XtfRead->OpenCurve(sCurve); //ȡ�õ�ǰ���߲���
		for(int j=0;j<DlisWrite->Channel[i].nAxis;j++)
		{
			DlisWrite->Axis[nAx].Origin=1; //Դ
			DlisWrite->Axis[nAx].CopyNum=0; //������
			DlisWrite->Axis[nAx].NumC=1; //������
			DlisWrite->Channel[i].Axis[j].Origin=1; //�᣺R=OBNAME	
			DlisWrite->Channel[i].Axis[j].CopyNum=0; //�᣺R=OBNAME	
			sprintf(DlisWrite->Axis[nAx].Name,"%d",1000+nAx); //��������Ӧ�ڶ�����
			strcpy(DlisWrite->Channel[i].Axis[j].Name,DlisWrite->Axis[nAx].Name); //�᣺R=OBNAME	
			if(j==0 && fabs(XtfRead->XtfCurve.timeinc)>0)
			{
				strcpy(DlisWrite->Axis[nAx]. AxisID,"MICRO_TIME");//��ID��:C=1,R=IDENT
				DlisWrite->Axis[nAx].Coordinates[0]=XtfRead->XtfCurve.starttm; //����
				DlisWrite->Axis[nAx].Spacing=XtfRead->XtfCurve.timeinc; //���
				s.Format ("%-8.8s",XtfRead->XtfCurve.TimeIncrementUnits);
				s.TrimRight();
				strcpy(DlisWrite->Axis[nAx].Units,s); //���굥λ
			}
			else
			{
				strcpy(DlisWrite->Axis[nAx].AxisID,"SENSOR");//��ID��:C=1,R=IDENT
				DlisWrite->Axis[nAx].Coordinates[0]=1; //����
				DlisWrite->Axis[nAx].Spacing=1; //���
				strcpy(DlisWrite->Axis[nAx].Units,""); //���굥λ
			}
			nAx++;
		}
	}
}
void CXtfToDLis::XtfDataToDlis()
{
	CString s;
	int NumLog=-1; //�������������
	for(int nItem=0;nItem<NumItem;nItem++)
	{
		if(sp_Break_Status) break;
		int NumCurve=DlisWrite->Frame[nItem].NumCurve;
		float Start=DlisWrite->Frame[nItem].Index_Start;
		float Stop=DlisWrite->Frame[nItem].Index_Stop;
		float Step=DlisWrite->Frame[nItem].Index_Step;
		int NumPoint=int((Stop-Start)/Step+0.5)+1;
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
			NumLog++; //�����������
			//index������DLISͨ��������
			int index=DlisWrite->Frame[nItem].ChannelID[i]; 
			int IndexXtf=CurveIndex.GetAt(index); //������XTF������
			int nResult=XtfRead->OpenCurve(IndexXtf); //��ͨ����Ϣ
			if(nResult<0)
			{
				fprintf(fp,"�Ҳ�������\n");
				continue; //��ͷ�д��󣬺��Ը�����
			}
			s=SelCurveName.GetAt(index)+"("+NewCurveName.GetAt(index)+")";
			s.Remove(' ');
			fprintf(fp,"====Write: %-4d %-20s %5d\n",NumLog,s,DlisWrite->Channel[index].Sample);
			/////////////////////////////////////////////////////////////////
			if(XtfRead->XtfHead.CurveAttrib[IndexXtf].CurType==1)
			{
				float *buf=new float[NumPoint];
				XtfRead->ReadCurve(IndexXtf,NumPoint,buf);
				for(int k=0;k<NumPoint;k++)
				{
					if(fabs(buf[k]+9999.0)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
					if(fabs(buf[k]+999.25)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
				}
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
				for(j=0;j<NumPoint;j++)
				{
					float depth=Start+j*Step;
					XtfRead->ReadMatrix(IndexXtf,depth,buf);
					DlisWrite->WriteArrayData(nItem,i,j,Count,buf);
				}
				delete []buf ; buf=NULL;
			}
		}
		DlisWrite->StopWriteData(nItem);
	}
}
