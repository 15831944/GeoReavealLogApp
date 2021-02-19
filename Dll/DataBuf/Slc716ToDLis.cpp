// Slc716ToDLis.cpp: implementation of the CSlc716ToDLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Slc716ToDLis.h"
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

CSlc716ToDLis::CSlc716ToDLis()
{
}

CSlc716ToDLis::~CSlc716ToDLis()
{
}
bool CSlc716ToDLis::Slc716ToDlis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo)
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
	Slc716R=new CSlc716R ;
	DlisWrite=new CDlisWrite ; 
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete Slc716R; Slc716R=NULL;
	delete DlisWrite; DlisWrite=NULL;
	return bResult;
}

bool CSlc716ToDLis::Transform()
{
	if(!IsSLC716(m_SourceFile))
	{
		fprintf(fp,"%s is not SLC716 Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	DlisFileName.Format ("%s\\%s.dlis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	DlisFileName.MakeLower();
	ModiFileName(DlisFileName); //�������������Ƿ��޸��ļ���

	if(!Slc716R->Open(m_SourceFile))
	{
		return false;
	}
	Slc716R->ReadHead(); //��SLC716ͷ
	SelectCurves();//ѡ�����߻�����߸���
	if(Slc716R->Slc716Head.NumLog<1)
	{
		Slc716R->Close();
		fprintf(fp,"\n!!!!û���������!!!\n\n");
		return false;
	}
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)DlisFileName);
	////////////////////////////////////////////
	SetDlisChannelsFromSlc716(); //����SLC716��Ϣ����ͨ����Ϣ
	SetDlisFrameFromSlc716(); //����DLIS֡��������
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
	Slc716DataToDlis(); //�������
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	CurveIndex.RemoveAll();
	DlisWrite->Close(); //�ر�DLIS�ļ�
	DlisWrite->Prt_Header();
	return !sp_Break_Status;
}

