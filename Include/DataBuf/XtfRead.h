// XtfRead.h: interface for the CXtfRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTFREAD_H_INCLUDED_)
#define AFX_XTFREAD_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "Xtf_Head.h"

class AFX_EXT_CLASS CXtfRead  
{
public:
	CXtfRead();
	virtual ~CXtfRead();
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	void GetHead(); //读文件头信息
	int OpenCurve(CString sCurveName); //打开输入曲线
	int OpenCurve(int Index); //打开输入曲线
	int ReadCurveHead(); //读曲线头
	DWORD ReadCurve(int index, DWORD count, float* buffer); //读常规曲线数据
	bool ReadWave(int index,float depth,float *buffer); //读波形数据
	bool ReadWaveFromMatrix(int index,int subindex,float depth,float *buffer); //读波形或阵列数据
	bool ReadMatrix(int index,float depth,float *buffer); //读阵列数据

	DWORD FileSize; //文件大小
	XTF_HEAD XtfHead;
	CURVE_HEAD XtfCurve;
private:
	CFile fXtf;
	void ReadData(DWORD pos,DWORD count,float *buffer); //读数据通用函数
	void SunXtfHead2PC(); //文件头内码转换
	void SunCurveHead2PC(); //曲线头内码转换
	int CheckCurveHeader(); //检查曲线头是否合法
	void ClearHeaderPack(); //清除文件头中异常曲线


private:
	int GetCurveIndex(CString sCurveName);
	int index; //当前曲线索引号

};
bool _declspec(dllexport) IsXTF(CString sDataFile);

#endif // !defined(AFX_XTFREAD_H_INCLUDED_)
