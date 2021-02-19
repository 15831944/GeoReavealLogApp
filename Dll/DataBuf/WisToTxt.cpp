// WisToTxt.cpp: implementation of the CWisToTxt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "WisToTxt.h"
#include <stdlib.h>
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

CWisToTxt::CWisToTxt()
{

}

CWisToTxt::~CWisToTxt()
{

}
bool CWisToTxt::WisToTxt(CString SourceFile,CString ObjectPath,int nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo)
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

	WisRead=new CWisRead ;
	TxtW=new CFwdTxtW ; //输出类
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	//输出转换结果信息
	if(bResult)
	{
		TxtW->Prt_Header(fp);
	}

	delete pGuage;  //删除进度条
	delete TxtW; TxtW=NULL;
	delete WisRead;	WisRead=NULL;

	CurveIndex.RemoveAll();
	return bResult;
}
bool CWisToTxt::Transform()
{
	CString s;
	if(!IsWIS(m_SourceFile))
	{
		fprintf(fp,"%s is not WIS Format\n\n",m_SourceFile);
		return false;
	}
	//根据输出目录及输入文件名确定井名和输出路径
	sOutFileName.Format ("%s\\%s.txt",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //根据条件决定是否修改文件名
	if(!WisRead->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!打开输入文件%s有错\n\n",m_SourceFile);
		return false;
	}
	WisRead->GetHead();
	WisRead->GetObjectEntry();
	pGuage->SetRange(0,WisRead->WisHead.ObjectNumber);

	//读Wis对象信息，并选择其中所要的曲线
	TxtW->ResetHead();
	SetTxtChannel();
	if(TxtW->TxtHd.NumLog<1)
	{
		WisRead->Close();
		fprintf(fp,"\n!!!!没有要处理的曲线\n");
		return false;
	}
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);
	if(TxtW->Open(sOutFileName))
	{
		TxtW->TxtHd.NumPoint=int((TxtW->TxtHd.Endep-TxtW->TxtHd.Stdep)/TxtW->TxtHd.Rlev+0.5)+1;
		TxtW->WriteHead(); //输出头信息
		pGuage->SetRange(0,TxtW->TxtHd.NumLog);
		for(int i=0;i<TxtW->TxtHd.NumLog;i++)
		{
			pGuage->SetPos(i+1);  //设置进度条当前值
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			int index=CurveIndex.GetAt(i); //从选择曲线中得到索引号
			//读通道信息
			WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
			//从WIS通道设置通道信息
			DWORD NumPoint=WisRead->WisChannel.DimInfo[0].Samples;
			float Start1=WisRead->WisChannel.DimInfo[0].StartVal;
			float Rlev1=WisRead->WisChannel.DimInfo[0].Delta;
			float Stop1=Start1+(NumPoint-1)*Rlev1;
			float *buf1=new float[NumPoint];
			float *buf2=new float[TxtW->TxtHd.NumPoint];
			for(int j=0;j<TxtW->TxtHd.NumPoint;j++) buf2[j]=-999.25;
			WisRead->ReadCurve(index,Start1,NumPoint,buf1);
			FloatResample(Start1,Stop1,Rlev1,buf1,TxtW->TxtHd.Stdep,TxtW->TxtHd.Endep,TxtW->TxtHd.Rlev,buf2);
			TxtW->WriteChannel(i,buf2);
			delete []buf1; buf1=NULL;
			delete []buf2; buf2=NULL;
		}
		TxtW->Close();
	}
	WisRead->Close();
	return !sp_Break_Status;
}
void CWisToTxt::SetTxtChannel()
{
	CString s;
	TxtW->TxtHd.NumLog=0;
	CurveIndex.RemoveAll();
	for(int index=0;index<WisRead->WisHead.ObjectNumber;index++)
	{
		if(WisRead->m_ObjectEntry[index].Status != 0) continue; //不是正常对象
		if(WisRead->m_ObjectEntry[index].Attribute != 1) continue; //不是通道对象
		if(WisRead->m_ObjectEntry[index].BlockNum<1) continue; //没有数据
		//读通道信息
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
		bool bIsContinue=true;//是否为连续采样通道
		for(int i=0;i<WisRead->WisChannel.NumOfDimension;i++)
		{
			if(WisRead->WisChannel.DimInfo[i].Delta==0)
			{
				bIsContinue=false;
				break;
			}
		}
		if(!bIsContinue) continue; //不是连续采样通道,如FMT等数据
		if(WisRead->WisChannel.NumOfDimension>1) continue; //只能转换常规曲线
		bool bOk=false;
		CString sCurveName;
		sCurveName.Format ("%s",WisRead->m_ObjectEntry[index].Name);
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
	SelectChannelFromWin();
	GetCurvesAndDepth();
	//设置井段
	if(CurveIndex.GetSize()>0)
	{
		SetStartStopRlev();
	}
}
void CWisToTxt::SetStartStopRlev()
{
	int nDep;
	CModiConvDlg ModiConvDlg;
	switch(m_nConvEncode)
	{
	case 1: //根据深度计算适当的起止深度及采样间隔
		nDep=int(TxtW->TxtHd.Stdep/m_FixRlev+0.95);
		TxtW->TxtHd.Stdep=nDep*m_FixRlev;
		nDep=int(TxtW->TxtHd.Endep/m_FixRlev);
		TxtW->TxtHd.Endep=nDep*m_FixRlev;
		TxtW->TxtHd.Rlev=m_FixRlev;
		break;
	case 2: //人工指定起止深度及采样间隔
		ModiConvDlg.m_OldStart=TxtW->TxtHd.Stdep;
		ModiConvDlg.m_OldStop=TxtW->TxtHd.Endep;
		ModiConvDlg.m_OldRlev=TxtW->TxtHd.Rlev;
		if(ModiConvDlg.DoModal()==IDOK)
		{
			TxtW->TxtHd.Stdep=ModiConvDlg.m_NewStart;
			TxtW->TxtHd.Endep=ModiConvDlg.m_NewStop;
			TxtW->TxtHd.Rlev=ModiConvDlg.m_NewRlev;
		}
		break;
	default://不必重新指定：以最小采样间隔及最大井段
		break;
	}
}
void CWisToTxt::SelectChannelFromWin()
{
	CString s,sCurveName;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	bool bLongName=false;
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		sCurveName.Format ("%s",WisRead->m_ObjectEntry[k].Name);
		if(sCurveName.GetLength()>15)
		{
			bLongName=true;
		}
	}
	if(!bLongName && m_nCurvesRadio!=3)
	{
		for(int i=0;i<CurveIndex.GetSize();i++)
		{
			int k=CurveIndex.GetAt(i);
			sCurveName.Format ("%s",WisRead->m_ObjectEntry[k].Name);
			SelCurveName.Add(sCurveName);
			sCurveName.TrimRight();
			NewCurveName.Add(sCurveName);
		}
		return;
	}
	//////////////////////////////////////////////
	CReCurNamDlg ReCurNamDlg;
	ReCurNamDlg.m_nMaxLen=15;
	ReCurNamDlg.m_nMaxLog=512;
	for(i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		sCurveName.Format ("%s",WisRead->m_ObjectEntry[k].Name);
		sCurveName.TrimLeft();
		sCurveName.TrimRight();
		ReCurNamDlg.m_SourceName.Add(sCurveName);
		ReCurNamDlg.m_ObjectName.Add(sCurveName);
		ReCurNamDlg.m_Units.Add("");
		ReCurNamDlg.m_NumDim.Add(1);
	}
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
void CWisToTxt::GetCurvesAndDepth()
{
	TxtW->TxtHd.NumLog=SelCurveName.GetSize();
	float Start,Stop,Step;
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[k].Name);
		if(i==0)
		{
			Start=WisRead->WisChannel.DimInfo[0].StartVal;
			Step=WisRead->WisChannel.DimInfo[0].Delta;
			Stop=Start+(WisRead->WisChannel.DimInfo[0].Samples-1)*Step;
		}
		else
		{
			float sdep=WisRead->WisChannel.DimInfo[0].StartVal;
			float rlev=WisRead->WisChannel.DimInfo[0].Delta;
			float edep=sdep+(WisRead->WisChannel.DimInfo[0].Samples-1)*rlev;
			if(Start>sdep) Start=sdep;
			if(Stop<edep) Stop=edep;
			if(Step>rlev) Step=rlev;
		}
		TxtW->TxtHd.Stdep=Start;
		TxtW->TxtHd.Endep=Stop;
		TxtW->TxtHd.Rlev=Step;
		sprintf(TxtW->TxtHd.CurveName[i],"%s",NewCurveName.GetAt(i));
	}
}

