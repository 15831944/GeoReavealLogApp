#ifndef __BIT_HEADER__
#define __BIT_HEADER__
//Bit_Head.h:BIT文件数据的头信息定义文件
typedef struct // BIT数据的头信息
{
	BYTE NumWF;//波形条数
	BYTE RlevTime;//波形采样时间间隔（US）
	short NumSample;//每条波形的采样点数
    char CompanyName[72]; //公司名
	short nMonth; //月
	short nDay; //日
	short nYear; //年
	short nReserved; //未定义
	char WellName[72]; //井名
	short nHour; //时
	short nMinute; //分
	short nSecond; //秒
	short nReserved2; //未定义
    short NumLog; //测井曲线条数
	short UnitFlag; //单位标志(公制/英制)
    char CurveName[20][4]; //曲线名
    float Stdep; //起深
	float Endep; //止深
	float Rlev; //采样间隔
    short Idc; //未定义
	short DataFlag; //格式标志：1024为波形，0为常规
    float Spcpr; //采样点数
	char  LogServ[6]; //测井服务号
	char  FileNum[2]; //文件号
} BIT_HEAD;
#endif 
