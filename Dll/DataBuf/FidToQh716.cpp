// FidToQh716.cpp: implementation of the CFidToQh716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToQh716.h"
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

CFidToQh716::CFidToQh716()
{

}

CFidToQh716::~CFidToQh716()
{

}

bool CFidToQh716::FidToQh716(CString SourceFile,CString sObjectPath,int nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo)
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
	Qh716W=new CQh716W;
	bool bResult=Transform();

	//输出转换结果信息
	if(bResult)
		Qh716W->PrintHead(fp);

	////////////////////////////////////
	CurveIndex.RemoveAll();
	delete Qh716W;	Qh716W=NULL;
	delete FidRead; FidRead=NULL;
	delete pGuage;  //删除进度条
	return bResult;
}
bool CFidToQh716::Transform()
{
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//根据输出目录及输入文件名确定输出文件名
	sOutFileName.Format ("%s\\%s.dat",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //根据条件决定是否修改文件名
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);

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
	Qh716W->ResetHead();
	GetQh716HeadFromFid();
	if(Qh716W->Head.NumLog<1)
	{
		fprintf(fp,"\n!!!!无适合曲线数据!\n\n");
		return false;
	}
	SetStartStopRlev();
	if(Qh716W->Head.Endep>Qh716W->Head.Stdep)
	{
		if(Qh716W->Open(sOutFileName))
		{
			Qh716W->WriteHead(); //输出头信息
			fprintf(fp,"====File Length = %d\n",Qh716W->FileSize);
			pGuage->SetRange(0,Qh716W->Head.NumLog);
			float *buf2=new float[Qh716W->NumBlock*int(Qh716W->Head.Feet)];
			for(int i=0;i<Qh716W->NumBlock*int(Qh716W->Head.Feet);i++)
			{
				buf2[i]=-9999.0;
			}
			for(i=0;i<Qh716W->Head.NumLog;i++)
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
				CString sCurveName;
				sCurveName.Format("%-.32s",FidRead->FidChannel[CurveIndex[i]].CurveName);
				FidRead->ReadGenData(sCurveName,Num1,buf1);
				FloatResample(Start1,Stop1,Rlev1,buf1,Qh716W->Head.Stdep,Qh716W->Head.Endep,Qh716W->Head.Rlev,buf2);
				Qh716W->WriteChannel(i,buf2);
				delete []buf1;	buf1=NULL;
			}
			delete []buf2;	buf2=NULL;
			Qh716W->Close();
		}
	}
	return !sp_Break_Status;
}

void CFidToQh716::GetQh716HeadFromFid()
{
	CString s,sCurveName;
	CStringArray SelCurveName,NewCurveName;
	Qh716W->Head.Feet=64;
	Qh716W->Head.ECC=0;
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return;
	}
	s.Format ("%-.80s",FidRead->FidHead.Company);
	s.TrimLeft();
	s.TrimRight();
	if(s.IsEmpty())
		sprintf(Qh716W->Head.CorpName,"%-80.80s","Si Chuan Logging Company");
	else
		sprintf(Qh716W->Head.CorpName,"%-80.80s",(const char*)s);
	s.Format ("%-.80s",FidRead->FidHead.Well);
	s.TrimLeft();
	s.TrimRight();
	if(s.IsEmpty())
		sprintf(Qh716W->Head.WellName,"%-80.80s","NoneWellName");
	else
		sprintf(Qh716W->Head.WellName,"%-80.80s",(const char*)s);
	bool bHasLenName=false; //是否有超长曲线名
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		if(FidRead->FidChannel[i].NumOfDimension != 2)
		{
			continue; //只要常规曲线
		}

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
		if(sCurveName.GetLength()>4)
		{	//曲线名长度大于SLC716文件对曲线名的要求
			bHasLenName=true;
		}
	}

	//对长曲线名进行修正
	if(bHasLenName || m_nCurvesRadio==3 || CurveIndex.GetSize()>128 || SelCurveName.GetSize()>128)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLog=128;
		ReCurNamDlg.m_nMaxLen=4;
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
	Qh716W->Head.NumLog=SelCurveName.GetSize();
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
				sprintf(Qh716W->Head.CurveName[i],"%-4.4s",(const char*)NewCurveName.GetAt(i));
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
			Qh716W->Head.Stdep=FidRead->FidChannel[k].DimInfo[0].Start;
			Qh716W->Head.Endep=FidRead->FidChannel[k].DimInfo[0].Stop;
			Qh716W->Head.Rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
		}
		else
		{
			if(Qh716W->Head.Stdep>FidRead->FidChannel[k].DimInfo[0].Start)
			{
				Qh716W->Head.Stdep=FidRead->FidChannel[k].DimInfo[0].Start;
			}
			if(Qh716W->Head.Endep<FidRead->FidChannel[k].DimInfo[0].Stop)
			{
				Qh716W->Head.Endep=FidRead->FidChannel[k].DimInfo[0].Stop;
			}
			if(Qh716W->Head.Rlev>FidRead->FidChannel[k].DimInfo[0].Rlev)
			{
				Qh716W->Head.Rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
			}
		}
	}
}
void CFidToQh716::SetStartStopRlev()
{
	CString s;
	int nDep;
	CModiConvDlg ModiConvDlg;
	switch(m_nConvEncode)
	{
	case 1: //根据深度计算适当的起止深度及采样间隔
		nDep=int(Qh716W->Head.Stdep/m_FixRlev+0.95);
		Qh716W->Head.Stdep=nDep*m_FixRlev;
		nDep=int(Qh716W->Head.Endep/m_FixRlev);
		Qh716W->Head.Endep=nDep*m_FixRlev;
		Qh716W->Head.Rlev=m_FixRlev;
		break;
	case 2: //人工指定起止深度及采样间隔
		ModiConvDlg.m_OldStart=Qh716W->Head.Stdep;
		ModiConvDlg.m_OldStop=Qh716W->Head.Endep;
		ModiConvDlg.m_OldRlev=Qh716W->Head.Rlev;
		if(ModiConvDlg.DoModal()==IDOK)
		{
			Qh716W->Head.Stdep=ModiConvDlg.m_NewStart;
			Qh716W->Head.Endep=ModiConvDlg.m_NewStop;
			Qh716W->Head.Rlev=ModiConvDlg.m_NewRlev;
		}
		break;
	default://不必重新指定：以最小采样间隔及最大井段
		break;
	}
}
