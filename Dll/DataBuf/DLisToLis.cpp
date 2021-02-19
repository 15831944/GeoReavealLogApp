// DLisToLis.cpp: implementation of the CDlisToLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "_Comext.h"
#include "DLisToLis.h"
#include "DlisCurDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlisToLis::CDlisToLis()
{

}

CDlisToLis::~CDlisToLis()
{

}
bool CDlisToLis::DlisToLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	LisWrite=new CLisWrite ; //输出类
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete LisWrite; LisWrite=NULL;
	delete DlisRead; DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDlisToLis::Transform()
{
	CString s;
	if(!IsDLIS(m_SourceFile))
	{
		fprintf(fp,"%s is not DLIS Format\n\n",m_SourceFile);
		return false;
	}
	//根据输出目录及输入文件名确定输出文件名
	LisFileName.Format ("%s\\%s.lis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	LisFileName.MakeLower();
	ModiFileName(LisFileName); //根据条件决定是否修改文件名

	if(!DlisRead->Open(m_SourceFile,fp)) 
	{
		fprintf(fp,"\n!!!!链接的数据文件打开失败\n\n");
	}
	else
	{
		fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
		fprintf(fp,"ToObject: %s\n",(const char*)LisFileName);
		NumLisLog=0;
		LisWrite->Open(LisFileName); //打开输出LIS文件
		SetReelHeader(); //设置卷头记录信息
		LisWrite->WriteRTHT(132); //卷头
		LisWrite->WriteRTHT(130); //带头

		//设置进度条为一小段，为了视觉
		pGuage->SetRange(0,100);
		pGuage->SetPos(3);
		//浏览整个DLIS文件，取得逻辑文件数及各逻辑文件的位置
		int NumLogFile=DlisRead->GetDlisLogicalFileNumber();

		pGuage->m_nTextMode=1; //设置文本显示模式为自定义
		pGuage->m_sTextInfo.Format ("开始转换"); //置文本内容
		pGuage->SetRange(0,100);
		pGuage->SetPos(1);
		for(int nf=0;nf<NumLogFile;nf++)
		{
			pGuage->m_sTextInfo.Format ("扫描逻辑文件信息"); //置文本内容
			pGuage->SetPos(nf*100/NumLogFile);
			DlisRead->ScanDlisOneLogicalFile(nf);
			/////////////////////////////////////////////////////////
			bool bResult=LogicalToLis(nf);
			if(sp_Break_Status) break; //中断
		}
		pGuage->m_nTextMode=0;
		//////////////////////////////////////
		LisWrite->WriteRTHT(131); //带尾
		LisWrite->WriteRTHT(133); //卷尾
		LisWrite->Close();//关闭输出文件
		DlisRead->Close(); //关闭输入文件
		if(NumLisLog>0)
		{
			fprintf(fp,"\n");
			fprintf(fp,"    From: %s\n",m_SourceFile);
			fprintf(fp,"      To: %s\n",LisFileName);
			fprintf(fp," Include: %d Logical File\n\n",NumLisLog);
		}
		else
		{
			remove(LisFileName);
			return false;
		}
	}
	return !sp_Break_Status;
}

bool CDlisToLis::LogicalToLis(int nf)
{
	CString s;
	/////////////////////////////////////////////////////////////////
	SetCurvesFromDlis();
	if(NewCurveName.GetSize()==0) return false;
	pGuage->m_nTextMode=1; //设置文本显示模式为自定义
	pGuage->m_sTextInfo.Format ("读取DLIS数据到临时文件"); //置文本内容
	pGuage->SetRange(0,100);
	pGuage->SetPos(1);
	DlisRead->ReadLogicalFile(nf); //DLIS数据读到以帧为单位的文件
	//设置进度条范围为解码曲线数
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		LisWrite->NumLog=GetNumLog(i);
		if(LisWrite->NumLog==0)
		{
			continue;//本帧无曲线要转换
		}
		NumLisLog++;
		fprintf(fp,"\n====Write No. %d LIS Logical File From No. %d DLIS Logical File\n",NumLisLog,nf+1);
		SetFileHeader(); //设置文件头参数
		SetEB(i);  //设置格式说明块
		int NumPoint=DlisRead->Frame[i].noFrame;
		float sdep,edep,rlev;
		rlev=float(fabs(DlisRead->Frame[i].Index_Step));
		if(DlisRead->Frame[i].Index_Start<DlisRead->Frame[i].Index_Stop)
		{
			sdep=DlisRead->Frame[i].Index_Start;
			edep=DlisRead->Frame[i].Index_Stop;
		}
		else
		{
			sdep=DlisRead->Frame[i].Index_Stop;
			edep=DlisRead->Frame[i].Index_Start;
		}
		LisWrite->StDep=sdep; //逻辑文件起始深度
		LisWrite->EnDep=edep; //逻辑文件结束深度
		LisWrite->Rlev=rlev;  //逻辑文件采样间隔
		///////////////////////////////////////////////////////////////
		LisWrite->WriteFHR(128); //File Header Record
		LisWrite->WriteEB(); //写格式说明记录
		LisWrite->StartWriteData(); //开始输出当前LIS逻辑文件数据
		////////////////////////////////////////////////////////////////
		int NumLog=-1;
		pGuage->SetRange(0,100);
		pGuage->SetPos(1);
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(sp_Break_Status) break;
			if(!DlisRead->Frame[i].bDecode[j]) continue;
			///////////////////////////////////////////////////
			NumLog++;
			int index=DlisRead->Frame[i].ChannelID[j];
			int Count=DlisRead->Channel[index].Sample; //一个采样点的数据项
			if(Count==1)
			{
				pGuage->m_sTextInfo.Format ("Curve: %s",DlisRead->Channel[index].Name); //置文本内容
			}
			else
			{
				pGuage->m_sTextInfo.Format ("Waveform: %s",DlisRead->Channel[index].Name); //置文本内容
			}
			pGuage->SetPos((NumLog+2)*100/LisWrite->NumLog);  //设置进度条当前值
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			/////////////////////////////////////////////////////////////////////
			fprintf(fp,"====TRANSFORM: %-8s  START=%8G  STOP=%8G  SPACING=%8G  COUNT=%5d\n",
				DlisRead->Channel[index].Name,sdep,edep,rlev,Count);
			fflush(fp);
			if(DlisRead->Channel[index].Sample==1)
			{
				//常规曲线
				float *buf=new float[NumPoint];
				DlisRead->ReadCurve(i,j,NumPoint,buf);
				LisWrite->WriteCurveData(NumLog,sdep,edep,rlev,NumPoint,buf);
				delete []buf; buf=NULL;
			}
			else if(DlisRead->Channel[index].NumDim==1)
			{
				float *buf=new float[Count];
				for(int k=0;k<Count;k++)
				{
					buf[k]=0;
				}
				pGuage->SetRange(0,100);
				pGuage->SetPos(1);
				for(k=0;k<NumPoint;k++)
				{
					pGuage->SetPos((k+1)*100/NumPoint);
					if (pGuage->CheckCancelButton())
						sp_Break_Status=true;
					if(sp_Break_Status) break;
					float DepFram=DlisRead->Frame[i].Index_Start+k*DlisRead->Frame[i].Index_Step;
					DlisRead->ReadWaveArray(i,j,DepFram,buf);
					LisWrite->WriteWaveData(NumLog,DepFram,Count,buf);
				}
				delete []buf; buf=NULL;
			}
		}
		//设置进度条信息为提示
		pGuage->m_sTextInfo="临时文件写入LIS文件";
		pGuage->SetPos(10);
		LisWrite->StopWriteData();//结束输出当前LIS逻辑文件数据
		pGuage->SetPos(100);
		LisWrite->WriteFHR(129); //File Trailer Record
	}
	pGuage->m_nTextMode=0;
	pGuage->m_sTextInfo.Empty();
	return true;
}
void CDlisToLis::SetFileHeader()
{
	sprintf(LisWrite->FileHead.Name,"%-6s.%03d","LIDT",NumLisLog);
	sprintf(LisWrite->FileHead.Nul1,"%-2.2s"," ");
	sprintf(LisWrite->FileHead.SubSerName,"%-6s","LIDT");
	sprintf(LisWrite->FileHead.Version,"%-8s","V 1.00");

	struct tm *newtime;
	long ltime;
	_tzset();
	time( &ltime );
	newtime = localtime( &ltime );
	sprintf(LisWrite->FileHead.Date,"%02d/%02d/%02d",newtime->tm_mon+1,newtime->tm_mday,newtime->tm_year%100);
	LisWrite->FileHead.Nul2=32;
	sprintf(LisWrite->FileHead.MaxPRL,"%5d",MAX_PHYRECORD_LENGTH);
	sprintf(LisWrite->FileHead.Nul3,"%-2s"," ");
	sprintf(LisWrite->FileHead.FileType,"%-2s","LO");
	sprintf(LisWrite->FileHead.Nul4,"%-2s","  ");
	sprintf(LisWrite->FileHead.PreConName,"%-10s"," ");
}
int CDlisToLis::GetNumLog(int nFrame)
{
	int NumLog=0;
	for(int i=0;i<DlisRead->Frame[nFrame].NumCurve;i++)
	{
		if(DlisRead->Frame[nFrame].bDecode[i])
		{
			NumLog++;
		}
	}
	return NumLog;
}
void CDlisToLis::SetEB(int nFrame)
{ 
	CString s;
	if(LisWrite->Dsbr)
	{
		delete []LisWrite->Dsbr; LisWrite->Dsbr=NULL;
	}
	//取当前逻辑文件曲线数
	LisWrite->NumLog=GetNumLog(nFrame);
	if(LisWrite->NumLog==0)
	{
		return;
	}
	//计算已转换过的曲线
	int NumLogT=0;
	for(int nItem=0;nItem<nFrame;nItem++)
	{
		for(int i=0;i<DlisRead->Frame[nItem].NumCurve;i++)
		{
			if(DlisRead->Frame[nItem].bDecode[i])
			{
				NumLogT++;
			}
		}
	}
	char str[255];
	LisWrite->Rlev=float(fabs(DlisRead->Frame[nFrame].Index_Step));
	LisWrite->Dsbr=new DataSpeBlockRec[LisWrite->NumLog];
	LisWrite->EnVa.DaFraSiz=0;
	int noi=-1;
	for(int i=0;i<DlisRead->Frame[nFrame].NumCurve;i++)
	{
		if(!DlisRead->Frame[nFrame].bDecode[i]) continue;
		int k=NumLogT;
		NumLogT++;
		noi++;
		int index=DlisRead->Frame[nFrame].ChannelID[i];
		sprintf(str,"%-4.4s",(const char*)(NewCurveName.GetAt(k)));
		memcpy(LisWrite->Dsbr[noi].Mnemon,str,4); //曲线名
		memcpy(LisWrite->Dsbr[noi].ServID,"SNAM  ",6); //仪器名
		memset(LisWrite->Dsbr[noi].ServOrd,' ',8); //服务号
		int kDim=1;
		if(DlisRead->Channel[index].Sample>1)
		{
			kDim=2;
		}
		//曲线单位，如果曲线单位长度超过4字节且含有“.”时删除,最终取前面4字节
		s.Format("%s",DlisRead->Channel[index].Units); 
		if(s.GetLength()>4)
		{
			s.Remove('.');
		}
		s.MakeUpper();
		sprintf(str,"%-4.4s",s);
		s=str;
		if(s.CompareNoCase("none")==0)
		{
			memcpy(LisWrite->Dsbr[noi].Units,"    ",4); //测量单位
		}
		else
		{
			memcpy(LisWrite->Dsbr[noi].Units,str,4); //测量单位
		}
		
		LisWrite->Dsbr[noi].APICode=0; //API代码,对于0型可将其分为4部分:测井类型、曲线类型、曲线等级、曲线修饰
		LisWrite->Dsbr[noi].FileNumber=NumLisLog; //文件号,指首次获得数据时的文件号
		LisWrite->Dsbr[noi].Nul=0; //两字节，值为0
		LisWrite->Dsbr[noi].ProLvl=0; //处理水平(类型0)，对于类型1没用
		LisWrite->Dsbr[noi].Samples=1; //采样点数
		if(LisWrite->Dsbr[noi].Samples<1)
		{
			LisWrite->Dsbr[noi].Samples=1;
		}
		LisWrite->Dsbr[noi].RepC=PcCodeToLis(DlisRead->Channel[index].PcCode); //数据代码
		LisWrite->Dsbr[noi].Size=DlisRead->Channel[index].Dimension[0]*LisWrite->GetCodeLength(LisWrite->Dsbr[noi].RepC); //样点总长度，指一帧内所占的字节数
		for(int j=0;j<5;j++)
		{
			LisWrite->Dsbr[noi].ProInd[j]=0; //处理水平指示器(类型1),对于类型0没用
		}
	}
	/////////////////////////////////////////////////////////////
	LisWrite->EnVa.DaRecTyp=0; //1:数据记录的逻辑类型
	LisWrite->EnVa.DSBTyp=0; //2:数据说明块类型
	//3:数据记录中每帧长度（字节）
	LisWrite->EnVa.DaFraSiz=0;
	for(i=0;i<LisWrite->NumLog;i++)
	{
		LisWrite->EnVa.DaFraSiz+=LisWrite->Dsbr[i].Size;
	}
	LisWrite->EnVa.UDFlag=255; //4:上测/下测标志,1为上测 
	LisWrite->EnVa.DepUnitFlag=255; //5:深度单位标志,1为英尺，255为米,0为时间
	LisWrite->EnVa.DaRefP=0; //6:数据参考点到测量点间的相对距离
	memcpy(LisWrite->EnVa.DaRefUnt,"M   ",4); //7:数据参考点到测量点间的相对距离单位
	LisWrite->EnVa.FraSpace=LisWrite->Rlev; //8:帧之间的采样间距
	memcpy(LisWrite->EnVa.FraUnt,"M   ",4); //9:采样间距的单位
	LisWrite->EnVa.MaxFraRec=1; //一个逻辑记录所包含的帧数
	for(i=0;i<127;i++)
	{	//最多127帧
		if(LisWrite->EnVa.MaxFraRec>=127) break;
		if((LisWrite->EnVa.MaxFraRec+1)*LisWrite->EnVa.DaFraSiz > 1014)break;
		LisWrite->EnVa.MaxFraRec++;
	}
	LisWrite->EnVa.AbsVal=-999.25;	//12:缺省值
	LisWrite->EnVa.DepMod=1; //13:深度记录方式标志
	memcpy(LisWrite->EnVa.DepUnt,"M   ",4);	//14:深度值计数单位
	LisWrite->EnVa.DepRpc=68;	//15:深度值信息代码(深度方式为1时)
	LisWrite->EnVa.DSBSTyp=1;	//16:数据说明块子类型号
}

