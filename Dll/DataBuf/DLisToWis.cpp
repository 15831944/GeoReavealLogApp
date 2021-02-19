// DLisToWis.cpp: implementation of the CDLisToWis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "_Comext.h"
#include "DLisToWis.h"
#include "DlisCurDlg.h"
#include "CurveSplit.h"
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDLisToWis::CDLisToWis()
{

}

CDLisToWis::~CDLisToWis()
{

}
bool CDLisToWis::DLisToWis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	
	DlisRead=new CDlisRead ;
	WisWrite=new CWisWrite ; //输出类
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete WisWrite; WisWrite=NULL;
	delete DlisRead; DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDLisToWis::Transform()
{
	CString s;
	if(!IsDLIS(m_SourceFile))
	{
		fprintf(fp,"%s is not DLIS Format\n\n",m_SourceFile);
		return false;
	}
	CString szWellName=MyGetFileTitle(m_SourceFile);
	if(!DlisRead->Open(m_SourceFile,fp)) 
	{
		fprintf(fp,"\n!!!!链接的数据文件打开失败\n\n");
	}
	else
	{
		//设置进度条为一小段，为了视觉
		pGuage->SetRange(0,100);
		pGuage->SetPos(6);
		//浏览整个DLIS文件，取得逻辑文件数及各逻辑文件的位置
		CString WellPath;
		int NumLogFile=DlisRead->GetDlisLogicalFileNumber();
		if(NumLogFile>0)
		{
			if(sp_bDirectory)
			{
				WellPath=m_sObjectPath+"\\#"+szWellName;
				_mkdir((const char*)WellPath);
			}
		}
		for(int nf=0;nf<NumLogFile;nf++)
		{
			fprintf(fp,"\nDataBuff: %s\n",(const char*)m_SourceFile);
			fprintf(fp,"第 %d 逻辑文件的起始位置: %u Bytes\n",nf+1,DlisRead->LogFilePos.GetAt(nf));
			pGuage->SetRange(0,NumLogFile);
			pGuage->SetPos(nf+1); //只是为了让进度条不为0
			DlisRead->ScanDlisOneLogicalFile(nf);
			//DlisRead->Prt_LogFileIndex();
			/////////////////////////////////////////////////////////
			CString WellName;
			if(nf==0)
			{
				WellName=szWellName;
			}
			else
			{
				WellName.Format("%s_%d",szWellName,nf+1);
			}
			bool bResult=LogicalToWis(WellName,WellPath,nf);
			if(sp_Break_Status) break; //中断
			//输出转换结果信息
			if(bResult)
				WisWrite->PrintHead(fp);
			//输出信息
		}
		DlisRead->Close(); //关闭输入文件
	}
	return !sp_Break_Status;
}
bool CDLisToWis::LogicalToWis(CString WellName,CString WellPath,int nf)
{
	CString s;
	//根据输出目录及输入文件名确定输出文件名
	if(sp_bDirectory)
		sOutFileName.Format ("%s\\%s.wis",WellPath,WellName);
	else
		sOutFileName.Format ("%s\\%s.wis",m_sObjectPath,WellName);
	ModiFileName(sOutFileName); //根据条件决定是否修改文件名

	SetWisHeadFromDlisHead();
	if(NewCurveName.GetSize()==0)
	{
		fprintf(fp,"\n!!!!没有要处理的曲线\n");
		return false;
	}
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);
	if(!WisWrite->Open(sOutFileName)) return false;
	WisWrite->InitWisHead(NewCurveName.GetSize());
	WisWrite->m_ObjectEntry=NULL; //对象入口
	WisWrite->WisChannel=NULL; //通道信息
	WisWrite->m_ObjectEntry=new WIS_OBJECT_ENTRY;
	WisWrite->WisChannel=new WIS_CHANNEL;
	WisWrite->InitWisObjectEntry();
	WisWrite->InitWisChannel();
	DlisRead->ReadLogicalFile(nf); //DLIS数据读到以帧为单位的文件
	//设置进度条范围为解码曲线数
	pGuage->SetRange(0,NewCurveName.GetSize());
	pGuage->SetPos(1);
	int NumLog=-1; //输出曲线序号
	fprintf(fp,"====输出曲线信息:\n");
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		if(sp_Break_Status) break;
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			pGuage->SetPos(NumLog+2);
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			if(!DlisRead->Frame[i].bDecode[j]) continue;
			NumLog++;
			int index=DlisRead->Frame[i].ChannelID[j];
			SetWisChannelFromFidHead(NumLog,i,j); //设置通道参数
			fprintf(fp,"%4d %-8.15s %9.3f %9.3f %6d %3d\n",
				NumLog+1,WisWrite->m_ObjectEntry->Name,
				WisWrite->WisChannel->DimInfo[0].StartVal,
				WisWrite->WisChannel->DimInfo[0].Delta,
				WisWrite->WisChannel->DimInfo[0].Samples,
				WisWrite->WisChannel->NumOfDimension);
			WisWrite->WriteChannel(); //写通道信息到WIS文件
			int NumPoint=DlisRead->Frame[i].noFrame;
			if(WisWrite->WisChannel->NumOfDimension==1)
			{
				float *buf=new float[NumPoint];
				DlisRead->ReadCurve(i,j,NumPoint,buf);
				WisWrite->WriteGendata(NumPoint,buf);
				delete []buf; buf=NULL;
			}
			else
			{	
				//波形或阵列曲线
				int Count=DlisRead->Channel[index].Sample;
				float *buf=new float[Count];
				for(int k=0;k<Count;k++)buf[k]=0; //初值
				for(k=0;k<NumPoint;k++)
				{
					float depth=WisWrite->WisChannel->DimInfo[0].StartVal+k*WisWrite->WisChannel->DimInfo[0].Delta;
					DlisRead->ReadWaveArray(i,j,depth,buf);
					WisWrite->WriteWaveArray(depth,buf);
				}
				delete []buf; buf=NULL;
			}
		}
	}
	WisWrite->Close();
	delete WisWrite->m_ObjectEntry; 
	WisWrite->m_ObjectEntry=NULL;
	delete WisWrite->WisChannel; 
	WisWrite->WisChannel=NULL;

	return true;
}
void CDLisToWis::SetWisChannelFromFidHead(int IndexWis,int nFra,int nCur)
{
	CString s;
	//对象设置(其它参数在WIS输出类中设置)
	int IndexDlis=DlisRead->Frame[nFra].ChannelID[nCur];
	sprintf(WisWrite->m_ObjectEntry->Name,"%-.15s",NewCurveName.GetAt(IndexWis));
	WisWrite->m_ObjectEntry->Attribute=1;
	int kDim=1; //WIS文件通道维数
	if(DlisRead->Channel[IndexDlis].Sample>1)
	{
		kDim=DlisRead->Channel[IndexDlis].NumDim+1;
	}
	if(kDim>1)
	{
		WisWrite->m_ObjectEntry->SubAttribute=2;
	}
	else
	{
		WisWrite->m_ObjectEntry->SubAttribute=1;
	}
	//通道参数
	sprintf(WisWrite->WisChannel->Unit,"%-.8s",DlisRead->Channel[IndexDlis].Units);
	sprintf(WisWrite->WisChannel->AliasName,"%-.15s",WisWrite->m_ObjectEntry->Name);
	sprintf(WisWrite->WisChannel->AliasUnit,"%-.15s",WisWrite->WisChannel->Unit);
	
	WisWrite->WisChannel->RepCode=DlisRepCToWis(DlisRead->Channel[IndexDlis].RepCode);
	WisWrite->WisChannel->CodeLen=WisCodeLen(WisWrite->WisChannel->RepCode);
	WisWrite->WisChannel->MinVal=0;
	WisWrite->WisChannel->MaxVal=100;
	WisWrite->WisChannel->Reserved=0;
	WisWrite->WisChannel->NumOfDimension=kDim;
	float Start=DlisRead->Frame[nFra].Index_Start;
	float Stop=DlisRead->Frame[nFra].Index_Stop;
	float Step=float(fabs(DlisRead->Frame[nFra].Index_Step));
	if(Start>Stop)
	{	
		Stop=DlisRead->Frame[nFra].Index_Start;
		Start=DlisRead->Frame[nFra].Index_Stop;
	}
	DWORD NumPoint=DWORD(fabs((Stop-Start)/Step)+0.5)+1;
	//设置一维信息
	s.Format ("%s",DlisRead->Frame[nFra].Index_Type);
	if(s.CompareNoCase("TIME")==0)
	{
		strcpy(WisWrite->WisChannel->DimInfo[0].Name,"TIME");
	}
	else
	{
		strcpy(WisWrite->WisChannel->DimInfo[0].Name,"DEPTH");
	}
	sprintf(WisWrite->WisChannel->DimInfo[0].Unit,"%-.7s",DlisRead->Frame[nFra].IndexUnits);
	strcpy(WisWrite->WisChannel->DimInfo[0].AliasName,WisWrite->WisChannel->DimInfo[0].Name);
	WisWrite->WisChannel->DimInfo[0].StartVal=Start;
	WisWrite->WisChannel->DimInfo[0].Delta=Step;
	WisWrite->WisChannel->DimInfo[0].Samples=NumPoint;
	WisWrite->WisChannel->DimInfo[0].MaxSamples=WisWrite->WisChannel->DimInfo[0].Samples;
	WisWrite->WisChannel->DimInfo[0].Size=DlisRead->Channel[IndexDlis].Sample*WisWrite->WisChannel->CodeLen; //////////////////////
	WisWrite->WisChannel->DimInfo[0].RepCode=WIS_REPR_FLOAT;
	WisWrite->WisChannel->DimInfo[0].Reserved=0;
	//设置维定义信息
	for(int k=1;k<WisWrite->WisChannel->NumOfDimension;k++)
	{
		int kx=WisWrite->WisChannel->NumOfDimension-k-1;
		//轴类型有：或TIME或SENSOR_OFFSET或SENSOR或ORDINAL
		s.Format("%s",DlisRead->Channel[IndexDlis].AxisID[kx]);
		if(s=="MICRO_TIME" || s=="TIME")
		{
			strcpy(WisWrite->WisChannel->DimInfo[k].Name,"TIME");
		}
		else if(s=="SENSOR_OFFSET" || s=="SENSOR")
		{
			strcpy(WisWrite->WisChannel->DimInfo[k].Name,"SENSOR");
		}
		else
		{
			strcpy(WisWrite->WisChannel->DimInfo[k].Name,"ORDINAL");
		}
		sprintf(WisWrite->WisChannel->DimInfo[k].Unit,"%-.7s",DlisRead->Channel[IndexDlis].AxisUnits[kx]);
		sprintf(WisWrite->WisChannel->DimInfo[k].AliasName,"%-.15s",DlisRead->Channel[IndexDlis].AxisID[kx]);

		WisWrite->WisChannel->DimInfo[k].StartVal=DlisRead->Channel[IndexDlis].AxisStart[kx];
		WisWrite->WisChannel->DimInfo[k].Delta=DlisRead->Channel[IndexDlis].AxisStep[kx];
		if(WisWrite->WisChannel->DimInfo[k].Delta==0)
		{
			WisWrite->WisChannel->DimInfo[k].Delta=1; //保证采样间隔不为０，以便于WIS文件处理
		}
		WisWrite->WisChannel->DimInfo[k].Samples=1;
		WisWrite->WisChannel->DimInfo[k].Samples=DlisRead->Channel[IndexDlis].Dimension[kx];
		WisWrite->WisChannel->DimInfo[k].MaxSamples=WisWrite->WisChannel->DimInfo[k].Samples;
		int TotalSample=1;
		for(int j=0;j<=kx;j++)
		{
			TotalSample*=DlisRead->Channel[IndexDlis].Dimension[j];
		}
		WisWrite->WisChannel->DimInfo[k].Size=TotalSample*WisWrite->WisChannel->CodeLen;
		WisWrite->WisChannel->DimInfo[k].RepCode=WIS_REPR_FLOAT;
		WisWrite->WisChannel->DimInfo[k].Reserved=0;
	}

}
BYTE CDLisToWis::DlisRepCToWis(BYTE RepCode)
{
	BYTE WisRepC=WIS_REPR_FLOAT;
	switch(RepCode)
	{
	case DLIS_REPC_FSHORT:
	case DLIS_REPC_FSINGL:
	case DLIS_REPC_FSING1:
	case DLIS_REPC_FSING2:
	case DLIS_REPC_ISINGL:
	case DLIS_REPC_VSINGL:
	case DLIS_REPC_CSINGL:
		WisRepC=WIS_REPR_FLOAT;
		break;
	case DLIS_REPC_FDOUBL:
	case DLIS_REPC_FDOUB1:
	case DLIS_REPC_FDOUB2:
	case DLIS_REPC_CDOUBL:
		WisRepC=WIS_REPR_DOUBLE;
		break;
	case DLIS_REPC_SSHORT:
		WisRepC=WIS_REPR_CHAR;
		break;
	case DLIS_REPC_SNORM:
		WisRepC=WIS_REPR_SHORT;
		break;
	case DLIS_REPC_SLONG:
		WisRepC=WIS_REPR_LONG;
		break;
	case DLIS_REPC_USHORT:
		WisRepC=WIS_REPR_UCHAR;
		break;
	case DLIS_REPC_UNORM:
		WisRepC=WIS_REPR_USHORT;
		break;
	case DLIS_REPC_ULONG:
	case DLIS_REPC_UVARI:
		WisRepC=WIS_REPR_ULONG;
		break;
	case DLIS_REPC_IDENT:
	case DLIS_REPC_ASCII:
	case DLIS_REPC_DTIME:
	case DLIS_REPC_ORIGIN:
	case DLIS_REPC_OBNAME:
	case DLIS_REPC_OBJREF:
	case DLIS_REPC_ATTREF:
	case DLIS_REPC_ATATUS:
	case DLIS_REPC_UNITS:
		break;
	};
	return WisRepC;
}
void CDLisToWis::SetWisHeadFromDlisHead()
{
	CString s;
	CString sCurveName;
	//////////////////////////////////////////////////////////////
	bool bResult=DlisCurvesFilter();//根据曲线选择开关(m_nCurvesRadio)对曲线进行过滤
	if(bResult)
		//要进行交互操作
		SelectChannelFromWin();
	else
	{//不必进行交互操作
		NewCurveName.RemoveAll();
		for(int i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					int k=DlisRead->Frame[i].ChannelID[j];
					NewCurveName.Add(DlisRead->Channel[k].Name);
				}
			}
		}
	}
}
bool CDLisToWis::DlisCurvesFilter()
{
	CString s;
	CStringArray sCurve;
	bool bFilter=false;
	int i;
	//根据选择开关进行过滤
	switch(m_nCurvesRadio)
	{
	case 0: //过滤曲线	
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				int k=DlisRead->Frame[i].ChannelID[j];
				CString sCurveName;
				bool bResult=false;
				sCurveName.Format ("%s",DlisRead->Channel[k].Name);
				for(int k1=0;k1<sp_FilterCurves.GetSize();k1++)
				{
					if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(k1))==0)
					{
						bResult=true;
						break;
					}
				}
				DlisRead->Frame[i].bDecode[j]=bResult;			
			}
		}
		break;
	case 2: //常规曲线
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				int k=DlisRead->Frame[i].ChannelID[j];
				if(DlisRead->Channel[k].Sample==1)
				{
					DlisRead->Frame[i].bDecode[j]=true;
				}
				else
				{
					DlisRead->Frame[i].bDecode[j]=false;
				}
			}
		}
		break;
	case 1: //全部曲线
	case 3: //交互选择
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				DlisRead->Frame[i].bDecode[j]=true;
			}
		}
		break;
	default:
		break;
	}
	//如果无帧数据，则相应帧曲线设置为不解码
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		if(DlisRead->Frame[i].noFrame>0) continue;
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	//根据帧的DIRECTION属性判断是否是连续曲线，对于离散数据，暂时没有对其进行处理
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		//如果本帧曲线不是连续采样，则忽略(将该帧所有曲线设置为不解码)
		if(DlisRead->Frame[i].bDiscrete)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				DlisRead->Frame[i].bDecode[j]=false;
			}
		}
	}
	//设置是否进行交互操作
	switch(m_nCurvesRadio)
	{
	case 3: //交互操作
		bFilter=true;
		break;
	default: //根据曲线名长度判断是否要进行改名等操作
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					int k=DlisRead->Frame[i].ChannelID[j];
					//判断长度是否符合要求
					if(strlen(DlisRead->Channel[k].Name)>FID_MAX_CURVENAME_LEN)
					{
						bFilter=true;
						break;
					}
					//判断曲线名是否已经存在
					for(int k1=0;k1<sCurve.GetSize();k1++)
					{
						if(sCurve.GetAt(k1).Compare(DlisRead->Channel[k].Name)==0)
						{
							bFilter=true;
							break;				
						}
					}
					//前面曲线暂时保存
					sCurve.Add(DlisRead->Channel[k].Name);
				}
			}
			if(bFilter) break;
		}
		break;
	}
	sCurve.RemoveAll();
	//设置索引曲线为不解码
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		DlisRead->Frame[i].bDecode[0]=false;
	}
	return bFilter;
}
BYTE CDLisToWis::PcCodeToFid(BYTE PcCode)
{
	BYTE FidCode=FID_REPR_FLOAT;
	switch(PcCode)
	{
	case  PC_REPR_FLOAT: //IEEE单精度浮点数 
	case  PC_REPR_DOUBLE: //IEE双精度浮点数
	case  PC_REPR_UINT: //无符号整型
	case  PC_REPR_ULONG: //无符号长整型数
		FidCode=FID_REPR_FLOAT;
		break;
	case  PC_REPR_SHORT://有符号短整型数
	case  PC_REPR_UCHAR: //无符号字节数
		FidCode=FID_REPR_SHORT;
		break;
	case  PC_REPR_INT: //有符号整型
	case  PC_REPR_LONG: //有符号长整型数
	case  PC_REPR_USHORT: //无符号短整型数
		FidCode=FID_REPR_LONG;
		break;
	case  PC_REPR_CHAR: //有符号字节数
		FidCode=FID_REPR_CHAR;
		break;
	case  PC_REPR_STRING: //字符串
		FidCode=FID_REPR_CHAR;
		break;
	default:
		FidCode=FID_REPR_FLOAT;
		break;
	}
	return FidCode;
}

