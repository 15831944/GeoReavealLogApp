// DlisWrite.h: interface for the CDlisWrite class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DLISWRITE_H__
#define __DLISWRITE_H__

#include "CodeSP.h"
#include "DlisOut.h"


class AFX_EXT_CLASS CDlisWrite  
{
public:
	CDlisWrite();
	virtual ~CDlisWrite();
	CFile fDlis;
	bool Open(CString sDataFile,FILE *fo); //打开输出文件
	void Close(); //关闭输出文件
	void SetTrailLengthFlag(bool bFlag=false); //设置尾长度标志
	void SetChannelNumber(int Num); //分配通道空间
	void SetFrameNumber(int Num); //分配帧空间
	void SetDlisAxisNumber(int Num); //分配轴空间
	void WriteStoUnitLab(); //写存储单元标志
	void WriteFileHeader(); //写文件头到盘缓冲区
	void WriteLogBufToDlisBuf(BYTE RecFlag,BYTE RecType,int Buflen,BYTE *buf,bool bWFlag=false); //将逻辑记录数据送入盘缓冲区
	void WriteOrigin(); //写源到盘缓冲区 
	void WriteParameter(); //写参数STATIC:PARAMETER到盘缓冲区
	void WriteAxis(); //写轴到盘缓冲区 
	void WriteChannel(); //写通道到盘缓冲区 
	void WriteFrame(); //写帧到盘缓冲区 
	void StartWriteData(int noFrame); //开始输出指定帧数据
	void StopWriteData(int noFrame); //结束输出指定帧数据
	void WriteCurveData(int nItem,int nCurve,int NumR,float *buf); //输出常规数据
	void WriteArrayData(int nItem,int iCurve,int noFra,int Count,float *vFloat); //输出阵列浮点数据
	void WriteArrayData(int nItem,int nCurve,int noFra,int Count,char *vChar); //输出阵列字节数据
	void WriteArrayData(int nItem,int nCurve,int noFra,int Count,short *vShort); //输出阵列短整型数据
	void Prt_Header(); //打印DLIS头参数

private:
	void VariableReset(); //变量始化
	void WriteDlisBufDToisk(); //输出缓冲区数据,并设置缓冲区为0字节
	void FrameDataSize(); //帧数据的长度
	int DlisCodeLen(BYTE RepCode); //DLIS代码长度
	void PutCompSET(const char *Type,const char *Name,int &IP,BYTE *buf); //送装置组件块到缓冲区
	void PutCompATTRIB(const char *Label,const DWORD Count,const BYTE RepCode,const char *Units,int &IP,BYTE *buf); //放属性组件块到缓冲区
	void PutCompOBJECT(const int Origin,const BYTE CopyNum,const char *Name,int &IP,BYTE *buf); //送对象组件块到缓冲区
	void SetIndexCurveInfo(); //设置索引曲线信息
	void CheckLogBufLen(); //检查并修改逻辑记录缓冲区长度
	void FloatToLogBuf(float vf,BYTE RepCode,int &NBYTE,BYTE *buf);

public:
	int MaxPhyLen; //最大物理记录长度
	_PhyRecHead PhyRecHead; //物理记录头信息
	bool bTrailLengthFlag; //是否记录尾长度
	_LogRecAttrib LogRecAttrib; //逻辑记录属性位

public:
	WORD NumChannel; //曲线条数
	WORD NumAxis; //实际轴数
	WORD NumFrame; //帧对象数
	_StoUnitLab StoUnitLab; //存储标志
	_FileHeader FileHeader; //文件头
	_Origin Origin; //源
	_Channel *Channel;
	_Frame *Frame;
	_Axis *Axis; //轴

private:
	CDlisOut DlisOut; //DLIS数据编码
	int DiskBufLen; //等待输出的数据长度
	int LogBufLen; //逻辑记录缓冲区长度
	BYTE *DiskBuf; //等待输出的数据
	BYTE *LogBuf; //逻辑记录数据缓冲区
private:
	DWORD *IORIGIN;//数据源
	BYTE  *ICOPYNUM;//数据拷贝数
	int   *IVALUE; //整数值
	float *VALUE; //浮点值
	char  *CVALUE; //子串集,假定一个子串平均长度为64字节


private:
	FILE *fp;
	CFile fTmp;
	CString sMidFile;
	CString m_sDataFile; //输出文件名
};

#endif 
