// FidRead.h: interface for the CFidRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIDREAD_H_INCLUDED_)
#define AFX_FIDREAD_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Fid_Head.h"

class AFX_EXT_CLASS CFidRead  
{
public:
	CFidRead();
	virtual ~CFidRead();
	bool ReadFidInfo(const CString FidFileName); //读FID文件的索引信息
	bool ReWriteFidFromData(const CString FidFileName); //根据目录曲线信息重写FID文件
	bool ReadFidChannelInfo(int Index,CString sCurveName); //读通道信息
	bool ReadGenData(const CString sCurveName,int NumR,float *buf); //读常规曲线数据
	bool OpenWave(CString sCurveName); //打开波形数据
	bool ReadWave(float Depth,float *buf); //读波形数据一个采样点数据
	void CloseWave(); //关闭波形文件
	bool OpenArray(CString sCurveName); //打开阵列数据
	bool ReadArray(float Depth,float *buf); //读阵列数据一个采样点数据
	void CloseArray(); //关闭阵列文件
	bool IsIndexFile(CString sCurIndexFile); //判断文件是否为索引文件
	int GetIndexFromCurveName(const CString sCurveName); //查询对应曲线的名索引号
	void CleanArrayCurve(); //清除阵列曲线
	void CleanWaveCurve(); //清除波形曲线
	void CleanConvCurve(); //清除常规曲线
private:
	void ReadDataFromFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,float *buffer); //从数据文件中读对应类型的数据
	void InitFidChannel(); //初始化曲线信息
	void ResetHead();
	bool ReadIndex(); //找出目录下所有曲线索引
	int RepCodeStrToInt(CString sRepCode,int CodeLen); //用串表示的表示码转换为整型表示法

public:
	CString m_sInWellPath; //FID曲线数据所在目录
	CString m_sInWellName; //当前读取数据的井名
	FID_HEAD FidHead; //文件头信息
	FID_CHANNEL *FidChannel; //曲线头信息

	CString sWaveFile; //当前读取波形数据文件
	bool bWaveOpen; //波形文件是否已被打开
	int IndexCurve; //当前曲线索引
	CFile fR; //读取数据文件指针
};
bool _declspec(dllexport) IsFID(CString sDataFile);
void _declspec(dllexport) GetorkSpace(CString &m_sWorkSpace);

#endif // !defined(AFX_FIDREAD_H_INCLUDED_)
