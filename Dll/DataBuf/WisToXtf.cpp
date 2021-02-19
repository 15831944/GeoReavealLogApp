// WisToXtf.cpp: implementation of the CWisToXtf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "WisToXtf.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWisToXtf::CWisToXtf()
{
	WisRead=NULL;

}

CWisToXtf::~CWisToXtf()
{

}
bool CWisToXtf::WisToXtf(CString SourceFile,CString ObjectPath,int nCurvesRadio,FILE *fo)
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
	
	XtfWrite=new CXtfWrite;
	WisRead=new CWisRead;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
	{
		XtfWrite->PrintHead(fp);
		fprintf(fp,"%d Curve:\n",NewCurveName.GetSize());
		for(int i=0;i<NewCurveName.GetSize();i++)
		{
			if(i>0 && i%5==0)
			{
				fprintf(fp,"\n");
			}
			fprintf(fp," %-10.15s",(const char*)(NewCurveName.GetAt(i)));
		}
		fprintf(fp,"\n");
		fprintf(fp,"---- End Head information ---\n\n");
	}

	delete WisRead; WisRead=NULL;
	delete XtfWrite; XtfWrite=NULL;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	delete pGuage;  //ɾ��������
	return true;
}
bool CWisToXtf::Transform()
{
	if(!IsWIS(m_SourceFile))
	{
		fprintf(fp,"%s is not WIS Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	XtfFileName.Format ("%s\\%s.xtf",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(XtfFileName); //�������������Ƿ��޸��ļ���
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)XtfFileName);
	WisRead->Open(m_SourceFile);
	WisRead->GetHead();
	WisRead->GetObjectEntry();
	//��Wis������Ϣ����ѡ��������Ҫ������
	GetChannelIndexFromWis();
	GetChannelFromWin();

	XtfWrite->Open(XtfFileName);
	XtfWrite->InitXtfHeader();
	SetXtfHeaderFromWis();
	XtfWrite->WriteXtfHeader();

	pGuage->SetRange(0,CurveIndex.GetSize());
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		pGuage->SetPos(i+1);  //���ý�������ǰֵ
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int k=CurveIndex.GetAt(i);
		SetXtfCurveFromWis(k,i);
		XtfWrite->WriteCurveHeader(i);
		int NumPoint=WisRead->WisChannel.DimInfo[0].Samples;
		if(WisRead->WisChannel.NumOfDimension==1)
		{
			float depth=WisRead->WisChannel.DimInfo[0].StartVal;
			float *buf=new float[NumPoint];
			WisRead->ReadCurve(k,depth,NumPoint,buf);
			XtfWrite->WriteGenData(NumPoint,buf);
			delete []buf; buf=NULL;
		}
		else
		{
			int Count=1;
			for(int j=1;j<WisRead->WisChannel.NumOfDimension;j++)
			{
				Count*=WisRead->WisChannel.DimInfo[j].Samples;
			}
			float *buf=new float[Count];
			for(j=0;j<Count;j++) buf[j]=0;
			for(j=0;j<NumPoint;j++)
			{
				float depth=WisRead->WisChannel.DimInfo[0].StartVal+j*WisRead->WisChannel.DimInfo[0].Delta;
				WisRead->ReadWaveArray(k,depth,buf);
				XtfWrite->WriteWaveArray(depth,buf);
			}
			delete []buf; buf=NULL;


		}
	}
	WisRead->Close();
	XtfWrite->Close();
	////////////////////////////////
	return !sp_Break_Status;
}
void CWisToXtf::GetChannelIndexFromWis()
{
	CString s;
	XtfWrite->XtfHead.isnumcv=0;
	CurveIndex.RemoveAll();
	for(int index=0;index<WisRead->WisHead.ObjectNumber;index++)
	{
		if(WisRead->m_ObjectEntry[index].Status != 0)continue; //������������
		if(WisRead->m_ObjectEntry[index].Attribute != 1)continue; //����ͨ������
		if(WisRead->m_ObjectEntry[index].BlockNum<1)continue; //û������
		//��ͨ����Ϣ
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
		//�ж��Ƿ�Ϊ��������ͨ��
		bool bIsContinue=true;
		for(int i=0;i<WisRead->WisChannel.NumOfDimension;i++)
		{
			if(WisRead->WisChannel.DimInfo[i].Delta==0)
			{
				bIsContinue=false;
				break;
			}
		}
		if(!bIsContinue)continue; //������������ͨ��,��FMT������
		if(WisRead->WisChannel.NumOfDimension>3)continue; 
		if(m_nCurvesRadio==2)
		{	//ѡ���˳�������
			if(WisRead->WisChannel.NumOfDimension>1)
			{
				continue; //���ǳ�������
			}
		}
		bool bOk=false;
		CString sCurveName;
		sCurveName.Format ("%-.15s",WisRead->m_ObjectEntry[index].Name);
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
		else
		{
			bOk=true;
		}
		if(bOk)
		{
			CurveIndex.Add(index);
		}
	}
}

