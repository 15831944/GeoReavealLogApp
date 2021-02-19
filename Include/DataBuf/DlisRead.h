// DlisRead.h: interface for the CDlisRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISREAD_H_INCLUDED_)
#define AFX_DLISREAD_H_INCLUDED_

#include "Dlis_Head.h"
#include "DlisInput.h"
#include "CodeSP.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CDlisRead  
{
public:
	CDlisRead();
	virtual ~CDlisRead();
	bool Open(CString sDataFile,FILE *fo); //打开DLIS文件
	void Close(); //关闭DLIS文件
	int  GetDlisLogicalFileNumber(); //最得指定逻辑文件数
	void ScanDlisFile(CString sDataFile,FILE *fo); //扫描DLIS文件
	bool ScanDlisOneLogicalFile(int nf); //扫描一个DLIS逻辑文件
	bool ReadLogicalFile(int nf);  //DLIS数据读到以帧为单位的文件
	bool ReadCurve(int iFrame, int iCurve,int Num,float *buffer); //读常规曲线数据的一条
	bool ReadWaveArray(int iFrame, int iCurve,float Depth,float *buffer); //读波形或阵列数据的一个点数据
	void GetNewName(CString &sNewName,int noFrame,int Index,int CopyNum,int Origin,BOOL m_bAddFrameNo,BOOL m_bAddOrigin,BOOL m_bAddCopyNum);

private:
	void ReadStoUnitLab(); //读DLIS存储设置(文件标志)
	bool ReadPhyHeader(); //读物理记录头信息
	int  GetLogicalRecord(); //取得一个逻辑记录信息
	CString RecordTypeToStr(BYTE RecType,BYTE Stru); //根据逻辑记录类型代码查询记录类型串
	CString DlisCodeToStr(BYTE IRepCode); //根据DLIS数据类型代码查询类型串
	BYTE DlisCodeOutType(BYTE IRepCode); //DLIS表示码到输出表示码
	void Prt_StoUnitLab(); //打印文件标志
	void Prt_LogFileIndex(); //逻辑文件索引参数
	void InitVariable(); //变量给初始化值
	void RenewChannelData(); //补充通道参数
	void Prt_LogFileCurveInfo();//逻辑文件曲线信息输出到特定文件
	CFile fDlis; //DLIS文件指针


public:
	CDWordArray LogFilePos; //逻辑文件开始位置
	DWORD FilePos; //文件当前位置
	DWORD FileSize; //文件大小
	WORD NumChannel; //曲线条数
	WORD NumAxis; //实际轴数
	WORD NumFrame; //帧对象数
	_StoUnitLab StoUnitLab;
	_PhyRecHead PhyRec;
	_FileHeader FileHeader;
	_LogRecHead LogRecHead;
	_LogRecAttrib LogRecAttrib;
	_Channel *Channel;
	_Axis *Axis;
	_Frame *Frame;
	_Origin Origin;
	int NumPrtFdata; //显示帧数据数
	int NumPrtSample; //显示采样数
	BOOL bPrtBaseInfo; //是否显示基本信息
private:
	FILE *fp;
	CString m_sDataFile; //输入文件名
	DWORD NumLogFile; //逻辑文件数
	DWORD NumEncry; //加密记录数
	DWORD NumPhyRec; //物理记录数
	DWORD NumLogRec; //逻辑记录数
	DWORD nLogLen; //逻辑记录长度(去掉尾)
	DWORD LogRecBytes; //逻辑记录全长(含尾)

    char SetType[255+1]; //装置类型
    char SetName[255+1]; //装置类型名
	DWORD nLogPos; //逻辑记录位置
	DWORD NpntLenC; //已心读出的逻辑记录字节数
	DWORD NumEod; //Eod记录数
	bool bRpr; //是否读下一个物理记录
	DWORD BufLen; //缓冲区长度
	BYTE *DlisBuf; //缓冲区

private:
	DWORD *IORIGIN;//数据源
	BYTE  *ICOPYNUM;//数据拷贝数
	int   *IVALUE; //整数值
	float *VALUE; //浮点值
	int   *LCVALUE; //子串长度
	char  *CVALUE; //子串集,假定一个子串平均长度为64字节

private:
	CDlisInput DlisIn; //DLIS数据类型解码
	int  NumOrigin; //当前逻辑文件的ORIGIN记录数 
	BOOL bPrintEFLR; //解码过程是否打印码值
	bool bTrans; //是否为数据转换(true为转换)
	bool InitObject(DWORD IVORG1,BYTE IVCOPY1,char *ObjName); //初始化对象
	void VariableReset(); //变量复位
	bool DlisBufToEFLR(); //直接格式记录解码
	bool ValueAnalyser(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //码值分析
	bool ValuesToFHLR(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //取文件头对象的码值
	bool ValuesToOLR(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //取源对象的码值
	bool ValuesToCHANNL(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //取通道对象的码值
	bool ValuesToFRAME(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //取帧对象的码值
	bool ValuesToAXIS(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //取帧对象的码值

	void Prt_EflrPar(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //非重要对象码值显示
	void Prt_RecordHeader(); //打印记录头信息
	void Prt_FileHeader(); //打印文件头信息
	void Prt_Channel(); //打印通道对象信息
	void Prt_Frame(); //打印帧对象(FRAME:FRAME)信息
	void Prt_Origin(); //打印源对象(OLR:ORIGIN)信息
	void Prt_Eod(int IVORG, int IVCOPY, char *ObjName);
	void Prt_Axis(); //打印轴对象

private:
	int	NumAttrib; //模板属性数
	int kAttrib; //当前属性序号
	CStringArray TempAttribLabel; //模板属性标识符
	CDWordArray TempAttribCount; //模板属性计数和
	CByteArray TempAttribRepCd; //模板属性表示码
	CStringArray TempAttribUnits; //模板属性单位
	int NumSaveAttrib; //保存模板属性数
	CString sTempFile; //保存模板属时临时文件
	bool SaveAttrib(DWORD ICount,BYTE IRepCd); //保存模板属性值
	void LoadAttrib(); //加载模板属性值
	void SetZeroVALUE(); //设置缺省值
	void GetIflrObject(int &IVORG, int &IVCOPY, int &NBYTE, char *ObjName); //取得间接格式逻辑记录对象名
	bool FdataDecode(int IVORG, int IVCOPY, int NBYTE, char *ObjName); //帧数据解码
	int GetFrameNo(int IVORG, int IVCOPY, char *ObjName); //取得给定对象对应的帧序号
	void Prt_FData(DWORD i,int iChannel,int ICount,BYTE IRepCd); //打印帧数据
	CStringArray sMidFile; //中间数据文件
	CFile *fMid,fTemp; //临时文件指针
	BYTE DlisCodeToFid(BYTE IRepCd); //DLIS表示码转换为FID表示码
	bool ReadData(DWORD pos,BYTE RepCode,BYTE CodeLen,DWORD count,float *buf); //从临时文件读取数据
};
bool _declspec(dllexport) IsDLIS(CString sDataFile);

#endif // !defined(AFX_DLISREAD_H_INCLUDED_)
