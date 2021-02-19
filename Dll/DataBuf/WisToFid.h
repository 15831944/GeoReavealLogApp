// WisToFid.h: interface for the CWisToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISTOFID_H_INCLUDED_)
#define AFX_WISTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FidWrite.h"
#include "WisRead.h"

class CWisToFid  
{
public:
	CWisToFid();
	virtual ~CWisToFid();
	bool WisToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //转换控制
	void SetFidHeadFromWisChannel(); //设置标题信息
	void SetFidChannelFromWisChannel(int index); //设置通道信息
	CFidWrite *FidWrite;
	CWisRead *WisRead;

private:

	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;

	CProgressDlg* pGuage;
};

#endif // !defined(AFX_WISTOFID_H_INCLUDED_)