void CWisToXtf::GetChannelFromWin()
{
	CString s,sCurveName;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	bool bLongName=false;
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		sCurveName.Format ("%-.15s",WisRead->m_ObjectEntry[k].Name);
		if(sCurveName.GetLength()>8)
		{
			bLongName=true;
		}
	}
	if(!bLongName && m_nCurvesRadio != 3)
	{
		for(int i=0;i<CurveIndex.GetSize();i++)
		{
			int k=CurveIndex.GetAt(i);
			sCurveName.Format ("%s",WisRead->m_ObjectEntry[k].Name);
			SelCurveName.Add(sCurveName);
			sCurveName.TrimRight();
			NewCurveName.Add(sCurveName);
			WisRead->OpenChannel(WisRead->m_ObjectEntry[k].Name);
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
		}
		return;
	}
	//////////////////////////////////////////////
	CReCurNamDlg ReCurNamDlg;
	ReCurNamDlg.m_nMaxLen=8;
	ReCurNamDlg.m_nMaxLog=512;
	for(i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[k].Name);
		sCurveName.Format ("%-.15s",WisRead->m_ObjectEntry[k].Name);
		sCurveName.TrimLeft();
		sCurveName.TrimRight();
		ReCurNamDlg.m_SourceName.Add(sCurveName);
		ReCurNamDlg.m_ObjectName.Add(sCurveName);
		s.Format ("%-.7s",WisRead->WisChannel.Unit);
		s.TrimRight();
		if(s.CompareNoCase("none")==0)
		{
			s.Empty();
		}
		ReCurNamDlg.m_Units.Add(s);
		ReCurNamDlg.m_NumDim.Add(WisRead->WisChannel.NumOfDimension);
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
	ReCurNamDlg.m_NumDim.RemoveAll();
	CWordArray NewIndex;
	for(i=0;i<SelCurveName.GetSize();i++)
	{
		for(int j=0;j<CurveIndex.GetSize();j++)
		{
			int k=CurveIndex.GetAt(j);
			sCurveName.Format ("%s",WisRead->m_ObjectEntry[k].Name);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
			{
				NewIndex.Add(k);
				break;
			}
		}
	}
	CurveIndex.RemoveAll();
	for(i=0;i<NewIndex.GetSize();i++)
	{
		CurveIndex.Add(NewIndex.GetAt(i));
	}
	NewIndex.RemoveAll();
}
void CWisToXtf::SetXtfHeaderFromWis()
{
	CString s;
	char str[256];
	sprintf(str,"%-80.80s",""); //����
	memcpy(XtfWrite->XtfHead.Wellname,str,80);
	memset(XtfWrite->XtfHead.Fieldname,' ',80); //������
	sprintf(str,"%-80.80s",""); //��˾��
	memcpy(XtfWrite->XtfHead.Companyname,str,80);
	memset(XtfWrite->XtfHead.Welllocation,' ',40);
	XtfWrite->XtfHead.Metricflag=0; //���Ƶ�λ
	XtfWrite->XtfHead.FileRecord=8;  //�ļ���¼��(��Сֵ)
	XtfWrite->XtfHead.isnumcv=CurveIndex.GetSize();  //��������
	if(XtfWrite->XtfHead.isnumcv==0) return ;

	XtfWrite->XtfHead.wttop=0;  //���ߵĶ������
	XtfWrite->XtfHead.wtbottom=0;  //���ߵĵײ����
	XtfWrite->XtfHead.wtlevel=0;  //���ߵĲ������
	int NumDepCurve=0; //��Ȳ���������
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		char str[256];
		sprintf(str,"%-8.8s",(const char*)(NewCurveName.GetAt(i)));
		memcpy(XtfWrite->XtfHead.CurveName[i],str,8);
		sprintf(str,"%-8.8s",WisRead->WisChannel.Unit);
		memcpy(XtfWrite->XtfHead.Units[i],str,8);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[k].Name);
		XtfWrite->XtfHead.Sdep[i]=WisRead->WisChannel.DimInfo[0].StartVal;
		XtfWrite->XtfHead.Level[i]=WisRead->WisChannel.DimInfo[0].Delta;
		XtfWrite->XtfHead.LevelNumber[i]=WisRead->WisChannel.DimInfo[0].Samples;
		XtfWrite->XtfHead.Edep[i]=XtfWrite->XtfHead.Sdep[i]+(XtfWrite->XtfHead.LevelNumber[i]-1)*XtfWrite->XtfHead.Level[i];
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
		if(WisRead->WisChannel.NumOfDimension==1)
		{
			XtfWrite->XtfHead.ndimension[i]=1;
			XtfWrite->XtfHead.idimen1[i]=1; //���ߵ�һά�ĵ�Ԫ��
			XtfWrite->XtfHead.idimen2[i]=0;    //���ߵڶ�ά�ĵ�Ԫ��
			XtfWrite->XtfHead.idimen3[i]=0;    //���ߵ���ά�ĵ�Ԫ��
		}
		else if(WisRead->WisChannel.NumOfDimension==2)
		{
			XtfWrite->XtfHead.ndimension[i]=1;
			XtfWrite->XtfHead.idimen1[i]=short(WisRead->WisChannel.DimInfo[1].Samples);
			XtfWrite->XtfHead.idimen2[i]=0;    //���ߵڶ�ά�ĵ�Ԫ��
			XtfWrite->XtfHead.idimen3[i]=0;    //���ߵ���ά�ĵ�Ԫ��
		}
		else if(WisRead->WisChannel.NumOfDimension==3)
		{
			XtfWrite->XtfHead.ndimension[i]=2;
			XtfWrite->XtfHead.idimen1[i]=short(WisRead->WisChannel.DimInfo[2].Samples);
			XtfWrite->XtfHead.idimen2[i]=short(WisRead->WisChannel.DimInfo[1].Samples);
			XtfWrite->XtfHead.idimen3[i]=0;    //���ߵ���ά�ĵ�Ԫ��
		}
		XtfWrite->XtfHead.CurveAttrib[i].CurType=BYTE(WisRead->WisChannel.NumOfDimension);//��������(1-3)�����桢���Ρ�����
		XtfWrite->XtfHead.CurveAttrib[i].RepCode=WisRepCToXtf(WisRead->WisChannel.RepCode); //��������(1-12)
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

