// XtfWrite.cpp: implementation of the CXtfWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XtfWrite.h"
#include "XtfRead.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXtfWrite::CXtfWrite()
{

}

CXtfWrite::~CXtfWrite()
{

}
bool CXtfWrite::Open(CString sDataFile)
{
	sXtfFile=sDataFile;
	if(fXtf.Open(sDataFile,CFile::modeCreate|CFile::modeReadWrite)==FALSE)
	{
		return false;
	}
	return true;
}
void CXtfWrite::Close()
{
	fXtf.Close();
}
void CXtfWrite::WriteXtfHeader()
{
	XTF_HEAD TmpHead;
	TmpHead=XtfHead; //保留头信息
	PcXtfHead2Sun(); //内码转换
	WriteHeader(); //头信息写盘
	XtfHead=TmpHead; //恢复头信息
	fXtf.SetLength(XtfHead.FileRecord*4096);
}
void CXtfWrite::InitXtfHeader()
{
	for(int i=0;i<8;i++) XtfHead.header_location[i]=i+1;//文件头记录的位置
	for(i=0;i<182;i++)XtfHead.iunused[i]=0; //未用
	strcpy(XtfHead.extension,".xtf");  //扩展名，一般为“.XTF”
	if(sXtfFile.GetLength()>80)
	{	//文件名太长，则取不含路径的文件名
		sprintf(XtfHead.FileFullName,"%-80.80s",MyGetFileName(sXtfFile)); //文件全名，含路径在内
	}
	else
	{
		sprintf(XtfHead.FileFullName,"%-80.80s",sXtfFile); //文件全名，含路径在内
	}
	strcpy(XtfHead.Depth_Unit,"meters  "); //深度单位
	for(i=0;i<8;i++)XtfHead.AIF_Revision_numbar[i]=32; //AIF获得版本号
	for(i=0;i<88;i++)XtfHead.strunused[i]=0;//未用
	XtfHead.System_Code=5;  //系统代码，1=PC，2=PE，3=VAX，4=IBM，5=上HP9000/SUN
	XtfHead.QC=0; //用于指示目标系统在进程中有格式转换
	XtfHead.Major_Revision_Number=4; //SURVLIB 主版本号
	XtfHead.Minor_Revision_Number=14; //SURVLIB 副版本号
	for(i=0;i<22;i++)XtfHead.hunused[i]=0; //未用
	XtfHead.isnumcv=0;   //曲线条数
	XtfHead.ismaxcv=512; //最多曲线数,一般为512
	for(i=0;i<5;i++) XtfHead.iunused2[i]=0;//未用
	for(i=0;i<64;i++)//数据记录类型及其起止记录
	{
		for(int j=0;j<8;j++)XtfHead.UserData[i].Name[j]=0;
		XtfHead.UserData[i].StartRecord=0;
		XtfHead.UserData[i].StopRecord=0;
	}
	XtfHead.wttop=0.0f;     //缺省起始深度
	XtfHead.wtbottom=0.0f;  //缺省结束深度
	XtfHead.wtlevel=0.0;    //缺省采样间隔
	for(i=0;i<1012;i++) XtfHead.strunused2[i]=0;//未用
	for(i=0;i<1020;i++) XtfHead.strunused3[i]=0;//未用
	XtfHead.FileRecord=8; //文件大小,以块(4096字节为1块)数表示
	//第二个头记录
	XtfHead.Datamap[0]=0xFF;  //第一位图，反映文件头的记录
	for(i=1;i<4072;i++) XtfHead.Datamap[i]=0X00; //位图，每一位表示4096字节
	XtfHead.bitmapfullflag=0; //位图满载标记
	XtfHead.firstrecord=0x1;     //位图中的首项记录
	XtfHead.lastrecord=0x7f40;      //位图中的最后一个记录
	XtfHead.prerecord=0;       //前一个位图的记录数
	XtfHead.thisrecord=0x2;      //当前位图记录数
	XtfHead.nextrecord=0;      //下一个位图的记录数
	//第三个头记录
	for(i=0;i<512;i++)
	{
		sprintf(XtfHead.CurveName[i],"%-8.8s",""); //曲线名
	}
	//第四记录
	for(i=0;i<512;i++)
	{
		XtfHead.StartOffset[i]=0; //每条曲线的起始记录号，从1开始计数
		XtfHead.LevelNumber[i]=0; //每条曲线的采样数
	}
	//第五记录
	for(i=0;i<512;i++)
	{
		XtfHead.ndimension[i]=0; //曲线的维数，取值为1-3
		XtfHead.idimen1[i]=0;    //曲线第一维的单元数
		XtfHead.idimen2[i]=0;    //曲线第二维的单元数
		XtfHead.idimen3[i]=0;    //曲线第三维的单元数
	}
	//第六记录
	for(i=0;i<512;i++)
	{
		XtfHead.Sdep[i]=0.0f; //曲线的起始深度
		XtfHead.Edep[i]=0.0f;   //曲线的结束深度
	}
	//第七记录
	for(i=0;i<512;i++)
	{
		XtfHead.Level[i]=0.0f;    //曲线采样间隔
		XtfHead.CurveAttrib[i].CurType=0; //曲线类型
		XtfHead.CurveAttrib[i].RepCode=0; //数据类型
		XtfHead.CurveAttrib[i].HorType=0; //水平类型
		XtfHead.CurveAttrib[i].VerType=0; //垂直类型
	}
	//第八个记录
	//第一块：ASCII
	sprintf(XtfHead.Wellsiteinfo,"%-8.8s","WSI"); //“WSI”标记，表示一个表效的井场信息记录
	sprintf(XtfHead.Wellname,"%-80.80s",""); //井名
	sprintf(XtfHead.Fieldname,"%-80.80s","SiChuan Petroleum Management Office"); //油田名称
	sprintf(XtfHead.Companyname,"%-80.80s","SiChuan Petroleum Logging Corporation,CHONG QING,400021"); //公司名
	sprintf(XtfHead.Countryname,"%-32.32s","CHINA"); //国家名
	sprintf(XtfHead.Countyname,"%-32.32s",""); //县名
	sprintf(XtfHead.Statename,"%-16.16s",""); //州名
	sprintf(XtfHead.OCTname,"%-8.8s",""); //操作控制表名
	sprintf(XtfHead.Welllocation,"%-40.40s",""); //井场位置
	sprintf(XtfHead.Section,"%-4.4s",""); //剖面名
	sprintf(XtfHead.Township,"%-4.4s",""); //城镇名
	sprintf(XtfHead.Range,"%-4.4s",""); //范围
	sprintf(XtfHead.Permanentdatum,"%-2.2s","GL"); //永久数据,“gl”或“sl”
	sprintf(XtfHead.Drillingmeasurementdatum,"%-2.2s","KB"); //钻井测量数据(“kb”、“gl”等)
	sprintf(XtfHead.LogAnalysisCenter,"%-4.4s","SLC"); //测井分析中心代号
	sprintf(XtfHead.Wellunits,"%-8.8s","meters"); //井场海拨单位
	for(i=0;i<620;i++)XtfHead.strunused4[i]=' '; //未用
	//第二块：浮点
	XtfHead.Latitude=-9999;   //经度
	XtfHead.Longitude=-9999;  //纬度
	XtfHead.Kellybushing=0;  //方钻补心海拨高度
	XtfHead.Derrickfloor=0;  //井架平台海拨高度
	XtfHead.Elevation=0;    //海拨永久数据
	XtfHead.MagneticDeclination=-9999;  //磁偏角
	XtfHead.ECC=0.0f;   //ECC号
	XtfHead.Groundtosealevelelevation=0; //地面海拨高度
	for(i=0;i<120;i++)XtfHead.funused[i]=0; //未用
	for(i=0;i<64;i++)XtfHead.dunused[i]=0; //未用
	//第三块：整型
	for(i=0;i<6;i++)XtfHead.hunused2[i]=0; //未用
	XtfHead.Metricflag=0;  //公制标记
	XtfHead.Maximumcurveversion=0; //最大曲线类型(1-99)
	for(i=0;i<504;i++)XtfHead.hunused3[i]=0; //未用
	//第四块：二进制
	for(i=0;i<1024;i++)XtfHead.unformattedbinaryinformation[i]=0; //未格式化二进制信息
}
void CXtfWrite::PcXtfHead2Sun()
{
	//第一记录需要转换的信息
	MSunI4TOPC(8,XtfHead.header_location);
	SunI4TOPC(XtfHead.isnumcv);
	SunI4TOPC(XtfHead.ismaxcv);
	for(int i=0;i<64;i++)
	{
		SunI4TOPC(XtfHead.UserData[i].StartRecord);
		SunI4TOPC(XtfHead.UserData[i].StopRecord);
	}
	SunR4TOPC(XtfHead.wttop);
	SunR4TOPC(XtfHead.wtbottom);
	SunR4TOPC(XtfHead.wtlevel);
	SunI4TOPC(XtfHead.FileRecord);

	//第二记录需要转换的信息
	SunI4TOPC(XtfHead.bitmapfullflag);
	SunI4TOPC(XtfHead.firstrecord);
	SunI4TOPC(XtfHead.lastrecord);
	SunI4TOPC(XtfHead.prerecord);
	SunI4TOPC(XtfHead.thisrecord);
	SunI4TOPC(XtfHead.nextrecord);

	//第三记录为曲线名，不必转换

	//第四记录需要转换的信息
	MSunI4TOPC(512,XtfHead.StartOffset);
	MSunI4TOPC(512,XtfHead.LevelNumber);

	//第五记录需要转换的信息
	MSunI2TOPC(512,XtfHead.ndimension);
	MSunI2TOPC(512,XtfHead.idimen1);
	MSunI2TOPC(512,XtfHead.idimen2);
	MSunI2TOPC(512,XtfHead.idimen3);

	//第六记录需要转换的信息
	MSunR4TOPC(512,XtfHead.Sdep);
	MSunR4TOPC(512,XtfHead.Edep);

	//第七记录需要转换的信息
	MSunR4TOPC(512,XtfHead.Level);

	//第八记录需要转换的信息
	SunR4TOPC(XtfHead.Latitude);
	SunR4TOPC(XtfHead.Longitude);
	SunR4TOPC(XtfHead.Kellybushing);
	SunR4TOPC(XtfHead.Derrickfloor);
	SunR4TOPC(XtfHead.Elevation);
	SunR4TOPC(XtfHead.MagneticDeclination);
	SunR4TOPC(XtfHead.ECC);
	SunR4TOPC(XtfHead.Groundtosealevelelevation);
	SunI2TOPC(XtfHead.Metricflag);
	SunI2TOPC(XtfHead.Maximumcurveversion);
}

