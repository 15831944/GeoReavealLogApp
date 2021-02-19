// XtfToSlc716.cpp: implementation of the CXtfToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "XtfToSlc716.h"
#include <stdlib.h>
#include <math.h>
#include "ModiConvDlg.h"
#include "ReCurNamDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXtfToSlc716::CXtfToSlc716()
{

}

CXtfToSlc716::~CXtfToSlc716()
{

}
bool CXtfToSlc716::XtfToSlc716(CString SourceFile,CString ObjectPath,int nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo)
{
	CString s;

	m_SourceFile=SourceFile; //源文件名   
	m_sObjectPath=ObjectPath;  //输出目录
	m_nCurvesRadio=nCurvesRadio;//过滤开关
	m_nConvEncode=nConvEncode;
	m_FixRlev=FixRlev;
	fp=fo;

	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	XtfRead=new CXtfRead ;
	Slc716W=new CSlc716W ; //输出类
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	//输出转换结果信息
	if(bResult)
	{
		Slc716W->PrintHead(fp);
	}

	delete pGuage;  //删除进度条
	delete Slc716W; Slc716W=NULL;
	delete XtfRead;	XtfRead=NULL;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	return bResult;
}
bool CXtfToSlc716::Transform()
{
	CString s;
	if(!IsXTF(m_SourceFile))
	{
		fprintf(fp,"%s is not XTF Format\n\n",m_SourceFile);
		return false;
	}
	//根据输出目录及输入文件名确定井名和输出路径
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //根据条件决定是否修改文件名

	if(!XtfRead->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!打开输入文件%s有错\n\n",m_SourceFile);
		return false;
	}
	XtfRead->GetHead();
	Slc716W->ResetHead();
	SetSlc716Channel();
	if(Slc716W->Slc716Head.NumLog<1)
	{
		XtfRead->Close();
		fprintf(fp,"\n!!!!没有要处理的曲线\n");
		return false;
	}
	if(Slc716W->Open(sOutFileName))
	{
		Slc716W->NumPoint=int((Slc716W->Slc716Head.Endep-Slc716W->Slc716Head.Stdep)/Slc716W->Slc716Head.Rlev+0.5)+1;
		Slc716W->NumBlock=(Slc716W->NumPoint-1)/64+1;
		Slc716W->ClearHead(); //清输出文件头
		Slc716W->WriteHead(); //输出头信息
		pGuage->SetRange(0,Slc716W->Slc716Head.NumLog);
		for(int i=0;i<Slc716W->Slc716Head.NumLog;i++)
		{
			pGuage->SetPos(i+1);  //设置进度条当前值
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			int index=CurveIndex.GetAt(i); //从选择曲线中得到索引号
			int nResult=XtfRead->OpenCurve(index); //读通道信息

			DWORD NumPoint=XtfRead->XtfCurve.NumberOfLevelsForTheCurve;
			float *buf1=new float[NumPoint];
			float *buf2=new float[Slc716W->NumBlock*64];
			for(int j=0;j<Slc716W->NumBlock*64;j++) buf2[j]=-999.25;
			float Start1=XtfRead->XtfCurve.deptop;
			float Rlev1=XtfRead->XtfCurve.rlevcv;
			float Stop1=Start1+(NumPoint-1)*Rlev1;
			XtfRead->ReadCurve(index,NumPoint,buf1);
			FloatResample(Start1,Stop1,Rlev1,buf1,Slc716W->Slc716Head.Stdep,Slc716W->Slc716Head.Endep,Slc716W->Slc716Head.Rlev,buf2);
			Slc716W->WriteChannel(i,buf2);
			delete []buf1; buf1=NULL;
			delete []buf2; buf2=NULL;
		}
		Slc716W->Close();
	}
	XtfRead->Close();
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);
	
	return !sp_Break_Status;
}
void CXtfToSlc716::SetSlc716Channel()
{
	CString s,sCurveName;
	memcpy(Slc716W->Slc716Head.WellName,XtfRead->XtfHead.Wellname,80);
	memcpy(Slc716W->Slc716Head.CompanyName,XtfRead->XtfHead.Companyname,80);
	Slc716W->Slc716Head.NumLog=0;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	if(m_nCurvesRadio==0)
	{
		for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			if(XtfRead->XtfHead.CurveAttrib[i].CurType>1)
			{
				continue; //不是常规曲线被忽略
			}
			sCurveName.Format("%-.8s",XtfRead->XtfHead.CurveName[i]);
			sCurveName.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					SelCurveName.Add(sCurveName);
					break;
				}
			}
		}
	}
	else
	{
		for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			if(XtfRead->XtfHead.CurveAttrib[i].CurType>1)
			{
				continue; //不是常规曲线被忽略
			}
			sCurveName.Format("%-.8s",XtfRead->XtfHead.CurveName[i]);
			sCurveName.TrimRight();
			SelCurveName.Add(sCurveName);
		}
	}
	//设置新曲线及对曲线名的长度进行判断
	bool bLongName=false;
	for(int i=0;i<SelCurveName.GetSize();i++)
	{
		s=SelCurveName.GetAt(i);
		s.TrimRight();
		NewCurveName.Add(s);
		if(s.GetLength()>4)
		{
			bLongName=true;
		}
	}
	if(!bLongName && m_nCurvesRadio!=3) 
	{
		GetIndex();
		return;
	}
	CReCurNamDlg ReCurNamDlg;
	ReCurNamDlg.m_nMaxLen=4;
	ReCurNamDlg.m_nMaxLog=128;
	for(i=0;i<SelCurveName.GetSize();i++)
	{
		ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
		ReCurNamDlg.m_ObjectName.Add(NewCurveName.GetAt(i));
		ReCurNamDlg.m_Units.Add("");
		ReCurNamDlg.m_NumDim.Add(1);
	}
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	int Result=ReCurNamDlg.DoModal();
	for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
	{
		SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
		NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
	}
	ReCurNamDlg.m_SourceName.RemoveAll();
	ReCurNamDlg.m_ObjectName.RemoveAll();
	ReCurNamDlg.m_Units.RemoveAll();
	ReCurNamDlg.m_NumDim.RemoveAll();
	GetIndex();
}
void CXtfToSlc716::GetIndex()
{

	CString sCurveName;
	CurveIndex.RemoveAll();
	//曲线及索引
	Slc716W->Slc716Head.NumLog=SelCurveName.GetSize();
	for(int i=0;i<SelCurveName.GetSize();i++)
	{
		char str[5];
		sprintf(str,"%-4.4s",NewCurveName.GetAt(i));
		memcpy(Slc716W->Slc716Head.CurveName[i],str,4);
		for(int j=0;j<XtfRead->XtfHead.isnumcv;j++)
		{
			sCurveName.Format("%-.8s",XtfRead->XtfHead.CurveName[j]);
			sCurveName.TrimRight();
			if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
			{
				CurveIndex.Add(j);
				break;
			}
		}
	}
	//索引最值及采样间隔
	float Start,Stop,Step;
	for(i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		int nResult=XtfRead->OpenCurve(k); //读通道信息
		Start=__min(XtfRead->XtfCurve.deptop,XtfRead->XtfCurve.depbot);
		Stop=__max(XtfRead->XtfCurve.deptop,XtfRead->XtfCurve.depbot);
		Step=float(fabs(XtfRead->XtfCurve.rlevcv));
		if(i==0)
		{
			Slc716W->Slc716Head.Stdep=Start;
			Slc716W->Slc716Head.Endep=Stop;
			Slc716W->Slc716Head.Rlev=Step;
		}
		else
		{
			if(Slc716W->Slc716Head.Stdep>Start)Slc716W->Slc716Head.Stdep=Start;
			if(Slc716W->Slc716Head.Endep<Stop)Slc716W->Slc716Head.Endep=Stop;
			if(Slc716W->Slc716Head.Rlev>Step)Slc716W->Slc716Head.Rlev=Step;
		}
	}
	if(CurveIndex.GetSize()>0)
	{
		SetStartStopRlev();
	}
}
void CXtfToSlc716::SetStartStopRlev()
{
	int nDep;
	CModiConvDlg ModiConvDlg;
	switch(m_nConvEncode)
	{
	case 1: //根据深度计算适当的起止深度及采样间隔
		nDep=int(Slc716W->Slc716Head.Stdep/m_FixRlev+0.95);
		Slc716W->Slc716Head.Stdep=nDep*m_FixRlev;
		nDep=int(Slc716W->Slc716Head.Endep/m_FixRlev);
		Slc716W->Slc716Head.Endep=nDep*m_FixRlev;
		Slc716W->Slc716Head.Rlev=m_FixRlev;
		break;
	case 2: //人工指定起止深度及采样间隔
		ModiConvDlg.m_OldStart=Slc716W->Slc716Head.Stdep;
		ModiConvDlg.m_OldStop=Slc716W->Slc716Head.Endep;
		ModiConvDlg.m_OldRlev=Slc716W->Slc716Head.Rlev;
		if(ModiConvDlg.DoModal()==IDOK)
		{
			Slc716W->Slc716Head.Stdep=ModiConvDlg.m_NewStart;
			Slc716W->Slc716Head.Endep=ModiConvDlg.m_NewStop;
			Slc716W->Slc716Head.Rlev=ModiConvDlg.m_NewRlev;
		}
		break;
	default://不必重新指定：以最小采样间隔及最大井段
		break;
	}
}