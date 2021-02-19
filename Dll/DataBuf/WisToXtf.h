// WisToXtf.h: interface for the CWisToXtf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISTOXTF_H_INCLUDED_)
#define AFX_WISTOXTF_H_INCLUDED_

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"

#include "XtfWrite.h"
#include "WisRead.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWisToXtf  
{
public:
	CWisToXtf();
	virtual ~CWisToXtf();
	bool WisToXtf(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE* fo);
	bool Transform();

private:
	void GetChannelIndexFromWis();
	void GetChannelFromWin();
	void SetXtfHeaderFromWis();
	void SetXtfCurveFromWis(int IndexWis,int IndexXtf);
	BYTE WisRepCToXtf(WORD WisRepC);
	CXtfWrite *XtfWrite; //输出类
	CWisRead *WisRead;

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;
	double IndexSacle;
private:
	CString XtfFileName; //当前输出文件
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_WISTOXTF_H_INCLUDED_)
