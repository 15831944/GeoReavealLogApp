// FwdTxtR.h: interface for the CFwdTxtR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FWDTXTR_H_INCLUDED_)
#define AFX_FWDTXTR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FwdTxt_head.h"

class AFX_EXT_CLASS CFwdTxtR  
{
public:
	CFwdTxtR();
	virtual ~CFwdTxtR();
	CStdioFile fFwdTxt;
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	bool ReadHead(); //读标题信息
	int ReadCurve(float *buf); //读曲线数据
	void Prt_Header(FILE *fp); //打印标题信息

public:
	bool GetSubStr(const char *str,int &StartPos,char *SubStr);
	int FileSize; //文件大小,文件头行数
	FWDTXT_HEAD FwdTxtHd;
private:
	float StringToFloat(CString sVal);
	void FwdTxtHdReset();
	void GetCurveName(CString sVal);

};
bool _declspec(dllexport) IsFwdTxt(CString sDataFile);

#endif // !defined(AFX_FWDTXTR_H_INCLUDED_)