void CDlisToLis::SetCurvesFromDlis()
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

bool CDlisToLis::DlisCurvesFilter()
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
				if(DlisRead->Channel[k].NumDim>1)
				{
					continue; //只能转换常规或波形曲线
				}
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
				int k=DlisRead->Frame[i].ChannelID[j];
				if(DlisRead->Channel[k].NumDim==1)
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
	//设置索引曲线为不解码
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		DlisRead->Frame[i].bDecode[0]=false;
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
					if(strlen(DlisRead->Channel[k].Name)>4)
					{
						bFilter=true;
						break;
					}
				}
			}
			if(bFilter) break;
		}
		break;
	}
	sCurve.RemoveAll();

	return bFilter;
}
BYTE CDlisToLis::PcCodeToLis(BYTE PcCode)
{
	BYTE LisCode=LIS_REPC_FLOAT32;
	switch(PcCode)
	{
	case  PC_REPR_FLOAT: //IEEE单精度浮点数 
	case  PC_REPR_DOUBLE: //IEE双精度浮点数
	case  PC_REPR_UINT: //无符号整型
	case  PC_REPR_ULONG: //无符号长整型数
		LisCode=LIS_REPC_FLOAT32;
		break;
	case  PC_REPR_SHORT://有符号短整型数
	case  PC_REPR_UCHAR: //无符号字节数
		LisCode=LIS_REPC_INT16;
		break;
	case  PC_REPR_INT: //有符号整型
	case  PC_REPR_LONG: //有符号长整型数
	case  PC_REPR_USHORT: //无符号短整型数
		LisCode=LIS_REPC_INT32;
		break;
	case  PC_REPR_CHAR: //有符号字节数
		LisCode=LIS_REPC_INT8;
		break;
	case  PC_REPR_STRING: //字符串
		LisCode=LIS_REPC_ASCII;
		break;
	default:
		LisCode=LIS_REPC_FLOAT32;
		break;
	}
	return LisCode;
}