void CDLisToWis::SelectChannelFromWin()
{
	CString s;
	CDlisCurDlg DlisCurDlg;
	NewCurveName.RemoveAll();
	int NumLog=0;
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				NumLog++;
				int k=DlisRead->Frame[i].ChannelID[j];
				//DlisCurDlg.m_ObjectName.Add(DlisRead->Channel[k].Name); //输出曲线名;

				CString sNewName=DlisRead->Channel[k].Name; //新名称
				DlisRead->GetNewName(sNewName,i,j,DlisRead->Channel[k].CopyNum,
					DlisRead->Channel[k].Origin,sp_bAddFrameNo,
					sp_bAddOrigin,sp_bAddCopyNum);
				DlisCurDlg.m_ObjectName.Add(sNewName); //输出曲线名;
				DlisCurDlg.m_SourceName.Add(DlisRead->Channel[k].Name); ////输入曲线名
				s.Format ("%u",DlisRead->Channel[k].Origin);
				DlisCurDlg.m_Origin.Add(s); //通道中源
				s.Format ("%u",DlisRead->Channel[k].CopyNum);
				DlisCurDlg.m_CopyNum.Add(s); //通道中拷贝数
				s.Format ("%.1f-%.1f",DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
				DlisCurDlg.m_IndexInfo.Add(s); //起止索引
				s.Format ("%.5f",DlisRead->Frame[i].Index_Step);
				DlisCurDlg.m_StepInfo.Add(s); //采样间隔
				s.Format ("%d",i);
				DlisCurDlg.m_FrameNum.Add(s); //帧名
				CString sValue="[";
				for(int k1=0;k1<DlisRead->Channel[k].NumDim;k1++)
				{
					if(k1>0)
					{
						sValue+=",";
					}
					s.Format ("%d",DlisRead->Channel[k].Dimension[k1]);
					sValue+=s;
				}
				sValue+="]";
				DlisCurDlg.m_Dimension.Add(sValue); //维定义
			}
		}
	}
	if(NumLog==0) return ; //没有曲线
	DlisCurDlg.m_nMaxLen=15;
	DlisCurDlg.m_nMaxLog=99999;
	DlisCurDlg.m_NumLog=NumLog;
	DlisCurDlg.nTransMode=m_nCurvesRadio;
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.DoModal();
	//////////////////////////////////////////////////////////////////
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	for(i=0;i<DlisCurDlg.m_ObjectName.GetSize();i++)
	{
		CString sFrame=DlisCurDlg.m_FrameNum.GetAt(i);
	
		int iFrame=atol((const char*)sFrame);
		DWORD Origin=atol(DlisCurDlg.m_Origin.GetAt(i));
		BYTE CopyNum=BYTE(atol(DlisCurDlg.m_CopyNum.GetAt(i)));
		CString sCurveName=	DlisCurDlg.m_SourceName.GetAt(i);

		for(int j=0;j<DlisRead->Frame[iFrame].NumCurve;j++)
		{
			int k=DlisRead->Frame[iFrame].ChannelID[j];
			if(DlisRead->Channel[k].Origin != Origin) continue;
			if(DlisRead->Channel[k].CopyNum != CopyNum) continue;
			if(sCurveName.Compare(DlisRead->Channel[k].Name)==0)
			{
				DlisRead->Frame[iFrame].bDecode[j]=true;
				NewCurveName.Add(DlisCurDlg.m_ObjectName.GetAt(i));
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.m_ObjectName.RemoveAll(); //输出曲线名
	DlisCurDlg.m_SourceName.RemoveAll(); //输入曲线名
	DlisCurDlg.m_IndexInfo.RemoveAll(); //起止索引
	DlisCurDlg.m_StepInfo.RemoveAll(); //采样间隔
	DlisCurDlg.m_FrameNum.RemoveAll(); //帧名（用帧对象号代替）
	DlisCurDlg.m_Dimension.RemoveAll(); //维定义
	DlisCurDlg.m_Origin.RemoveAll(); //通道中源
	DlisCurDlg.m_CopyNum.RemoveAll(); //通道中拷贝数
}
