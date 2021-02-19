// Xb716R.h: interface for the CQb716R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XB716R_H_INCLUDED_)
#define AFX_XB716R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Xb716_Head.h"

class AFX_EXT_CLASS CXb716R  
{
public:
	CXb716R();
	virtual ~CXb716R();
	XB716_HEAD Head;
	int FileSize; //文件大小
	CFile fXb716;
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	bool ReadHead(); //读标题信息
	void Prt_Header(FILE *fp); //打印标题信息
	void ReadCurve(int index,float *buf); //读曲线数据

public:
	int NumPoint; //点数
	int NumBlock; //块数
	int BlockSize; //块长
	int HeadSize; //文件头长度

};
bool _declspec(dllexport) IsXB716(CString sDataFile);

#endif // !defined(AFX_XB716R_H_INCLUDED_)
