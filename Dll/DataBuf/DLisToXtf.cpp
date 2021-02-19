// DLisToXtf.cpp: implementation of the CDLisToXtf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include "_Comext.h"
#include "DLisToXtf.h"
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

CDLisToXtf::CDLisToXtf()
{

}

CDLisToXtf::~CDLisToXtf()
{

}
bool CDLisToXtf::DlisToXtf(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	XtfWrite=new CXtfWrite ; //输出类
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete XtfWrite; XtfWrite=NULL;
	delete DlisRead; DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDLisToXtf::Transform()
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
			{
				WellName=szWellName;
			}
			else
			{
				WellName.Format("%s_%d",szWellName,nf+1);
			}
			bool bResult=LogicalToXtf(WellName,nf);
			if(sp_Break_Status) break; //中断
			//输出转换结果信息
			if(bResult)
				XtfWrite->PrintHead(fp);
			//输出信息
		}
		DlisRead->Close(); //关闭输入文件
	}
	return !sp_Break_Status;
}
bool CDLisToXtf::LogicalToXtf(CString WellName,int nf)
{
	CString s;
	//根据输出目录及输入文件名确定输出文件名
	sOutFileName.Format ("%s\\%s.xtf",m_sObjectPath,WellName);
	ModiFileName(sOutFileName); //根据条件决定是否修改文件名
	SetXtfHeadFromDlisHead();
	if(NewCurveName.GetSize()==0)
	{
		fprintf(fp,"\n!!!!没有要处理的曲线\n");
		return false;
	}
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);
	if(!XtfWrite->Open(sOutFileName)) return false;
	XtfWrite->InitXtfHeader();
	SetXtfHeaderFromDlis();
	XtfWrite->WriteXtfHeader();
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
			int nChannel=DlisRead->Frame[i].ChannelID[j];
			SetXtfCurveHead(NumLog,nChannel,i,j); //设置曲线头参数
			XtfWrite->WriteCurveHeader(NumLog);
			int NumPoint=DlisRead->Frame[i].noFrame;
			if(XtfWrite->XtfHead.CurveAttrib[NumLog].CurType==1)
			{
				float *buf=new float[NumPoint];
				DlisRead->ReadCurve(i,j,NumPoint,buf);
				XtfWrite->WriteGenData(NumPoint,buf);
				delete []buf; buf=NULL;
			}
			else
			{	
				//波形或阵列曲线
				int Count=DlisRead->Channel[nChannel].Sample;
				float *buf=new float[Count];
				for(int k=0;k<Count;k++)buf[k]=0; //初值
				for(k=0;k<NumPoint;k++)
				{
					float depth=XtfWrite->XtfHead.Sdep[NumLog]+k*XtfWrite->XtfHead.Level[NumLog];
					DlisRead->ReadWaveArray(i,j,depth,buf);
					XtfWrite->WriteWaveArray(depth,buf);
				}
				delete []buf; buf=NULL;
			}
		}
	}
	XtfWrite->Close();
	
	return true;
}
void CDLisToXtf::SetXtfHeadFromDlisHead()
{
	CString s;
	CString sCurveName;
	//////////////////////////////////////////////////////////////
	bool bResult=DlisCurvesFilter();//根据曲线选择开关(m_nCurvesRadio)对曲线进行过滤
	if(bResult)
	{
		//要进行交互操作
		SelectChannelFromWin();
	}
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
bool CDLisToXtf::DlisCurvesFilter()
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
					if(strlen(DlisRead->Channel[k].Name)>8)
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
void CDLisToXtf::SelectChannelFromWin()
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
	if(NumLog==0) return ; //没有曲线
	DlisCurDlg.m_nMaxLen=8;
	DlisCurDlg.m_nMaxLog=512;
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

BYTE CDLisToXtf::DlisRepCToXtf(BYTE RepCode)
{
	BYTE XtfRepC=XTF_REPR_FLOAT;
	switch(RepCode)
	{
	case DLIS_REPC_FSHORT:
	case DLIS_REPC_FSINGL:
	case DLIS_REPC_FSING1:
	case DLIS_REPC_FSING2:
	case DLIS_REPC_ISINGL:
	case DLIS_REPC_VSINGL:
	case DLIS_REPC_CSINGL:
		XtfRepC=XTF_REPR_FLOAT;
		break;
	case DLIS_REPC_FDOUBL:
	case DLIS_REPC_FDOUB1:
	case DLIS_REPC_FDOUB2:
	case DLIS_REPC_CDOUBL:
		XtfRepC=XTF_REPR_DOUBLE;
		break;
	case DLIS_REPC_SSHORT:
		XtfRepC=XTF_REPR_CHAR;
		break;
	case DLIS_REPC_SNORM:
		XtfRepC=XTF_REPR_INT2;
		break;
	case DLIS_REPC_SLONG:
		XtfRepC=XTF_REPR_INT4;
		break;
	case DLIS_REPC_USHORT:
		XtfRepC=XTF_REPR_UCHAR;
		break;
	case DLIS_REPC_UNORM:
		XtfRepC=XTF_REPR_UINT2;
		break;
	case DLIS_REPC_ULONG:
	case DLIS_REPC_UVARI:
		XtfRepC=XTF_REPR_UINT4;
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
	return XtfRepC;
}
void CDLisToXtf::FindFrameAndCurveIndex(const int index,int &nFrame,int &nCurve)
{
	int Num=-1;
	nFrame=-1;
	nCurve=-1;
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				Num++;
				if(index==Num)
				{
					nFrame=i;
					nCurve=j;
					return;
				}
			}
		}
	}
	return;
}
void CDLisToXtf::SetXtfHeaderFromDlis()
{
	CString s;
	char str[256];
	sprintf(str,"%-80.80s",DlisRead->Origin.WellName); //井名
	memcpy(XtfWrite->XtfHead.Wellname,str,80);
	sprintf(str,"%-80.80s",DlisRead->Origin.FieldName); //油田名
	memcpy(XtfWrite->XtfHead.Fieldname,str,80); 
	sprintf(str,"%-80.80s",DlisRead->Origin.Company); //公司名
	memcpy(XtfWrite->XtfHead.Companyname,str,80);
	memset(XtfWrite->XtfHead.Welllocation,' ',40);
	XtfWrite->XtfHead.Metricflag=0; //公制单位
	XtfWrite->XtfHead.FileRecord=8;  //文件记录数(最小值)
	XtfWrite->XtfHead.isnumcv=NewCurveName.GetSize();  //曲线条数
	if(XtfWrite->XtfHead.isnumcv==0) return ;

	XtfWrite->XtfHead.wttop=0;  //曲线的顶部深度
	XtfWrite->XtfHead.wtbottom=0;  //曲线的底部深度
	XtfWrite->XtfHead.wtlevel=0;  //曲线的采样间隔
	int NumDepCurve=0; //深度采样曲线数
	for(int i=0;i<NewCurveName.GetSize();i++)
	{
		int nFrame=-1; //第i条曲线所在帧号
		int nCurve=-1; //第i条曲线所在帧的曲线序号
		FindFrameAndCurveIndex(i,nFrame,nCurve);
		int nChannel=DlisRead->Frame[nFrame].ChannelID[nCurve];
		sprintf(str,"%-8.8s",(const char*)(NewCurveName.GetAt(i)));
		memcpy(XtfWrite->XtfHead.CurveName[i],str,8);
		sprintf(str,"%-8.8s",DlisRead->Channel[nChannel].Units);
		memcpy(XtfWrite->XtfHead.Units[i],str,8);

		XtfWrite->XtfHead.LevelNumber[i]=DlisRead->Frame[nFrame].noFrame;
		XtfWrite->XtfHead.Level[i]=float(fabs(DlisRead->Frame[nFrame].Index_Step));
		if(DlisRead->Frame[nFrame].Index_Start<DlisRead->Frame[nFrame].Index_Stop)
		{
			XtfWrite->XtfHead.Sdep[i]=DlisRead->Frame[nFrame].Index_Start;
			XtfWrite->XtfHead.Edep[i]=DlisRead->Frame[nFrame].Index_Stop;
		}
		else
		{
			XtfWrite->XtfHead.Sdep[i]=DlisRead->Frame[nFrame].Index_Stop;
			XtfWrite->XtfHead.Edep[i]=DlisRead->Frame[nFrame].Index_Start;	
		}
		//深度采样曲线
		NumDepCurve++;
		if(NumDepCurve==1)
		{
			XtfWrite->XtfHead.wttop=XtfWrite->XtfHead.Sdep[i];
			XtfWrite->XtfHead.wtbottom=XtfWrite->XtfHead.Edep[i];
			XtfWrite->XtfHead.wtlevel=XtfWrite->XtfHead.Level[i];
		}
		else
		{
			if(XtfWrite->XtfHead.wttop>XtfWrite->XtfHead.Sdep[i])
			{
				XtfWrite->XtfHead.wttop=XtfWrite->XtfHead.Sdep[i];
			}
			if(XtfWrite->XtfHead.wtbottom<XtfWrite->XtfHead.Edep[i])
			{
				XtfWrite->XtfHead.wtbottom=XtfWrite->XtfHead.Edep[i];
			}
			if(XtfWrite->XtfHead.wtlevel>XtfWrite->XtfHead.Level[i])
			{
				XtfWrite->XtfHead.wtlevel=XtfWrite->XtfHead.Level[i];
			}
		}
		XtfWrite->XtfHead.ndimension[i]=DlisRead->Channel[nChannel].NumDim;
		XtfWrite->XtfHead.idimen1[i]=DlisRead->Channel[nChannel].Dimension[0]; //曲线第一维的单元数
		XtfWrite->XtfHead.idimen2[i]=DlisRead->Channel[nChannel].Dimension[1]; //曲线第二维的单元数
		XtfWrite->XtfHead.idimen3[i]=DlisRead->Channel[nChannel].Dimension[2]; //曲线第三维的单元数
		if(DlisRead->Channel[nChannel].Sample==1)
		{
			XtfWrite->XtfHead.CurveAttrib[i].CurType=1;//曲线类型(1-3)：常规
		}
		else if(DlisRead->Channel[nChannel].NumDim==1)
		{
			XtfWrite->XtfHead.CurveAttrib[i].CurType=2;//曲线类型(1-3)：波形
		}
		else
		{
			XtfWrite->XtfHead.CurveAttrib[i].CurType=3;//曲线类型(1-3)：阵列
		}
		XtfWrite->XtfHead.CurveAttrib[i].RepCode=DlisRepCToXtf(DlisRead->Channel[nChannel].RepCode); //数据类型(1-12)
		XtfWrite->XtfHead.CurveAttrib[i].HorType=1;   //水平类型
		XtfWrite->XtfHead.CurveAttrib[i].VerType=1;  //垂直类型
	
		int NumItem=XtfWrite->XtfHead.idimen1[i];
		if(XtfWrite->XtfHead.ndimension[i]>1)
		{
			NumItem*=XtfWrite->XtfHead.idimen2[i];
		}
		if(XtfWrite->XtfHead.ndimension[i]>2)
		{
			NumItem*=XtfWrite->XtfHead.idimen3[i];
		}
		short CodeLen=XtfWrite->XtfRepCodeToLen(XtfWrite->XtfHead.CurveAttrib[i].RepCode);
		int NumBytes=NumItem*XtfWrite->XtfHead.LevelNumber[i]*CodeLen/8;
		int NumRec=(NumBytes-1)/4096+2; //本曲线的数据记录数，含曲线头
		XtfWrite->XtfHead.StartOffset[i]=XtfWrite->XtfHead.FileRecord+1; 
		XtfWrite->XtfHead.FileRecord+=NumRec;
	}
	XtfWrite->XtfHead.firstrecord=1;     //位图中的首项记录
	XtfWrite->XtfHead.lastrecord=32576;      //位图中的最后一个记录
	XtfWrite->XtfHead.prerecord=0;       //前一个位图的记录数
	XtfWrite->XtfHead.thisrecord=2;      //当前位图记录数
	if(XtfWrite->XtfHead.FileRecord<XtfWrite->XtfHead.lastrecord)
	{	
		XtfWrite->XtfHead.bitmapfullflag=0;  //位图未满
		XtfWrite->XtfHead.nextrecord=0;      //下一个位图的记录数
		DWORD Num1=XtfWrite->XtfHead.FileRecord/8;
		DWORD Num2=XtfWrite->XtfHead.FileRecord%8;
		for(DWORD i=0;i<Num1;i++)
		{
			XtfWrite->XtfHead.Datamap[i]=0XFF;  //位图
		}
		XtfWrite->XtfHead.Datamap[Num1]=0;
		for(i=0;i<Num2;i++)
		{
			XtfWrite->XtfHead.Datamap[Num1]=(XtfWrite->XtfHead.Datamap[Num1]>>1)|0x80;
		}
	}
	else
	{	
		XtfWrite->XtfHead.bitmapfullflag=1;  //位图已满
		XtfWrite->XtfHead.nextrecord=32576;  //下一个位图的记录数
		for(int i=0;i<4072;i++)
		{
			XtfWrite->XtfHead.Datamap[i]=0XFF;
		}
	}
}

