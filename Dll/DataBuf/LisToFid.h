// LisToFid.h: interface for the CLisToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTOFID_H_INCLUDED_)
#define AFX_LISTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LisRead.h"
#include "FidWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CLisToFid  
{
public:
	CLisToFid();
	virtual ~CLisToFid();
	bool LisToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo);

private:
	bool Transform(); //转换控制
	void SetFidHeadFromLisHead(); //设置索引参数
	bool LogicalToFid(CString WellName); //转换一个逻辑文件

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;
	CString sProducerName; //公司名
	CString OriginOfdata; //数据来源

private:
	CLisRead *LisRead;
	CFidWrite *FidWrite;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;

	CProgressDlg* pGuage;

};

#endif // !defined(AFX_LISTOFID_H_INCLUDED_)
