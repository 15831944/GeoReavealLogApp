// FidToWis.cpp: implementation of the CFidToWis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToWis.h"
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidToWis::CFidToWis()
{
	WisWrite=NULL;

}

CFidToWis::~CFidToWis()
{

}
bool CFidToWis::FidToWis(CString SourceFile,CString ObjectPath,int nCurvesRadio,FILE *fo)
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
	
	FidRead=new CFidRead;
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
	delete FidRead; FidRead=NULL;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	delete pGuage;  //删除进度条
	return true;
}
bool CFidToWis::Transform()
{
	CString s;
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}
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

	bool bResult=FidRead->ReadFidInfo(m_SourceFile);
	if(!bResult)
	{
		fprintf(fp,"\n!!!!链接的数据文件打开失败\n\n");
		return false;
	}
//****************************************************************
	//由于FID文件只有一、二维有意义，所以当存在多维曲线时，该曲线应忽略
	FidRead->CleanArrayCurve(); //清除阵列曲线
//****************************************************************

	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return false;
	}
	//////////////
	CString sInWellPath=MyGetPathName(m_SourceFile);//FID曲线数据所在目录
	CString sInWellName=MyGetFileTitle(m_SourceFile);//FID曲线数据对应井名
	WisWrite->Open(WisFileName);
	GetWisObjectIndexFromFid();
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
	for(int index=0;index<CurveIndex.GetSize();index++)
	{
		pGuage->SetPos(index+1);  //设置进度条当前值
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int k=CurveIndex.GetAt(index);
		SetWisChannelFromFidHead(k,index); //设置通道参数
		WisWrite->WriteChannel(); //写通道信息到WIS文件
		sCurveName.Format("%-.32s",FidRead->FidChannel[k].CurveName);
		int NumR=int((FidRead->FidChannel[k].DimInfo[0].Stop-FidRead->FidChannel[k].DimInfo[0].Start)/FidRead->FidChannel[k].DimInfo[0].Rlev+0.5)+1;
		if(FidRead->FidChannel[k].NumOfDimension==2)
		{
			float *buf=new float[NumR];
			FidRead->ReadGenData(sCurveName,NumR,buf);
			//写常规曲线数据到WIS文件
			WisWrite->WriteGendata(NumR,buf);
			delete []buf; buf=NULL;
		}
		else if(FidRead->FidChannel[k].NumOfDimension==3)
		{
			if(FidRead->OpenWave(sCurveName))
			{
				float *buf=new float[FidRead->FidChannel[k].DimInfo[2].Npw];
				for(int j=0;j<NumR;j++)
				{
					float depth=FidRead->FidChannel[k].DimInfo[0].Start+j*FidRead->FidChannel[k].DimInfo[0].Rlev;
					if(!FidRead->ReadWave(depth,buf))
					{
						for(int k1=0;k1<FidRead->FidChannel[k].DimInfo[2].Npw;k1++)
						{
							buf[k1]=0;
						}
					}
					//输出一个深度点的波形数据
					WisWrite->WriteWave(depth,buf);
				}
				FidRead->CloseWave();
				delete []buf;
				buf=NULL;
			}
		}
	}
	WisWrite->Close();
	
	////////////////////////////////
	delete WisWrite->m_ObjectEntry; 
	WisWrite->m_ObjectEntry=NULL;
	delete WisWrite->WisChannel; 
	WisWrite->WisChannel=NULL;
	
	return !sp_Break_Status;
}
void CFidToWis::GetWisObjectIndexFromFid()
{
	CString s,sCurveName;
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return;
	}
	CurveIndex.RemoveAll();
	WisWrite->WisHead.ObjectNumber=0;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	bool bHasLongName=false; //存在长曲线名
	if(m_nCurvesRadio<3)
	{
		for(int i=0;i<FidRead->FidHead.NumLog;i++)
		{
			if(m_nCurvesRadio==2)
			{
				if(FidRead->FidChannel[i].NumOfDimension != 2)
				{
					continue; //只要常规曲线
				}
			}
			bool bOk=false;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
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
			int nDim=FidRead->FidChannel[i].NumOfDimension-1;
			NewUnits.Add(FidRead->FidChannel[i].DimInfo[nDim].Unit);
			if(sCurveName.GetLength()>15)
			{
				bHasLongName=true;
			}
		}
	}
	if(m_nCurvesRadio==3 || bHasLongName)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=15; //曲线名最大长度
		ReCurNamDlg.m_nMaxLog=2048; //XTF文件允许最多曲线数
		if(m_nCurvesRadio==3)
		{
			ReCurNamDlg.m_NumLog=FidRead->FidHead.NumLog; //实际曲线数
			for(int i=0;i<FidRead->FidHead.NumLog;i++)
			{
				int nDim=FidRead->FidChannel[i].NumOfDimension-1;
				sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
				sCurveName.TrimLeft();
				sCurveName.TrimRight();
				ReCurNamDlg.m_SourceName.Add(sCurveName);
				ReCurNamDlg.m_ObjectName.Add(sCurveName);
				ReCurNamDlg.m_Units.Add(FidRead->FidChannel[i].DimInfo[nDim].Unit);
				ReCurNamDlg.m_NumDim.Add(nDim);
			}
		}
		else
		{
			ReCurNamDlg.m_NumLog=SelCurveName.GetSize(); //实际曲线数
			for(int i=0;i<SelCurveName.GetSize();i++)
			{
				ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
				ReCurNamDlg.m_ObjectName.Add(SelCurveName.GetAt(i));
				int k=GetCurveIndex(SelCurveName.GetAt(i));
				int nDim=FidRead->FidChannel[k].NumOfDimension-1;
				ReCurNamDlg.m_NumDim.Add(nDim);
				ReCurNamDlg.m_Units.Add(FidRead->FidChannel[k].DimInfo[nDim].Unit);
			}
			//清除原来的曲线名
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
int CFidToWis::GetCurveIndex(CString SelCurve)
{
	SelCurve.TrimRight();
	int index=-1;
	CString sCurveName;
	for(int j=0;j<FidRead->FidHead.NumLog;j++)
	{
		sCurveName.Format ("%-.32s",FidRead->FidChannel[j].CurveName);
		sCurveName.TrimLeft();
		sCurveName.TrimRight();
		if(sCurveName.CompareNoCase(SelCurve)==0)
		{
			index=j;
			break;
		}
	}
	return index;
}
void CFidToWis::SetWisChannelFromFidHead(int IndexFid,int IndexWis)
{
	//对象设置(其它参数在WIS输出类中设置)
	sprintf(WisWrite->m_ObjectEntry->Name,"%-.15s",NewCurveName.GetAt(IndexWis));
	WisWrite->m_ObjectEntry->Attribute=1;
	WisWrite->m_ObjectEntry->SubAttribute=FidRead->FidChannel[IndexFid].NumOfDimension-1;
	
	int kDim=FidRead->FidChannel[IndexFid].NumOfDimension-1; //WIS文件通道维数
	//通道参数
	sprintf(WisWrite->WisChannel->Unit,"%-.7s",NewUnits.GetAt(IndexWis));
	sprintf(WisWrite->WisChannel->AliasName,"%-.15s",FidRead->FidChannel[IndexFid].CurveName);
	sprintf(WisWrite->WisChannel->AliasUnit,"%-.15s",WisWrite->WisChannel->Unit);
	
	WisWrite->WisChannel->RepCode=FidRead->FidChannel[IndexFid].DimInfo[kDim].RepCode;
	WisWrite->WisChannel->CodeLen=FidRead->FidChannel[IndexFid].DimInfo[kDim].CodeLen;
	WisWrite->WisChannel->MinVal=FidRead->FidChannel[IndexFid].DimInfo[kDim].Start;
	WisWrite->WisChannel->MaxVal=FidRead->FidChannel[IndexFid].DimInfo[kDim].Stop;
	WisWrite->WisChannel->Reserved=0;
	WisWrite->WisChannel->NumOfDimension=kDim;
	
	DWORD NumPoint=DWORD((FidRead->FidChannel[IndexFid].DimInfo[0].Stop-FidRead->FidChannel[IndexFid].DimInfo[0].Start)/FidRead->FidChannel[IndexFid].DimInfo[0].Rlev+0.5)+1;
	//设置一维信息
	sprintf(WisWrite->WisChannel->DimInfo[0].Name,"%-.7s",FidRead->FidChannel[IndexFid].DimInfo[0].Name);
	sprintf(WisWrite->WisChannel->DimInfo[0].Unit,"%-.7s",FidRead->FidChannel[IndexFid].DimInfo[0].Unit);
	sprintf(WisWrite->WisChannel->DimInfo[0].AliasName,"%-.16s",FidRead->FidChannel[IndexFid].DimInfo[0].Name);
	WisWrite->WisChannel->DimInfo[0].StartVal=FidRead->FidChannel[IndexFid].DimInfo[0].Start;
	WisWrite->WisChannel->DimInfo[0].Delta=FidRead->FidChannel[IndexFid].DimInfo[0].Rlev;
	WisWrite->WisChannel->DimInfo[0].Samples=NumPoint;
	WisWrite->WisChannel->DimInfo[0].MaxSamples=WisWrite->WisChannel->DimInfo[0].Samples;
	WisWrite->WisChannel->DimInfo[0].Size=FidRead->FidChannel[IndexFid].DimInfo[kDim].Npw*FidRead->FidChannel[IndexFid].DimInfo[kDim].CodeLen;
	WisWrite->WisChannel->DimInfo[0].RepCode=FidRead->FidChannel[IndexFid].DimInfo[0].RepCode;
	WisWrite->WisChannel->DimInfo[0].Reserved=0;
	
	//设置二维信息
	if(WisWrite->WisChannel->NumOfDimension==2)
	{
		sprintf(WisWrite->WisChannel->DimInfo[1].Name,"%-.7s",FidRead->FidChannel[IndexFid].DimInfo[1].Name);
		sprintf(WisWrite->WisChannel->DimInfo[1].Unit,"%-.7s",FidRead->FidChannel[IndexFid].DimInfo[1].Unit);
		sprintf(WisWrite->WisChannel->DimInfo[1].AliasName,"%-.15s",FidRead->FidChannel[IndexFid].DimInfo[1].Name);
		WisWrite->WisChannel->DimInfo[1].StartVal=FidRead->FidChannel[IndexFid].DimInfo[1].Start;
		WisWrite->WisChannel->DimInfo[1].Delta=FidRead->FidChannel[IndexFid].DimInfo[1].Rlev;
		WisWrite->WisChannel->DimInfo[1].Samples=FidRead->FidChannel[IndexFid].DimInfo[kDim].Npw;
		WisWrite->WisChannel->DimInfo[1].MaxSamples=WisWrite->WisChannel->DimInfo[1].Samples;
		WisWrite->WisChannel->DimInfo[1].Size=FidRead->FidChannel[IndexFid].DimInfo[kDim].CodeLen;
		WisWrite->WisChannel->DimInfo[1].RepCode=FidRead->FidChannel[IndexFid].DimInfo[1].RepCode;
		WisWrite->WisChannel->DimInfo[1].Reserved=0;
	}
}
