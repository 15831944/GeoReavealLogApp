// XtfRead.cpp: implementation of the CXtfRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XtfRead.h"
#include <math.h>
#include <ctype.h>
#include <float.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXtfRead::CXtfRead()
{
}

CXtfRead::~CXtfRead()
{
}
bool _declspec(dllexport) IsXTF(CString sDataFile)
{
	CFile f;
	char Xtf_Label[4],str[5];
	int HeaderMap[8];
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Seek(0,CFile::begin);
	f.Read(HeaderMap,32);
	f.Seek(948,CFile::begin);
	BYTE System_Code;//系统代码，1=PC，2=PE，3=VAX，4=IBM，5=HP9000/SUN
	f.Read(&System_Code,1);
	if(System_Code==5)
	{
		MSunI4TOPC(8,HeaderMap);
	}
	f.Seek(760,CFile::begin);
	f.Read(Xtf_Label,4);
	f.Close();
	for(int i=0;i<8;i++)
	{
		if(HeaderMap[i]!=(i+1))
		{
			return false;
		}
	}
	sprintf(str,"%-4.4s",Xtf_Label);
	if(stricmp(str,".xtf")!=0)
	{
		return false;
	}
	return true;
}
bool CXtfRead::Open(CString sDataFile)
{
	if(fXtf.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fXtf.GetLength(); //文件长度
	fXtf.Seek(948,CFile::begin);
	BYTE System_Code;
	fXtf.Read(&System_Code,1);
	if(System_Code!=5 && System_Code!=1)
	{
		//系统代码不是hp9000/Sun或PC,程序无法识别
		fXtf.Close();
		return false;
	}
	return true;
}

void CXtfRead::Close()
{
	fXtf.Close();
}
void CXtfRead::GetHead()
{
	CString s;
	//第一记录
	fXtf.Seek(0,CFile::begin);
	fXtf.Read(XtfHead.header_location,32);
	fXtf.Read(XtfHead.iunused,182*4);
	fXtf.Read(XtfHead.extension,4);
	fXtf.Read(XtfHead.FileFullName,80);
	fXtf.Read(XtfHead.Depth_Unit,8);
	fXtf.Read(XtfHead.AIF_Revision_numbar,8);
	fXtf.Read(XtfHead.strunused,88);
	fXtf.Read(&XtfHead.System_Code,1);
	fXtf.Read(&XtfHead.QC,1);
	fXtf.Read(&XtfHead.Major_Revision_Number,1);
	fXtf.Read(&XtfHead.Minor_Revision_Number,1);
	fXtf.Read(XtfHead.hunused,44);
	fXtf.Read(&XtfHead.isnumcv,4);
	fXtf.Read(&XtfHead.ismaxcv,4);
	fXtf.Read(XtfHead.iunused2,20);
	fXtf.Read(XtfHead.UserData,64*16);
	fXtf.Read(&XtfHead.wttop,4);
	fXtf.Read(&XtfHead.wtbottom,4);
	fXtf.Read(&XtfHead.wtlevel,4);
	fXtf.Read(XtfHead.strunused2,1012);
	fXtf.Read(XtfHead.strunused3,1020);
	fXtf.Read(&XtfHead.FileRecord,4);

	//第二记录
	fXtf.Read(XtfHead.Datamap,4072);
	fXtf.Read(&XtfHead.bitmapfullflag,4);
	fXtf.Read(&XtfHead.firstrecord,4);
	fXtf.Read(&XtfHead.lastrecord,4);
	fXtf.Read(&XtfHead.prerecord,4);
	fXtf.Read(&XtfHead.thisrecord,4);
	fXtf.Read(&XtfHead.nextrecord,4);

	//第三记录
	fXtf.Read(XtfHead.CurveName,4096);
	
	//第四记录
	for(int i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.StartOffset[i],sizeof(int));
		fXtf.Read(&XtfHead.StartOffset[256+i],sizeof(int));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.LevelNumber[i],sizeof(int));
		fXtf.Read(&XtfHead.LevelNumber[256+i],sizeof(int));
	}

	//第五记录
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.ndimension[i],sizeof(short));
		fXtf.Read(&XtfHead.idimen1[i],sizeof(short));
		fXtf.Read(&XtfHead.ndimension[256+i],sizeof(short));
		fXtf.Read(&XtfHead.idimen1[256+i],sizeof(short));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.idimen2[i],sizeof(short));
		fXtf.Read(&XtfHead.idimen3[i],sizeof(short));
		fXtf.Read(&XtfHead.idimen2[256+i],sizeof(short));
		fXtf.Read(&XtfHead.idimen3[256+i],sizeof(short));
	}

	//第六记录
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.Sdep[i],sizeof(float));
		fXtf.Read(&XtfHead.Sdep[i+256],sizeof(float));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.Edep[i],sizeof(float));
		fXtf.Read(&XtfHead.Edep[i+256],sizeof(float));
	}

	//第七记录
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.Level[i],sizeof(float));
		fXtf.Read(&XtfHead.Level[256+i],sizeof(float));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.CurveAttrib[i],sizeof(XtfHead.CurveAttrib[i]));
		fXtf.Read(&XtfHead.CurveAttrib[256+i],sizeof(XtfHead.CurveAttrib[256+i]));
	}

	//第八记录
	fXtf.Read(XtfHead.Wellsiteinfo,8); //“WSI”标记，表示一个表效的井场信息记录
	fXtf.Read(XtfHead.Wellname,80);     //井名
	fXtf.Read(XtfHead.Fieldname,80);    //油田名称
	fXtf.Read(XtfHead.Companyname,80);  //公司名
	fXtf.Read(XtfHead.Countryname,32);  //国家名
	fXtf.Read(XtfHead.Countyname,32);   //县名
	fXtf.Read(XtfHead.Statename,16);    //州名
	fXtf.Read(XtfHead.OCTname,8);       //操作控制表名
	fXtf.Read(XtfHead.Welllocation,40); //井场位置
	fXtf.Read(XtfHead.Section,4);       //剖面名
	fXtf.Read(XtfHead.Township,4);      //城镇名
	fXtf.Read(XtfHead.Range,4);         //范围
	fXtf.Read(XtfHead.Permanentdatum,2); //永久数据,“gl”或“sl”
	fXtf.Read(XtfHead.Drillingmeasurementdatum,2);  //钻井测量数据(“kb”、“gl”等)
	fXtf.Read(XtfHead.LogAnalysisCenter,4);  //测井分析中心代号
	fXtf.Read(XtfHead.Wellunits,8);          //井场海拨单位
	fXtf.Read(XtfHead.strunused4,620);    //未用

	fXtf.Read(&XtfHead.Latitude,4);   //经度
	fXtf.Read(&XtfHead.Longitude,4);  //纬度
	fXtf.Read(&XtfHead.Kellybushing,4);  //方钻补心海拨高度
	fXtf.Read(&XtfHead.Derrickfloor,4);  //井架平台海拨高度
	fXtf.Read(&XtfHead.Elevation,4);    //海拨永久数据
	fXtf.Read(&XtfHead.MagneticDeclination,4);  //磁偏角
	fXtf.Read(&XtfHead.ECC,4);   //ECC号
	fXtf.Read(&XtfHead.Groundtosealevelelevation,4); //地面海拨高度
	fXtf.Read(XtfHead.funused,120*sizeof(float));   //未用单精度数
	fXtf.Read(XtfHead.dunused,64*sizeof(double));   //未用双精度数

	fXtf.Read(XtfHead.hunused2,6*sizeof(short));  //未用
	fXtf.Read(&XtfHead.Metricflag,sizeof(short));  //公制标记
	fXtf.Read(&XtfHead.Maximumcurveversion,sizeof(short)); //最大曲线类型(1-99)
	fXtf.Read(XtfHead.hunused3,504*sizeof(short));  //未用
	fXtf.Read(XtfHead.unformattedbinaryinformation,1024); //未格式化二进制信息

	//由系统代码决定是否进行不同机器之间数据转换
	if(XtfHead.System_Code==0x05)
	{
		//数据为SUN工作站代码,应转换
		SunXtfHead2PC();
	}
	XtfHead.ismaxcv=512; //有些文件的最大曲线数存在问题
	//因XTF文件被删除曲线以空曲线名出现，故增加此功能
	ClearHeaderPack();
	for(int Index=0;Index<XtfHead.isnumcv;Index++)
	{
		OpenCurve(Index);
		memcpy(XtfHead.Units[Index],XtfCurve.Units,8);
		memcpy(XtfHead.IndexUnit,XtfCurve.DepthUnits,8);
	}
}
void CXtfRead::SunXtfHead2PC()
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