BYTE CWisToXtf::WisRepCToXtf(WORD WisRepC)
{
	BYTE XtfRepC=XTF_REPR_CHAR;
	switch(WisRepC)
	{
	case WIS_REPR_INT://4�ֽ��з�������
	case WIS_REPR_LONG://4�ֽ��з��ų�������
		XtfRepC=XTF_REPR_INT4;
		break;
	case WIS_REPR_SHORT://2�ֽ��з��Ŷ�������
		XtfRepC=XTF_REPR_INT2;
		break;
	case WIS_REPR_FLOAT://4�ֽ�IEEE�����ȸ�����	
		XtfRepC=XTF_REPR_FLOAT;
		break;
	case WIS_REPR_DOUBLE://8�ֽ�IEE˫���ȸ�����
		XtfRepC=XTF_REPR_DOUBLE;
		break;
	case WIS_REPR_CHAR://1�ֽ��з����ֽ���
		XtfRepC=XTF_REPR_CHAR;
		break;
	case WIS_REPR_UCHAR://1�ֽ��޷����ֽ���
		XtfRepC=XTF_REPR_UCHAR;
		break;
	case WIS_REPR_USHORT://2�ֽ��޷��Ŷ�������
		XtfRepC=XTF_REPR_UINT2;
		break;
	case WIS_REPR_UINT://4�ֽ��޷�������
	case WIS_REPR_ULONG://4�ֽ��޷��ų�������
		XtfRepC=XTF_REPR_UINT4;
		break;
	default:
		break;
	}
	return XtfRepC;
}


