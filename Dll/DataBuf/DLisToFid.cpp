// DLisToFid.cpp: implementation of the CDLisToFid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "_Comext.h"
#include "DLisToFid.h"
#include "DlisCurDlg.h"
#include "CurveSplit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDLisToFid::CDLisToFid()
{

}

CDLisToFid::~CDLisToFid()
{

}
bool CDLisToFid::DLisToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,BOOL bCurveSplit,BOOL bDelSplitCur,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //源文件名   
	m_sObjectPath=sObjectPath;  //输出目录
	m_nCurvesRadio=nCurvesRadio;//过滤开关
	m_bCurveSplit=bCurveSplit; //阵列曲线拆分开关
	m_bDelSplitCur=bDelSplitCur; //删除被拆分曲线
	fp=fo;
	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);
	DlisRead=new CDlisRead ;
	FidWrite=new CFidWrite ; //输出类
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete FidWrite;
	FidWrite=NULL;
	delete DlisRead; 
	DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDLisToFid::Transform()
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
		int NumLogFile=DlisRead->GetDlisLogicalFileNumber();
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
				WellName=szWellName;
			else
				WellName.Format("%s_%d",szWellName,nf+1);
			bool bResult=LogicalToFid(WellName,nf);
			
			if(sp_Break_Status) break; //中断
			//输出转换结果信息
			if(bResult)
				FidWrite->PrintHead(fp);
			//输出信息
			fprintf(fp,"ToObject: %s\n",(const char*)FidWrite->m_sFidFile);
		}
		DlisRead->Close(); //关闭输入文件
	}
	return !sp_Break_Status;
}
bool CDLisToFid::DlisCurvesFilter()
{
	CString s;
	bool bFilter=false;
	int i,j,k;
	//根据选择开关进行过滤
	switch(m_nCurvesRadio)
	{
	case 0: //过滤曲线
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				k=DlisRead->Frame[i].ChannelID[j];
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
	case 1: //全部曲线
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				k=DlisRead->Frame[i].ChannelID[j];
				DlisRead->Frame[i].bDecode[j]=true;
			}
		}
		break;
	case 2: //常规曲线
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				k=DlisRead->Frame[i].ChannelID[j];
				if(DlisRead->Channel[k].Sample==1)
					DlisRead->Frame[i].bDecode[j]=true;
				else
					DlisRead->Frame[i].bDecode[j]=false;
			}
		}
		break;
	case 3: //交互选择
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
				DlisRead->Frame[i].bDecode[j]=true;
		}
		break;
	default:
		break;
	}
	//如果无帧数据，则相应帧曲线设置为不解码
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		if(DlisRead->Frame[i].noFrame>0) continue;
		for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
			DlisRead->Frame[i].bDecode[j]=false;
	}
	//根据帧的DIRECTION属性判断是否是连续曲线，对于离散数据，暂时没有对其进行处理
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		//如果本帧曲线不是连续采样，则忽略(将该帧所有曲线设置为不解码)
		if(DlisRead->Frame[i].bDiscrete)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
				DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	//设置是否进行交互操作
	CStringArray sCurve;
	switch(m_nCurvesRadio)
	{
	case 3: //交互操作
		bFilter=true;
		break;
	default: //根据曲线名(超长或同名)判断是否要进行改名等操作
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					k=DlisRead->Frame[i].ChannelID[j];
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
		DlisRead->Frame[i].bDecode[0]=false;
	return bFilter;
}
BYTE CDLisToFid::PcCodeToFid(BYTE PcCode)
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


