// BitR.h: interface for the CBitR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITR_H_INCLUDED_)
#define AFX_BITR_H_INCLUDED_

#include "Bit_Head.h"
#include "CodeSP.h"
#include <math.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class _declspec(dllexport) CBitR  
{
public:
	CBitR();
	virtual ~CBitR();
	CFile fBit;
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	bool ReadHead(); //读标题信息
	void ClearHead(); //清标题信息
	bool ReadChannel(int Index,float *buf); //读常规数据
	bool ReadWaveform(float Depth,char *buf); //读全部波形数据
	bool ReadWaveform(int nWave,float Depth,char *buf); //读一条波形数据


	int FileSize; //文件大小
	BIT_HEAD BitHead;
	int NumPoint; //一条曲线的采样点数
	int NumBlock; //数据记录数
	int nBlockSize; //一个块的大小
	bool bWaveform; //是否为波形数据，1表示波形
	int NumConvLog;//常规曲线数
	int NumWaveLog; //波形曲线数

};
bool _declspec(dllexport) IsBIT(CString sDataFile);

#endif // !defined(AFX_BITR_H_INCLUDED_)
