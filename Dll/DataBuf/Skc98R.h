// Skc98R.h: interface for the CSkc98R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC98R_H_INCLUDED_)
#define AFX_SKC98R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Skc98_Head.h"

class AFX_EXT_CLASS CSkc98R  
{
public:
	CSkc98R();
	virtual ~CSkc98R();
	CFile fSkc98;
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	bool ReadHead(); //读标题块信息
	void ClearHead(); //清标题块信息
	bool ReadChannel(int Index,float *buf); //读常规曲线
	bool ReadVDL(int Index,float Depth,short *buf); //读变密度曲线
	bool IsConvCurve(int index); //是否为常规曲线

public:
	int FileSize; //文件大小
	SKC98_HEAD Skc98Head;
	int NumPoint; //一条曲线的采样点数
	int NumBlock; //数据记录数
	int nBlockSize; //一个块的大小
	int NumVdlSamp; //VDL曲线时间采样数
	float yStep[30]; //各曲线采样间隔，根据数据占用空间推算
};
bool _declspec(dllexport) IsSKC98(CString sDataFile);

#endif // !defined(AFX_SKC98R_H_INCLUDED_)
