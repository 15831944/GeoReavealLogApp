// XtfToFid.h: interface for the CXtfToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTFTOFID_H_INCLUDED_)
#define AFX_XTFTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "FidWrite.h"
#include "XtfRead.h"

class CXtfToFid  
{
public:
	CXtfToFid();
	virtual ~CXtfToFid();
	bool XtfToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //转换控制
	void SetFidHeadFromXtfHead(); //设置标题信息
	void SetFidChannelFromXtfChannel(int index); //设置通道信息

private:
	CFidWrite *FidWrite; 
	CXtfRead *XtfRead;

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
	float IndexSacle; //深度换算因子

};

#endif // !defined(AFX_XTFTOFID_H_INCLUDED_)
