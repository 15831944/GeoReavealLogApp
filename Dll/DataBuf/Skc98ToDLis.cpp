// Skc98ToDLis.cpp: implementation of the CSkc98ToDLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Skc98ToDLis.h"
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

CSkc98ToDLis::CSkc98ToDLis()
{
	ItemNum=NULL;
}

CSkc98ToDLis::~CSkc98ToDLis()
{
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
}

bool CSkc98ToDLis::Skc98ToDLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo)
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

	Skc98R=new CSkc98R ;
	DlisWrite=new CDlisWrite ; 
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete Skc98R; Skc98R=NULL;
	delete DlisWrite; DlisWrite=NULL;

	return bResult;
}


bool CSkc98ToDLis::Transform()
{
	if(!IsSKC98(m_SourceFile))
	{
		fprintf(fp,"%s is not SKC98 Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	DlisFileName.Format ("%s\\%s.dlis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	DlisFileName.MakeLower();
	ModiFileName(DlisFileName); //�������������Ƿ��޸��ļ���

	if(!Skc98R->Open(m_SourceFile))
	{
		return false;
	}
	Skc98R->Open(m_SourceFile);
	Skc98R->ReadHead();
	SelectCurves();//ѡ�����߻�����߸���
	if(CurveIndex.GetSize()==0)
	{
		Skc98R->Close();
		fprintf(fp,"\n!!!!û���������!!!\n\n");
		return false;
	}
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)DlisFileName);
	////////////////////////////////////////////
	GetDlisFDataIndexFromSkc98(); //��SKC98ͨ����Ϣ���з���
	Prt_FdataItemInfo(); //��ӡ֡������Ϣ
	SetDlisChannelsFromSkc98(); //����SKC98��Ϣ����ͨ����Ϣ
	SetDlisFrameFromSkc98(); //����DLIS֡��������
	SetAxisFromSkc98(); //����DLIS���������
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
	Skc98DataToDlis(); //�������
	CurveIndex.RemoveAll(); //Դ����������
	SelCurveName.RemoveAll(); //ѡ����������
	NewCurveName.RemoveAll(); //����������
	NewUnits.RemoveAll(); ////�µ�λ����
	DlisWrite->Close(); //�ر�DLIS�ļ�
	DlisWrite->Prt_Header();
	return !sp_Break_Status;
}