void CWisToXtf::SetXtfCurveFromWis(int IndexWis,int IndexXtf)
{
	CString s;
	WisRead->OpenChannel(WisRead->m_ObjectEntry[IndexWis].Name);

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
	sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s"," ");  //�������ͣ��硰WAVEFORM������DIPLOG����
	if(WisRead->WisChannel.NumOfDimension==2)
	{
		sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s","WAVEFORM");  //�������ͣ��硰WAVEFORM������DIPLOG����
	}
	else if(WisRead->WisChannel.NumOfDimension==3)
	{
		sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s","ARRAY");  //�������ͣ��硰WAVEFORM������DIPLOG����
	}
	s.Format ("%-.7s",WisRead->WisChannel.Unit);
	s.TrimRight();
	if(s.CompareNoCase("m")==0 || s.CompareNoCase("meters")==0)
	{
		sprintf(XtfWrite->XtfCurve.DepthUnits,"%-8.8s","meters");  //��ȵ�λ
	}
	else
	{
		sprintf(XtfWrite->XtfCurve.DepthUnits,"%-8.8s",s);  //��ȵ�λ
	}
	if(WisRead->WisChannel.NumOfDimension>1)
	{
		sprintf(XtfWrite->XtfCurve.TimeIncrementUnits,"%-8.8s",WisRead->WisChannel.DimInfo[1].Unit); //�������ߵ�ʱ��������λ
		sprintf(XtfWrite->XtfCurve.StartTimeUnits,"%-8.8s",XtfWrite->XtfCurve.TimeIncrementUnits);  //��ʼʱ�䵥λ,���������й̶��Ϳɱ����ʼʱ��
	}
	else
	{
		sprintf(XtfWrite->XtfCurve.TimeIncrementUnits,"%-8.8s"," ");
		sprintf(XtfWrite->XtfCurve.StartTimeUnits,"%-8.8s"," "); 
	}
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

	XtfWrite->XtfCurve.deptop=XtfWrite->XtfHead.Sdep[IndexXtf];
	XtfWrite->XtfCurve.rlevcv=XtfWrite->XtfHead.Level[IndexXtf];
	XtfWrite->XtfCurve.depbot=XtfWrite->XtfHead.Edep[IndexXtf];
	XtfWrite->XtfCurve.curvmin=0; //������Сֵ
	XtfWrite->XtfCurve.curvmax=0; //�������ֵ
	XtfWrite->XtfCurve.curvavg=0; //����ƽ��ֵ
	if(WisRead->WisChannel.NumOfDimension>1)
	{
		XtfWrite->XtfCurve.timeinc=WisRead->WisChannel.DimInfo[1].Delta; //ʱ�����������������
		XtfWrite->XtfCurve.starttm=WisRead->WisChannel.DimInfo[1].StartVal; //������������ʼʱ��
	}
	else
	{
		XtfWrite->XtfCurve.timeinc=0;
		XtfWrite->XtfCurve.starttm=0;
	}
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
	XtfWrite->XtfCurve.Ictype=WisRead->WisChannel.NumOfDimension;  //��������:1-����,2-����,3-����
	XtfWrite->XtfCurve.RepCode=XtfWrite->XtfHead.CurveAttrib[IndexXtf].RepCode;
	XtfWrite->XtfCurve.Ivtype=1;  //��ֱ��������
	XtfWrite->XtfCurve.Ihtype=1;  //ˮƽ��������
	XtfWrite->XtfCurve.Ndims=XtfWrite->XtfHead.ndimension[IndexXtf];   //���ߵ�ά��
	//���õ�iά�ĵ�Ԫ��
	XtfWrite->XtfCurve.Idims[0]=XtfWrite->XtfHead.idimen1[IndexXtf];
	XtfWrite->XtfCurve.Idims[1]=XtfWrite->XtfHead.idimen2[IndexXtf];
	XtfWrite->XtfCurve.Idims[2]=XtfWrite->XtfHead.idimen3[IndexXtf];
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