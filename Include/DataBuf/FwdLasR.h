// FwdLasR.h: interface for the CFwdLasR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FWDLASR_H_INCLUDED_)
#define AFX_FWDLASR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FwdLas_head.h"

class AFX_EXT_CLASS CFwdLasR  
{
public:
	CFwdLasR();
	virtual ~CFwdLasR();
	CStdioFile fFwdLas;
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	bool ReadHead(); //读标题信息
	int ReadCurve(float *buf); //读曲线数据
	void Prt_Header(FILE *fp); //打印标题信息

public:
	bool GetSubStr(const char *str,int &StartPos,char *SubStr);
	int FileSize; //文件大小,文件头行数
	FWDLAS_HEAD FwdLasHd;
private:
	float StringToFloat(CString sVal);
	void FwdLasHdReset();
	void GetCurveName(CString sVal);

};
bool _declspec(dllexport) IsFwdLas(CString sDataFile);

#endif // !defined(AFX_FWDLASR_H_INCLUDED_)