void CXtfWrite::WriteHeader()
{
	//第一记录
	fXtf.SeekToBegin();
	fXtf.Write(XtfHead.header_location,32);
	fXtf.Write(XtfHead.iunused,182*4);
	fXtf.Write(XtfHead.extension,4);
	fXtf.Write(XtfHead.FileFullName,80);
	fXtf.Write(XtfHead.Depth_Unit,8);
	fXtf.Write(XtfHead.AIF_Revision_numbar,8);
	fXtf.Write(XtfHead.strunused,88);
	fXtf.Write(&XtfHead.System_Code,1);
	fXtf.Write(&XtfHead.QC,1);
	fXtf.Write(&XtfHead.Major_Revision_Number,1);
	fXtf.Write(&XtfHead.Minor_Revision_Number,1);
	fXtf.Write(XtfHead.hunused,44);
	fXtf.Write(&XtfHead.isnumcv,4);
	fXtf.Write(&XtfHead.ismaxcv,4);
	fXtf.Write(XtfHead.iunused2,20);
	fXtf.Write(XtfHead.UserData,64*16);
	fXtf.Write(&XtfHead.wttop,4);
	fXtf.Write(&XtfHead.wtbottom,4);
	fXtf.Write(&XtfHead.wtlevel,4);
	fXtf.Write(XtfHead.strunused2,1012);
	fXtf.Write(XtfHead.strunused3,1020);
	fXtf.Write(&XtfHead.FileRecord,4);

	//第二记录
	fXtf.Write(XtfHead.Datamap,4072);
	fXtf.Write(&XtfHead.bitmapfullflag,4);
	fXtf.Write(&XtfHead.firstrecord,4);
	fXtf.Write(&XtfHead.lastrecord,4);
	fXtf.Write(&XtfHead.prerecord,4);
	fXtf.Write(&XtfHead.thisrecord,4);
	fXtf.Write(&XtfHead.nextrecord,4);

	//第三记录
	fXtf.Write(XtfHead.CurveName,4096);
	
	//第四记录
	for(int i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.StartOffset[i],sizeof(int));
		fXtf.Write(&XtfHead.StartOffset[256+i],sizeof(int));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.LevelNumber[i],sizeof(int));
		fXtf.Write(&XtfHead.LevelNumber[256+i],sizeof(int));
	}

	//第五记录
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.ndimension[i],sizeof(short));
		fXtf.Write(&XtfHead.idimen1[i],sizeof(short));
		fXtf.Write(&XtfHead.ndimension[256+i],sizeof(short));
		fXtf.Write(&XtfHead.idimen1[256+i],sizeof(short));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.idimen2[i],sizeof(short));
		fXtf.Write(&XtfHead.idimen3[i],sizeof(short));
		fXtf.Write(&XtfHead.idimen2[256+i],sizeof(short));
		fXtf.Write(&XtfHead.idimen3[256+i],sizeof(short));
	}

	//第六记录
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.Sdep[i],sizeof(float));
		fXtf.Write(&XtfHead.Sdep[i+256],sizeof(float));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.Edep[i],sizeof(float));
		fXtf.Write(&XtfHead.Edep[i+256],sizeof(float));
	}

	//第七记录
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.Level[i],sizeof(float));
		fXtf.Write(&XtfHead.Level[256+i],sizeof(float));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.CurveAttrib[i],sizeof(XtfHead.CurveAttrib[i]));
		fXtf.Write(&XtfHead.CurveAttrib[256+i],sizeof(XtfHead.CurveAttrib[256+i]));
	}

	//第八记录
	fXtf.Write(XtfHead.Wellsiteinfo,8); //“WSI”标记，表示一个表效的井场信息记录
	fXtf.Write(XtfHead.Wellname,80);     //井名
	fXtf.Write(XtfHead.Fieldname,80);    //油田名称
	fXtf.Write(XtfHead.Companyname,80);  //公司名
	fXtf.Write(XtfHead.Countryname,32);  //国家名
	fXtf.Write(XtfHead.Countyname,32);   //县名
	fXtf.Write(XtfHead.Statename,16);    //州名
	fXtf.Write(XtfHead.OCTname,8);       //操作控制表名
	fXtf.Write(XtfHead.Welllocation,40); //井场位置
	fXtf.Write(XtfHead.Section,4);       //剖面名
	fXtf.Write(XtfHead.Township,4);      //城镇名
	fXtf.Write(XtfHead.Range,4);         //范围
	fXtf.Write(XtfHead.Permanentdatum,2); //永久数据,“gl”或“sl”
	fXtf.Write(XtfHead.Drillingmeasurementdatum,2);  //钻井测量数据(“kb”、“gl”等)
	fXtf.Write(XtfHead.LogAnalysisCenter,4);  //测井分析中心代号
	fXtf.Write(XtfHead.Wellunits,8);          //井场海拨单位
	fXtf.Write(XtfHead.strunused4,620);    //未用

	fXtf.Write(&XtfHead.Latitude,4);   //经度
	fXtf.Write(&XtfHead.Longitude,4);  //纬度
	fXtf.Write(&XtfHead.Kellybushing,4);  //方钻补心海拨高度
	fXtf.Write(&XtfHead.Derrickfloor,4);  //井架平台海拨高度
	fXtf.Write(&XtfHead.Elevation,4);    //海拨永久数据
	fXtf.Write(&XtfHead.MagneticDeclination,4);  //磁偏角
	fXtf.Write(&XtfHead.ECC,4);   //ECC号
	fXtf.Write(&XtfHead.Groundtosealevelelevation,4); //地面海拨高度
	fXtf.Write(XtfHead.funused,120*sizeof(float));   //未用单精度数
	fXtf.Write(XtfHead.dunused,64*sizeof(double));   //未用双精度数

	fXtf.Write(XtfHead.hunused2,6*sizeof(short));  //未用
	fXtf.Write(&XtfHead.Metricflag,sizeof(short));  //公制标记
	fXtf.Write(&XtfHead.Maximumcurveversion,sizeof(short)); //最大曲线类型(1-99)
	fXtf.Write(XtfHead.hunused3,504*sizeof(short));  //未用
	fXtf.Write(XtfHead.unformattedbinaryinformation,1024); //未格式化二进制信息
}

