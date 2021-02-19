// WisToFid.cpp: implementation of the CWisToFid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "WisToFid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWisToFid::CWisToFid()
{

}

CWisToFid::~CWisToFid()
{

}
bool CWisToFid::WisToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //源文件名   
	m_sObjectPath=sObjectPath;  //输出目录
	m_nCurvesRadio=nCurvesRadio;//过滤开关
	fp=fo;

	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	WisRead=new CWisRead ;
	FidWrite=new CFidWrite ; //输出类
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	//输出转换结果信息
	if(bResult)
	{
		FidWrite->PrintHead(fp);
	}

	delete pGuage;  //删除进度条
	delete FidWrite; FidWrite=NULL;
	delete WisRead;	WisRead=NULL;

	CurveIndex.RemoveAll();
	return bResult;
}
bool CWisToFid::Transform()
{
	CString s;
	if(!IsWIS(m_SourceFile))
	{
		fprintf(fp,"%s is not WIS Format\n\n",m_SourceFile);
		return false;
	}
	//根据输出目录及输入文件名确定井名和输出路径
	CString sFileTitle=MyGetFileTitle(m_SourceFile);
	CString sOutWellPath=m_sObjectPath+"\\#"+sFileTitle;
	ModiFileName(sOutWellPath); //根据条件决定是否修改文件名
	sFileTitle=MyGetFileTitle(sOutWellPath);
	CString sWellName=sFileTitle.Mid(1); //去掉“#”
	FidWrite->SetFidFileName(m_sObjectPath,sWellName);

	if(!WisRead->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!打开输入文件%s有错\n\n",m_SourceFile);
		return false;
	}

	WisRead->GetHead();
	WisRead->GetObjectEntry();
	pGuage->SetRange(0,WisRead->WisHead.ObjectNumber);

	//读Wis对象信息，并选择其中所要的曲线
	SetFidHeadFromWisChannel();

	FidWrite->WriteFidInfo();
	for(int i=0;i<CurveIndex.GetSize();i++)
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
		
		//从WIS通道设置FID通道信息
		SetFidChannelFromWisChannel(i);

		if(WisRead->WisChannel.NumOfDimension==1)
		{
			float depth=WisRead->WisChannel.DimInfo[0].StartVal;
			DWORD NumPoint=WisRead->WisChannel.DimInfo[0].Samples;
			float *buffer=new float[NumPoint];
			WisRead->ReadCurve(index,depth,NumPoint,buffer);
			FidWrite->WriteGenData(i,NumPoint,buffer);
			delete []buffer;
			buffer=NULL;
		}
		else if(WisRead->WisChannel.NumOfDimension==2)
		{
			if(FidWrite->OpenWave(i))
			{
				float *buffer=new float[FidWrite->FidChannel[i].DimInfo[2].Npw];
				for(int k=0;k<FidWrite->FidChannel[i].DimInfo[2].Npw;k++)
				{
					buffer[k]=0;
				}
				int Num=int((FidWrite->FidChannel[i].DimInfo[0].Stop-FidWrite->FidChannel[i].DimInfo[0].Start)/FidWrite->FidChannel[i].DimInfo[0].Rlev+0.5)+1;
				for(int j=0;j<Num;j++)
				{
					float depth=FidWrite->FidChannel[i].DimInfo[0].Start+j*FidWrite->FidChannel[i].DimInfo[0].Rlev;
					WisRead->ReadWave(index,depth,buffer);
					FidWrite->WriteWave(depth,buffer);
				}
				delete []buffer;
				buffer=NULL;
				FidWrite->CloseWave();
			}
		}
		else
		{
			//阵列曲线
			if(FidWrite->OpenArray(i))
			{
				int Count=1;
				for(int k=1;k<WisRead->WisChannel.NumOfDimension;k++)
				{
					Count*=WisRead->WisChannel.DimInfo[k].Samples;
				}

				float *buf=new float[Count];
				for(k=0;k<Count;k++)buf[k]=0; //初值
				int NumPoint=WisRead->WisChannel.DimInfo[0].Samples;
				for(k=0;k<NumPoint;k++)
				{
					float depth=WisRead->WisChannel.DimInfo[0].StartVal+k*WisRead->WisChannel.DimInfo[0].Delta;
					WisRead->ReadWaveArray(index,depth,buf);
					FidWrite->WriteArray(depth,buf);
				}
				delete []buf; buf=NULL;
				FidWrite->CloseArray();
			}
		}
	}
	WisRead->Close();
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)FidWrite->m_sFidFile);
	
	return !sp_Break_Status;
}
void CWisToFid::SetFidHeadFromWisChannel()
{
	sprintf(FidWrite->FidHead.Oil_Field,"%-80.80s","");
	sprintf(FidWrite->FidHead.Region,"%-80.80s","");
	sprintf(FidWrite->FidHead.Company,"%-80.80s","");
	sprintf(FidWrite->FidHead.Well,"%-80.80s","");
	sprintf(FidWrite->FidHead.X_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Y_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Exformation,"%-80.80s","");
	FidWrite->FidHead.NumLog=0;
	CurveIndex.RemoveAll();
	CString szName;
	for(int index=0;index<WisRead->WisHead.ObjectNumber;index++)
	{
		if(WisRead->m_ObjectEntry[index].Status != 0)
		{
			continue; //不是正常对象
		}
		if(WisRead->m_ObjectEntry[index].Attribute != 1)
		{
			continue; //不是通道对象
		}
		if(WisRead->m_ObjectEntry[index].BlockNum<1)
		{
			continue; //没有数据
		}
		//读通道信息
		if(WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name)<0)
		{
			fprintf(fp,"\n!!!!Channel: %s ERROR: Dimmension=%d\n",
				WisRead->m_ObjectEntry[index].Name,WisRead->WisChannel.NumOfDimension);
			continue;
		}
		//判断是否为连续采样通道
		bool bIsContinue=true;
		for(int i=0;i<WisRead->WisChannel.NumOfDimension;i++)
		{
			if(WisRead->WisChannel.DimInfo[i].Delta==0)
			{
				bIsContinue=false;
				break;
			}
		}
		if(!bIsContinue)
		{
			continue; //不是连续采样通道,如FMT等数据
		}
		if(WisRead->WisChannel.NumOfDimension>3)
		{
			continue; //只能转换一维和二维曲线
		}
		if(m_nCurvesRadio==2)
		{	//选择了常规曲线
			if(WisRead->WisChannel.NumOfDimension>1)
			{
				continue; //不是常规曲线
			}
		}
		bool bOk=false;
		szName.Format ("%-.16s",WisRead->m_ObjectEntry[index].Name);
		szName.Trim();
		szName.Replace('.','_');
		if(m_nCurvesRadio==0)
		{
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(szName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
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

	FidWrite->FidHead.NumLog=CurveIndex.GetSize();
	//根据曲线数(FidWrite->FidHead.NumLog)开辟空间
	FidWrite->InitFidChannel();
	//得到每条曲线的索引信息
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		szName.Format ("%-.16s",WisRead->m_ObjectEntry[k].Name);
		szName.Replace('.','_');
		sprintf(FidWrite->FidChannel[i].CurveName,"%-.16s",szName);//WisRead->m_ObjectEntry[k].Name);
	}
}

void CWisToFid::SetFidChannelFromWisChannel(int index)
{
	FidWrite->FidChannel[index].NumOfDimension=WisRead->WisChannel.NumOfDimension+1;
	
	CString szCurveName;
	szCurveName.Format ("%-.16s",FidWrite->FidChannel[index].CurveName);
	szCurveName.Trim();
	szCurveName.Replace('.','_');

	if(WisRead->WisChannel.NumOfDimension==1)
	{
		sprintf(FidWrite->FidChannel[index].DimInfo[0].Name,"%-.16s",WisRead->WisChannel.DimInfo[0].Name);
		sprintf(FidWrite->FidChannel[index].DimInfo[0].Unit,"%-.16s",WisRead->WisChannel.DimInfo[0].Unit);
		FidWrite->FidChannel[index].DimInfo[0].RepCode=WisRead->WisChannel.DimInfo[0].RepCode;
		FidWrite->FidChannel[index].DimInfo[0].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[0].RepCode);
		FidWrite->FidChannel[index].DimInfo[0].Nps=1;
		FidWrite->FidChannel[index].DimInfo[0].Npw=0;
		FidWrite->FidChannel[index].DimInfo[0].Start=WisRead->WisChannel.DimInfo[0].StartVal;
		FidWrite->FidChannel[index].DimInfo[0].Stop=WisRead->WisChannel.DimInfo[0].StartVal+(WisRead->WisChannel.DimInfo[0].Samples-1)*WisRead->WisChannel.DimInfo[0].Delta;
		FidWrite->FidChannel[index].DimInfo[0].Rlev=WisRead->WisChannel.DimInfo[0].Delta;

		sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.16s",szCurveName);//FidWrite->FidChannel[index].CurveName);
		sprintf(FidWrite->FidChannel[index].DimInfo[1].Unit,"%-.16s",WisRead->WisChannel.Unit);
		FidWrite->FidChannel[index].DimInfo[1].RepCode=WisRead->WisChannel.RepCode;
		FidWrite->FidChannel[index].DimInfo[1].RepCode=WIS_REPR_FLOAT;//FID一维数据只能是单精度浮点
		FidWrite->FidChannel[index].DimInfo[1].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[1].RepCode);
		FidWrite->FidChannel[index].DimInfo[1].Nps=1;
		FidWrite->FidChannel[index].DimInfo[1].Npw=1;
		FidWrite->FidChannel[index].DimInfo[1].Start=WisRead->WisChannel.MinVal;
		FidWrite->FidChannel[index].DimInfo[1].Stop=WisRead->WisChannel.MaxVal;
		FidWrite->FidChannel[index].DimInfo[1].Rlev=0;
	}
	else if(WisRead->WisChannel.NumOfDimension==2)
	{
		sprintf(FidWrite->FidChannel[index].DimInfo[0].Name,"%-.16s",WisRead->WisChannel.DimInfo[0].Name);
		sprintf(FidWrite->FidChannel[index].DimInfo[0].Unit,"%-.16s",WisRead->WisChannel.DimInfo[0].Unit);
		FidWrite->FidChannel[index].DimInfo[0].RepCode=WisRead->WisChannel.DimInfo[0].RepCode;
		FidWrite->FidChannel[index].DimInfo[0].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[0].RepCode);
		FidWrite->FidChannel[index].DimInfo[0].Nps=1;
		FidWrite->FidChannel[index].DimInfo[0].Npw=0;
		FidWrite->FidChannel[index].DimInfo[0].Start=WisRead->WisChannel.DimInfo[0].StartVal;
		FidWrite->FidChannel[index].DimInfo[0].Stop=WisRead->WisChannel.DimInfo[0].StartVal+(WisRead->WisChannel.DimInfo[0].Samples-1)*WisRead->WisChannel.DimInfo[0].Delta;
		FidWrite->FidChannel[index].DimInfo[0].Rlev=WisRead->WisChannel.DimInfo[0].Delta;

		//sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.16s",WisRead->WisChannel.DimInfo[1].Name);
		sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.16s","T");

		sprintf(FidWrite->FidChannel[index].DimInfo[1].Unit,"%-.16s",WisRead->WisChannel.DimInfo[1].Unit);
		FidWrite->FidChannel[index].DimInfo[1].RepCode=WisRead->WisChannel.DimInfo[1].RepCode;
		FidWrite->FidChannel[index].DimInfo[1].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[1].RepCode);
		FidWrite->FidChannel[index].DimInfo[1].Nps=WisRead->WisChannel.DimInfo[1].Samples;
		FidWrite->FidChannel[index].DimInfo[1].Npw=0;
		FidWrite->FidChannel[index].DimInfo[1].Start=WisRead->WisChannel.DimInfo[1].StartVal;
		FidWrite->FidChannel[index].DimInfo[1].Stop=WisRead->WisChannel.DimInfo[1].StartVal+(WisRead->WisChannel.DimInfo[1].Samples-1)*WisRead->WisChannel.DimInfo[1].Delta;
		FidWrite->FidChannel[index].DimInfo[1].Rlev=WisRead->WisChannel.DimInfo[1].Delta;

		sprintf(FidWrite->FidChannel[index].DimInfo[2].Name,"%-.16s",szCurveName);//FidWrite->FidChannel[index].CurveName);
		sprintf(FidWrite->FidChannel[index].DimInfo[2].Unit,"%-.16s",WisRead->WisChannel.Unit);
		
		FidWrite->FidChannel[index].DimInfo[2].RepCode=WisRead->WisChannel.RepCode;
		FidWrite->FidChannel[index].DimInfo[2].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[2].RepCode);

		FidWrite->FidChannel[index].DimInfo[2].Nps=WisRead->WisChannel.DimInfo[1].Samples;
		FidWrite->FidChannel[index].DimInfo[2].Npw=WisRead->WisChannel.DimInfo[1].Samples;
		FidWrite->FidChannel[index].DimInfo[2].Start=WisRead->WisChannel.MinVal;
		FidWrite->FidChannel[index].DimInfo[2].Stop=WisRead->WisChannel.MaxVal;
		FidWrite->FidChannel[index].DimInfo[2].Rlev=WisRead->WisChannel.DimInfo[1].Delta;
	}
	else if(WisRead->WisChannel.NumOfDimension==3)
	{
		sprintf(FidWrite->FidChannel[index].DimInfo[0].Name,"%-.16s",WisRead->WisChannel.DimInfo[0].Name);
		sprintf(FidWrite->FidChannel[index].DimInfo[0].Unit,"%-.16s",WisRead->WisChannel.DimInfo[0].Unit);
		FidWrite->FidChannel[index].DimInfo[0].RepCode=WisRead->WisChannel.DimInfo[0].RepCode;
		FidWrite->FidChannel[index].DimInfo[0].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[0].RepCode);
		FidWrite->FidChannel[index].DimInfo[0].Nps=1;
		FidWrite->FidChannel[index].DimInfo[0].Npw=0;
		FidWrite->FidChannel[index].DimInfo[0].Start=WisRead->WisChannel.DimInfo[0].StartVal;
		FidWrite->FidChannel[index].DimInfo[0].Stop=WisRead->WisChannel.DimInfo[0].StartVal+(WisRead->WisChannel.DimInfo[0].Samples-1)*WisRead->WisChannel.DimInfo[0].Delta;
		FidWrite->FidChannel[index].DimInfo[0].Rlev=WisRead->WisChannel.DimInfo[0].Delta;

		sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.16s","T");
		sprintf(FidWrite->FidChannel[index].DimInfo[1].Unit,"%-.16s",WisRead->WisChannel.DimInfo[2].Unit);
		FidWrite->FidChannel[index].DimInfo[1].RepCode=WisRead->WisChannel.DimInfo[1].RepCode;
		FidWrite->FidChannel[index].DimInfo[1].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[2].RepCode);
		FidWrite->FidChannel[index].DimInfo[1].Nps=WisRead->WisChannel.DimInfo[2].Samples;
		FidWrite->FidChannel[index].DimInfo[1].Npw=0;
		FidWrite->FidChannel[index].DimInfo[1].Start=WisRead->WisChannel.DimInfo[2].StartVal;
		FidWrite->FidChannel[index].DimInfo[1].Stop=WisRead->WisChannel.DimInfo[2].StartVal+(WisRead->WisChannel.DimInfo[2].Samples-1)*WisRead->WisChannel.DimInfo[2].Delta;
		FidWrite->FidChannel[index].DimInfo[1].Rlev=WisRead->WisChannel.DimInfo[2].Delta;

		sprintf(FidWrite->FidChannel[index].DimInfo[2].Name,"%-.16s","SENSOR");
		sprintf(FidWrite->FidChannel[index].DimInfo[2].Unit,"%-.16s",WisRead->WisChannel.DimInfo[1].Unit);
		FidWrite->FidChannel[index].DimInfo[2].RepCode=WisRead->WisChannel.DimInfo[1].RepCode;
		FidWrite->FidChannel[index].DimInfo[2].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[1].RepCode);
		FidWrite->FidChannel[index].DimInfo[2].Nps=WisRead->WisChannel.DimInfo[1].Samples;
		FidWrite->FidChannel[index].DimInfo[2].Npw=0;
		FidWrite->FidChannel[index].DimInfo[2].Start=WisRead->WisChannel.DimInfo[1].StartVal;
		FidWrite->FidChannel[index].DimInfo[2].Stop=WisRead->WisChannel.DimInfo[1].StartVal+(WisRead->WisChannel.DimInfo[1].Samples-1)*WisRead->WisChannel.DimInfo[1].Delta;
		FidWrite->FidChannel[index].DimInfo[2].Rlev=WisRead->WisChannel.DimInfo[1].Delta;

		sprintf(FidWrite->FidChannel[index].DimInfo[3].Name,"%-.16s",szCurveName);//FidWrite->FidChannel[index].CurveName);
		sprintf(FidWrite->FidChannel[index].DimInfo[3].Unit,"%-.16s",WisRead->WisChannel.Unit);
		FidWrite->FidChannel[index].DimInfo[3].RepCode=WisRead->WisChannel.RepCode;
		FidWrite->FidChannel[index].DimInfo[3].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[3].RepCode);
		FidWrite->FidChannel[index].DimInfo[3].Nps=WisRead->WisChannel.DimInfo[2].Samples;
		FidWrite->FidChannel[index].DimInfo[3].Npw=WisRead->WisChannel.DimInfo[2].Samples;
		FidWrite->FidChannel[index].DimInfo[3].Start=WisRead->WisChannel.MinVal;
		FidWrite->FidChannel[index].DimInfo[3].Stop=WisRead->WisChannel.MaxVal;
		FidWrite->FidChannel[index].DimInfo[3].Rlev=WisRead->WisChannel.DimInfo[2].Delta;
	}

	if(strnicmp(FidWrite->FidChannel[index].DimInfo[0].Unit,"none",4)==0)
	{
		strcpy(FidWrite->FidChannel[index].DimInfo[0].Unit,"m");
	}
}

