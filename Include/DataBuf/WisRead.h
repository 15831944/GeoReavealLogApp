// WisRead.h: interface for the CWisRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISREAD_H_INCLUDED_)
#define AFX_WISREAD_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wis_Head.h"

class AFX_EXT_CLASS CWisRead  
{
public:
	CWisRead();
	virtual ~CWisRead();
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	bool GetHead(); //读文件头信息
	bool GetObjectEntry(); //读对象入口信息
	int OpenChannel(const char *name); //读通道信息
	DWORD ReadCurve(int index, float depth, DWORD count, float* buffer); //读常规曲线数据
	bool ReadWave(int index,float depth,float *buffer); //读波形数据
	bool ReadWaveArray(int index,float depth,float *buffer); //读阵列数据
	int GetCurveDim(int index); //取得通道的维数
	void Prt_HeaderInfo(FILE *fp); //打印文件头信息
	void Prt_ObjectInfo(FILE *fp); //打印对象信息
	void Prt_ChannelInfo(FILE *fp); //打印通道信息
	void Prt_TableInfo(FILE *fp); //打印通道信息
	void Prt_StreamInfo(FILE *fp); //打印通道信息
	int FileSize; //文件大小
	WIS_HEAD WisHead; //头信息
	WIS_OBJECT_ENTRY* m_ObjectEntry; //对象入口
	WIS_CHANNEL WisChannel; //通道信息
private:
	CFile fWis;
	DWORD ReadData(DWORD pos,int RepCode,DWORD count,float *buffer); //读数据公用函数

};
bool _declspec(dllexport) IsWIS(CString sDataFile);

#endif // !defined(AFX_WISREAD_H_INCLUDED_)