void CSlc716ToDLis::SelectCurves()
{
	CString s;
	bool *CurveID=new bool[Slc716R->Slc716Head.NumLog];
	for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
	{
		CurveID[i]=false;
	}
	CString sCurveName;
	switch(m_nCurvesRadio)
	{
	case 0://���߹���
		for(i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
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
	case 1: //ȫ������
	case 2: //��������
	case 3: //����ѡ��
		for(i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			CurveID[i]=true;
		}
		break;
	}
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	//���ݿ������þ����Ƿ���н���ѡ��
	if(m_nCurvesRadio !=3 )
	{	//û�н�������
		for(i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			if(!CurveID[i]) continue;
			sCurveName.Format("%-.4s",Slc716R->Slc716Head.CurveName[i]);
			SelCurveName.Add(sCurveName);
			sCurveName.MakeUpper();
			sCurveName.TrimRight();
			NewCurveName.Add(sCurveName);
			NewUnits.Add("");
		}
	}
	else
	{	//��������
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=255; //������߳���
		ReCurNamDlg.m_nMaxLog=99999; //���������
		for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			if(!CurveID[i]) continue;
			sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			sCurveName.Remove(' ');
			ReCurNamDlg.m_ObjectName.Add(sCurveName);//Ԥ����������
			ReCurNamDlg.m_Units.Add("");
			ReCurNamDlg.m_NumDim.Add(1);
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
		ReCurNamDlg.m_NumDim.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
	}
	///////////////////////////////////////////////////
	delete []CurveID; CurveID=NULL;
	CurveIndex.RemoveAll();
	for(i=0;i<SelCurveName.GetSize();i++)
	{
		for(int j=0;j<Slc716R->Slc716Head.NumLog;j++)
		{
			s.Format ("%-4.4s",Slc716R->Slc716Head.CurveName[j]);
			if(s.CompareNoCase(SelCurveName.GetAt(i))==0)
			{
				CurveIndex.Add(j);
				break;
			}
		}
	}
}
void CSlc716ToDLis::SetDlisChannelsFromSlc716()
{
	CString s;
	DlisWrite->SetFrameNumber(1); //֡��
	DlisWrite->SetChannelNumber(CurveIndex.GetSize()); //������
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
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
		DlisWrite->Channel[i].RepCode=DLIS_REPC_FSINGL;
		DlisWrite->Channel[i].NumDim=1; //ά��
		DlisWrite->Channel[i].Dimension[0]=1; //άԪ��
		DlisWrite->Channel[i].Sample=1;//�������ݸ���
	}
}
void CSlc716ToDLis::SetDlisFrameFromSlc716()
{
	CString s;
	DlisWrite->Frame[0].Origin=1;//Դ(����)
	DlisWrite->Frame[0].CopyNum=0;//������(����)
	sprintf(DlisWrite->Frame[0].Name,"B1");//֡��(������)
	strcpy(DlisWrite->Frame[0].Index_Type,"BOREHOLE-DEPTH"); //��������
	strcpy(DlisWrite->Frame[0].Direction,"INCREASING"); //�⾮����
	strcpy(DlisWrite->Frame[0].IndexUnits,"m"); //������λ 
	DlisWrite->Frame[0].Index_Start=Slc716R->Slc716Head.Stdep; //������Сֵ
	DlisWrite->Frame[0].Index_Stop=Slc716R->Slc716Head.Endep; //�������ֵ
	DlisWrite->Frame[0].Index_Step=Slc716R->Slc716Head.Rlev; //����
	DlisWrite->Frame[0].NumCurve=CurveIndex.GetSize(); //��֡������
	for(int i=0;i<DlisWrite->Frame[0].NumCurve;i++)
	{
		DlisWrite->Frame[0].ChannelID[i]=i; //֡������ͨ����������
	}
}
void CSlc716ToDLis::SetStoUnitLab()
{
	char str[256];
	memcpy(DlisWrite->StoUnitLab.SeqNum,"   1",4); //�������
	memcpy(DlisWrite->StoUnitLab.DLISVer,"V1.00",5); //�汾��
	memcpy(DlisWrite->StoUnitLab.Stru,"RECORD",6); //�洢���ṹ
	sprintf(str,"%5d",DlisWrite->MaxPhyLen); 
	memcpy(DlisWrite->StoUnitLab.MaxRecLen,str,5); 
	sprintf(str,"%-60.60s","SLC716 to DLIS Conversion"); //�洢����־
	memcpy(DlisWrite->StoUnitLab.StoSetIden,str,60);
}
void CSlc716ToDLis::SetFileHeader()
{
	sprintf(DlisWrite->FileHeader.SequenceNumber,"%10.10s","1");//���к�
	sprintf(DlisWrite->FileHeader.ID,"%-65.65s","SLC716.001");//ID��
}
void CSlc716ToDLis::SetOrigin()
{
	CString s;
	strcpy(DlisWrite->Origin.FileID,"SLC716.001");//FILE-ID
	strcpy(DlisWrite->Origin.FileSetName,"SLC716-TO-DLIS-CONVERSION");//FILE-SET-NAME
	strcpy(DlisWrite->Origin.FileSetNumber,"1");//FILE-SET-NUMBER
	strcpy(DlisWrite->Origin.FileNumber,"1"); //FILE-NUMBER
	strcpy(DlisWrite->Origin.FileType,"RE-FMT"); //FILE-TYPE
	strcpy(DlisWrite->Origin.Product,"GeoReveal "); //PRODUCT
	strcpy(DlisWrite->Origin.Version,"1.0"); //VERSION
	strcpy(DlisWrite->Origin.Programs,"SLC716-TO-DLIS"); //PROGRAMS
	CTime t=CTime::GetCurrentTime(); //ȡ��ǰϵͳ���ں�ʱ��
	sprintf(DlisWrite->Origin.CreationTime,"YEAR=%d MONTH=%d DAY=%d HOUR=%d MINUTE=%d SECOND=%d",
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),
		t.GetSecond()); //CREATION-TIME
	strcpy(DlisWrite->Origin.OrderNumber,""); //ORDER-NUMBER
	strcpy(DlisWrite->Origin.DescentNumber,""); //DESCENT-NUMBER
	strcpy(DlisWrite->Origin.RunNumber,""); //RUN-NUMBER
	strcpy(DlisWrite->Origin.WellID,""); //WELL-ID
	s.Format ("%-80.80s",Slc716R->Slc716Head.WellName);
	s.TrimRight();
	s.TrimLeft();
	if(s.IsEmpty())
	{
		s=MyGetFileTitle(m_SourceFile);
	}
	strcpy(DlisWrite->Origin.WellName,s); //WELL-NAME
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
	strcpy(DlisWrite->Origin.Company,Slc716R->Slc716Head.CompanyName); //COMPANY
	strcpy(DlisWrite->Origin.NameSpaceVersion,"0"); //NAME-SPACE-VERSION
}

void CSlc716ToDLis::SetDlisParameter()
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

void CSlc716ToDLis::Slc716DataToDlis()
{
	CString s;
	DlisWrite->StartWriteData(0);
	int nBufLen=Slc716R->NumBlock*64;
	int NumPoint=Slc716R->NumPoint;
	float *buf=new float[nBufLen];
	pGuage->SetRange(0,CurveIndex.GetSize());
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		pGuage->SetPos(i+1);
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int index=CurveIndex.GetAt(i);
		Slc716R->ReadChannel(index,buf);
		for(int k=0;k<NumPoint;k++)
		{
			if(fabs(buf[k]+9999.0)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
			if(fabs(buf[k]+999.25)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
		}
		DlisWrite->WriteCurveData(0,i,NumPoint,buf);
	}
	DlisWrite->StopWriteData(0);
	delete []buf; buf=NULL;
}
