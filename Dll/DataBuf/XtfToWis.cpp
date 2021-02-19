// XtfToWis.cpp: implementation of the CXtfToWis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "XtfToWis.h"
#include <math.h>
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXtfToWis::CXtfToWis()
{
	WisWrite=NULL;

}

CXtfToWis::~CXtfToWis()
{

}
bool CXtfToWis::XtfToWis(CString SourceFile,CString ObjectPath,int nCurvesRadio,FILE *fo)
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
	
	XtfRead=new CXtfRead;
	WisWrite=new CWisWrite;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
	{
		WisWrite->PrintHead(fp);
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

	delete WisWrite; WisWrite=NULL;
	delete XtfRead; XtfRead=NULL;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	delete pGuage;  //ɾ��������
	return true;
}
bool CXtfToWis::Transform()
{
	if(!IsXTF(m_SourceFile))
	{
		fprintf(fp,"%s is not XTF Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	CString Filetitle=MyGetFileTitle(m_SourceFile);
	if(sp_bDirectory)
	{
		WisFileName.Format ("%s\\#%s\\%s.wis",m_sObjectPath,Filetitle,Filetitle);
		CString WellPath=m_sObjectPath+"\\#"+Filetitle;
		_mkdir(WellPath);
	}
	else
	{
		WisFileName.Format ("%s\\%s.wis",m_sObjectPath,Filetitle);
	}
	ModiFileName(WisFileName); //�������������Ƿ��޸��ļ���

	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)WisFileName);
	if(!XtfRead->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!�������ļ�%s�д�\n\n",m_SourceFile);
		return false;
	}
	XtfRead->GetHead();
	//////////////////////////////////////////////////////////////////////
	CString sInWellPath=MyGetPathName(m_SourceFile);//XTF������������Ŀ¼
	CString sInWellName=MyGetFileTitle(m_SourceFile);//XTF�������ݶ�Ӧ����
	WisWrite->Open(WisFileName);
	GetWisObjectIndexFromXtf();
	//���������¼����������
	WisWrite->InitWisHead(CurveIndex.GetSize());
	CString sCurveName;
	WisWrite->m_ObjectEntry=NULL; //�������
	WisWrite->WisChannel=NULL; //ͨ����Ϣ
	WisWrite->m_ObjectEntry=new WIS_OBJECT_ENTRY;
	WisWrite->WisChannel=new WIS_CHANNEL;
	WisWrite->InitWisObjectEntry();
	WisWrite->InitWisChannel();
	///////////////////////////
	pGuage->SetRange(0,CurveIndex.GetSize());
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		pGuage->SetPos(i+1);  //���ý�������ǰֵ
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int index=CurveIndex.GetAt(i); //��ѡ�������еõ�������
		int nResult=XtfRead->OpenCurve(index); //��ͨ����Ϣ
		if(nResult<0)
		{
			continue; //��ͷ�д��󣬺��Ը�����
		}
		SetWisChannelFromXtfChannel(index,i);	//��XTFͨ������FIDͨ����Ϣ
		WisWrite->WriteChannel(); //дͨ����Ϣ��WIS�ļ�
		int NumR=int((XtfRead->XtfCurve.depbot-XtfRead->XtfCurve.deptop)/XtfRead->XtfCurve.rlevcv+0.5)+1;
		if(XtfRead->XtfCurve.Ictype==1)
		{
			float *buf=new float[NumR];
			XtfRead->ReadCurve(index,NumR,buf);
			WisWrite->WriteGendata(NumR,buf);
			delete []buf; buf=NULL;
		}
		else if(XtfRead->XtfCurve.Ictype==2)
		{
			int Count=XtfRead->XtfCurve.Idims[0];
			float *buffer=new float[Count];
			for(int k=0;k<Count;k++)buffer[k]=0;
			for(int j=0;j<NumR;j++)
			{
				float depth=XtfRead->XtfCurve.deptop+j*XtfRead->XtfCurve.rlevcv;
				XtfRead->ReadWave(index,depth,buffer);
				WisWrite->WriteWave(depth,buffer);
			}
			delete []buffer; buffer=NULL;
		}
		else if(XtfRead->XtfCurve.Ictype==3)
		{
			int Count=XtfRead->XtfCurve.Idims[0]*XtfRead->XtfCurve.Idims[1];
			float *buffer=new float[Count];
			for(int k=0;k<Count;k++)buffer[k]=0;
			for(int j=0;j<NumR;j++)
			{
				float depth=XtfRead->XtfCurve.deptop+j*XtfRead->XtfCurve.rlevcv;
				XtfRead->ReadMatrix(index,depth,buffer);
				WisWrite->WriteWaveArray(depth,buffer);
			}
			delete []buffer; buffer=NULL;
		}
	}
	WisWrite->Close();
	XtfRead->Close();
	////////////////////////////////
	delete WisWrite->m_ObjectEntry; 
	WisWrite->m_ObjectEntry=NULL;
	delete WisWrite->WisChannel; 
	WisWrite->WisChannel=NULL;
	
	return !sp_Break_Status;
}
void CXtfToWis::GetWisObjectIndexFromXtf()
{
	CString s,sCurveName;

	if(XtfRead->XtfHead.isnumcv<=0)
	{
		fprintf(fp,"\n!!!!����û������ʵ��\n\n");
		return;
	}
	CurveIndex.RemoveAll();
	WisWrite->WisHead.ObjectNumber=0;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	if(m_nCurvesRadio==3)
	{	//��������
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=15;
		ReCurNamDlg.m_nMaxLog=9999;
		for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			int nDim=XtfRead->XtfHead.CurveAttrib[i].CurType;
			sCurveName.Format ("%-.8s",XtfRead->XtfHead.CurveName[i]);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			ReCurNamDlg.m_ObjectName.Add(sCurveName);
			s.Format ("%-.8s",XtfRead->XtfHead.Units[i]);
			s.TrimRight();
			ReCurNamDlg.m_Units.Add(s);
			ReCurNamDlg.m_NumDim.Add(nDim);
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
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			for(int j=0;j<XtfRead->XtfHead.isnumcv;j++)
			{
				sCurveName.Format ("%-.8s",XtfRead->XtfHead.CurveName[j]);
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
	{	//�ǽ�������
		for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			if(m_nCurvesRadio==2)
			{
				if(XtfRead->XtfHead.CurveAttrib[i].CurType>1)
				{
					continue; //ֻҪ��������
				}
			}
			bool bOk=false;
			sCurveName.Format ("%-.8s",XtfRead->XtfHead.CurveName[i]);
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
			s.Format ("%-.8s",XtfRead->XtfHead.Units[i]);
			s.TrimRight();
			NewUnits.Add(s);
		}
	}
}
short CXtfToWis::XtfRepCToWis(short XtfRepC)
{
	short WisRepC=XTF_REPR_FLOAT;
	switch(XtfRepC)
	{
	case XTF_REPR_INT2://2�ֽ��з��Ŷ�������
		WisRepC=WIS_REPR_SHORT;
		break;
	case XTF_REPR_UCHAR://1�ֽ��޷����ֽ�
		WisRepC=WIS_REPR_UCHAR;
		break;
	case XTF_REPR_FLOAT://4�ֽ�IEEE�����ȸ�����	
		WisRepC=WIS_REPR_FLOAT;
		break;
	case XTF_REPR_BIT://��λֵ
	case XTF_REPR_CHAR://1�ֽ��з����ֽ���
		WisRepC=WIS_REPR_CHAR;
		break;
	case XTF_REPR_DOUBLE://8�ֽ�˫���ȸ���
		WisRepC=WIS_REPR_DOUBLE;
		break;
	case XTF_REPR_UINT2://2�ֽ��޷��Ŷ�������
		WisRepC=WIS_REPR_USHORT;
		break;
	case XTF_REPR_INT4://4�ֽڳ�����
		WisRepC=WIS_REPR_LONG;
		break;
	case XTF_REPR_UINT4://4�ֽ��޷��ų�������
		WisRepC=WIS_REPR_ULONG;
	default:
		WisRepC=XTF_REPR_FLOAT;
		break;
	}
	return WisRepC;
}
void CXtfToWis::SetWisChannelFromXtfChannel(int IndexXtf,int IndexWis)
{
	CString s;
	
	//��������(����������WIS�����������)
	sprintf(WisWrite->m_ObjectEntry->Name,"%-.15s",NewCurveName.GetAt(IndexWis));
	WisWrite->m_ObjectEntry->Attribute=1;
	int kDim=XtfRead->XtfCurve.Ictype; //WIS�ļ�ͨ��ά��
	if(kDim==1)
	{
		WisWrite->m_ObjectEntry->SubAttribute=1;
	}
	else
	{
		WisWrite->m_ObjectEntry->SubAttribute=2;
	}
	//ͨ������
	sprintf(WisWrite->WisChannel->Unit,"%-.7s",NewUnits.GetAt(IndexWis));
	sprintf(WisWrite->WisChannel->AliasName,"%-.8s",XtfRead->XtfCurve.Name);
	sprintf(WisWrite->WisChannel->AliasUnit,"%-.15s",WisWrite->WisChannel->Unit);
	
	WisWrite->WisChannel->RepCode=XtfRepCToWis(XtfRead->XtfCurve.RepCode);
	WisWrite->WisChannel->CodeLen=WisCodeLen(WisWrite->WisChannel->RepCode);
	WisWrite->WisChannel->MinVal=XtfRead->XtfCurve.curvmin;
	WisWrite->WisChannel->MaxVal=XtfRead->XtfCurve.curvmax;
	WisWrite->WisChannel->Reserved=0;
	WisWrite->WisChannel->NumOfDimension=kDim;//ά��
	DWORD NumPoint=DWORD(fabs((XtfRead->XtfCurve.depbot-XtfRead->XtfCurve.deptop)/XtfRead->XtfCurve.rlevcv)+0.5)+1;
	//����һά��Ϣ
	//����������λ�ж����ߵĲ����������ͣ�ʱ�����ȣ�
	IndexSacle=1; //�������ת������
	int IndexType=0; //��Ȼ�ʱ������

	//��һά
	sprintf(WisWrite->WisChannel->DimInfo[0].Name,"%-.7s","Depth");
	sprintf(WisWrite->WisChannel->DimInfo[0].Unit,"%-.7s","m");
	//����Ϊ���ס�
	IndexSacle=1.0;
	s.Format("%-.8s",XtfRead->XtfCurve.DepthUnits);
	s.TrimRight();
	if(s.CompareNoCase("dm")==0)
	{	
		IndexSacle=0.1f;//����
	}
	else if(s.CompareNoCase("feet")==0)
	{	
		IndexSacle=0.3048f;//Ӣ��
	}

	if(s.CompareNoCase("seconds")==0 || s.CompareNoCase("millsecs")==0)
	{	//ʱ������
		IndexType=1;
		sprintf(WisWrite->WisChannel->DimInfo[0].Name,"%-.7s","Time");
		sprintf(WisWrite->WisChannel->DimInfo[0].Unit,"%-.7s",XtfRead->XtfCurve.DepthUnits);
	}
	sprintf(WisWrite->WisChannel->DimInfo[0].AliasName,"%-.15s",WisWrite->WisChannel->DimInfo[0].Name);
	WisWrite->WisChannel->DimInfo[0].StartVal=XtfRead->XtfCurve.deptop;
	WisWrite->WisChannel->DimInfo[0].Delta=XtfRead->XtfCurve.rlevcv;
	WisWrite->WisChannel->DimInfo[0].Samples=NumPoint;
	WisWrite->WisChannel->DimInfo[0].MaxSamples=WisWrite->WisChannel->DimInfo[0].Samples;
	int Count=1;
	if(XtfRead->XtfHead.ndimension[IndexXtf]==1)
	{
		Count=XtfRead->XtfHead.idimen1[IndexXtf];
	}
	else if(XtfRead->XtfHead.ndimension[IndexXtf]==2)
	{
		Count=XtfRead->XtfHead.idimen1[IndexXtf]*XtfRead->XtfHead.idimen2[IndexXtf];
	}
	else if(XtfRead->XtfHead.ndimension[IndexXtf]==3)
	{
		Count=XtfRead->XtfHead.idimen1[IndexXtf]*XtfRead->XtfHead.idimen2[IndexXtf]*XtfRead->XtfHead.idimen3[IndexXtf];
	}
	WisWrite->WisChannel->DimInfo[0].Size=Count*WisWrite->WisChannel->CodeLen;
	WisWrite->WisChannel->DimInfo[0].RepCode=WIS_REPR_FLOAT;
	WisWrite->WisChannel->DimInfo[0].Reserved=0;
	//���ö�ά��Ϣ
	if(WisWrite->WisChannel->NumOfDimension==2)
	{
		sprintf(WisWrite->WisChannel->DimInfo[1].Name,"%-.7s","Time");
		sprintf(WisWrite->WisChannel->DimInfo[1].Unit,"%-.7s",XtfRead->XtfCurve.TimeIncrementUnits);
		sprintf(WisWrite->WisChannel->DimInfo[1].AliasName,"%-.15s",WisWrite->WisChannel->DimInfo[1].Name);
		WisWrite->WisChannel->DimInfo[1].StartVal=XtfRead->XtfCurve.starttm;
		WisWrite->WisChannel->DimInfo[1].Delta=XtfRead->XtfCurve.timeinc;
		if(XtfRead->XtfCurve.timeinc==0)
		{
			WisWrite->WisChannel->DimInfo[1].Delta=5;
		}
		WisWrite->WisChannel->DimInfo[1].Samples=XtfRead->XtfCurve.Idims[0];
		WisWrite->WisChannel->DimInfo[1].MaxSamples=WisWrite->WisChannel->DimInfo[1].Samples;
		WisWrite->WisChannel->DimInfo[1].Size=WisWrite->WisChannel->DimInfo[1].Samples*WisWrite->WisChannel->CodeLen;
		WisWrite->WisChannel->DimInfo[1].RepCode=WIS_REPR_FLOAT;
		WisWrite->WisChannel->DimInfo[1].Reserved=0;
	}
	if(WisWrite->WisChannel->NumOfDimension==3)
	{
		sprintf(WisWrite->WisChannel->DimInfo[2].Name,"%-.7s","Time");
		sprintf(WisWrite->WisChannel->DimInfo[2].Unit,"%-.7s",XtfRead->XtfCurve.TimeIncrementUnits);
		sprintf(WisWrite->WisChannel->DimInfo[2].AliasName,"%-.15s",WisWrite->WisChannel->DimInfo[2].Name);
		WisWrite->WisChannel->DimInfo[2].StartVal=XtfRead->XtfCurve.starttm;
		WisWrite->WisChannel->DimInfo[2].Delta=XtfRead->XtfCurve.timeinc;
		if(XtfRead->XtfCurve.timeinc==0)
		{
			WisWrite->WisChannel->DimInfo[2].Delta=5;
		}
		WisWrite->WisChannel->DimInfo[2].Samples=XtfRead->XtfCurve.Idims[0];
		WisWrite->WisChannel->DimInfo[2].MaxSamples=WisWrite->WisChannel->DimInfo[1].Samples;
		WisWrite->WisChannel->DimInfo[2].Size=WisWrite->WisChannel->DimInfo[1].Samples*WisWrite->WisChannel->CodeLen;
		WisWrite->WisChannel->DimInfo[2].RepCode=WIS_REPR_FLOAT;
		WisWrite->WisChannel->DimInfo[2].Reserved=0;

		sprintf(WisWrite->WisChannel->DimInfo[1].Name,"%-.7s","SENSOR");
		sprintf(WisWrite->WisChannel->DimInfo[1].Unit,"%-.7s","1");
		sprintf(WisWrite->WisChannel->DimInfo[1].AliasName,"%-.15s",WisWrite->WisChannel->DimInfo[1].Name);
		WisWrite->WisChannel->DimInfo[1].StartVal=1;
		WisWrite->WisChannel->DimInfo[1].Delta=1;
		WisWrite->WisChannel->DimInfo[1].Samples=XtfRead->XtfCurve.Idims[1];
		WisWrite->WisChannel->DimInfo[1].MaxSamples=WisWrite->WisChannel->DimInfo[1].Samples;
		WisWrite->WisChannel->DimInfo[1].Size=Count*WisWrite->WisChannel->CodeLen;
		WisWrite->WisChannel->DimInfo[1].RepCode=WIS_REPR_FLOAT;
		WisWrite->WisChannel->DimInfo[1].Reserved=0;
	}

}

