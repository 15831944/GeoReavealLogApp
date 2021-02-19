// Skh2000R.h: interface for the CSkh2000R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKH2000R_H_INCLUDED_)
#define AFX_SKH2000R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Skh2000_Head.h"

class AFX_EXT_CLASS CSkh2000R  
{
public:
	CSkh2000R();
	virtual ~CSkh2000R();
	CFile fSkh2000;
	bool Open(CString sDataFile); //打开输入文件
	void Close();//关闭输入文件
	bool ReadHead(); //读标题块信息
	void ClearHead(); //清标题信息
	bool ReadChannel(int Index,float *buf); //读常规曲线
	bool ReadWAVE(float Depth,short *buf); //读所有波形曲线
	bool ReadWAVE(int index,float Depth,short *buf);//读一条波曲线

	int FileSize; //文件大小
	SKH2000_HEAD Skh2000Head;
	int NumPoint; //一条曲线的采样点数
	int NumBlock; //数据记录数
	int nBlockSize; //一个块的大小
	int nHeadSize; //标题块大小
	int nTrailSize; //文件尾的大小
	BOOL bIsWAVE; //文件数据类型
};
bool _declspec(dllexport) IsSKH2000(CString sDataFile);

#endif // !defined(AFX_SKH2000R_H_INCLUDED_)
