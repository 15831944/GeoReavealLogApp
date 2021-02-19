// Slc716ToXtf.cpp: implementation of the CSlc716ToXtf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Slc716ToXtf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlc716ToXtf::CSlc716ToXtf()
{
	XtfWrite=NULL;
}

CSlc716ToXtf::~CSlc716ToXtf()
{

}
bool CSlc716ToXtf::Slc716ToXtf(CString SourceFile,CString ObjectPath,int nCurvesRadio,FILE *fo)
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
	
	Slc716R=new CSlc716R;
	XtfWrite=new CXtfWrite;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
	{
		XtfWrite->PrintHead(fp);
	}

	delete XtfWrite; XtfWrite=NULL;
	delete Slc716R; Slc716R=NULL;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	delete pGuage;  //ɾ��������
	return true;
}
bool CSlc716ToXtf::Transform()
{
	CString s;
	if(!IsSLC716(m_SourceFile))
	{
		fprintf(fp,"%s is not SLC716 Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ������ļ���
	XtfFileName.Format ("%s\\%s.xtf",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	XtfFileName.MakeLower();
	ModiFileName(XtfFileName); //�������������Ƿ��޸��ļ���

	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)XtfFileName);

	//////////////
	if(!Slc716R->Open(m_SourceFile))
	{
		return false;
	}
	Slc716R->ReadHead(); //��SLC716ͷ
	if(Slc716R->Slc716Head.NumLog<1)
	{
		Slc716R->Close();
		return false;
	}
	XtfWrite->Open(XtfFileName);
	GetXtfCurveIndexFromSlc716();
	XtfWrite->InitXtfHeader();
	SetXtfHeaderFromSlc716();
	XtfWrite->WriteXtfHeader();
	CString sCurveName;
	///////////////////////////
	pGuage->SetRange(0,CurveIndex.GetSize());
	int nBufLen=Slc716R->NumBlock*64;
	float *buf=new float[nBufLen];
	for(int i=0;i<nBufLen;i++) buf[i]=-999.25;
	for(int index=0;index<CurveIndex.GetSize();index++)
	{
		pGuage->SetPos(index+1);  //���ý�������ǰֵ
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int k=CurveIndex.GetAt(index);
		SetXtfCurveFromSlc716Head(k,index); //����ͨ������
		XtfWrite->WriteCurveHeader(index); //дͨ����Ϣ��XTF�ļ�
		sCurveName.Format("%-4.4s",Slc716R->Slc716Head.CurveName[k]);
		sCurveName.TrimRight();
		Slc716R->ReadChannel(sCurveName,buf);
		XtfWrite->WriteGenData(Slc716R->NumPoint,buf);
	}
	delete []buf; buf=NULL;
	XtfWrite->Close();
	Slc716R->Close();
	////////////////////////////////
	return !sp_Break_Status;
}
void CSlc716ToXtf::GetXtfCurveIndexFromSlc716()
{
	CString s,sCurveName;
	if(Slc716R->Slc716Head.NumLog<1)
	{
		fprintf(fp,"\n!!!!����û������ʵ��\n\n");
		return;
	}
	CurveIndex.RemoveAll();
	XtfWrite->XtfHead.isnumcv=0;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	if(m_nCurvesRadio<3)
	{
		for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
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
			NewUnits.Add("");
		}
	}
	if(m_nCurvesRadio==3)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=8; //��������󳤶�
		ReCurNamDlg.m_nMaxLog=512; //XTF�ļ��������������
		if(m_nCurvesRadio==3)
		{
			ReCurNamDlg.m_NumLog=Slc716R->Slc716Head.NumLog; //ʵ��������
			for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
			{
				sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
				sCurveName.TrimLeft();
				sCurveName.TrimRight();
				ReCurNamDlg.m_SourceName.Add(sCurveName);
				ReCurNamDlg.m_ObjectName.Add(sCurveName);
				ReCurNamDlg.m_Units.Add("");
				ReCurNamDlg.m_NumDim.Add(1);
			}
		}
		else
		{
			ReCurNamDlg.m_NumLog=SelCurveName.GetSize(); //ʵ��������
			for(int i=0;i<SelCurveName.GetSize();i++)
			{
				ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
				ReCurNamDlg.m_ObjectName.Add(SelCurveName.GetAt(i));
				ReCurNamDlg.m_NumDim.Add(1);
				ReCurNamDlg.m_Units.Add("");
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
int CSlc716ToXtf::GetCurveIndex(CString SelCurve)
{
	SelCurve.TrimRight();
	int index=-1;
	CString sCurveName;
	for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
	{
		sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
		sCurveName.TrimLeft();
		sCurveName.TrimRight();
		if(sCurveName.CompareNoCase(SelCurve)==0)
		{
			index=i;
			break;
		}
	}
	return index;
}
void CSlc716ToXtf::SetXtfHeaderFromSlc716()
{
	CString s;
	//��̬��Ϣ,��ʽΪ��%-79.79s����Ŀ���Ǳ�֤sprintf����Ӱ����һ��������ֵ
	char str[256];
	sprintf(str,"%-80.80s",Slc716R->Slc716Head.WellName);
	memcpy(XtfWrite->XtfHead.Wellname,str,80);
	memset(XtfWrite->XtfHead.Fieldname,' ',80);
	sprintf(str,"%-80.80s",Slc716R->Slc716Head.CompanyName);
	memcpy(XtfWrite->XtfHead.Companyname,str,80);
	memset(XtfWrite->XtfHead.Welllocation,' ',40);
	XtfWrite->XtfHead.Metricflag=0; //���Ƶ�λ
	XtfWrite->XtfHead.FileRecord=8;  //�ļ���¼��(��Сֵ)
	XtfWrite->XtfHead.isnumcv=CurveIndex.GetSize();  //��������
	if(XtfWrite->XtfHead.isnumcv==0)
	{
		return ;
	}
	int k=CurveIndex.GetAt(0);
	XtfWrite->XtfHead.wttop=0;  //���ߵĶ������
	XtfWrite->XtfHead.wtbottom=0;  //���ߵĵײ����
	XtfWrite->XtfHead.wtlevel=0;  //���ߵĲ������
	int NumDepCurve=0; //��Ȳ���������
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		k=CurveIndex.GetAt(i);
		char str[256];
		sprintf(str,"%-8.8s",(const char*)(NewCurveName.GetAt(i)));
		memcpy(XtfWrite->XtfHead.CurveName[i],str,8);
		sprintf(str,"%-8.8s",(const char*)(NewUnits.GetAt(i)));
		memcpy(XtfWrite->XtfHead.Units[i],str,8);
		XtfWrite->XtfHead.Sdep[i]=Slc716R->Slc716Head.Stdep;
		XtfWrite->XtfHead.Edep[i]=Slc716R->Slc716Head.Endep;
		XtfWrite->XtfHead.Level[i]=Slc716R->Slc716Head.Rlev;
		XtfWrite->XtfHead.LevelNumber[i]=Slc716R->NumPoint;
		//��Ȳ�������
		NumDepCurve++;
		if(NumDepCurve==1)
		{
			XtfWrite->XtfHead.wttop=XtfWrite->XtfHead.Sdep[i];
			XtfWrite->XtfHead.wtbottom=XtfWrite->XtfHead.Edep[i];
			XtfWrite->XtfHead.wtlevel=XtfWrite->XtfHead.Level[i];
		}
		else
		{
			if(XtfWrite->XtfHead.wttop>XtfWrite->XtfHead.Sdep[i])
			{
				XtfWrite->XtfHead.wttop=XtfWrite->XtfHead.Sdep[i];
			}
			if(XtfWrite->XtfHead.wtbottom<XtfWrite->XtfHead.Edep[i])
			{
				XtfWrite->XtfHead.wtbottom=XtfWrite->XtfHead.Edep[i];
			}
			if(XtfWrite->XtfHead.wtlevel>XtfWrite->XtfHead.Level[i])
			{
				XtfWrite->XtfHead.wtlevel=XtfWrite->XtfHead.Level[i];
			}
		}
		XtfWrite->XtfHead.ndimension[i]=1; //���ߵ�ά����ȡֵΪ1-3
		XtfWrite->XtfHead.idimen1[i]=1; //���ߵ�һά�ĵ�Ԫ��
		XtfWrite->XtfHead.idimen2[i]=0;    //���ߵڶ�ά�ĵ�Ԫ��
		XtfWrite->XtfHead.idimen3[i]=0;    //���ߵ���ά�ĵ�Ԫ��

		XtfWrite->XtfHead.CurveAttrib[i].CurType=1;//��������(1-3)�����桢���Ρ�����
		XtfWrite->XtfHead.CurveAttrib[i].RepCode=XTF_REPR_FLOAT; //��������(1-12)
		XtfWrite->XtfHead.CurveAttrib[i].HorType=1;   //ˮƽ����
		XtfWrite->XtfHead.CurveAttrib[i].VerType=1;  //��ֱ����
		DWORD NumItem=XtfWrite->XtfHead.idimen1[i];
		if(XtfWrite->XtfHead.ndimension[i]>1)
		{
			NumItem*=XtfWrite->XtfHead.idimen2[i];
		}
		if(XtfWrite->XtfHead.ndimension[i]>2)
		{
			NumItem*=XtfWrite->XtfHead.idimen3[i];
		}
		short CodeLen=XtfWrite->XtfRepCodeToLen(XtfWrite->XtfHead.CurveAttrib[i].RepCode);
		int NumBytes=NumItem*XtfWrite->XtfHead.LevelNumber[i]*CodeLen/8;
		int NumRec=(NumBytes-1)/4096+2; //�����ߵ����ݼ�¼����������ͷ
		XtfWrite->XtfHead.StartOffset[i]=XtfWrite->XtfHead.FileRecord+1; 
		XtfWrite->XtfHead.FileRecord+=NumRec;
	}

	XtfWrite->XtfHead.firstrecord=1;     //λͼ�е������¼
	XtfWrite->XtfHead.lastrecord=32576;      //λͼ�е����һ����¼
	XtfWrite->XtfHead.prerecord=0;       //ǰһ��λͼ�ļ�¼��
	XtfWrite->XtfHead.thisrecord=2;      //��ǰλͼ��¼��
	if(XtfWrite->XtfHead.FileRecord<XtfWrite->XtfHead.lastrecord)
	{	
		XtfWrite->XtfHead.bitmapfullflag=0;  //λͼδ��
		XtfWrite->XtfHead.nextrecord=0;      //��һ��λͼ�ļ�¼��
		DWORD Num1=XtfWrite->XtfHead.FileRecord/8;
		DWORD Num2=XtfWrite->XtfHead.FileRecord%8;
		for(DWORD i=0;i<Num1;i++)
		{
			XtfWrite->XtfHead.Datamap[i]=0XFF;  //λͼ
		}
		XtfWrite->XtfHead.Datamap[Num1]=0;
		for(i=0;i<Num2;i++)
		{
			XtfWrite->XtfHead.Datamap[Num1]=(XtfWrite->XtfHead.Datamap[Num1]>>1)|0x80;
		}
	}
	else
	{	
		XtfWrite->XtfHead.bitmapfullflag=1;  //λͼ����
		XtfWrite->XtfHead.nextrecord=32576;  //��һ��λͼ�ļ�¼��
		for(int i=0;i<4072;i++)
		{
			XtfWrite->XtfHead.Datamap[i]=0XFF;
		}
	}
}
void CSlc716ToXtf::SetXtfCurveFromSlc716Head(int IndexFid,int IndexXtf)
{
	CString s;
	sprintf(XtfWrite->XtfCurve.Name,"%-8.8s",NewCurveName.GetAt(IndexXtf));//����������������ĸ��ͷ
	if(NewUnits.GetAt(IndexXtf)=="none")
	{
		memset(XtfWrite->XtfCurve.Units,' ',8);
	}
	else
	{
		sprintf(XtfWrite->XtfCurve.Units,"%-8.8s",NewUnits.GetAt(IndexXtf));
	}
	sprintf(XtfWrite->XtfCurve.Comment,"%-24.24s"," "); //�����߱�ע�ֶΣ�����AIF�ļ�����ı�ע��
	sprintf(XtfWrite->XtfCurve.ServiceCompany,"%-8.8s","SLC"); //��˾��
	sprintf(XtfWrite->XtfCurve.ToolType,"%-8.8s",""); //��������
	sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s"," ");  //���߷������ͣ��硰WAVEFORM������DIPLOG����
	sprintf(XtfWrite->XtfCurve.DepthUnits,"%-8.8s","m");  //��ȵ�λ
	sprintf(XtfWrite->XtfCurve.DepthUnits,"%-8.8s","meters");  //��ȵ�λ
	sprintf(XtfWrite->XtfCurve.TimeIncrementUnits,"%-8.8s"," ");
	sprintf(XtfWrite->XtfCurve.StartTimeUnits,"%-8.8s"," "); 
	sprintf(XtfWrite->XtfCurve.RawTapeHeaderMnemonic,"%-8.8s"," ");  //ԭʼ����ͷ���Ƿ�
	sprintf(XtfWrite->XtfCurve.UnitsOfStationary,"%-8.8s",XtfWrite->XtfCurve.StartTimeUnits); //��̬��ȵ�λ
	sprintf(XtfWrite->XtfCurve.CorrelogramParamterUnits,"%-8.8s"," ");  //���ͼ������λ
	sprintf(XtfWrite->XtfCurve.CbilOrientationMnemonic,"%-8.8s"," ");  //CBIL��λ���Ƿ�
	sprintf(XtfWrite->XtfCurve.LoggingDirection,"%-8.8s","down"); //�⾮����,ȡֵΪ��up������down����
	sprintf(XtfWrite->XtfCurve.AifCurveName,"%-24.24s"," ");//AIF�������Ͱ汾
	sprintf(XtfWrite->XtfCurve.OCT,"%-8.8s"," "); //���������ߵ�OCT
	sprintf(XtfWrite->XtfCurve.ToolNames,"%-48.48s"," "); //8�����ַ���������
	sprintf(XtfWrite->XtfCurve.strunused,"%-8.8s"," ");  //δ��
	sprintf(XtfWrite->XtfCurve.strunused2,"%-8.8s"," "); //δ��
	sprintf(XtfWrite->XtfCurve.LisName,"%-4.4s",XtfWrite->XtfCurve.Name);  //LIS������
	sprintf(XtfWrite->XtfCurve.strunused3,"%-4.4s"," "); //δ��
	sprintf(XtfWrite->XtfCurve.CreationRoutineName,"%-16.16s"," "); //ʮ���ַ��Ĵ������̺Ͱ汾
	sprintf(XtfWrite->XtfCurve.ModifiedRoutineName,"%-16.16s",XtfWrite->XtfCurve.CreationRoutineName); //ʮ���ַ����޸����̺Ͱ汾
	sprintf(XtfWrite->XtfCurve.strunused4,"%-28.28s"," "); //δ��
	sprintf(XtfWrite->XtfCurve.LongName,"%-20.20s"," ");  //�ӳ��˵�������
	sprintf(XtfWrite->XtfCurve.strunused5,"%-712.712s"," ");  //δ��

	XtfWrite->XtfCurve.deptop=Slc716R->Slc716Head.Stdep;  //���ߵĶ������
	XtfWrite->XtfCurve.depbot=Slc716R->Slc716Head.Endep;  //���ߵĵײ����
	XtfWrite->XtfCurve.rlevcv=Slc716R->Slc716Head.Rlev;  //���ߵĲ������
	XtfWrite->XtfCurve.curvmin=0; //������Сֵ
	XtfWrite->XtfCurve.curvmax=0; //�������ֵ
	XtfWrite->XtfCurve.curvavg=0; //����ƽ��ֵ
	XtfWrite->XtfCurve.timeinc=0;
	XtfWrite->XtfCurve.starttm=0;
	XtfWrite->XtfCurve.stadepth=0; //��̬�������
	XtfWrite->XtfCurve.stddev=0;  //��׼����
	XtfWrite->XtfCurve.cpwinbeg=0; //���ͼ������Window begin��
	XtfWrite->XtfCurve.cpwinstp=0; //���ͼ������Window step��
	XtfWrite->XtfCurve.cpwinlen=0; //���ͼ������Window length��
	XtfWrite->XtfCurve.cbilodeg=0; //CBIL��λ�Ķ���
	XtfWrite->XtfCurve.IntervalTR=0;  //���������������ľ���
	XtfWrite->XtfCurve.IntervalRR=0;  //���������������ľ���
	XtfWrite->XtfCurve.RawOffset=0;   //ԭʼ����ƫת
	XtfWrite->XtfCurve.TimeDelay=0;   //ʱ���ӳ�
	XtfWrite->XtfCurve.Angle=0;       //�ο��۵������б۵�ƽ��
	XtfWrite->XtfCurve.XPad=0;    //�����ϴ�������Xλ��
	XtfWrite->XtfCurve.YPad=0;    //�����ϴ�������Yλ��
	for(int i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.LossMinor[i]=0; //�μ���ʧ����ֵ
	}
	for(i=0;i<101;i++)
	{
		XtfWrite->XtfCurve.funused[i]=0; //δ��
	}

	XtfWrite->XtfCurve.dblinit=-9999.0;  //˫���ȳ�ʼֵ
	XtfWrite->XtfCurve.dbtopdep=XtfWrite->XtfCurve.deptop; //˫���ȶ������
	XtfWrite->XtfCurve.dbbotdep=XtfWrite->XtfCurve.depbot; //˫���ȵײ����
	XtfWrite->XtfCurve.dbrlevel=XtfWrite->XtfCurve.rlevcv; //˫���Ȳ������
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.smvalsdb[i]=0; //Array pf 6 secondary Missing Datum Value
	}
	XtfWrite->XtfCurve.idxtop2=0; //�μ���������ͷ��
	XtfWrite->XtfCurve.idxbot2=0; //�μ����������ײ�
	XtfWrite->XtfCurve.idxrlev2=0; //�μ������������
	for(i=0;i<51;i++)
	{
		XtfWrite->XtfCurve.dunused[i]=0; //δ��
	}

	CTime t=CTime::GetCurrentTime(); //ȡ��ǰϵͳ���ں�ʱ��
	int iyear=t.GetYear();
	int imonth=t.GetMonth();
	int iday=t.GetDay();
	int ihour=t.GetHour();
	int iminute=t.GetMinute();
	int isecond=t.GetSecond();

	XtfWrite->XtfCurve.EncodedCreationDate=(iyear<<16)|(imonth<<8)|iday;  //�������ڱ���
	XtfWrite->XtfCurve.EncodedCreationTime=(ihour<<24)|(iminute<<16)|isecond;  //����ʱ�����
	XtfWrite->XtfCurve.EncodedLastAccessDate=XtfWrite->XtfCurve.EncodedCreationDate;  //����ȡ���ڱ���
	XtfWrite->XtfCurve.EncodedLastAccessTime=XtfWrite->XtfCurve.EncodedCreationTime;  //����ȡʱ�����

	XtfWrite->XtfCurve.NumberOfCurveHeaderrecords=1;  //����ͷ��¼��
	//�������һ�����ݼ�¼���ں������

	XtfWrite->XtfCurve.DatumInitializationVariable=-9999;  //���ݳ�ʼ������
	XtfWrite->XtfCurve.NumberOfLevelsForTheCurve=XtfWrite->XtfHead.LevelNumber[IndexXtf];  //���ߵĲ�����
	XtfWrite->XtfCurve.CurrentEncodedDateSignature=0;  //��ǰ�������ڱ��
	XtfWrite->XtfCurve.CurrentEncodedTimeSignature=0;  //��ǰ�������ڱ��
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.LossMinor2[i]=0; //6���μ���ʧ����ֵ������
	}
	for(i=0;i<2;i++)
	{
		XtfWrite->XtfCurve.Arrays2[i]=0;  //��ֵ���У�����ṹ�����߶������ĩ��¼
		XtfWrite->XtfCurve.Arrays3[i]=0;  //��ֵ���У�����ṹ����������������ĩ��¼
	}
	for(i=0;i<108;i++)
	{
		XtfWrite->XtfCurve.iunused[i]=0;  //δ��
	}
	XtfWrite->XtfCurve.SurvlibMajor=4;  //SURVLIB��Ҫ��
	XtfWrite->XtfCurve.SurvlibMinor=14; //SURVLIB��Ҫ��
	XtfWrite->XtfCurve.Ihcurv=IndexXtf+1;  //����ʱ�����߾��
	XtfWrite->XtfCurve.Ictype=1;  //��������:1-����,2-����,3-����
	XtfWrite->XtfCurve.RepCode=XTF_REPR_FLOAT;
	XtfWrite->XtfCurve.Ivtype=1;  //��ֱ��������
	XtfWrite->XtfCurve.Ihtype=1;  //ˮƽ��������
	XtfWrite->XtfCurve.Ndims=1;   //���ߵ�ά��
	//���õ�iά�ĵ�Ԫ��
	XtfWrite->XtfCurve.Idims[0]=1;
	for(i=XtfWrite->XtfCurve.Ndims;i<7;i++)
	{
		XtfWrite->XtfCurve.Idims[i]=0;  //��iά�ĵ�Ԫ��
	}
	XtfWrite->XtfCurve.I2init=-32767;  //INT*2�����ĳ�ʼֵ
	XtfWrite->XtfCurve.Intflfix=XtfWrite->XtfRepCodeToIntFlFix(XtfWrite->XtfCurve.RepCode);
	XtfWrite->XtfCurve.Isign=XtfWrite->XtfRepCodeToSign(XtfWrite->XtfCurve.RepCode); //�޷��ű��
	XtfWrite->XtfCurve.Numbits=XtfWrite->XtfRepCodeToLen(XtfWrite->XtfCurve.RepCode);
	XtfWrite->XtfCurve.Lisrepcd=0; //��ȡ��ֵ����������ԭʼLIS������ʽ
	XtfWrite->XtfCurve.Lisnsamp=0;  //Nsamp���ͬʱLisrepcd�й�
	XtfWrite->XtfCurve.Lisrcsiz=0;  //LIS�����С���ֽڣ�
	XtfWrite->XtfCurve.Lisnsize=0;  //ÿ������LIS�����ܽڽ���
	XtfWrite->XtfCurve.Istartfv=0;  //0-�̶���������ʼʱ��,1-�ɱ��������ʼʱ��
	XtfWrite->XtfCurve.Igainfv=0;   //��Ϊ0����������������̶�
	XtfWrite->XtfCurve.Igainepx=0;  //���й̶���������л����ߵ�����ָ��
	XtfWrite->XtfCurve.Igainmth=0;  //������������л����ߵ������ʾ��-�̶���ɱ�
	XtfWrite->XtfCurve.Igainapp=0;  //����Ӧ�ñ�־
	XtfWrite->XtfCurve.Lntaphed=0;  //RAWTPHD�ĳ���
	XtfWrite->XtfCurve.Ntools=0;    //������
	XtfWrite->XtfCurve.Idxingrp=0;  //��������
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.Smvalsi2[i]=0;  //6���μ���ʧ����ֵ������
	}
	for(i=0;i<219;i++)
	{
		XtfWrite->XtfCurve.hunused[i]=0;  //δ��
	}
	for(i=0;i<24;i++)
	{
		XtfWrite->XtfCurve.TransmitterID[i]=0;  //24�ֽڴ���24��������ID��
		XtfWrite->XtfCurve.ReceiverID[i]=0;  //24�ֽڴ���24��������ID��
	}
	for(i=0;i<276;i++)
	{
		XtfWrite->XtfCurve.RawTapeHeaderInfo[i]=0;  //ԭʼ��ͷ��Ϣ
	}
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.LossMinor3[i]=0;  //6�ֽڴ������ʧ��ֵ������
		XtfWrite->XtfCurve.LossMinor4[i]=0;  //6�ֽڴ������ʧ��ֵ������
	}
	for(i=0;i<687;i++)
	{
		XtfWrite->XtfCurve.bunused[i]=0;   //δ��
	}
	XtfWrite->XtfCurve.SystemCode=5;  //ϵͳ����:1-PC,2-PE,3-VAX,4-IBM Mainframe,5-hp9000/Sun,etc.

	DWORD BlockSize=XtfWrite->XtfHead.LevelNumber[IndexXtf]*XtfWrite->XtfCurve.Idims[0]*XtfWrite->XtfCurve.Numbits/8;
	DWORD NumRecord=(BlockSize-1)/4096+2; //����һ������ͷ
	XtfWrite->XtfCurve.RecordNumberOfLastCurvedataRecord=XtfWrite->XtfHead.StartOffset[IndexXtf]+NumRecord-1;
}