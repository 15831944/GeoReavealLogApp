#if !defined(QH716_HEAD_INCLUDE_)
#define QH716_HEAD_INCLUDE_
//Qh716_Head.h 头信息定义
typedef struct
{
	int Head; 
	float ECC; 
	char CorpName[80];
	char WellName[80];
	short NumLog,IZ;
	char CurveName[128][4];
	float Stdep;
	float Endep;
	float Rlev;
	short IDC,IZZ;
	float Feet;
	short DumFull[4];
	int End; 
}QH716_HEAD;

#endif // !defined(QH716_HEAD_INCLUDE_)
/*
青海油田LA716文件格式
1.文件标题块总长度大于或等于284字节
				名称			类型		长度		例值
Head  			文件头开始		long		4字节		276
ECC				成果号			float		4字节		1.0
CorpName		公司名			char		80字节		实际公司名
WellName		井名			char		80字节		实际井名
NumLog			曲线数			short		2字节		实际曲线数
IZ				单位标记		short		2字节		0
LogName			曲线名			char		80宇节		不足补空格
StartDep		开始深度		float		4字节		实际起始深度
EndDep			结束深度		float	 	4字节		实际结束深度
Rlev			采样间距		float		4字节		实际采样间隔
IDC								short		2字节		12842
IZZ								short		2字节		0
Feet			采样点数		float		4字节		125或64
DumFull			填充数			short		4*2字节		空格值
LogName曲线名表(只有当曲线数多于20时才有)
End	
2.文件数据块
一个数据块＝4+4*NumLog*Feet+4字节
(前面和后面各4字节为块标，记录块内字节数)
3.代码格式
  输出文件代码：SUN工作站格式，与微机格式顺序相反
*/