// FwdTxtW.h: interface for the CFwdTxtW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FWDTXTW_H_INCLUDED_)
#define AFX_FWDTXTW_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FwdTxt_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CFwdTxtW  
{
public:
	CFwdTxtW();
	virtual ~CFwdTxtW();
	FWDTXT_HEAD TxtHd;
	bool Open(CString sDataFile); //打开输出文件
	void Close(); //关闭输出文件
	void WriteHead(); //输出标题信息
	void WriteChannel(int index,float *buf); //输出曲线数据
	void Prt_Header(FILE *fp); //打印标题信息
	void ResetHead(); //标题信息初始化
public:
	int FileSize; //文件大小
	CString m_sFileName;
private:
	FILE *fTxt;
	void WriteData();
	void StartWrite();
	CString sMidFile;
	int BlockSize;
	CFile fTmp;
};

#endif // !defined(AFX_FWDTXTW_H_INCLUDED_)
