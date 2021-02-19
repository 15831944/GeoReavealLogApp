// DLisToLis.h: interface for the CDLisToLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISTOLIS_H_INCLUDED_)
#define AFX_DLISTOLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DLisRead.h"
#include "LisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CDlisToLis  
{
public:
	CDlisToLis();
	virtual ~CDlisToLis();
	bool DlisToLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo);
	bool LogicalToLis(int nf);

private:
	bool Transform();
	void SetCurvesFromDlis();
	bool DlisCurvesFilter();
	void SetReelHeader();
	void SetFileHeader();
	int GetNumLog(int nFrame);
	void SetEB(int nFrame);

	BYTE PcCodeToLis(BYTE PcCode);
	void SelectChannelFromWin();

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	CString LisFileName; //LIS文件名
	FILE *fp;
	int NumLisLog; //输出LIS逻辑文件数

private:
	CDlisRead *DlisRead;
	CLisWrite *LisWrite;
	
private:
	CStringArray NewCurveName;
	CProgressDlg* pGuage;

};

#endif // !defined(AFX_DLISTOLIS_H_INCLUDED_)