//void CWisToFid::SetFidChannelFromWisChannel(int index)
//{
//	FidWrite->FidChannel[index].NumOfDimension=WisRead->WisChannel.NumOfDimension+1;
//
//	if(WisRead->WisChannel.NumOfDimension==1)
//	{
//		sprintf(FidWrite->FidChannel[index].DimInfo[0].Name,"%-.16s",WisRead->WisChannel.DimInfo[0].Name);
//		sprintf(FidWrite->FidChannel[index].DimInfo[0].Unit,"%-.16s",WisRead->WisChannel.DimInfo[0].Unit);
//		FidWrite->FidChannel[index].DimInfo[0].RepCode=WisRead->WisChannel.DimInfo[0].RepCode;
//		FidWrite->FidChannel[index].DimInfo[0].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[0].RepCode);
//		FidWrite->FidChannel[index].DimInfo[0].Nps=1;
//		FidWrite->FidChannel[index].DimInfo[0].Npw=0;
//		FidWrite->FidChannel[index].DimInfo[0].Start=WisRead->WisChannel.DimInfo[0].StartVal;
//		FidWrite->FidChannel[index].DimInfo[0].Stop=WisRead->WisChannel.DimInfo[0].StartVal+(WisRead->WisChannel.DimInfo[0].Samples-1)*WisRead->WisChannel.DimInfo[0].Delta;
//		FidWrite->FidChannel[index].DimInfo[0].Rlev=WisRead->WisChannel.DimInfo[0].Delta;
//
//		sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.16s",FidWrite->FidChannel[index].CurveName);
//		sprintf(FidWrite->FidChannel[index].DimInfo[1].Unit,"%-.16s",WisRead->WisChannel.Unit);
//		FidWrite->FidChannel[index].DimInfo[1].RepCode=WisRead->WisChannel.RepCode;
//		FidWrite->FidChannel[index].DimInfo[1].RepCode=WIS_REPR_FLOAT;//FID一维数据只能是单精度浮点
//		FidWrite->FidChannel[index].DimInfo[1].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[1].RepCode);
//		FidWrite->FidChannel[index].DimInfo[1].Nps=1;
//		FidWrite->FidChannel[index].DimInfo[1].Npw=1;
//		FidWrite->FidChannel[index].DimInfo[1].Start=WisRead->WisChannel.MinVal;
//		FidWrite->FidChannel[index].DimInfo[1].Stop=WisRead->WisChannel.MaxVal;
//		FidWrite->FidChannel[index].DimInfo[1].Rlev=0;
//	}
//	else if(WisRead->WisChannel.NumOfDimension==2)
//	{
//		sprintf(FidWrite->FidChannel[index].DimInfo[0].Name,"%-.16s",WisRead->WisChannel.DimInfo[0].Name);
//		sprintf(FidWrite->FidChannel[index].DimInfo[0].Unit,"%-.16s",WisRead->WisChannel.DimInfo[0].Unit);
//		FidWrite->FidChannel[index].DimInfo[0].RepCode=WisRead->WisChannel.DimInfo[0].RepCode;
//		FidWrite->FidChannel[index].DimInfo[0].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[0].RepCode);
//		FidWrite->FidChannel[index].DimInfo[0].Nps=1;
//		FidWrite->FidChannel[index].DimInfo[0].Npw=0;
//		FidWrite->FidChannel[index].DimInfo[0].Start=WisRead->WisChannel.DimInfo[0].StartVal;
//		FidWrite->FidChannel[index].DimInfo[0].Stop=WisRead->WisChannel.DimInfo[0].StartVal+(WisRead->WisChannel.DimInfo[0].Samples-1)*WisRead->WisChannel.DimInfo[0].Delta;
//		FidWrite->FidChannel[index].DimInfo[0].Rlev=WisRead->WisChannel.DimInfo[0].Delta;
//
//		//sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.16s",WisRead->WisChannel.DimInfo[1].Name);
//		sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.16s","T");
//
//		sprintf(FidWrite->FidChannel[index].DimInfo[1].Unit,"%-.16s",WisRead->WisChannel.DimInfo[1].Unit);
//		FidWrite->FidChannel[index].DimInfo[1].RepCode=WisRead->WisChannel.DimInfo[1].RepCode;
//		FidWrite->FidChannel[index].DimInfo[1].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[1].RepCode);
//		FidWrite->FidChannel[index].DimInfo[1].Nps=WisRead->WisChannel.DimInfo[1].Samples;
//		FidWrite->FidChannel[index].DimInfo[1].Npw=0;
//		FidWrite->FidChannel[index].DimInfo[1].Start=WisRead->WisChannel.DimInfo[1].StartVal;
//		FidWrite->FidChannel[index].DimInfo[1].Stop=WisRead->WisChannel.DimInfo[1].StartVal+(WisRead->WisChannel.DimInfo[1].Samples-1)*WisRead->WisChannel.DimInfo[1].Delta;
//		FidWrite->FidChannel[index].DimInfo[1].Rlev=WisRead->WisChannel.DimInfo[1].Delta;
//
//		sprintf(FidWrite->FidChannel[index].DimInfo[2].Name,"%-.16s",FidWrite->FidChannel[index].CurveName);
//		sprintf(FidWrite->FidChannel[index].DimInfo[2].Unit,"%-.16s",WisRead->WisChannel.Unit);
//		
//		FidWrite->FidChannel[index].DimInfo[2].RepCode=WisRead->WisChannel.RepCode;
//		FidWrite->FidChannel[index].DimInfo[2].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[2].RepCode);
//
//		FidWrite->FidChannel[index].DimInfo[2].Nps=WisRead->WisChannel.DimInfo[1].Samples;
//		FidWrite->FidChannel[index].DimInfo[2].Npw=WisRead->WisChannel.DimInfo[1].Samples;
//		FidWrite->FidChannel[index].DimInfo[2].Start=WisRead->WisChannel.MinVal;
//		FidWrite->FidChannel[index].DimInfo[2].Stop=WisRead->WisChannel.MaxVal;
//		FidWrite->FidChannel[index].DimInfo[2].Rlev=WisRead->WisChannel.DimInfo[1].Delta;
//	}
//	else if(WisRead->WisChannel.NumOfDimension==3)
//	{
//		sprintf(FidWrite->FidChannel[index].DimInfo[0].Name,"%-.16s",WisRead->WisChannel.DimInfo[0].Name);
//		sprintf(FidWrite->FidChannel[index].DimInfo[0].Unit,"%-.16s",WisRead->WisChannel.DimInfo[0].Unit);
//		FidWrite->FidChannel[index].DimInfo[0].RepCode=WisRead->WisChannel.DimInfo[0].RepCode;
//		FidWrite->FidChannel[index].DimInfo[0].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[0].RepCode);
//		FidWrite->FidChannel[index].DimInfo[0].Nps=1;
//		FidWrite->FidChannel[index].DimInfo[0].Npw=0;
//		FidWrite->FidChannel[index].DimInfo[0].Start=WisRead->WisChannel.DimInfo[0].StartVal;
//		FidWrite->FidChannel[index].DimInfo[0].Stop=WisRead->WisChannel.DimInfo[0].StartVal+(WisRead->WisChannel.DimInfo[0].Samples-1)*WisRead->WisChannel.DimInfo[0].Delta;
//		FidWrite->FidChannel[index].DimInfo[0].Rlev=WisRead->WisChannel.DimInfo[0].Delta;
//
//		sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.16s","T");
//		sprintf(FidWrite->FidChannel[index].DimInfo[1].Unit,"%-.16s",WisRead->WisChannel.DimInfo[2].Unit);
//		FidWrite->FidChannel[index].DimInfo[1].RepCode=WisRead->WisChannel.DimInfo[1].RepCode;
//		FidWrite->FidChannel[index].DimInfo[1].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[2].RepCode);
//		FidWrite->FidChannel[index].DimInfo[1].Nps=WisRead->WisChannel.DimInfo[2].Samples;
//		FidWrite->FidChannel[index].DimInfo[1].Npw=0;
//		FidWrite->FidChannel[index].DimInfo[1].Start=WisRead->WisChannel.DimInfo[2].StartVal;
//		FidWrite->FidChannel[index].DimInfo[1].Stop=WisRead->WisChannel.DimInfo[2].StartVal+(WisRead->WisChannel.DimInfo[2].Samples-1)*WisRead->WisChannel.DimInfo[2].Delta;
//		FidWrite->FidChannel[index].DimInfo[1].Rlev=WisRead->WisChannel.DimInfo[2].Delta;
//
//		sprintf(FidWrite->FidChannel[index].DimInfo[2].Name,"%-.16s","SENSOR");
//		sprintf(FidWrite->FidChannel[index].DimInfo[2].Unit,"%-.16s",WisRead->WisChannel.DimInfo[1].Unit);
//		FidWrite->FidChannel[index].DimInfo[2].RepCode=WisRead->WisChannel.DimInfo[1].RepCode;
//		FidWrite->FidChannel[index].DimInfo[2].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[1].RepCode);
//		FidWrite->FidChannel[index].DimInfo[2].Nps=WisRead->WisChannel.DimInfo[1].Samples;
//		FidWrite->FidChannel[index].DimInfo[2].Npw=0;
//		FidWrite->FidChannel[index].DimInfo[2].Start=WisRead->WisChannel.DimInfo[1].StartVal;
//		FidWrite->FidChannel[index].DimInfo[2].Stop=WisRead->WisChannel.DimInfo[1].StartVal+(WisRead->WisChannel.DimInfo[1].Samples-1)*WisRead->WisChannel.DimInfo[1].Delta;
//		FidWrite->FidChannel[index].DimInfo[2].Rlev=WisRead->WisChannel.DimInfo[1].Delta;
//
//		sprintf(FidWrite->FidChannel[index].DimInfo[3].Name,"%-.16s",FidWrite->FidChannel[index].CurveName);
//		sprintf(FidWrite->FidChannel[index].DimInfo[3].Unit,"%-.16s",WisRead->WisChannel.Unit);
//		FidWrite->FidChannel[index].DimInfo[3].RepCode=WisRead->WisChannel.RepCode;
//		FidWrite->FidChannel[index].DimInfo[3].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[3].RepCode);
//		FidWrite->FidChannel[index].DimInfo[3].Nps=WisRead->WisChannel.DimInfo[2].Samples;
//		FidWrite->FidChannel[index].DimInfo[3].Npw=WisRead->WisChannel.DimInfo[2].Samples;
//		FidWrite->FidChannel[index].DimInfo[3].Start=WisRead->WisChannel.MinVal;
//		FidWrite->FidChannel[index].DimInfo[3].Stop=WisRead->WisChannel.MaxVal;
//		FidWrite->FidChannel[index].DimInfo[3].Rlev=WisRead->WisChannel.DimInfo[2].Delta;
//	}
//
//	if(strnicmp(FidWrite->FidChannel[index].DimInfo[0].Unit,"none",4)==0)
//	{
//		strcpy(FidWrite->FidChannel[index].DimInfo[0].Unit,"m");
//	}
//}
