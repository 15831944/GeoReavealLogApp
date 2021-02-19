#if !defined(_SKC98_HEADER_INCLUDED_)
#define _SKC98_HEADER_INCLUDED_
//Skc98_Head.h:SKC98文件数据的头信息定义文件
typedef struct
{
	DWORD SKCFlag; //0:4 bytes
	char CompanyName[80]; //4:80 bytes
	char WellName[80]; //84:80 bytes
	short NumLog; //164:2 bytes
	short DriverFlag; //166:2 bytes;  "0" -- Depth Driver Logging,"1" --Time Driver Logging;
	char CurveName[30][4]; //168:120 bytes
	short CurveTypes[30]; //288:MaxCurveNum*2=60 Bytes   
	float Stdep; //348:4 bytes
	float Endep; //352:4 bytes
	float Rlev; //356:4 bytes
	short Idc; //360:2 bytes
	short Iee; //362:2 bytes
	float Feet; //364:4 bytes
	float Fill1; //368:4 Bytes
	float Fill2; //372:4 Bytes
	char LogDate[18]; //376:18 bytes
	char LogTime[18]; //394:18 bytes
	char TeamName[50]; //412:50 bytes
	char AreaName[50]; //462:50bytes
} SKC98_HEAD;

//数据块由若干数据记录组成，每一深度点的所有曲线的采样数是一个数据记录。
//数据记录的长度根据曲线性质的变化而变化。
//数据记录的长度= CurveTypes数组中各元素的和。
//CurveTypes=曲线采样率/服务表采样率
//与SKC92不同，CCL曲线的采样率不再的隐含的
//曲线VDL为变密度曲线，一般为960个数据，时间采样间隔为2us,数据类型为short
#endif //!defined(SKC98_HEAD_)
