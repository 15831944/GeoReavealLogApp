// Skc92R.h: interface for the CSkc92R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC92R_H_INCLUDED_)
#define AFX_SKC92R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Skc92_Head.h"

class AFX_EXT_CLASS CSkc92R  
{
public:
	CSkc92R();
	virtual ~CSkc92R();
	CFile fSkc92;
	bool Open(CString sDataFile); //打开输入文件
	void Close();  //关闭输入文件
	bool ReadHead(); //读标题块信息
	void ClearHead(); //清标题块信息
	bool ReadChannel(int Index,float *buf); //读常规曲线数据
	bool ReadVDL(int Index,float Depth,short *buf); //读波数据
	bool IsCCLHasFourPoint(); //判断CCL曲线是否有加密采样
	bool ExistVDL(); //是否存在VDL曲线

public:
	int FileSize; //文件大小
	SKC92_HEAD Skc92Head;
	int NumPoint; //一条曲线的采样点数
	int NumBlock; //数据记录数
	int nBlockSize; //一个块的大小
	int CurveTypes[20]; //一个采样点的字节数
	bool bVdl; //VDL测井且存在VDL曲线
	bool bCCLFour; //CCL曲线是否加密采样

};
bool _declspec(dllexport) IsSKC92(CString sDataFile);

#endif // !defined(AFX_SKC92R_H_INCLUDED_)