void CSkc98ToDLis::SelectCurves()
{
	CString s;
	bool *CurveID=new bool[Skc98R->Skc98Head.NumLog];
	for(int i=0;i<Skc98R->Skc98Head.NumLog;i++)
		CurveID[i]=false;
	CString sCurveName;
	switch(m_nCurvesRadio)
	{
	case 0://���߹���
		for(i=0;i<Skc98R->Skc98Head.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.4s",Skc98R->Skc98Head.CurveName[i]);
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
		for(i=0;i<Skc98R->Skc98Head.NumLog;i++)
		{
			if(Skc98R->IsConvCurve(i))
			{
				CurveID[i]=true;
			}
		}
		break;
	default://����ѡ���ȫ������
		for(i=0;i<Skc98R->Skc98Head.NumLog;i++)
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
	for(i=0;i<Skc98R->Skc98Head.NumLog;i++)
	{
		if(!CurveID[i]) continue;
		s.Format("%-.4s",Skc98R->Skc98Head.CurveName[i]);
		SelCurveName.Add(s);
		s.TrimRight();
		s.Remove('.');
		NewCurveName.Add(s);
		NewUnits.Add("");
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
			int NumDim=1;
			if(!Skc98R->IsConvCurve(index))
			{
				NumDim=2;
			}
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
				s.Format ("%-.4s",Skc98R->Skc98Head.CurveName[index]);
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

void CSkc98ToDLis::GetDlisFDataIndexFromSkc98()
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
		float Start=Skc98R->Skc98Head.Stdep;
		float Stop=Skc98R->Skc98Head.Endep;
		float Rlev=Skc98R->yStep[index];
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
void CSkc98ToDLis::Prt_FdataItemInfo()
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
void CSkc98ToDLis::SetDlisChannelsFromSkc98()
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
		if(Skc98R->IsConvCurve(index))
		{
			DlisWrite->Channel[i].RepCode=DLIS_REPC_FSINGL;
			DlisWrite->Channel[i].NumDim=1; //����
			DlisWrite->Channel[i].Dimension[0]=1; //άԪ��
			DlisWrite->Channel[i].Dimension[1]=0; //άԪ��
			DlisWrite->Channel[i].Dimension[2]=0; //άԪ��
		}
		else
		{
			DlisWrite->Channel[i].RepCode=DLIS_REPC_SNORM;
			DlisWrite->Channel[i].NumDim=1; //����
			DlisWrite->Channel[i].Dimension[0]=Skc98R->Skc98Head.CurveTypes[index]/2;//άԪ��
			DlisWrite->Channel[i].Dimension[1]=0; //άԪ��
			DlisWrite->Channel[i].Dimension[2]=0; //άԪ��
		}
	}
}
void CSkc98ToDLis::SetDlisFrameFromSkc98()
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
				if(Skc98R->Skc98Head.Stdep<Skc98R->Skc98Head.Endep)
				{
					DlisWrite->Frame[i].Index_Start=Skc98R->Skc98Head.Stdep; //������Сֵ
					DlisWrite->Frame[i].Index_Stop=Skc98R->Skc98Head.Endep; //�������ֵ
				}
				else
				{
					DlisWrite->Frame[i].Index_Start=Skc98R->Skc98Head.Endep; //������Сֵ
					DlisWrite->Frame[i].Index_Stop=Skc98R->Skc98Head.Stdep; //�������ֵ
				}
				DlisWrite->Frame[i].Index_Step=float(fabs(Skc98R->yStep[index])); //����
				DlisWrite->Frame[i].ChannelID[NumCurve]=k;
				NumCurve++;
			}
		}
		DlisWrite->Frame[i].NumCurve=NumCurve; //��֡������
	}
}
void CSkc98ToDLis::SetAxisFromSkc98()
{
	CString s;
	int NumAxis=0;
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		if(!Skc98R->IsConvCurve(index))
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
		int index=CurveIndex.GetAt(i); //SKC98����������
		int NumDim=1;
		if(!Skc98R->IsConvCurve(index))
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
		strcpy(DlisWrite->Axis[nAx]. AxisID,"MICRO_TIME");//��ID��:C=1,R=IDENT
		DlisWrite->Axis[nAx].Coordinates[0]=0; //����
		DlisWrite->Axis[nAx].Spacing=2; //���
		strcpy(DlisWrite->Axis[nAx].Units,"us"); //���굥λ
		nAx++;
	}
}
void CSkc98ToDLis::SetStoUnitLab()
{
	char str[256];
	memcpy(DlisWrite->StoUnitLab.SeqNum,"   1",4); //�������
	memcpy(DlisWrite->StoUnitLab.DLISVer,"V1.00",5); //�汾��
	memcpy(DlisWrite->StoUnitLab.Stru,"RECORD",6); //�洢���ṹ
	sprintf(str,"%5d",DlisWrite->MaxPhyLen); 
	memcpy(DlisWrite->StoUnitLab.MaxRecLen,str,5); 
	sprintf(str,"%-60.60s","SKC98 to DLIS Conversion"); //�洢����־
	memcpy(DlisWrite->StoUnitLab.StoSetIden,str,60);
}
void CSkc98ToDLis::SetFileHeader()
{
	sprintf(DlisWrite->FileHeader.SequenceNumber,"%10.10s","1");//���к�
	sprintf(DlisWrite->FileHeader.ID,"%-65.65s","SKC98.001");//ID��
}
void CSkc98ToDLis::SetOrigin()
{
	CString s;
	strcpy(DlisWrite->Origin.FileID,"SKC98.001");//FILE-ID
	strcpy(DlisWrite->Origin.FileSetName,"SKC98-TO-DLIS-CONVERSION");//FILE-SET-NAME
	strcpy(DlisWrite->Origin.FileSetNumber,"1");//FILE-SET-NUMBER
	strcpy(DlisWrite->Origin.FileNumber,"1"); //FILE-NUMBER
	strcpy(DlisWrite->Origin.FileType,"RE-FMT"); //FILE-TYPE
	strcpy(DlisWrite->Origin.Product,"GeoReveal "); //PRODUCT
	strcpy(DlisWrite->Origin.Version,"1.0"); //VERSION
	strcpy(DlisWrite->Origin.Programs,"SKC98-TO-DLIS"); //PROGRAMS
	CTime t=CTime::GetCurrentTime(); //ȡ��ǰϵͳ���ں�ʱ��
	sprintf(DlisWrite->Origin.CreationTime,"YEAR=%d MONTH=%d DAY=%d HOUR=%d MINUTE=%d SECOND=%d",
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),
		t.GetSecond()); //CREATION-TIME
	strcpy(DlisWrite->Origin.OrderNumber,""); //ORDER-NUMBER
	strcpy(DlisWrite->Origin.DescentNumber,""); //DESCENT-NUMBER
	strcpy(DlisWrite->Origin.RunNumber,""); //RUN-NUMBER
	strcpy(DlisWrite->Origin.WellID,""); //WELL-ID
	s.Format ("%-.80s",Skc98R->Skc98Head.WellName);
	s.TrimRight();
	strcpy(DlisWrite->Origin.WellName,s); //WELL-NAME
	s.Format ("%-.50s",Skc98R->Skc98Head.AreaName);
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
	s.Format ("%-.80s",Skc98R->Skc98Head.CompanyName);
	s.TrimRight();
	strcpy(DlisWrite->Origin.Company,s); //COMPANY
	strcpy(DlisWrite->Origin.NameSpaceVersion,"0"); //NAME-SPACE-VERSION
}