void CXtfWrite::WriteCurveHeader(int index)
{
	Index=index;
	CURVE_HEAD TmpCurveHead;
	TmpCurveHead=XtfCurve; //保留头信息
	DWORD pos=(XtfHead.StartOffset[index]-1)*4096;
	PcCurveHead2Sun(); //内码转换
	fXtf.Seek(pos,CFile::begin);
	fXtf.Write(&XtfCurve,sizeof(XtfCurve));
	XtfCurve=TmpCurveHead; //恢复头信息
}
void CXtfWrite::PcCurveHead2Sun()
{
	SunR4TOPC(XtfCurve.deptop);  //曲线的顶部深度
	SunR4TOPC(XtfCurve.depbot);  //曲线的底部深度
	SunR4TOPC(XtfCurve.rlevcv);  //曲线的采样间隔
	SunR4TOPC(XtfCurve.curvmin); //曲线最小值
	SunR4TOPC(XtfCurve.curvmax); //曲线最大值
	SunR4TOPC(XtfCurve.curvavg); //曲线平均值
	SunR4TOPC(XtfCurve.timeinc); //时间增量（采样间隔）
	SunR4TOPC(XtfCurve.starttm); //阵列声波的起始时间
	SunR4TOPC(XtfCurve.stadepth); //静态测量深度
	SunR4TOPC(XtfCurve.stddev);  //标准方差
	SunR4TOPC(XtfCurve.cpwinbeg); //相关图参数（Window begin）
	SunR4TOPC(XtfCurve.cpwinstp); //相关图参数（Window step）
	SunR4TOPC(XtfCurve.cpwinlen); //相关图参数（Window length）
	SunR4TOPC(XtfCurve.cbilodeg); //CBIL方位的度数
	SunR4TOPC(XtfCurve.IntervalTR);  //发射器到接收器的距离
	SunR4TOPC(XtfCurve.IntervalRR);  //接收器到接收器的距离
	SunR4TOPC(XtfCurve.RawOffset);   //原始数据偏转
	SunR4TOPC(XtfCurve.TimeDelay);   //时间延迟
	SunR4TOPC(XtfCurve.Angle);       //参考臂到带传感臂的夹角
	SunR4TOPC(XtfCurve.XPad);    //极板上传感器的X位置
	SunR4TOPC(XtfCurve.YPad);    //极板上传感器的Y位置
	MSunR4TOPC(6,XtfCurve.LossMinor); //次级遗失数据值

	SunR8TOPC(XtfCurve.dblinit);  //双精度初始值
	SunR8TOPC(XtfCurve.dbtopdep); //双精度顶部深度
	SunR8TOPC(XtfCurve.dbbotdep); //双精度底部深度
	SunR8TOPC(XtfCurve.dbrlevel); //双精度采样间隔
	MSunR8TOPC(6,XtfCurve.smvalsdb); //Array pf 6 secondary Missing Datum Value
	SunR8TOPC(XtfCurve.idxtop2); //次级采样索引头部
	SunR8TOPC(XtfCurve.idxbot2); //次级采样索引底部
	SunR8TOPC(XtfCurve.idxrlev2); //次级极样索引间距

	SunI4TOPC(XtfCurve.EncodedCreationDate);  //创建日期编码
	SunI4TOPC(XtfCurve.EncodedCreationTime);  //创建时间编码
	SunI4TOPC(XtfCurve.EncodedLastAccessDate);  //最后存取日期编码
	SunI4TOPC(XtfCurve.EncodedLastAccessTime);  //最后存取时间编码
	SunI4TOPC(XtfCurve.NumberOfCurveHeaderrecords);  //曲线头记录数
	SunI4TOPC(XtfCurve.RecordNumberOfLastCurvedataRecord);  //曲线最后一个数据记录数
	SunI4TOPC(XtfCurve.DatumInitializationVariable);  //数据初始化变量，对4字节数
	SunI4TOPC(XtfCurve.NumberOfLevelsForTheCurve);  //曲线的采样数
	SunI4TOPC(XtfCurve.CurrentEncodedDateSignature);  //当前编码日期标记
	SunI4TOPC(XtfCurve.CurrentEncodedTimeSignature);  //当前编码日期标记
	MSunI4TOPC(6,XtfCurve.LossMinor2); //6个次级遗失数据值的阵列
	MSunI4TOPC(2,XtfCurve.Arrays2);  //两值阵列，代表结构化曲线定义的首末记录
	MSunI4TOPC(2,XtfCurve.Arrays3);  //两值阵列，代表结构化曲线助记名的首末记录
	
	SunI2TOPC(XtfCurve.SurvlibMajor);  //SURVLIB主要版
	SunI2TOPC(XtfCurve.SurvlibMinor); //SURVLIB次要版
	SunI2TOPC(XtfCurve.Ihcurv);  //创建时的曲线句柄
	SunI2TOPC(XtfCurve.Ictype);  //曲线类型:1-常规,2-波形,3-阵列
	SunI2TOPC(XtfCurve.RepCode);  //数据类型:2-INT*2,4=REAL*4,等
	SunI2TOPC(XtfCurve.Ivtype);  //垂直索引类型
	SunI2TOPC(XtfCurve.Ihtype);  //水平索引类型
	SunI2TOPC(XtfCurve.Ndims);   //曲线的维数
	MSunI2TOPC(7,XtfCurve.Idims);  //第i维的单元数
	SunI2TOPC(XtfCurve.I2init);  //INT*2变量的初始值
	SunI2TOPC(XtfCurve.Intflfix);  //整型/实型/固定型标记
	SunI2TOPC(XtfCurve.Isign); //有符号/无符号标记
	SunI2TOPC(XtfCurve.Numbits); //每数据单元的字位数
	SunI2TOPC(XtfCurve.Lisrepcd); //若取正值，该曲线以原始LIS编码形式
	SunI2TOPC(XtfCurve.Lisnsamp);  //Nsamp与此同时Lisrepcd有关
	SunI2TOPC(XtfCurve.Lisrcsiz);  //LIS代码大小（字节）
	SunI2TOPC(XtfCurve.Lisnsize);  //每个样点LIS代码总节节数
	SunI2TOPC(XtfCurve.Istartfv);  //0-固定的声波起始时间,1-可变的声波起始时间
	SunI2TOPC(XtfCurve.Igainfv);   //若为0，阵列声波的增益固定
	SunI2TOPC(XtfCurve.Igainepx);  //具有固定增益的阵列或曲线的增益指数
	SunI2TOPC(XtfCurve.Igainmth);  //具有增益的阵列或曲线的增益表示法-固定或可变
	SunI2TOPC(XtfCurve.Igainapp);  //增益应用开关
	SunI2TOPC(XtfCurve.Lntaphed);  //RAWTPHD的长度
	SunI2TOPC(XtfCurve.Ntools);    //仪器数
	SunI2TOPC(XtfCurve.Idxingrp);  //曲线索引
	MSunI2TOPC(6,XtfCurve.Smvalsi2);  //6个次级遗失数据值的阵列
}
void CXtfWrite::WriteDataToXtf(DWORD pos,DWORD count,float *buffer)
{
	CString s;
	DWORD i;
	short *hBuf=0;
	unsigned char *ucBuf=0;
	char *cBuf=0;
	double *dfBuf=0;
	unsigned short *uhBuf=0;
	int *iBuf=0;
	unsigned int *uiBuf=0;
	fXtf.Seek(pos,CFile::begin);
	switch(XtfCurve.RepCode)
	{
	case XTF_REPR_INT2://有符号短整型数
		hBuf=new short[count];
		for(i=0;i<count;i++)
		{
			hBuf[i]=short(buffer[i]);
		}
		MSunI2TOPC(count,hBuf);
		fXtf.Write(hBuf,count*sizeof(short));
		delete []hBuf; hBuf=0;
		break;
	case XTF_REPR_UCHAR://无符号字节
		ucBuf=new unsigned char[count];
		for(i=0;i<count;i++)
		{
			ucBuf[i]=unsigned char(buffer[i]);
		}
		fXtf.Write(ucBuf,count);
		delete []ucBuf; ucBuf=0;
		break;
	case XTF_REPR_FLOAT://IEEE单精度浮点数
		MSunR4TOPC(count,buffer);
		fXtf.Write(buffer,count*sizeof(float));
		break;
	case XTF_REPR_CHAR://有符号字节数
		cBuf=new char[count];
		for(i=0;i<count;i++)
		{
			cBuf[i]=char(buffer[i]);
		}
		fXtf.Write(cBuf,count);
		delete []cBuf; cBuf=0;
		break;
	case XTF_REPR_DOUBLE://双精度浮点
		dfBuf=new double[count];
		for(i=0;i<count;i++)
		{
			dfBuf[i]=double(buffer[i]);
		}
		MSunR8TOPC(count,dfBuf);
		fXtf.Write(dfBuf,count*sizeof(double));
		delete []dfBuf; dfBuf=0;
		break;
	case XTF_REPR_UINT2://无符号短整型数
		uhBuf=new unsigned short[count];
		for(i=0;i<count;i++)
		{
			uhBuf[i]=unsigned short(buffer[i]);
		}
		MSunI2TOPC(count,uhBuf);
		fXtf.Write(uhBuf,count*sizeof(unsigned short));
		delete []uhBuf; uhBuf=0;
		break;
	case XTF_REPR_INT4://长整型
		iBuf=new int[count];
		for(i=0;i<count;i++)
		{
			iBuf[i]=int(buffer[i]);
		}
		MSunI4TOPC(count,iBuf);
		fXtf.Write(iBuf,count*sizeof(int));
		delete []iBuf; iBuf=0;
		break;
	case XTF_REPR_UINT4://无符号长整型数
		uiBuf=new unsigned int[count];
		for(i=0;i<count;i++)
		{
			uiBuf[i]=unsigned int(buffer[i]);
		}
		MSunI4TOPC(count,uiBuf);
		fXtf.Write(uiBuf,count*sizeof(unsigned int));
		delete []uiBuf; uiBuf=0;
		break;
	default:
		break;
	}
}