void CDlisToLis::SetReelHeader()
{
	sprintf(LisWrite->TapeHead.SerName,"%-6.6s","LIDT");
	sprintf(LisWrite->TapeHead.Nul1,"%-6.6s"," ");
	struct tm *newtime;
	long ltime;
	time( &ltime );
	newtime = gmtime( &ltime );
	sprintf(LisWrite->TapeHead.Date,"%02d/%02d/%02d",newtime->tm_mon+1,newtime->tm_mday,newtime->tm_year%100);
	sprintf(LisWrite->TapeHead.Nul2,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.OriginData,"%-4.4s","DLIS");	
	sprintf(LisWrite->TapeHead.Nul3,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.Name,"%-8.8s"," ");
	sprintf(LisWrite->TapeHead.Nul4,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.ConnectNum,"%-2.2s","01");	
	sprintf(LisWrite->TapeHead.Nul5,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.PreConName,"%-8.8s"," ");
	sprintf(LisWrite->TapeHead.Nul6,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.Comments,"%-74s","SICHUAN LOGGING SERVICES , LIS DATA");
}

void CDlisToLis::SelectChannelFromWin()
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
	if(NumLog==0) 
		return ; //没有曲线
	DlisCurDlg.m_nMaxLen=4;
	DlisCurDlg.m_nMaxLog=1000;
	DlisCurDlg.m_NumLog=NumLog;
	DlisCurDlg.m_sModuleName="DLISTOLIS";
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
