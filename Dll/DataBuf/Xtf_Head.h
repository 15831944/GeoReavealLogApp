#if !defined(_XTF_HEADER_INCLUDED_)
#define _XTF_HEADER_INCLUDED_

//XTF文件数据类型定义
//					代码值		类型长度  	类型说明
#define XTF_REPR_BIT     1	//	1			位值
#define XTF_REPR_INT2    2	//	2			有符号短整型数
#define XTF_REPR_UCHAR   3	//	1			无符号字节
#define XTF_REPR_FLOAT   4	//	4			IEEE单精度浮点数	
#define XTF_REPR_I12S    5	//	?			（不清楚）
#define XTF_REPR_I12U    6	//  ?			（不清楚）
#define XTF_REPR_CHAR    7	//	1			有符号字节数
#define XTF_REPR_DOUBLE  8	//	8			双精度浮点
#define XTF_REPR_UINT2   9	//	2			无符号短整型数
#define XTF_REPR_INT4    10	//	4			长整型
#define XTF_REPR_UINT4   11	//	4			无符号长整型数
#define XTF_REPR_UNKNOWN  12	//	?		（不清楚）

//XTF数据文件的头信息结构
//用户定义数据类型，在XTF文件第一个记录中用到
typedef struct
{
	char Name[8];  //数据类型名
	int StartRecord;  //起始记录
	int StopRecord;   //终止记录
}USERDATA;

//每条曲线类型及属性，在XTF文件第七个记录中用到
typedef struct
{
	BYTE CurType;  //曲线类型(1-3)：常规、波形、阵列
	BYTE RepCode;  //数据类型(1-12)
	BYTE HorType;   //水平类型
	BYTE VerType;  //垂直类型
}CURVEATTRIB;