void CXtfWrite::WriteGenData(int count,float *buf)
{
	DWORD pos=XtfHead.StartOffset[Index]*4096;
	WriteDataToXtf(pos,count,buf);
}
void CXtfWrite::WriteWave(float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype != 2) 
	{
		return; //必须是波形曲线
	}
	if(depth<XtfCurve.deptop || depth>XtfCurve.depbot)
	{
		return; //深度不对
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	DWORD count=XtfCurve.Idims[0];
	DWORD BlockSize=count*XtfRepCodeToLen(XtfCurve.RepCode)/8;
	DWORD pos=XtfHead.StartOffset[Index]*4096+np*BlockSize;
	WriteDataToXtf(pos,count,buffer);
}
void CXtfWrite::WriteWaveArray(float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype < 2) 
	{
		return; //必须是波形或阵列曲线
	}
	if(depth<XtfCurve.deptop || depth>XtfCurve.depbot)
	{
		return; //深度不对
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	int count=1;
	for(int i=0;i<XtfCurve.Ndims;i++)
	{
		count*=XtfCurve.Idims[i];
	}
	DWORD BlockSize=count*XtfRepCodeToLen(XtfCurve.RepCode)/8;
	DWORD pos=XtfHead.StartOffset[Index]*4096+np*BlockSize;
	WriteDataToXtf(pos,count,buffer);
}

