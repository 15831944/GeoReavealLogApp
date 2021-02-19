// FidWrite.h: interface for the CFidWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIDWRITE_H_INCLUDED_)
#define AFX_FIDWRITE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Fid_Head.h"

class AFX_EXT_CLASS CFidWrite  
{
public:
	CFidWrite();
	virtual ~CFidWrite();

	void SetFidFileName(CString m_sObjectPath,CString sWellName); //设置FID文件名
	void InitFidChannel(); //初始化曲线通道
	bool WriteFidInfo(); //输出FID文件
	bool FefurbishFidInfo(); //重新刷新FID文件信息
	bool WriteGenData(int Index,int Num,float *buf); //输出常规数据
	bool WriteIndexFile(int Index); //写索引文件
	void PrintHead(FILE *fp); //打印标题信息
	//波形数据
	bool OpenWave(int Index); //打开波形通道
	bool WriteWave(float Depth,float *buf); //输出浮点波形数据
	bool WriteWave(float Depth,short *buf); //输出短整型波形数据
	bool WriteWave(float Depth,char *buf); //输出字节型波数据
	void CloseWave(); //关闭波形通道
	//阵列数据
	bool OpenArray(int Index); //打开阵列通道
	bool WriteArray(float Depth,float *buf); //读阵列数据
	void CloseArray(); //关闭阵列通道
private:
	bool WriteDataFile(int Index,int Num,float *buf); //写数据到盘
	DWORD GetChannelSize(int Index);
	void ClaerHead();
	CString RepCodeToStr(int RepCode);
	void WriteDataToFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,float *buffer);
	void WriteDataToFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,short *buffer);
	void WriteDataToFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,char *buffer);

public:
	CString sObjectPath; //输出工作路径
	CString m_sFidFile; //FID文件名
	CString m_sOutWellPath; //FID曲线数据所在目录
	CString m_sOutWellName; //井名
	FID_HEAD FidHead;
	FID_CHANNEL *FidChannel;

	CFile fw; //输出波形数据文件指针
	CString sWaveFile; //当前输出波形数据文件
	bool bWaveOpen; //输出波形文件是否已打开
	int IndexCurve; //当前曲线索引

};

#endif // !defined(AFX_FIDWRITE_H_INCLUDED_)
