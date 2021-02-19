// FidToFid.h: interface for the CFidToFid class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOFID_H__
#define __FIDTOFID_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FidWrite.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"


class CFidToFid  
{
public:
	CFidToFid();
	virtual ~CFidToFid();
	bool FidToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //转换控制
	void SetFidHeadFromFidHead(); //索引信息传送
	CFidWrite *FidWrite; 
	CFidRead *FidRead;

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
};

#endif // !defined(AFX_FIDTOFID_H_INCLUDED_)