void CSkc98ToDLis::SetDlisParameter()
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

void CSkc98ToDLis::Skc98DataToDlis()
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
			NumLog++; //�����������
			//index������DLISͨ��������
			int index=DlisWrite->Frame[nItem].ChannelID[i]; 
			int IndexSkc98=CurveIndex.GetAt(index); //������SKC98������
			s=SelCurveName.GetAt(index)+"("+NewCurveName.GetAt(index)+")";
			s.Remove(' ');
			/////////////////////////////////////////////////////////////////
			if(Skc98R->IsConvCurve(IndexSkc98))
			{
				fprintf(fp,"====Transform Curve: %-8s %5d\n",s,DlisWrite->Channel[index].Sample);
				int NumR=Skc98R->NumPoint;
				int NumY=int(Skc98R->Skc98Head.Rlev/Skc98R->yStep[IndexSkc98]+0.5);
				if(NumY>1)
				{
					NumR*=NumY;
				}
				float *buf=new float[NumR];
				Skc98R->ReadChannel(IndexSkc98,buf);
				DlisWrite->WriteCurveData(nItem,i,NumR,buf);
				delete []buf; buf=NULL;
			}
			else
			{
				fprintf(fp,"====Transform Waveform: %-8s %5d\n",s,DlisWrite->Channel[index].Sample);
				int Count=DlisWrite->Channel[index].Sample;
				short *buf=new short[Count];
				for(int j=0;j<Count;j++)
				{
					buf[j]=0;
				}
				pGuage->SetRange(0,Skc98R->NumPoint);
				for(j=0;j<Skc98R->NumPoint;j++)
				{
					pGuage->SetPos(j+1);
					if (pGuage->CheckCancelButton())
					{
						sp_Break_Status=true;
						break;
					}
					float depth=Start+j*Step;
					Skc98R->ReadVDL(IndexSkc98,depth,buf);
					DlisWrite->WriteArrayData(nItem,i,j,Count,buf);
				}
				delete []buf ; buf=NULL;
			}
		}
		DlisWrite->StopWriteData(nItem);
	}
}
