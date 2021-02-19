#if !defined(_HEAD_XB716_INCLUDED_)
#define _HEAD_XB716_INCLUDED_
typedef struct /* 新星公司LA716数据文件头信息*/
{
	float ECC; 
	char CorpName[80];
	char WellName[80];
	short NumLog,IZ;
	char CurveName[64][4];
	float Stdep;
	float Endep;
	float Rlev;
	short IDC,IZZ;
	float Feet;
	char  DumFull[68];
} XB716_HEAD;
#endif //(_HEAD_XB716_INCLUDED_)
/*
西北局LA716文件格式
1.文件标题块总长度等于512字节
				名称			类型		长度		例值
ECC				成果号			float		4字节		0.0
CorpName		公司名			char		80字节		实际公司名
WellName		井名			char		80字节		实际井名
NumLog			曲线数			short		2字节		实际曲线数
IZ				单位标记		short		2字节		0
LogName			曲线名			char		256宇节		不足补空格
StartDep		开始深度		float		4字节		实际起始深度
EndDep			结束深度		float	 	4字节		实际结束深度
Rlev			采样间距		float		4字节		实际采样间隔
IDC								short		2字节		0
IZZ								short		2字节		0
Feet			采样点数		float		4字节		125或64
DumFull			填充数			char		68字节		空格值
End	
2.文件大小
  一个数据块＝NumLog*4字节
  文件大小=512+4*NumLog*每条曲线采样点数
3.代码格式
  输出文件代码：微机格式
*/