bool CDLisToFid::LogicalToFid(CString WellName,int nf)
{
	CString s;
	/////////////////////////////////////////////////////////////////
	//根据输出目录及输入文件名确定井名和输出路径
	CString sFileTitle=WellName;
	CString sOutWellPath=m_sObjectPath+"\\#"+WellName;
	ModiFileName(sOutWellPath); //根据条件决定是否修改文件名
	sFileTitle=MyGetFileTitle(sOutWellPath);

	CString sWellName=sFileTitle.Mid(1); //去掉“#”
	FidWrite->SetFidFileName(m_sObjectPath,sWellName);
	SetFidHeadFromDlisHead();
	if(NewCurveName.GetSize()==0) return false;
	//设置进度条范围为解码曲线数
	pGuage->SetRange(0,NewCurveName.GetSize());
	pGuage->SetPos(1);
	DlisRead->ReadLogicalFile(nf); //DLIS数据读到以帧为单位的文件
	FidWrite->WriteFidInfo();
	
	int NumLog=-1; //输出曲线序号
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
			int NumPoint=DlisRead->Frame[i].noFrame;
			/////////////////////////////////////////////////////////////////////
			if(FidWrite->FidChannel[NumLog].NumOfDimension==2)
			{	
				//常规曲线
				float *buffer=new float[NumPoint];
				DlisRead->ReadCurve(i,j,NumPoint,buffer);
				FidWrite->WriteGenData(NumLog,NumPoint,buffer);
				delete []buffer; buffer=NULL;
			}
			else if(FidWrite->FidChannel[NumLog].NumOfDimension==3)
			{
				//波形曲线
				if(FidWrite->OpenWave(NumLog))
				{
					int BufSize=DlisRead->Channel[index].Sample;
					float *buffer=new float[BufSize];
					for(int k=0;k<BufSize;k++)
					{
						buffer[k]=0;
					}
					for(k=0;k<NumPoint;k++)
					{
						float DepFram=DlisRead->Frame[i].Index_Start+k*DlisRead->Frame[i].Index_Step;
						DlisRead->ReadWaveArray(i,j,DepFram,buffer);
						FidWrite->WriteWave(DepFram,buffer);
					}
					delete []buffer; buffer=NULL;
					FidWrite->CloseWave();
				}
			}
			else
			{
				//阵列曲线
				if(FidWrite->OpenArray(NumLog))
				{
					int Count=DlisRead->Channel[index].Sample;
					float *buffer=new float[Count];
					for(int k=0;k<Count;k++)buffer[k]=0; //初值
					for(k=0;k<NumPoint;k++)
					{
						float DepFram=DlisRead->Frame[i].Index_Start+k*DlisRead->Frame[i].Index_Step;
						DlisRead->ReadWaveArray(i,j,DepFram,buffer);
						FidWrite->WriteArray(DepFram,buffer);
					}
					delete []buffer; buffer=NULL;
					FidWrite->CloseArray();
				}
				//对阵列曲线进行降维处理
				if(m_bCurveSplit)
				{
					CCurveSplit CurveSplit;
					bool bResult=CurveSplit.CurveSplit(FidWrite->m_sFidFile,FidWrite->FidChannel[NumLog].CurveName);
					if(m_bDelSplitCur)
					{	//删除被拆分曲线
						CString sPath=MyGetPathName(FidWrite->m_sFidFile);
						CString sWell=MyGetFileTitle(FidWrite->m_sFidFile);
						CString sCurveFile;
						sCurveFile.Format ("%s\\I%s.%s",sPath,sWell,FidWrite->FidChannel[NumLog].CurveName);
						remove(sCurveFile);
						sCurveFile.Format ("%s\\D%s.%s",sPath,sWell,FidWrite->FidChannel[NumLog].CurveName);
						remove(sCurveFile);
					}
				}
			}
		}
	}
	//将目标目录中所有曲线名取出放入FID文件中
	FidWrite->FefurbishFidInfo();
	return true;
}

