// Slc716W.h: interface for the CSlc716W class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716W_H_INCLUDED_)
#define AFX_SLC716W_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Slc716_Head.h"

class AFX_EXT_CLASS CSlc716W  
{
public:
	CSlc716W();
	virtual ~CSlc716W();
	CFile fSlc716;
	bool Open(CString sDataFile); //打开输出文件
	void Close(); //关闭输出文件
	void ClearHead(); //清标题信息
	void ResetHead(); //标题信息始初化
	bool WriteHead(); //输出标题信息
	void WriteChannel(int Index,float *buf); //输出曲线数据
	void PrintHead(FILE *fp); //显示标题信息

	SLC716_HEAD Slc716Head;
	int NumPoint; //一条曲线的采样点数
	int NumBlock; //数据记录数
};

#endif // !defined(AFX_SLC716W_H_INCLUDED_)