typedef struct tagXTF_HEAD
{
	//第一个头记录：1-4096
	//含有文件名、系统编码、数据类型等
	int header_location[8];  //文件头记录的位置
	int iunused[182];   //未用
	char extension[4];  //扩展名，一般为“.XTF”
	char FileFullName[80];  //文件全名，含路径在内
	char Depth_Unit[8];  //深度单位
	char AIF_Revision_numbar[8];  //AIF获得版本号
	char strunused[88];  //未用
	BYTE System_Code;  //系统代码，1=PC，2=PE，3=VAX，4=IBM，5=HP9000/SUN
	BYTE QC; //用于指示目标系统在进程中有格式转换
	BYTE Major_Revision_Number; //SURVLIB 主版本号
	BYTE Minor_Revision_Number; //SURVLIB 副版本号
	short hunused[22];  //未用
	int isnumcv;  //曲线条数
	int ismaxcv;  //最多曲线数,一般为512
	int iunused2[5];  //未用
	USERDATA UserData[64];  //数据记录类型及其起止记录
	float wttop;      //缺省起始深度
	float wtbottom;   //缺省结束深度
	float wtlevel;    //缺省采样间隔
	char strunused2[1012];  //未用
	char strunused3[1020]; //未用
	int  FileRecord;  //文件记录数

	//第二头记录：4097-8192
	//含磁盘存储信息，用于确定文件中未使用的或已删除的区域，以及文件
	//满时与其它记录相连的部分。
	//当分配一个4096字节的数据块时，标记为1
	//总空间4072*8*4096=133,431,296字节 =127.25MB
	BYTE Datamap[4072];  //位图，第一字节反映8个头记录，紧接的的数据图位，
	int bitmapfullflag;  //位图满载标记
	int firstrecord;     //位图中的首项记录
	int lastrecord;      //位图中的最后一个记录
	int prerecord;       //前一个位图的记录数
	int thisrecord;      //当前位图记录数
	int nextrecord;      //下一个位图的记录数
	
	//第三头记录：8193-12288
	//曲线名，当为空时表示该曲线不存在或已被删除
	char CurveName[512][8];
	
	//第四头记录：12289-16384
	//曲线的起始位置及采样数，
	//注意：1)起始位置的排序是1,257,2,258,...256,512;
	//      2)曲线采样数的排序与起始位置相同
	int StartOffset[512];   //每条曲线的起始记录号，从1开始计数
	int LevelNumber[512];   //每条曲线的采样数

	//第五头记录:16385-20480
	//曲线维数定义，存放顺序为：
	//曲线1的维数,第一维单元数,第257的维数,第一维单元数,曲线2的...
	//曲线256的维数,第一维单元数,第512的维数,第一维单元数
	//曲线1第二维的单元数，第二维单元数,
	//曲线257第二维的单元数，第二维单元数,
	//......
	//曲线256第二维的单元数，第二维单元数,
	//曲线512第二维的单元数，第二维单元数,
	short ndimension[512]; //曲线的维数，取值为1-3
	short idimen1[512];    //曲线第一维的单元数
	short idimen2[512];    //曲线第二维的单元数
	short idimen3[512];    //曲线第三维的单元数

	//第六头记录:20481-24576
	//记录每条曲线的起止深度,单精度浮点数
	//记录顺序：
	//第1,257,2,258,...,256,512曲线的顶部深度
	//第1,257,2,258,...,256,512曲线的底部深度
	float Sdep[512];  //曲线的起始深度
	float Edep[512];    //曲线的结束深度

	//第七头记录:24577-28672
	//记录每条曲线采样间隔、数据类型、采样间隔为float型，
	//数据类型定义曲线、数据、水平和垂直等类型属性
	//记录顺序：
	//第1,257,2,258,...,256,512曲线的采样间距
	//第1,257,2,258,...,256,512曲线的曲线类型
	float Level[512];     //曲线采样间隔
	CURVEATTRIB CurveAttrib[512]; //曲线、数据、水平和垂直等类型属性

	//第八头记录:28673-32768
	//井场信息，分这4个区，每个区具有相同的数据类型
	//第一区：ASCII信息
	//第二区：浮点数（单精度和双精度）
	//第三区：整型（短整型和长整型）
	//第四区：二进制（未用）
	char Wellsiteinfo[8];  //“WSI”标记，表示一个表效的井场信息记录
	char Wellname[80];     //井名
	char Fieldname[80];    //油田名称
	char Companyname[80];  //公司名
	char Countryname[32];  //国家名
	char Countyname[32];   //县名
	char Statename[16];    //州名
	char OCTname[8];       //操作控制表名
	char Welllocation[40]; //井场位置
	char Section[4];       //剖面名
	char Township[4];      //城镇名
	char Range[4];         //范围
	char Permanentdatum[2]; //永久数据,“gl”或“sl”
	char Drillingmeasurementdatum[2];  //钻井测量数据(“kb”、“gl”等)
	char LogAnalysisCenter[4];  //测井分析中心代号
	char Wellunits[8];          //井场海拨单位
	char strunused4[620];    //未用

	float Latitude;   //经度
	float Longitude;  //纬度
	float Kellybushing;  //方钻补心海拨高度
	float Derrickfloor;  //井架平台海拨高度
	float Elevation;    //海拨永久数据
	float MagneticDeclination;  //磁偏角
	float ECC;   //ECC号
	float Groundtosealevelelevation; //地面海拨高度
	float funused[120];   //未用单精度数
	double dunused[64];   //未用双精度数

	short hunused2[6];  //未用
	short Metricflag;  //公制标记:0为公制,1为英制
	short Maximumcurveversion; //最大曲线类型(1-99)
	short hunused3[504];  //未用
	char unformattedbinaryinformation[1024]; //未格式化二进制信息

	//曲线单位,数据从曲线头中得到
	char Units[512][8];
	char IndexUnit[8]; //索引单位

}XTF_HEAD;

