// DLisToXtf.h: interface for the CDLisToXtf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISTOXTF_H_INCLUDED_)
#define AFX_DLISTOXTF_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DLisRead.h"
#include "XtfWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CDLisToXtf  
{
public:
	CDLisToXtf();
	virtual ~CDLisToXtf();
	bool DlisToXtf(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo);
	bool LogicalToXtf(CString WellName,int nf);

private:
	bool Transform();
	void SetXtfHeadFromDlisHead();
	bool DlisCurvesFilter();
	void SelectChannelFromWin();
	void SetXtfHeaderFromDlis();
	BYTE DlisRepCToXtf(BYTE RepCode);
	void FindFrameAndCurveIndex(const int index,int &nFrame,int &nCurve);
	void SetXtfCurveHead(int Index,int nChannel,int nFrame,int nCurve);

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	CString sOutFileName; //输出文件名
	int m_nCurvesRadio; //过滤开关
	FILE *fp;

private:
	CDlisRead *DlisRead;
	CXtfWrite *XtfWrite;

private:
	CStringArray NewCurveName;
	CProgressDlg* pGuage;

};

#endif // !defined(AFX_DLISTOXTF_H_INCLUDED_)
