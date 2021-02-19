// WisToTxt.h: interface for the CWisToTxt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISTOTXT_H_INCLUDED_)
#define AFX_WISTOTXT_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FwdTxtW.h"
#include "WisRead.h"

class CWisToTxt  
{
public:
	CWisToTxt();
	virtual ~CWisToTxt();
	bool WisToTxt(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);
private:
	bool Transform();
	void SetTxtChannel();
	void SelectChannelFromWin();
	void GetCurvesAndDepth();
	void SetStartStopRlev();
	CFwdTxtW *TxtW; 
	CWisRead *WisRead;

private:

	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	CString sOutFileName; //输出文件名
	int m_nConvEncode;
	float m_FixRlev;
	FILE *fp;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_WISTOTXT_H_INCLUDED_)