void CDLisToXtf::SetXtfCurveHead(int Index,int nChannel,int nFrame,int nCurve)
{
	CString s;
	sprintf(XtfWrite->XtfCurve.Name,"%-8.8s",NewCurveName.GetAt(Index));//曲线名，必须以字母开头
	sprintf(XtfWrite->XtfCurve.Units,"%-8.8s",DlisRead->Channel[nChannel].Units);

	sprintf(XtfWrite->XtfCurve.Comment,"%-24.24s"," "); //主曲线备注字段，即（AIF文件定义的备注）
	sprintf(XtfWrite->XtfCurve.ServiceCompany,"%-8.8s","SLC"); //公司名
	sprintf(XtfWrite->XtfCurve.ToolType,"%-8.8s",""); //仪器类型
	sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s"," ");  //曲线类型，如“WAVEFORM”、“DIPLOG”等
	if(XtfWrite->XtfHead.CurveAttrib[Index].CurType==2)
	{
		sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s","WAVEFORM");  //曲线类型，如“WAVEFORM”、“DIPLOG”等
	}
	else if(XtfWrite->XtfHead.CurveAttrib[Index].CurType==3)
	{
		sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s","ARRAY");  //曲线类型，如“WAVEFORM”、“DIPLOG”等
	}
	sprintf(XtfWrite->XtfCurve.DepthUnits,"%-8.8s",DlisRead->Frame[nFrame].IndexUnits); //深度单位
	if(XtfWrite->XtfHead.CurveAttrib[Index].CurType>1)
	{
		sprintf(XtfWrite->XtfCurve.TimeIncrementUnits,"%-8.8s",DlisRead->Channel[nChannel].AxisUnits[0]); //波型曲线的时间增量单位
		sprintf(XtfWrite->XtfCurve.StartTimeUnits,"%-8.8s",XtfWrite->XtfCurve.TimeIncrementUnits);  //起始时间单位,阵列声波有固定和可变的起始时间
	}
	else
	{
		sprintf(XtfWrite->XtfCurve.TimeIncrementUnits,"%-8.8s"," ");
		sprintf(XtfWrite->XtfCurve.StartTimeUnits,"%-8.8s"," "); 
	}
	sprintf(XtfWrite->XtfCurve.RawTapeHeaderMnemonic,"%-8.8s"," ");  //原始带带头助记符
	sprintf(XtfWrite->XtfCurve.UnitsOfStationary,"%-8.8s",XtfWrite->XtfCurve.StartTimeUnits); //静态深度单位
	sprintf(XtfWrite->XtfCurve.CorrelogramParamterUnits,"%-8.8s"," ");  //相关图参数单位
	sprintf(XtfWrite->XtfCurve.CbilOrientationMnemonic,"%-8.8s"," ");  //CBIL方位助记符
	sprintf(XtfWrite->XtfCurve.LoggingDirection,"%-8.8s","down"); //测井方向,取值为“up”、“down”等
	sprintf(XtfWrite->XtfCurve.AifCurveName,"%-24.24s"," ");//AIF曲线名和版本
	sprintf(XtfWrite->XtfCurve.OCT,"%-8.8s"," "); //创建该曲线的OCT
	sprintf(XtfWrite->XtfCurve.ToolNames,"%-48.48s"," "); //8个六字符的仪器名
	sprintf(XtfWrite->XtfCurve.strunused,"%-8.8s"," ");  //未用
	sprintf(XtfWrite->XtfCurve.strunused2,"%-8.8s"," "); //未用
	sprintf(XtfWrite->XtfCurve.LisName,"%-4.4s",XtfWrite->XtfCurve.Name);  //LIS曲线名
	sprintf(XtfWrite->XtfCurve.strunused3,"%-4.4s"," "); //未用
	sprintf(XtfWrite->XtfCurve.CreationRoutineName,"%-16.16s"," "); //十六字符的创建例程和版本
	sprintf(XtfWrite->XtfCurve.ModifiedRoutineName,"%-16.16s",XtfWrite->XtfCurve.CreationRoutineName); //十六字符的修改例程和版本
	sprintf(XtfWrite->XtfCurve.strunused4,"%-28.28s"," "); //未用
	sprintf(XtfWrite->XtfCurve.LongName,"%-20.20s"," ");  //加长了的曲线名
	sprintf(XtfWrite->XtfCurve.strunused5,"%-712.712s"," ");  //未用

	XtfWrite->XtfCurve.deptop=XtfWrite->XtfHead.Sdep[Index];
	XtfWrite->XtfCurve.rlevcv=XtfWrite->XtfHead.Level[Index];
	XtfWrite->XtfCurve.depbot=XtfWrite->XtfHead.Edep[Index];
	XtfWrite->XtfCurve.curvmin=0; //曲线最小值
	XtfWrite->XtfCurve.curvmax=0; //曲线最大值
	XtfWrite->XtfCurve.curvavg=0; //曲线平均值
	if(XtfWrite->XtfHead.CurveAttrib[Index].CurType>1)
	{
		XtfWrite->XtfCurve.timeinc=DlisRead->Channel[nChannel].AxisStep[0];; //时间增量（采样间隔）
		XtfWrite->XtfCurve.starttm=DlisRead->Channel[nChannel].AxisStart[0]; //阵列声波的起始时间
	}
	else
	{
		XtfWrite->XtfCurve.timeinc=0;
		XtfWrite->XtfCurve.starttm=0;
	}
	XtfWrite->XtfCurve.stadepth=0; //静态测量深度
	XtfWrite->XtfCurve.stddev=0;  //标准方差
	XtfWrite->XtfCurve.cpwinbeg=0; //相关图参数（Window begin）
	XtfWrite->XtfCurve.cpwinstp=0; //相关图参数（Window step）
	XtfWrite->XtfCurve.cpwinlen=0; //相关图参数（Window length）
	XtfWrite->XtfCurve.cbilodeg=0; //CBIL方位的度数
	XtfWrite->XtfCurve.IntervalTR=0;  //发射器到接收器的距离
	XtfWrite->XtfCurve.IntervalRR=0;  //接收器到接收器的距离
	XtfWrite->XtfCurve.RawOffset=0;   //原始数据偏转
	XtfWrite->XtfCurve.TimeDelay=0;   //时间延迟
	XtfWrite->XtfCurve.Angle=0;       //参考臂到带传感臂的平角
	XtfWrite->XtfCurve.XPad=0;    //极板上传感器的X位置
	XtfWrite->XtfCurve.YPad=0;    //极板上传感器的Y位置
	for(int i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.LossMinor[i]=0; //次级遗失数据值
	}
	for(i=0;i<101;i++)
	{
		XtfWrite->XtfCurve.funused[i]=0; //未用
	}

	XtfWrite->XtfCurve.dblinit=-9999.0;  //双精度初始值
	XtfWrite->XtfCurve.dbtopdep=XtfWrite->XtfCurve.deptop; //双精度顶部深度
	XtfWrite->XtfCurve.dbbotdep=XtfWrite->XtfCurve.depbot; //双精度底部深度
	XtfWrite->XtfCurve.dbrlevel=XtfWrite->XtfCurve.rlevcv; //双精度采样间隔
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.smvalsdb[i]=0; //Array pf 6 secondary Missing Datum Value
	}
	XtfWrite->XtfCurve.idxtop2=0; //次级采样索引头部
	XtfWrite->XtfCurve.idxbot2=0; //次级采样索引底部
	XtfWrite->XtfCurve.idxrlev2=0; //次级极样索引间距
	for(i=0;i<51;i++)
	{
		XtfWrite->XtfCurve.dunused[i]=0; //未用
	}

	CTime t=CTime::GetCurrentTime(); //取当前系统日期和时间
	int iyear=t.GetYear();
	int imonth=t.GetMonth();
	int iday=t.GetDay();
	int ihour=t.GetHour();
	int iminute=t.GetMinute();
	int isecond=t.GetSecond();

	XtfWrite->XtfCurve.EncodedCreationDate=(iyear<<16)|(imonth<<8)|iday;  //创建日期编码
	XtfWrite->XtfCurve.EncodedCreationTime=(ihour<<24)|(iminute<<16)|isecond;  //创建时间编码
	XtfWrite->XtfCurve.EncodedLastAccessDate=XtfWrite->XtfCurve.EncodedCreationDate;  //最后存取日期编码
	XtfWrite->XtfCurve.EncodedLastAccessTime=XtfWrite->XtfCurve.EncodedCreationTime;  //最后存取时间编码

	XtfWrite->XtfCurve.NumberOfCurveHeaderrecords=1;  //曲线头记录数
	//曲线最后一个数据记录数在后面计算

	XtfWrite->XtfCurve.DatumInitializationVariable=-9999;  //数据初始化变量
	XtfWrite->XtfCurve.NumberOfLevelsForTheCurve=XtfWrite->XtfHead.LevelNumber[Index];  //曲线的采样数
	XtfWrite->XtfCurve.CurrentEncodedDateSignature=0;  //当前编码日期标记
	XtfWrite->XtfCurve.CurrentEncodedTimeSignature=0;  //当前编码日期标记
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.LossMinor2[i]=0; //6个次级遗失数据值的阵列
	}
	for(i=0;i<2;i++)
	{
		XtfWrite->XtfCurve.Arrays2[i]=0;  //两值阵列，代表结构化曲线定义的首末记录
		XtfWrite->XtfCurve.Arrays3[i]=0;  //两值阵列，代表结构化曲线助记名的首末记录
	}
	for(i=0;i<108;i++)
	{
		XtfWrite->XtfCurve.iunused[i]=0;  //未用
	}
	XtfWrite->XtfCurve.SurvlibMajor=4;  //SURVLIB主要版
	XtfWrite->XtfCurve.SurvlibMinor=14; //SURVLIB次要版
	XtfWrite->XtfCurve.Ihcurv=Index+1;  //创建时的曲线句柄
	XtfWrite->XtfCurve.Ictype=XtfWrite->XtfHead.CurveAttrib[Index].CurType;  //曲线类型:1-常规,2-波形,3-阵列
	XtfWrite->XtfCurve.RepCode=XtfWrite->XtfHead.CurveAttrib[Index].RepCode;
	XtfWrite->XtfCurve.Ivtype=1;  //垂直索引类型
	XtfWrite->XtfCurve.Ihtype=1;  //水平索引类型
	XtfWrite->XtfCurve.Ndims=XtfWrite->XtfHead.ndimension[Index];   //曲线的维数
	//设置第i维的单元数
	XtfWrite->XtfCurve.Idims[0]=XtfWrite->XtfHead.idimen1[Index];
	XtfWrite->XtfCurve.Idims[1]=XtfWrite->XtfHead.idimen2[Index];
	XtfWrite->XtfCurve.Idims[2]=XtfWrite->XtfHead.idimen3[Index];
	for(i=XtfWrite->XtfCurve.Ndims;i<7;i++)
	{
		XtfWrite->XtfCurve.Idims[i]=0;  //第i维的单元数
	}
	XtfWrite->XtfCurve.I2init=-32767;  //INT*2变量的初始值
	XtfWrite->XtfCurve.Intflfix=XtfWrite->XtfRepCodeToIntFlFix(XtfWrite->XtfCurve.RepCode);
	XtfWrite->XtfCurve.Isign=XtfWrite->XtfRepCodeToSign(XtfWrite->XtfCurve.RepCode); //无符号标记
	XtfWrite->XtfCurve.Numbits=XtfWrite->XtfRepCodeToLen(XtfWrite->XtfCurve.RepCode);
	XtfWrite->XtfCurve.Lisrepcd=0; //若取正值，该曲线以原始LIS编码形式
	XtfWrite->XtfCurve.Lisnsamp=0;  //Nsamp与此同时Lisrepcd有关
	XtfWrite->XtfCurve.Lisrcsiz=0;  //LIS代码大小（字节）
	XtfWrite->XtfCurve.Lisnsize=0;  //每个样点LIS代码总节节数
	XtfWrite->XtfCurve.Istartfv=0;  //0-固定的声波起始时间,1-可变的声波起始时间
	XtfWrite->XtfCurve.Igainfv=0;   //若为0，阵列声波的增益固定
	XtfWrite->XtfCurve.Igainepx=0;  //具有固定增益的阵列或曲线的增益指数
	XtfWrite->XtfCurve.Igainmth=0;  //具有增益的阵列或曲线的增益表示法-固定或可变
	XtfWrite->XtfCurve.Igainapp=0;  //增益应用标志
	XtfWrite->XtfCurve.Lntaphed=0;  //RAWTPHD的长度
	XtfWrite->XtfCurve.Ntools=0;    //仪器号
	XtfWrite->XtfCurve.Idxingrp=0;  //曲线索引
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.Smvalsi2[i]=0;  //6个次级遗失数据值的阵列
	}
	for(i=0;i<219;i++)
	{
		XtfWrite->XtfCurve.hunused[i]=0;  //未用
	}
	for(i=0;i<24;i++)
	{
		XtfWrite->XtfCurve.TransmitterID[i]=0;  //24字节代表24个发射器ID场
		XtfWrite->XtfCurve.ReceiverID[i]=0;  //24字节代表24个接收器ID场
	}
	for(i=0;i<276;i++)
	{
		XtfWrite->XtfCurve.RawTapeHeaderInfo[i]=0;  //原始带头信息
	}
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.LossMinor3[i]=0;  //6字节代表二遗失数值码阵列
		XtfWrite->XtfCurve.LossMinor4[i]=0;  //6字节代表二遗失数值码阵列
	}
	for(i=0;i<687;i++)
	{
		XtfWrite->XtfCurve.bunused[i]=0;   //未用
	}
	XtfWrite->XtfCurve.SystemCode=5;  //系统代码:1-PC,2-PE,3-VAX,4-IBM Mainframe,5-hp9000/Sun,etc.

	DWORD BlockSize=XtfWrite->XtfHead.LevelNumber[Index]*XtfWrite->XtfCurve.Idims[0]*XtfWrite->XtfCurve.Numbits/8;
	DWORD NumRecord=(BlockSize-1)/4096+2; //含有一个曲线头
	XtfWrite->XtfCurve.RecordNumberOfLastCurvedataRecord=XtfWrite->XtfHead.StartOffset[Index]+NumRecord-1;
}