void CDLisToFid::SelectChannelFromWin()
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
						sValue+=",";
					s.Format ("%d",DlisRead->Channel[k].Dimension[k1]);
					sValue+=s;
				}
				sValue+="]";
				DlisCurDlg.m_Dimension.Add(sValue); //维定义
			}
		}
	}
	if(NumLog==0) return ; //没有曲线
	DlisCurDlg.m_nMaxLen=FID_MAX_CURVENAME_LEN;
	DlisCurDlg.m_nMaxLog=99999;
	DlisCurDlg.m_NumLog=NumLog;
	DlisCurDlg.nTransMode=m_nCurvesRadio;
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.DoModal();
	//////////////////////////////////////////////////////////////////
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			DlisRead->Frame[i].bDecode[j]=false;
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

void CDLisToFid::SetFidHeadFromDlisHead()
{
	CString s;
	sprintf(FidWrite->FidHead.Oil_Field,"%-80.80s",DlisRead->Origin.FieldName);
	sprintf(FidWrite->FidHead.Region,"%-80.80s","");
	sprintf(FidWrite->FidHead.Company,"%-80.80s",DlisRead->Origin.Company);
	sprintf(FidWrite->FidHead.Well,"%-80.80s",DlisRead->Origin.WellName);
	sprintf(FidWrite->FidHead.X_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Y_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Exformation,"%-80.80s",DlisRead->Origin.CreationTime);
	FidWrite->FidHead.NumLog=0;
	CString sCurveName;
	//////////////////////////////////////////////////////////////
	bool bResult=DlisCurvesFilter();//根据曲线选择开关(m_nCurvesRadio)对曲线进行过滤
	if(bResult)		
		SelectChannelFromWin();//要进行交互操作
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
	////////////////////////////////////////////////////////////////////
	if(NewCurveName.GetSize()==0) return ;
	FidWrite->FidHead.NumLog=NewCurveName.GetSize();
	//根据曲线数(FidWrite->FidHead.NumLog)开辟空间
	FidWrite->InitFidChannel();
	int NumLog=-1; //实际曲线数序号
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				NumLog++;
				sprintf(FidWrite->FidChannel[NumLog].CurveName,"%s",(const char*)(NewCurveName.GetAt(NumLog)));
				int index=DlisRead->Frame[i].ChannelID[j]; //曲线在DLIS通道中的索引号
				if(DlisRead->Channel[index].Sample==1)
				{
					FidWrite->FidChannel[NumLog].NumOfDimension=2;
				}
				else
				{
					FidWrite->FidChannel[NumLog].NumOfDimension=DlisRead->Channel[index].NumDim+2;
				}
				int kD=FidWrite->FidChannel[NumLog].NumOfDimension-1; //最后一维下标
				//设置第一维数据
				if((strcmp(DlisRead->Frame[i].IndexUnits,"m")==0) && (kD<3))
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[0].Name,"DEP");//将维的名称作变换
				else
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[0].Name,DlisRead->Frame[i].Index_Type);
				strcpy(FidWrite->FidChannel[NumLog].DimInfo[0].Unit,DlisRead->Frame[i].IndexUnits);
				FidWrite->FidChannel[NumLog].DimInfo[0].RepCode=FID_REPR_FLOAT;
				FidWrite->FidChannel[NumLog].DimInfo[0].CodeLen=WisCodeLen(FID_REPR_FLOAT);
				FidWrite->FidChannel[NumLog].DimInfo[0].Nps=1;
				FidWrite->FidChannel[NumLog].DimInfo[0].Npw=0;
				FidWrite->FidChannel[NumLog].DimInfo[0].Rlev=float(fabs(DlisRead->Frame[i].Index_Step));
				if(DlisRead->Frame[i].Index_Start < DlisRead->Frame[i].Index_Stop)
				{
					FidWrite->FidChannel[NumLog].DimInfo[0].Start=DlisRead->Frame[i].Index_Start;
					FidWrite->FidChannel[NumLog].DimInfo[0].Stop= DlisRead->Frame[i].Index_Stop;
				}
				else
				{
					FidWrite->FidChannel[NumLog].DimInfo[0].Start=DlisRead->Frame[i].Index_Stop;
					FidWrite->FidChannel[NumLog].DimInfo[0].Stop=DlisRead->Frame[i].Index_Start;
				}
				//设置曲线最后一维(工程值)数据
				sprintf(FidWrite->FidChannel[NumLog].DimInfo[kD].Name,"%s",FidWrite->FidChannel[NumLog].CurveName);
				if(j==0)
					s.Format("%s",DlisRead->Frame[i].IndexUnits); //索引曲线
				else
					s.Format("%s",DlisRead->Channel[index].Units); //非索引曲线
				s.TrimLeft();
				s.TrimRight();
				s.Remove(' ');
				if(s.IsEmpty())
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[kD].Unit,"none");
				else
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[kD].Unit,(const char*)s);
				FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode=PcCodeToFid(DlisRead->Channel[index].PcCode);
				if(kD==1)
				{	//常规曲线，FID中表示码一般可设置为4字节浮点
					FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode=FID_REPR_FLOAT;
				}
				else
				{
					FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode=PcCodeToFid(DlisRead->Channel[index].PcCode);
					//FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode=FID_REPR_SHORT;
				}
				FidWrite->FidChannel[NumLog].DimInfo[kD].CodeLen=WisCodeLen(FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode);
				FidWrite->FidChannel[NumLog].DimInfo[kD].Nps=DlisRead->Channel[index].Dimension[0];
				FidWrite->FidChannel[NumLog].DimInfo[kD].Npw=FidWrite->FidChannel[NumLog].DimInfo[kD].Nps;
				FidWrite->FidChannel[NumLog].DimInfo[kD].Start=0;
				FidWrite->FidChannel[NumLog].DimInfo[kD].Stop=100;
				FidWrite->FidChannel[NumLog].DimInfo[kD].Rlev=0;
				//设置曲线中间各维信息（对于常规kD为1,下面这段程序不会运行）
				for(int k=1;k<kD;k++)
				{
					//由于FID文件只定义了波形曲线，所以对阵列曲线将不能正常读出
					//int kx=DlisRead->Channel[index].NumDim-k; //序号为反
					int kx=k-1; //
					if(kD==2)
						strcpy(FidWrite->FidChannel[NumLog].DimInfo[k].Name,"T");//对于波形曲线，
					else
					{
						strcpy(FidWrite->FidChannel[NumLog].DimInfo[k].Name,DlisRead->Channel[index].AxisID[kx]); //轴名
						if(strlen(FidWrite->FidChannel[NumLog].DimInfo[k].Name)==0)
							sprintf(FidWrite->FidChannel[NumLog].DimInfo[k].Name,"ORDINAL");
					}
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[k].Unit,DlisRead->Channel[index].AxisUnits[kx]); //轴的单位
					if(strlen(FidWrite->FidChannel[NumLog].DimInfo[k].Unit)==0)
						strcpy(FidWrite->FidChannel[NumLog].DimInfo[k].Unit,"none");
					FidWrite->FidChannel[NumLog].DimInfo[k].RepCode=FID_REPR_FLOAT;
					FidWrite->FidChannel[NumLog].DimInfo[k].CodeLen=WisCodeLen(FID_REPR_FLOAT);
					FidWrite->FidChannel[NumLog].DimInfo[k].Nps=DlisRead->Channel[index].Dimension[kx];
					FidWrite->FidChannel[NumLog].DimInfo[k].Npw=0;
					FidWrite->FidChannel[NumLog].DimInfo[k].Rlev=DlisRead->Channel[index].AxisStep[kx];
					FidWrite->FidChannel[NumLog].DimInfo[k].Start=DlisRead->Channel[index].AxisStart[kx];
					FidWrite->FidChannel[NumLog].DimInfo[k].Stop=FidWrite->FidChannel[NumLog].DimInfo[k].Start+(FidWrite->FidChannel[NumLog].DimInfo[k].Nps-1)*FidWrite->FidChannel[NumLog].DimInfo[k].Rlev;
				}
			}
		}
	}
}
