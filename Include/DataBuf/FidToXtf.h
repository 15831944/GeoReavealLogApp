// FidToXtf.h: interface for the CFidToXtf class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOXTF_H__
#define __FIDTOXTF_H__

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"
#include "FidRead.h"
#include "XtfWrite.h"

class CFidToXtf  
{
public:
	CFidToXtf();
	virtual ~CFidToXtf();
	bool FidToXtf(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fp);
	bool Transform();
	void GetXtfCurveIndexFromFid();
	void SetXtfHeaderFromFid();
	void SetXtfCurveFromFidHead(int IndexFid,int IndexXtf);
	short FidRepCodeToXtf(short FidRepCode);
	int GetCurveIndex(CString SelCurve);

private:
	CXtfWrite *XtfWrite; //输出类
	CFidRead *FidRead;

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;
private:
	CString XtfFileName; //当前输出文件
	CStringArray SourceCurve; //选择的源曲线名
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif 