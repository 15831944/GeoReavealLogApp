// Qh716R.h: interface for the CQh716R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QH716R_H_INCLUDED_)
#define AFX_QH716R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Qh716_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CQh716R  
{
public:
	CQh716R();
	virtual ~CQh716R();
	int FileSize; //文件大小
	QH716_HEAD Head;
	CFile fQh716;
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	bool ReadHead(); //读标题信息
	void ReadCurve(int index,float *buf); //读曲线数据
	void Prt_Header(FILE *fp); //打印标题信息

public:
	int NumPoint; //点数
	int NumBlock; //块数
	int BlockSize; //块长
	int HeadSize; //文件头长度

};
bool _declspec(dllexport) IsQH716(CString sDataFile);

#endif // !defined(AFX_QH716R_H_INCLUDED_)
