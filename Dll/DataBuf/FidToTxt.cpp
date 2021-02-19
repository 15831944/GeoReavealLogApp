// FidToTxt.cpp: implementation of the CFidToTxt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToTxt.h"
#include "ModiConvDlg.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidToTxt::CFidToTxt()
{

}

CFidToTxt::~CFidToTxt()
{

}
bool CFidToTxt::FidToTxt(CString SourceFile,CString sObjectPath,int nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //源文件名   
	m_sObjectPath=sObjectPath;  //输出目录
	m_nCurvesRadio=nCurvesRadio;//过滤开关
	m_nCurvesRadio=nCurvesRadio;//过滤开关
	m_FixRlev=FixRlev;
	m_nConvEncode=nConvEncode;
	fp=fo;

	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	CurveIndex.RemoveAll();
	FidRead=new CFidRead;
	TxtW=new CFwdTxtW;
	bool bResult=Transform();

	//输出转换结果信息
	if(bResult)
	{
		TxtW->Prt_Header(fp);
	}

	////////////////////////////////////
	CurveIndex.RemoveAll();
	delete TxtW;	TxtW=NULL;
	delete FidRead; FidRead=NULL;
	delete pGuage;  //删除进度条
	return bResult;
}
bool CFidToTxt::Transform()
{
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//根据输出目录及输入文件名确定输出文件名
	sOutFileName.Format ("%s\\%s.txt",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //根据条件决定是否修改文件名
	bool bResult=FidRead->ReadFidInfo(m_SourceFile);
	if(!bResult)
	{
		fprintf(fp,"\n!!!!链接的数据文件打开失败\n\n");
		return false;
	}
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return false;
	}

	//////////////
	CString sInWellPath=MyGetPathName(m_SourceFile);//FID曲线数据所在目录
	CString sInWellName=MyGetFileTitle(m_SourceFile);//FID曲线数据所在目录
	TxtW->ResetHead();
	GetTxtHeadFromFid();
	if((TxtW->TxtHd.Endep<=TxtW->TxtHd.Stdep) || (TxtW->TxtHd.NumLog<1))
	{
		fprintf(fp,"\n!!!!没有要处理的曲线\n");
		return false;
	}
	SetStartStopRlev();
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
			float Start1=FidRead->FidChannel[CurveIndex[i]].DimInfo[0].Start;
			float Stop1=FidRead->FidChannel[CurveIndex[i]].DimInfo[0].Stop;
			float Rlev1=FidRead->FidChannel[CurveIndex[i]].DimInfo[0].Rlev;
			int Num1=int((Stop1-Start1)/Rlev1+0.5)+1;
			float *buf1=new float[Num1];
			float *buf2=new float[TxtW->TxtHd.NumPoint];
			CString sCurveName;
			sCurveName.Format("%-.32s",FidRead->FidChannel[CurveIndex[i]].CurveName);
			FidRead->ReadGenData(sCurveName,Num1,buf1);
			FloatResample(Start1,Stop1,Rlev1,buf1,TxtW->TxtHd.Stdep,TxtW->TxtHd.Endep,TxtW->TxtHd.Rlev,buf2);
			TxtW->WriteChannel(i,buf2);
			delete []buf1;	buf1=NULL;
			delete []buf2;	buf2=NULL;
		}
		TxtW->Close();
	}
	return !sp_Break_Status;
}

void CFidToTxt::GetTxtHeadFromFid()
{
	CString s,sCurveName;
	CStringArray SelCurveName,NewCurveName;
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return;
	}
	bool bHasLenName=false; //是否有超长曲线名
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		if(FidRead->FidChannel[i].NumOfDimension != 2)
			continue; //只要常规曲线

		bool bOk=false;
		sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
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
		if(!bOk) continue;
		SelCurveName.Add(sCurveName);
		if(sCurveName.GetLength()>15)
		{	//曲线名长度大于TXT文件对曲线名的要求
			bHasLenName=true;
		}
	}

	//对长曲线名进行修正
	if(bHasLenName || m_nCurvesRadio==3 || CurveIndex.GetSize()>128 || SelCurveName.GetSize()>128)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLog=128;
		ReCurNamDlg.m_nMaxLen=15;
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
			ReCurNamDlg.m_ObjectName.Add(SelCurveName.GetAt(i));
			ReCurNamDlg.m_Units.Add("");
			ReCurNamDlg.m_NumDim.Add(1);
		}
		SelCurveName.RemoveAll();
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
	}
	else
	{
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			NewCurveName.Add(SelCurveName.GetAt(i));
		}
	}
	//查找曲线名的索引号
	CurveIndex.RemoveAll();
	TxtW->TxtHd.NumLog=SelCurveName.GetSize();
	for(i=0;i<SelCurveName.GetSize();i++)
	{
		for(int j=0;j<FidRead->FidHead.NumLog;j++)
		{
			sCurveName.Format("%-.32s",FidRead->FidChannel[j].CurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
			{
				CurveIndex.Add(j);
				strcpy(TxtW->TxtHd.CurveName[i],(const char*)NewCurveName.GetAt(i));
			}
		}
	}
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	//搜索曲线的起止深度、采样间隔最值
	for(i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		if(i==0)
		{
			TxtW->TxtHd.Stdep=FidRead->FidChannel[k].DimInfo[0].Start;
			TxtW->TxtHd.Endep=FidRead->FidChannel[k].DimInfo[0].Stop;
			TxtW->TxtHd.Rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
		}
		else
		{
			if(TxtW->TxtHd.Stdep>FidRead->FidChannel[k].DimInfo[0].Start)
			{
				TxtW->TxtHd.Stdep=FidRead->FidChannel[k].DimInfo[0].Start;
			}
			if(TxtW->TxtHd.Endep<FidRead->FidChannel[k].DimInfo[0].Stop)
			{
				TxtW->TxtHd.Endep=FidRead->FidChannel[k].DimInfo[0].Stop;
			}
			if(TxtW->TxtHd.Rlev>FidRead->FidChannel[k].DimInfo[0].Rlev)
			{
				TxtW->TxtHd.Rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
			}
		}
	}
}
void CFidToTxt::SetStartStopRlev()
{
	CString s;
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