short CXtfWrite::XtfRepCodeToLen(short RepCode)
{
	short XtfCodeLen=8;
	switch(RepCode)
	{
	case XTF_REPR_BIT:
		XtfCodeLen=1;
		break;
	case XTF_REPR_UCHAR:
	case XTF_REPR_CHAR:
		XtfCodeLen=8;
		break;
	case XTF_REPR_INT2:
	case XTF_REPR_UINT2:
		XtfCodeLen=16;
		break;
	case XTF_REPR_FLOAT:
	case XTF_REPR_INT4:
	case XTF_REPR_UINT4:
		XtfCodeLen=32;
		break;
	case XTF_REPR_DOUBLE:
		XtfCodeLen=64;
		break;
	default:
		XtfCodeLen=8;
		break;
	}
	return XtfCodeLen;
}
short CXtfWrite::XtfRepCodeToIntFlFix(short XtfRepCode)
{
	short nIntFlFix=1;
	switch(XtfRepCode)
	{
	case XTF_REPR_INT2:
	case XTF_REPR_UCHAR:
	case XTF_REPR_INT4:
	case XTF_REPR_UINT4:
	case XTF_REPR_UINT2:
		nIntFlFix=1;
		break;
	case XTF_REPR_FLOAT:
		nIntFlFix=2;
		break;
	case XTF_REPR_DOUBLE:
		nIntFlFix=4;
		break;
	case XTF_REPR_CHAR:
		nIntFlFix=5;
		break;
	default:
		nIntFlFix=3;
	}
	return nIntFlFix;
}
short CXtfWrite::XtfRepCodeToSign(short XtfRepCode)
{
	if((XtfRepCode==XTF_REPR_BIT)
		|| (XtfRepCode==XTF_REPR_UCHAR)
		|| (XtfRepCode==XTF_REPR_UINT2)
		|| (XtfRepCode==XTF_REPR_UINT4))
	{
		return 2; //无符号标记
	}
	else
	{
		return 1; //有符号
	}
}
void CXtfWrite::PrintHead(FILE *fp)
{
	CString s;
	fprintf(fp,"\n------- XTF FILE HEADER ---------\n");
	fprintf(fp,"%+25s: %d\n","System Code",XtfHead.System_Code);
	fprintf(fp,"%+25s: %-.80s\n","Fully expanded filename",XtfHead.FileFullName);
	fprintf(fp,"%+25s: %-.8s\n","Depth Units",XtfHead.Depth_Unit);
	fprintf(fp,"%+25s: %d\n","Number of Curves",XtfHead.isnumcv);
	fprintf(fp,"%+25s: %d\n","Max    of Curves",XtfHead.ismaxcv);
	fprintf(fp,"%+25s: %G\n","Default Start Depth",XtfHead.wttop);
	fprintf(fp,"%+25s: %G\n","Default End Depth",XtfHead.wtbottom);
	fprintf(fp,"%+25s: %G\n","Default Level Spacing",XtfHead.wtlevel);
	fprintf(fp,"%+25s: %d (%d Bytes)\n","Number of Record",XtfHead.FileRecord,XtfHead.FileRecord*4096);

	fprintf(fp,"\n------- XTF SELF-DEFINED DATATYPE ---------\n");
	fprintf(fp," %15s %15s %15s\n","DataType Name","Start Record","End Record");
	for(int i=0;i<64;i++)
	{
		s.Format ("%-.8s",XtfHead.UserData[i].Name);
		s.TrimLeft();
		s.TrimRight();
		if(s.IsEmpty()) continue;
		fprintf(fp," %15s %15d %15d\n",(const char*)s,XtfHead.UserData[i].StartRecord,XtfHead.UserData[i].StopRecord);
	}
	//曲线信息
	fprintf(fp,"\n----------------------  XTF Curve Information  -------------------\n");
	fprintf(fp,"%-8s %-4s %-8s %8s %8s %8s %5s %5s %4s %4s %4s %4s %4s %4s %4s\n",
		"Name","Type","Units","Start","Stop","Rlev","Num","Offs",
		"#Dim","#El1","#El2","#El3",
		"RepC","Hor","Ver");
	CString sCurveName;
	int NumCurveHeadErr=0; //曲线头有错的曲线数
	for(i=0;i<XtfHead.isnumcv;i++)
	{
		fprintf(fp,"%-8.8s",XtfHead.CurveName[i]);
		if(XtfHead.CurveAttrib[i].CurType==1)
		{
			fprintf(fp," %-4s","C");
		}
		else if(XtfHead.CurveAttrib[i].CurType==2)
		{
			fprintf(fp," %-4s","WF");
		}
		else
		{
			fprintf(fp," %-4s","MATR");
		}
		fprintf(fp," %-.8s",XtfHead.Units[i]);
		fprintf(fp," %8G",XtfHead.Sdep[i]);
		fprintf(fp," %8G",XtfHead.Edep[i]);
		fprintf(fp," %8G",XtfHead.Level[i]);
		fprintf(fp," %5u",XtfHead.LevelNumber[i]);
		fprintf(fp," %5u",XtfHead.StartOffset[i]);
		fprintf(fp," %4u",XtfHead.ndimension[i]);
		fprintf(fp," %4u",XtfHead.idimen1[i]);
		fprintf(fp," %4u",XtfHead.idimen2[i]);
		fprintf(fp," %4u",XtfHead.idimen3[i]);
		fprintf(fp," %4u",XtfHead.CurveAttrib[i].RepCode);
		fprintf(fp," %4u",XtfHead.CurveAttrib[i].HorType);
		fprintf(fp," %4u",XtfHead.CurveAttrib[i].VerType);
		fprintf(fp,"\n");

		////////////////////////////////////////////////////////
		//PrtXtfCurveHead(XtfHead.CurveName[i]);
	}
	if(NumCurveHeadErr>0)
	{
		fprintf(fp,"\nNumber of Curve Header Error: %d\n",NumCurveHeadErr);
	}
	fprintf(fp,"\n");
	fprintf(fp,"---- End Head information ---\n\n");

}
