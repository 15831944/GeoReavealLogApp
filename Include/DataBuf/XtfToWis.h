// XtfToWis.h: interface for the CXtfToWis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTFTOWIS_H_INCLUDED_)
#define AFX_XTFTOWIS_H_INCLUDED_

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"

#include "WisWrite.h"
#include "XtfRead.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXtfToWis  
{
public:
	CXtfToWis();
	virtual ~CXtfToWis();
	bool XtfToWis(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE* fo);
	bool Transform();

private:
	void GetWisObjectIndexFromXtf();
	void SetWisChannelFromXtfChannel(int IndexXtf,int IndexWis);
	short XtfRepCToWis(short XtfRepC);
	CWisWrite *WisWrite; //输出类
	CXtfRead *XtfRead;

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	//CString m_sInFormat; //输入文件数据格式
	int m_nCurvesRadio; //过滤开关
	FILE *fp;
	double IndexSacle;
private:
	CString WisFileName; //当前输出文件
	//CStringArray SourceCurve; //选择的源曲线名
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_XTFTOWIS_H_INCLUDED_)
