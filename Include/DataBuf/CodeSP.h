// CodeSP.h: interface for .
//
//////////////////////////////////////////////////////////////////////

#ifndef __CODESP_H__
#define __CODESP_H__

//C++基本数据类型定义
//                     代码值  类型长度   类型说明
#define PC_REPR_INT     1     // 4        有符号整型
#define PC_REPR_SHORT   2     // 2        有符号短整型数
#define PC_REPR_LONG    3     // 4        有符号长整型数
#define PC_REPR_FLOAT   4     // 4        IEEE单精度浮点数 
#define PC_REPR_DOUBLE  5     // 8        IEE双精度浮点数
#define PC_REPR_STRING  6     // 不定     字符串
#define PC_REPR_CHAR    7     // 1        有符号字节数
#define PC_REPR_UCHAR   8     // 1        无符号字节数
#define PC_REPR_USHORT  9     // 2        无符号短整型数
#define PC_REPR_UINT   10     // 4        无符号整型
#define PC_REPR_ULONG  11     // 4        无符号长整型数

//WIS文件数据类型定义
//					代码值		类型长度  	类型说明
#define WIS_REPR_INT     1	//	4			有符号整型
#define WIS_REPR_SHORT   2	//	2			有符号短整型数
#define WIS_REPR_LONG    3	//	4			有符号长整型数
#define WIS_REPR_FLOAT   4	//	4			IEEE单精度浮点数	
#define WIS_REPR_DOUBLE  5	//	8			IEE双精度浮点数
#define WIS_REPR_STRING  6	//  不定		字符串
#define WIS_REPR_CHAR    7	//	1			有符号字节数
#define WIS_REPR_UCHAR   8	//	1			无符号字节数
#define WIS_REPR_USHORT  9	//	2			无符号短整型数
#define WIS_REPR_UINT   10	//	4			无符号整型
#define WIS_REPR_ULONG  11	//	4			无符号长整型数

//文件或文本送到剪粘板
BOOL _declspec(dllexport) TextFileToClipboard(char *ResultFile);
BOOL _declspec(dllexport) TextToClipboard(char *szBuf);

//删除串的前、后或前后空字符
void _declspec(dllexport) StrTrimLeft(char *str);
void _declspec(dllexport) StrTrimRight(char *str);
void _declspec(dllexport) StrTrimLeftRight(char *str);

//UNIX与DOS内码转换
void _declspec(dllexport) SunI2TOPC(short &i2);
void _declspec(dllexport) SunI2TOPC(unsigned short &i2);
void _declspec(dllexport) SunI4TOPC(int &i4);
void _declspec(dllexport) SunI4TOPC(unsigned int &i4);
void _declspec(dllexport) SunR4TOPC(float &r4);
void _declspec(dllexport) SunR8TOPC(double &r8);
void _declspec(dllexport) MSunI2TOPC(int count,short *i2);
void _declspec(dllexport) MSunI2TOPC(int count,unsigned short *i2);
void _declspec(dllexport) MSunI4TOPC(int count,int *i4);
void _declspec(dllexport) MSunI4TOPC(int count,unsigned int *i4);
void _declspec(dllexport) MSunR4TOPC(int count,float *r4);
void _declspec(dllexport) MSunR8TOPC(int count,double *r8);

//4字节长整型数与浮点的转换
float _declspec(dllexport) Int4ToFloat(int i4);
void _declspec(dllexport) MInt4ToFloat(int count,int *i4,float *r4);
int _declspec(dllexport) FloatToInt4(float r4);
void _declspec(dllexport) MFloatToInt4(int count,float *r4,int *i4);

//BIT文件浮点内码转换
void _declspec(dllexport) GetBitR4(float &r4);

//是否为一合法路径
bool _declspec(dllexport) IsDirectory (const CString sDirName);

//是否为一合法文件
bool _declspec(dllexport) IsFileName(const CString sFileName);

//是否为一合法路径或文件
bool _declspec(dllexport) IsFilePathName(const CString sName);

//取得文件长度
int _declspec(dllexport) GetFileLength(CString SCurDataFile);

//从全路径文件取得子串
CString _declspec(dllexport) MyGetPathName(const CString szFilename);
CString _declspec(dllexport) MyGetFileName(const CString szFilename);
CString _declspec(dllexport) MyGetFileExt(const CString szFilename);
CString _declspec(dllexport) MyGetFileTitle(const CString szFilename);
CString _declspec(dllexport) MyGetPathNameFileTitle(const CString szFilename);

//重新采样
void _declspec(dllexport) FloatResample(float *buf,int NumX,int NumY);
void _declspec(dllexport) FloatResample(float Start1,float Stop1,float Rlev1,float *buf1,float Start2,float Stop2,float Rlev2,float *buf2);

//选择文件
void _declspec(dllexport) SelectOneFile(const char *szTitle,bool bOpen,const char *szFilter,char *szFileName,const char *ExtName,const char *OldFile);

//文件合并
bool _declspec(dllexport) FileUnite(const CString sObjectFile,const CString SourceFile); //将SourceFile合并到sObjectFile

//串到文件
bool _declspec(dllexport) PutStringToFile(const CString sText,const CString sFile,BOOL bWarnIfExists=TRUE);

//注册表读写
bool _declspec(dllexport) RegKeyValueQuery(const CString szKey,const CString lpszEntry,CString &strValue);
bool _declspec(dllexport) RegKeyValueWrite(const CString szKey, const CString lpszEntry,const CString strValue);
bool _declspec(dllexport) RegKeyValueQueryDword(const CString szKey,const CString lpszEntry,DWORD *dwFeatureSet);

//从串中取出第n项子项
CString _declspec(dllexport) MyGetSubStr(const CString szSource,int NumItem);

//Wis文件数据表示码对应长度
int _declspec(dllexport) WisCodeLen(DWORD RepCode);

//C++基本数据表示码对应长度
int _declspec(dllexport) PcCodeLen(DWORD RepCode);

//字符串是否没有可显示字符
BOOL _declspec(dllexport) IsNullString(const char *str);
//是否全为7位ASCII值的串,且不为空格
BOOL _declspec(dllexport) IsAsciiString(CString Source);

BOOL _declspec(dllexport) CheckLoginCode();
void GetHardwareCode(char *HardwareCode);
void HardwareCode2LoginCode(char *HardwareCode,char *LoginCode);
void _declspec(dllexport) ModiFileName(const char* Editer);
CString _declspec(dllexport) GetWorkPath(CString flag);
void _declspec(dllexport) LoadDataBufProPara();
BOOL _declspec(dllexport) DelDirAllFile(CString DirName);
#endif 
