// Qh716W.h: interface for the CQh716W class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QH716W_H_INCLUDED_)
#define AFX_QH716W_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Qh716_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CQh716W  
{
public:
	CQh716W();
	virtual ~CQh716W();
	CFile fQh716;
	bool Open(CString sDataFile); //打开输出文件
	void Close(); //关闭输出文件
	void ResetHead(); //头标题信息
	bool WriteHead(); //输出标题信息
	void WriteChannel(int Index,float *buf); //输出曲线数据
	void PrintHead(FILE *fp); //打印标题信息

	QH716_HEAD Head;
	int NumPoint; //一条曲线的采样点数
	int NumBlock; //数据记录数
	int BlockSize; //块的长度
	int FileSize; //文件大小
};

#endif // !defined(AFX_QH716W_H_INCLUDED_)
