// DLisToFid.h: interface for the CDLisToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISTOFID_H_INCLUDED_)
#define AFX_DLISTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DLisRead.h"
#include "FidWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CDLisToFid  
{
public:
	CDLisToFid();
	virtual ~CDLisToFid();
	bool DLisToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,BOOL bCurveSplit,BOOL bDelSplitCur,FILE *fo);

private:
	bool Transform(); //转换控制程序
	bool LogicalToFid(CString WellName,int nf); //转换一个逻辑文件
	void SetFidHeadFromDlisHead(); //设置FID文件索引信息
	bool DlisCurvesFilter(); //选择曲线
	BYTE PcCodeToFid(BYTE PcCode); //表示码转换
	void SelectChannelFromWin(); //交互式选择曲线、修改曲线信息

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	BOOL m_bCurveSplit; //阵列曲线拆分
	BOOL m_bDelSplitCur; //删除被拆分曲线
	FILE *fp;

private:
	CDlisRead *DlisRead;
	CFidWrite *FidWrite;

private:
	CStringArray NewCurveName;
	CProgressDlg* pGuage;

};

#endif // !defined(AFX_DLISTOFID_H_INCLUDED_)
