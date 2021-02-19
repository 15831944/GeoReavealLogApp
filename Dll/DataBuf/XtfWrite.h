// XtfWrite.h: interface for the CXtfWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTFWRITE_H_INCLUDED_)
#define AFX_XTFWRITE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Xtf_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CXtfWrite  
{
public:
	CXtfWrite();
	virtual ~CXtfWrite();
	bool Open(CString sDataFile); //打开输出文件
	void Close(); //关闭输出文件
	void InitXtfHeader(); //初始化文件头
	void WriteXtfHeader(); //输出文件头
	void WriteCurveHeader(int index); //输出曲线头
	void WriteGenData(int NumR,float *buf); //输出常规数据
	void WriteWave(float depth,float *buffer); //输出波数据
	void WriteWaveArray(float depth,float *buffer); //输出波形或阵列数据
	short XtfRepCodeToLen(short FidRepCode); //获取数据长度
	short XtfRepCodeToIntFlFix(short XtfRepCode); //取得代码类型
	short XtfRepCodeToSign(short XtfRepCode); //取得代码符号

	CURVE_HEAD XtfCurve;
	int Index; //当前曲线索引
	void PrintHead(FILE *fp);
	XTF_HEAD XtfHead; //头信息
	CString sXtfFile; //输出的XTF数据文件名

private:
	CFile fXtf;
	void PcXtfHead2Sun();
	void WriteHeader();
	void PcCurveHead2Sun();
	void WriteDataToXtf(DWORD pos,DWORD count,float *buffer);

};

#endif // !defined(AFX_XTFWRITE_H_INCLUDED_)
