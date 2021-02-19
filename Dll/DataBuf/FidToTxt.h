// FidToTxt.h: interface for the CFidToTxt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOTXT_H__
#define __FIDTOTXT_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FwdTxtW.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"

class CFidToTxt  
{
public:
	CFidToTxt();
	virtual ~CFidToTxt();
	bool FidToTxt(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);

private:
	bool Transform();
	void GetTxtHeadFromFid();
	void SetStartStopRlev();

private:
	CFwdTxtW *TxtW; //TXT文件输出
	CFidRead *FidRead; //SKC98输入类
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	int m_nConvEncode; //常规曲线编码方式
	float m_FixRlev; //常规曲线采样间隔
	CString sOutFileName; //当前输出文件
	CWordArray CurveIndex; //选择的在源文件中的索引号
	CProgressDlg* pGuage;
	FILE *fp;
};

#endif // !defined(AFX_FIDTOTXT_H_INCLUDED_)
