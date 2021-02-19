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
	m_SourceFile=SourceFile; //源文件名
	m_sObjectPath=ObjectPath; //输出目录
	m_nCurvesRadio=nCurvesRadio; //过滤开关
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

	//输出转换结果信息
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
	delete pGuage;  //删除进度条
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
	//根据输出目录及输入文件名确定输出文件名
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
	ModiFileName(WisFileName); //根据条件决定是否修改文件名

	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)WisFileName);
	if(!XtfRead->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!打开输入文件%s有错\n\n",m_SourceFile);
		return false;
	}
	XtfRead->GetHead();
	//////////////////////////////////////////////////////////////////////
	CString sInWellPath=MyGetPathName(m_SourceFile);//XTF曲线数据所在目录
	CString sInWellName=MyGetFileTitle(m_SourceFile);//XTF曲线数据对应井名
	WisWrite->Open(WisFileName);
	GetWisObjectIndexFromXtf();
	//调整允许记录的最大对象数
	WisWrite->InitWisHead(CurveIndex.GetSize());
	CString sCurveName;
	WisWrite->m_ObjectEntry=NULL; //对象入口
	WisWrite->WisChannel=NULL; //通道信息
	WisWrite->m_ObjectEntry=new WIS_OBJECT_ENTRY;
	WisWrite->WisChannel=new WIS_CHANNEL;
	WisWrite->InitWisObjectEntry();
	WisWrite->InitWisChannel();
	///////////////////////////
	pGuage->SetRange(0,CurveIndex.GetSize());
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		pGuage->SetPos(i+1);  //设置进度条当前值
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int index=CurveIndex.GetAt(i); //从选择曲线中得到索引号
		int nResult=XtfRead->OpenCurve(index); //读通道信息
		if(nResult<0)
		{
			continue; //曲头有错误，忽略该曲线
		}
		SetWisChannelFromXtfChannel(index,i);	//从XTF通道设置FID通道信息
		WisWrite->WriteChannel(); //写通道信息到WIS文件
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
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return;
	}
	CurveIndex.RemoveAll();
	WisWrite->WisHead.ObjectNumber=0;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	if(m_nCurvesRadio==3)
	{	//交互操作
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
	{	//非交互操作
		for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			if(m_nCurvesRadio==2)
			{
				if(XtfRead->XtfHead.CurveAttrib[i].CurType>1)
				{
					continue; //只要常规曲线
				}
			}
			bool bOk=false;
			sCurveName.Format ("%-.8s",XtfRead->XtfHead.CurveName[i]);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			if(m_nCurvesRadio==0)
			{	//过滤
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
	case XTF_REPR_INT2://2字节有符号短整型数
		WisRepC=WIS_REPR_SHORT;
		break;
	case XTF_REPR_UCHAR://1字节无符号字节
		WisRepC=WIS_REPR_UCHAR;
		break;
	case XTF_REPR_FLOAT://4字节IEEE单精度浮点数	
		WisRepC=WIS_REPR_FLOAT;
		break;
	case XTF_REPR_BIT://单位值
	case XTF_REPR_CHAR://1字节有符号字节数
		WisRepC=WIS_REPR_CHAR;
		break;
	case XTF_REPR_DOUBLE://8字节双精度浮点
		WisRepC=WIS_REPR_DOUBLE;
		break;
	case XTF_REPR_UINT2://2字节无符号短整型数
		WisRepC=WIS_REPR_USHORT;
		break;
	case XTF_REPR_INT4://4字节长整型
		WisRepC=WIS_REPR_LONG;
		break;
	case XTF_REPR_UINT4://4字节无符号长整型数
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
	
	//对象设置(其它参数在WIS输出类中设置)
	sprintf(WisWrite->m_ObjectEntry->Name,"%-.15s",NewCurveName.GetAt(IndexWis));
	WisWrite->m_ObjectEntry->Attribute=1;
	int kDim=XtfRead->XtfCurve.Ictype; //WIS文件通道维数
	if(kDim==1)
	{
		WisWrite->m_ObjectEntry->SubAttribute=1;
	}
	else
	{
		WisWrite->m_ObjectEntry->SubAttribute=2;
	}
	//通道参数
	sprintf(WisWrite->WisChannel->Unit,"%-.7s",NewUnits.GetAt(IndexWis));
	sprintf(WisWrite->WisChannel->AliasName,"%-.8s",XtfRead->XtfCurve.Name);
	sprintf(WisWrite->WisChannel->AliasUnit,"%-.15s",WisWrite->WisChannel->Unit);
	
	WisWrite->WisChannel->RepCode=XtfRepCToWis(XtfRead->XtfCurve.RepCode);
	WisWrite->WisChannel->CodeLen=WisCodeLen(WisWrite->WisChannel->RepCode);
	WisWrite->WisChannel->MinVal=XtfRead->XtfCurve.curvmin;
	WisWrite->WisChannel->MaxVal=XtfRead->XtfCurve.curvmax;
	WisWrite->WisChannel->Reserved=0;
	WisWrite->WisChannel->NumOfDimension=kDim;//维数
	DWORD NumPoint=DWORD(fabs((XtfRead->XtfCurve.depbot-XtfRead->XtfCurve.deptop)/XtfRead->XtfCurve.rlevcv)+0.5)+1;
	//设置一维信息
	//根据索引单位判断曲线的采样驱动类型（时间或深度）
	IndexSacle=1; //深度索引转换因子
	int IndexType=0; //深度或时间索引

	//第一维
	sprintf(WisWrite->WisChannel->DimInfo[0].Name,"%-.7s","Depth");
	sprintf(WisWrite->WisChannel->DimInfo[0].Unit,"%-.7s","m");
	//隐含为“米”
	IndexSacle=1.0;
	s.Format("%-.8s",XtfRead->XtfCurve.DepthUnits);
	s.TrimRight();
	if(s.CompareNoCase("dm")==0)
	{	
		IndexSacle=0.1f;//分米
	}
	else if(s.CompareNoCase("feet")==0)
	{	
		IndexSacle=0.3048f;//英尺
	}

	if(s.CompareNoCase("seconds")==0 || s.CompareNoCase("millsecs")==0)
	{	//时间驱动
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
	//设置二维信息
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