//曲线头信息
typedef struct 
{
	char Name[8];  //曲线名，必须以字母开头
	char Units[8]; //必须左对齐且含可打印字符
	char Comment[24];   //主曲线备注字段，即（AIF文件定义的备注）
	char ServiceCompany[8]; //服务公司,如“ATLAS”
	char ToolType[8];  //仪器类型
	char CurveClassType[8];  //曲线分类类型，如“WAVEFORM”、“DIPLOG”等
	char DepthUnits[8];  //深度单位，提供单位的意义
	char TimeIncrementUnits[8];  //波型曲线的时间增量单位
	char StartTimeUnits[8];  //起始时间单位,阵列声波有固定和可变的起始时间
	char RawTapeHeaderMnemonic[8];  //原始带带头助记符
	char UnitsOfStationary[8]; //静态深度单位
	char CorrelogramParamterUnits[8];  //相关图参数单位
	char CbilOrientationMnemonic[8];  //CBIL方位助记符
	char LoggingDirection[8];  //测井方向,取值为“up”、“down”等
	char AifCurveName[24]; //AIF曲线名和版本
	char OCT[8];  //创建该曲线的OCT
	char ToolNames[48];  //8个六字符的仪器名
	char strunused[8];  //未用
	char strunused2[8];  //未用
	char LisName[4];  //LIS曲线名
	char strunused3[4]; //未用
	char CreationRoutineName[16]; //十六字符的创建例程和版本
	char ModifiedRoutineName[16]; //十六字符的修改例程和版本
	char strunused4[28];   //未用
	char LongName[20];  //加长了的曲线名
	char strunused5[712];  //未用

	float deptop;  //曲线的顶部深度
	float depbot;  //曲线的底部深度
	float rlevcv;  //曲线的采样间隔
	float curvmin; //曲线最小值
	float curvmax; //曲线最大值
	float curvavg; //曲线平均值
	float timeinc; //时间增量（采样间隔）
	float starttm; //阵列声波的起始时间
	float stadepth; //静态测量深度
	float stddev;  //标准方差
	float cpwinbeg; //相关图参数（Window begin）
	float cpwinstp; //相关图参数（Window step）
	float cpwinlen; //相关图参数（Window length）
	float cbilodeg; //CBIL方位的度数
	float IntervalTR;  //发射器到接收器的距离
	float IntervalRR;  //接收器到接收器的距离
	float RawOffset;   //原始数据偏转
	float TimeDelay;   //时间延迟
	float Angle;       //参考臂到带传感臂的平角
	float XPad;    //极板上传感器的X位置
	float YPad;    //极板上传感器的Y位置
	float LossMinor[6]; //次级遗失数据值
	float funused[101]; //未用

	double dblinit;  //双精度初始值
	double dbtopdep; //双精度顶部深度
	double dbbotdep; //双精度底部深度
	double dbrlevel; //双精度采样间隔
	double smvalsdb[6]; //Array pf 6 secondary Missing Datum Value
	double idxtop2; //次级采样索引头部
	double idxbot2; //次级采样索引底部
	double idxrlev2; //次级极样索引间距
	double dunused[51]; //未用

	int EncodedCreationDate;  //创建日期编码
	int EncodedCreationTime;  //创建时间编码
	int EncodedLastAccessDate;  //最后存取日期编码
	int EncodedLastAccessTime;  //最后存取时间编码
	int NumberOfCurveHeaderrecords;  //曲线头记录数
	int RecordNumberOfLastCurvedataRecord;  //曲线最后一个数据记录数
	int DatumInitializationVariable;  //数据初始化变量
	int NumberOfLevelsForTheCurve;  //曲线的采样数
	int CurrentEncodedDateSignature;  //当前编码日期标记
	int CurrentEncodedTimeSignature;  //当前编码时间标记
	int LossMinor2[6]; //6个次级遗失数据值的阵列
	int Arrays2[2];  //两值阵列，代表结构化曲线定义的首末记录
	int Arrays3[2];  //两值阵列，代表结构化曲线助记名的首末记录
	int iunused[108];  //未用
	
	short SurvlibMajor;  //SURVLIB主要版
	short SurvlibMinor; //SURVLIB次要版
	short Ihcurv;  //创建时的曲线句柄
	short Ictype;  //曲线类型:1-常规,2-波形,3-阵列
	short RepCode;  //数据类型:2-INT*2,4=REAL*4,等
	short Ivtype;  //垂直索引类型
	short Ihtype;  //水平索引类型
	short Ndims;   //曲线的维数
	short Idims[7];  //第i维的单元数
	short I2init;  //INT*2变量的初始值
	short Intflfix;  //1-INT/2-FLOAT/3-FIXED的标志
	short Isign; //有符号/无符号标记
	short Numbits; //每数据单元的字位数
	short Lisrepcd; //若取正值，该曲线以原始LIS编码形式
	short Lisnsamp;  //Nsamp与此同时Lisrepcd有关
	short Lisrcsiz;  //LIS代码大小（字节）
	short Lisnsize;  //每个样点LIS代码总节节数
	short Istartfv;  //0-固定的声波起始时间,1-可变的声波起始时间
	short Igainfv;   //若为0，阵列声波的增益固定
	short Igainepx;  //具有固定增益的阵列或曲线的增益指数
	short Igainmth;  //具有增益的阵列或曲线的增益表示法-固定或可变
	short Igainapp;  //增益应用标志
	short Lntaphed;  //RAWTPHD的长度
	short Ntools;    //仪器号
	short Idxingrp;  //曲线索引
	short Smvalsi2[6];  //6个次级遗失数据值的阵列
	short hunused[219];  //未用
	
	BYTE TransmitterID[24];  //24字节代表24个发射器ID场
	BYTE ReceiverID[24];  //24字节代表24个接收器ID场
	BYTE RawTapeHeaderInfo[276];  //原始带头信息
	BYTE LossMinor3[6];  //6字节代表二遗失数值码阵列
	BYTE LossMinor4[6];  //6字节代表二遗失数值码阵列
	BYTE bunused[687];   //未用
	BYTE SystemCode;    //系统代码:1-PC,2-PE,3-VAX,4-IBM Mainframe,5-hp9000/Sun,etc.

}CURVE_HEAD;
#endif //(_XTF_HEADER_INCLUDED_)