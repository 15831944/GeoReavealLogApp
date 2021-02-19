// FwdLasW.h: interface for the CFwdLasW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FWDLASW_H_INCLUDED_)
#define AFX_FWDLASW_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FwdLas_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CFwdLasW  
{
public:
	CFwdLasW();
	virtual ~CFwdLasW();
	FWDLAS_HEAD LasHd;
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
	FILE *fLas;
	void WriteData();
	void StartWrite();
	CString sMidFile;
	int BlockSize;
	CFile fTmp;
};

#endif // !defined(AFX_FWDLasW_H_INCLUDED_)