void CXtfRead::SunCurveHead2PC()
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
	if(XtfCurve.RepCode==4 || XtfCurve.RepCode==2)
	{
		float *f4=(float*)&XtfCurve.DatumInitializationVariable;
		if(!_isnan(*f4))
		{
			XtfCurve.DatumInitializationVariable=int(*f4);		
		}
	}
}
void CXtfRead::ClearHeaderPack()
{
	CString s;
	int ki[512]={-1};
	int NumLog=0;
	for(int i=0;i<XtfHead.ismaxcv;i++)
	{
		s.Format("%-.8s",XtfHead.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		if(!s.IsEmpty())
		{
			ki[NumLog]=i;
			NumLog++;
		}
		if(NumLog==XtfHead.isnumcv) break;
	}

	for(i=0;i<XtfHead.isnumcv;i++)
	{
		if(i == ki[i]) continue; //此句不可省去，否则曲线名不对，因字符串自身拷贝有问题
		sprintf(XtfHead.CurveName[i],"%-.8s",XtfHead.CurveName[ki[i]]); //Head3
		XtfHead.StartOffset[i]=XtfHead.StartOffset[ki[i]]; //Head4
		XtfHead.LevelNumber[i]=XtfHead.LevelNumber[ki[i]]; //Head4
		XtfHead.ndimension[i]=XtfHead.ndimension[ki[i]]; //Head5
		XtfHead.idimen1[i]=XtfHead.idimen1[ki[i]]; //Head5
		XtfHead.idimen2[i]=XtfHead.idimen2[ki[i]]; //Head5
		XtfHead.idimen3[i]=XtfHead.idimen3[ki[i]]; //Head5
		XtfHead.Sdep[i]=XtfHead.Sdep[ki[i]]; //Head6
		XtfHead.Edep[i]=XtfHead.Edep[ki[i]]; //Head6
		XtfHead.Level[i]=XtfHead.Level[ki[i]]; //Head7
		XtfHead.CurveAttrib[i]=XtfHead.CurveAttrib[ki[i]]; //Head7
	}
}
int CXtfRead::OpenCurve(CString sCurveName)
{
	index=GetCurveIndex(sCurveName);
	if(index<0) return false;
	return ReadCurveHead();
}

int CXtfRead::OpenCurve(int Index)
{
	if(Index<0) return false;
	index=Index;
	return ReadCurveHead();
}
int CXtfRead::ReadCurveHead()
{
	CString s;
	int pos=(XtfHead.StartOffset[index]-1)*4096;
	fXtf.Seek(pos,CFile::begin);
	fXtf.Read(&XtfCurve,sizeof(XtfCurve));
	if(XtfHead.System_Code==5)
	{
		SunCurveHead2PC();
	}
	return CheckCurveHeader();
}
int CXtfRead::CheckCurveHeader()
{
	CString s;
	if(!isalpha(XtfCurve.Name[0]))
	{	
		return -1;//曲线名首字符不是字母
	}
	s.Format("%-.8s",XtfCurve.Name);
	s.TrimRight();
	if(s.IsEmpty())
	{	
		return -1;//曲线名为空
	}
	if(XtfCurve.NumberOfCurveHeaderrecords<=0)
	{
		return -2;  //曲线头记录数
	}
	if(XtfCurve.RecordNumberOfLastCurvedataRecord<0)
	{
		return -3;  //曲线最后一个数据记录数
	}
	if(XtfCurve.NumberOfLevelsForTheCurve<0)
	{
		return -4;  //曲线的采样数
	}
	if(XtfCurve.Ihcurv<0 || XtfCurve.Ihcurv>512)
	{
		return -5; //曲线索引号
	}
	if(XtfCurve.Ictype<1 || XtfCurve.Ictype>3)
	{
		return -6; //曲线类型:1-常规,2-波形,3-阵列
	}
	if(XtfCurve.RepCode<0 || XtfCurve.RepCode>12)
	{
		return -7;  //数据类型:2-INT*2,4=REAL*4,等
	}
	if(XtfCurve.Ndims>7 || XtfCurve.Ndims<1)
	{
		return -8;   //曲线的维数
	}
	if(XtfCurve.Numbits<1 || XtfCurve.Numbits>64)
	{
		return -9; //每数据单元的字位数
	}
	return 0;
}
int CXtfRead::GetCurveIndex(CString sCurveName)
{
	CString s;
	sCurveName.TrimLeft();
	sCurveName.TrimRight();
	index=-1;
	for(int i=0;i<XtfHead.ismaxcv;i++)
	{
		s.Format ("%-.8s",XtfHead.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		if(s.CompareNoCase(sCurveName)==0)
		{
			index=i;
			break;
		}
	}
	return index;
}
DWORD CXtfRead::ReadCurve(int index, DWORD count, float* buffer)
{
	if(XtfCurve.Ictype>1) 
	{
		return 0; //非常规数据
	}
	DWORD pos=XtfHead.StartOffset[index]*4096;
	ReadData(pos,count,buffer);
	return count;
}
bool CXtfRead::ReadWave(int index,float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype != 2) 
	{
		return false; //必须是波形曲线
	}
	if(depth<XtfCurve.deptop)
	{
		return false; //深度不对
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	DWORD count=XtfCurve.Idims[0];
	int NumData=1;
	for(int i=0;i<XtfCurve.Ndims;i++)
	{
		NumData*=XtfCurve.Idims[i];
	}
	DWORD BlockSize=NumData*XtfCurve.Numbits/8;
	DWORD pos=XtfHead.StartOffset[index]*4096+np*BlockSize;
	ReadData(pos,count,buffer);
	return true;
}
bool CXtfRead::ReadWaveFromMatrix(int index,int subindex,float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype != 3) 
	{
		return false; //必须是波形曲线
	}
	if(depth<XtfCurve.deptop)
	{
		return false; //深度不对
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	DWORD count=XtfCurve.Idims[0];
	int NumData=1;
	for(int i=0;i<XtfCurve.Ndims;i++)
	{
		NumData*=XtfCurve.Idims[i];
	}
	DWORD BlockSize=NumData*XtfCurve.Numbits/8;
	DWORD SubBlockSize=count*XtfCurve.Numbits/8;
	DWORD pos=XtfHead.StartOffset[index]*4096+np*BlockSize+subindex*SubBlockSize;
	ReadData(pos,count,buffer);
	return true;
}
bool CXtfRead::ReadMatrix(int index,float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype <2) 
	{
		return false; //必须是波形或阵列曲线
	}
	if(depth<XtfCurve.deptop)
	{
		return false; //深度不对
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	int count=1;
	for(int i=0;i<XtfCurve.Ndims;i++)
	{
		count*=XtfCurve.Idims[i];
	}
	DWORD BlockSize=count*XtfCurve.Numbits/8;
	DWORD pos=XtfHead.StartOffset[index]*4096+np*BlockSize;
	ReadData(pos,count,buffer);
	return true;
}
void CXtfRead::ReadData(DWORD pos,DWORD count,float *buffer)
{
	short *hBuf=NULL;
	int *iBuf=NULL;
	double *dfBuf=NULL;
	char *cBuf=NULL;
	unsigned char *ucBuf=NULL;
	unsigned short *uhBuf=NULL;
	unsigned int *uiBuf=NULL;
	DWORD i,NumBytes;

	fXtf.Seek(pos,CFile::begin);
	switch(XtfCurve.RepCode)
	{
	case XTF_REPR_BIT: //位值
		NumBytes=(count-1)/8+1;
		cBuf=new char[NumBytes];
		fXtf.Read(cBuf,NumBytes*sizeof(char));
		for(i=0;i<count;i++)
		{
			int k1=i/8;
			int k2=i%8;
			if( (cBuf[k1]<<k2) & 0x80 )
			{
				buffer[i]=1;
			}
			else
			{
				buffer[i]=0;
			}
		}
		delete []cBuf; cBuf=NULL;
		break;
	case XTF_REPR_INT2: //有符号短整型数
		hBuf=new short[count];
		fXtf.Read(hBuf,count*sizeof(short));
		if(XtfHead.System_Code==5)
		{
			MSunI2TOPC(count,hBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(hBuf[i]);
		delete []hBuf; hBuf=NULL;
		break;
	case XTF_REPR_UCHAR: //无符号字节数
		ucBuf=new unsigned char[count];
		fXtf.Read(ucBuf,count*sizeof(unsigned char));
		for(i=0;i<count;i++) 
		{
			buffer[i]=float(ucBuf[i]);
		}
		delete []ucBuf; ucBuf=NULL;
		break;
	case XTF_REPR_FLOAT: //IEEE单精度浮点数	
		fXtf.Read(buffer,count*sizeof(float));
		if(XtfHead.System_Code==5)
		{
			MSunR4TOPC(count,buffer);
		}
		break;
	case XTF_REPR_CHAR: //有符号字节数
		cBuf=new char[count];
		fXtf.Read(cBuf,count*sizeof(char));
		for(i=0;i<count;i++) 
		{
			buffer[i]=float(cBuf[i]);
		}
		delete []cBuf; cBuf=NULL;
		break;
	case XTF_REPR_DOUBLE: //IEE双精度浮点数
		dfBuf=new double[count];
		fXtf.Read(dfBuf,count*sizeof(double));
		if(XtfHead.System_Code==5)
		{
			MSunR8TOPC(count,dfBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(dfBuf[i]);
		delete []dfBuf; dfBuf=NULL;
		break;
	case XTF_REPR_UINT2: //无符号短整型数
		uhBuf=new unsigned short[count];
		fXtf.Read(uhBuf,count*sizeof(unsigned short));
		if(XtfHead.System_Code==5)
		{
			MSunI2TOPC(count,uhBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(uhBuf[i]);
		delete []uhBuf; uhBuf=NULL;
		break;
	case XTF_REPR_INT4: //有符号长整型数
		iBuf=new int[count];
		fXtf.Read(iBuf,count*sizeof(int));
		if(XtfHead.System_Code==5)
		{
			MSunI4TOPC(count,iBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(iBuf[i]);
		delete []iBuf; iBuf=NULL;
		break;
	case XTF_REPR_UINT4: //无符号长整型数
		uiBuf=new unsigned int[count];
		fXtf.Read(uiBuf,count*sizeof(unsigned int));
		if(XtfHead.System_Code==5)
		{
			MSunI4TOPC(count,uiBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(uiBuf[i]);
		delete []uiBuf; uiBuf=NULL;
		break;
	}	
}

