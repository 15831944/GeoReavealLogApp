// Slc716R.h: interface for the CSlc716R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716R_H__INCLUDED_)
#define AFX_SLC716R_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Slc716_Head.h"

class AFX_EXT_CLASS CSlc716R  
{
public:
	CSlc716R();
	virtual ~CSlc716R();
	CFile fSlc716;
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	bool ReadHead(); //读标题信息
	void ClearHead(); //清标题信息
	bool ReadChannel(int Index,float *buf); //读常规数据
	bool ReadChannel(CString sCurveName,float *buf); //读常规数据
	int GetCurveIndex(CString sCurveName); //获得曲线索引

	int FileSize; //文件大小
	SLC716_HEAD Slc716Head;
	int NumPoint; //一条曲线的采样点数
	int NumBlock; //数据数

};
bool _declspec(dllexport) IsSLC716(CString sDataFile);
#endif // !defined(AFX_SLC716R_H_INCLUDED_)
