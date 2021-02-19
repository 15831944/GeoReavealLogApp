// Slc716ToXtf.cpp: implementation of the CSlc716ToXtf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Slc716ToXtf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlc716ToXtf::CSlc716ToXtf()
{
	XtfWrite=NULL;
}

CSlc716ToXtf::~CSlc716ToXtf()
{

}
bool CSlc716ToXtf::Slc716ToXtf(CString SourceFile,CString ObjectPath,int nCurvesRadio,FILE *fo)
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
	
	Slc716R=new CSlc716R;
	XtfWrite=new CXtfWrite;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	bool bResult=Transform();

	//输出转换结果信息
	if(bResult)
	{
		XtfWrite->PrintHead(fp);
	}

	delete XtfWrite; XtfWrite=NULL;
	delete Slc716R; Slc716R=NULL;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	delete pGuage;  //删除进度条
	return true;
}
bool CSlc716ToXtf::Transform()
{
	CString s;
	if(!IsSLC716(m_SourceFile))
	{
		fprintf(fp,"%s is not SLC716 Format\n\n",m_SourceFile);
		return false;
	}
	//根据输出目录及输入文件名确定输出文件名
	XtfFileName.Format ("%s\\%s.xtf",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	XtfFileName.MakeLower();
	ModiFileName(XtfFileName); //根据条件决定是否修改文件名

	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)XtfFileName);

	//////////////
	if(!Slc716R->Open(m_SourceFile))
	{
		return false;
	}
	Slc716R->ReadHead(); //读SLC716头
	if(Slc716R->Slc716Head.NumLog<1)
	{
		Slc716R->Close();
		return false;
	}
	XtfWrite->Open(XtfFileName);
	GetXtfCurveIndexFromSlc716();
	XtfWrite->InitXtfHeader();
	SetXtfHeaderFromSlc716();
	XtfWrite->WriteXtfHeader();
	CString sCurveName;
	///////////////////////////
	pGuage->SetRange(0,CurveIndex.GetSize());
	int nBufLen=Slc716R->NumBlock*64;
	float *buf=new float[nBufLen];
	for(int i=0;i<nBufLen;i++) buf[i]=-999.25;
	for(int index=0;index<CurveIndex.GetSize();index++)
	{
		pGuage->SetPos(index+1);  //设置进度条当前值
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int k=CurveIndex.GetAt(index);
		SetXtfCurveFromSlc716Head(k,index); //设置通道参数
		XtfWrite->WriteCurveHeader(index); //写通道信息到XTF文件
		sCurveName.Format("%-4.4s",Slc716R->Slc716Head.CurveName[k]);
		sCurveName.TrimRight();
		Slc716R->ReadChannel(sCurveName,buf);
		XtfWrite->WriteGenData(Slc716R->NumPoint,buf);
	}
	delete []buf; buf=NULL;
	XtfWrite->Close();
	Slc716R->Close();
	////////////////////////////////
	return !sp_Break_Status;
}
void CSlc716ToXtf::GetXtfCurveIndexFromSlc716()
{
	CString s,sCurveName;
	if(Slc716R->Slc716Head.NumLog<1)
	{
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return;
	}
	CurveIndex.RemoveAll();
	XtfWrite->XtfHead.isnumcv=0;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	if(m_nCurvesRadio<3)
	{
		for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
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
			NewUnits.Add("");
		}
	}
	if(m_nCurvesRadio==3)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=8; //曲线名最大长度
		ReCurNamDlg.m_nMaxLog=512; //XTF文件允许最多曲线数
		if(m_nCurvesRadio==3)
		{
			ReCurNamDlg.m_NumLog=Slc716R->Slc716Head.NumLog; //实际曲线数
			for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
			{
				sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
				sCurveName.TrimLeft();
				sCurveName.TrimRight();
				ReCurNamDlg.m_SourceName.Add(sCurveName);
				ReCurNamDlg.m_ObjectName.Add(sCurveName);
				ReCurNamDlg.m_Units.Add("");
				ReCurNamDlg.m_NumDim.Add(1);
			}
		}
		else
		{
			ReCurNamDlg.m_NumLog=SelCurveName.GetSize(); //实际曲线数
			for(int i=0;i<SelCurveName.GetSize();i++)
			{
				ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
				ReCurNamDlg.m_ObjectName.Add(SelCurveName.GetAt(i));
				ReCurNamDlg.m_NumDim.Add(1);
				ReCurNamDlg.m_Units.Add("");
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
int CSlc716ToXtf::GetCurveIndex(CString SelCurve)
{
	SelCurve.TrimRight();
	int index=-1;
	CString sCurveName;
	for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
	{
		sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
		sCurveName.TrimLeft();
		sCurveName.TrimRight();
		if(sCurveName.CompareNoCase(SelCurve)==0)
		{
			index=i;
			break;
		}
	}
	return index;
}
void CSlc716ToXtf::SetXtfHeaderFromSlc716()
{
	CString s;
	//静态信息,格式为“%-79.79s”的目的是保证sprintf不会影响下一个变量的值
	char str[256];
	sprintf(str,"%-80.80s",Slc716R->Slc716Head.WellName);
	memcpy(XtfWrite->XtfHead.Wellname,str,80);
	memset(XtfWrite->XtfHead.Fieldname,' ',80);
	sprintf(str,"%-80.80s",Slc716R->Slc716Head.CompanyName);
	memcpy(XtfWrite->XtfHead.Companyname,str,80);
	memset(XtfWrite->XtfHead.Welllocation,' ',40);
	XtfWrite->XtfHead.Metricflag=0; //公制单位
	XtfWrite->XtfHead.FileRecord=8;  //文件记录数(最小值)
	XtfWrite->XtfHead.isnumcv=CurveIndex.GetSize();  //曲线条数
	if(XtfWrite->XtfHead.isnumcv==0)
	{
		return ;
	}
	int k=CurveIndex.GetAt(0);
	XtfWrite->XtfHead.wttop=0;  //曲线的顶部深度
	XtfWrite->XtfHead.wtbottom=0;  //曲线的底部深度
	XtfWrite->XtfHead.wtlevel=0;  //曲线的采样间隔
	int NumDepCurve=0; //深度采样曲线数
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		k=CurveIndex.GetAt(i);
		char str[256];
		sprintf(str,"%-8.8s",(const char*)(NewCurveName.GetAt(i)));
		memcpy(XtfWrite->XtfHead.CurveName[i],str,8);
		sprintf(str,"%-8.8s",(const char*)(NewUnits.GetAt(i)));
		memcpy(XtfWrite->XtfHead.Units[i],str,8);
		XtfWrite->XtfHead.Sdep[i]=Slc716R->Slc716Head.Stdep;
		XtfWrite->XtfHead.Edep[i]=Slc716R->Slc716Head.Endep;
		XtfWrite->XtfHead.Level[i]=Slc716R->Slc716Head.Rlev;
		XtfWrite->XtfHead.LevelNumber[i]=Slc716R->NumPoint;
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
		XtfWrite->XtfHead.ndimension[i]=1; //曲线的维数，取值为1-3
		XtfWrite->XtfHead.idimen1[i]=1; //曲线第一维的单元数
		XtfWrite->XtfHead.idimen2[i]=0;    //曲线第二维的单元数
		XtfWrite->XtfHead.idimen3[i]=0;    //曲线第三维的单元数

		XtfWrite->XtfHead.CurveAttrib[i].CurType=1;//曲线类型(1-3)：常规、波形、阵列
		XtfWrite->XtfHead.CurveAttrib[i].RepCode=XTF_REPR_FLOAT; //数据类型(1-12)
		XtfWrite->XtfHead.CurveAttrib[i].HorType=1;   //水平类型
		XtfWrite->XtfHead.CurveAttrib[i].VerType=1;  //垂直类型
		DWORD NumItem=XtfWrite->XtfHead.idimen1[i];
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
void CSlc716ToXtf::SetXtfCurveFromSlc716Head(int IndexFid,int IndexXtf)
{
	CString s;
	sprintf(XtfWrite->XtfCurve.Name,"%-8.8s",NewCurveName.GetAt(IndexXtf));//曲线名，必须以字母开头
	if(NewUnits.GetAt(IndexXtf)=="none")
	{
		memset(XtfWrite->XtfCurve.Units,' ',8);
	}
	else
	{
		sprintf(XtfWrite->XtfCurve.Units,"%-8.8s",NewUnits.GetAt(IndexXtf));
	}
	sprintf(XtfWrite->XtfCurve.Comment,"%-24.24s"," "); //主曲线备注字段，即（AIF文件定义的备注）
	sprintf(XtfWrite->XtfCurve.ServiceCompany,"%-8.8s","SLC"); //公司名
	sprintf(XtfWrite->XtfCurve.ToolType,"%-8.8s",""); //仪器类型
	sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s"," ");  //曲线分类类型，如“WAVEFORM”、“DIPLOG”等
	sprintf(XtfWrite->XtfCurve.DepthUnits,"%-8.8s","m");  //深度单位
	sprintf(XtfWrite->XtfCurve.DepthUnits,"%-8.8s","meters");  //深度单位
	sprintf(XtfWrite->XtfCurve.TimeIncrementUnits,"%-8.8s"," ");
	sprintf(XtfWrite->XtfCurve.StartTimeUnits,"%-8.8s"," "); 
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

	XtfWrite->XtfCurve.deptop=Slc716R->Slc716Head.Stdep;  //曲线的顶部深度
	XtfWrite->XtfCurve.depbot=Slc716R->Slc716Head.Endep;  //曲线的底部深度
	XtfWrite->XtfCurve.rlevcv=Slc716R->Slc716Head.Rlev;  //曲线的采样间隔
	XtfWrite->XtfCurve.curvmin=0; //曲线最小值
	XtfWrite->XtfCurve.curvmax=0; //曲线最大值
	XtfWrite->XtfCurve.curvavg=0; //曲线平均值
	XtfWrite->XtfCurve.timeinc=0;
	XtfWrite->XtfCurve.starttm=0;
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
	XtfWrite->XtfCurve.NumberOfLevelsForTheCurve=XtfWrite->XtfHead.LevelNumber[IndexXtf];  //曲线的采样数
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
	XtfWrite->XtfCurve.Ihcurv=IndexXtf+1;  //创建时的曲线句柄
	XtfWrite->XtfCurve.Ictype=1;  //曲线类型:1-常规,2-波形,3-阵列
	XtfWrite->XtfCurve.RepCode=XTF_REPR_FLOAT;
	XtfWrite->XtfCurve.Ivtype=1;  //垂直索引类型
	XtfWrite->XtfCurve.Ihtype=1;  //水平索引类型
	XtfWrite->XtfCurve.Ndims=1;   //曲线的维数
	//设置第i维的单元数
	XtfWrite->XtfCurve.Idims[0]=1;
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

	DWORD BlockSize=XtfWrite->XtfHead.LevelNumber[IndexXtf]*XtfWrite->XtfCurve.Idims[0]*XtfWrite->XtfCurve.Numbits/8;
	DWORD NumRecord=(BlockSize-1)/4096+2; //含有一个曲线头
	XtfWrite->XtfCurve.RecordNumberOfLastCurvedataRecord=XtfWrite->XtfHead.StartOffset[IndexXtf]+NumRecord-1;
}