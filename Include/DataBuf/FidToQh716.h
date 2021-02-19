// FidToQh716.h: interface for the CFidToQh716 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOQH716_H__
#define __FIDTOQH716_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "Qh716W.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"

class CFidToQh716  
{
public:
	CFidToQh716();
	virtual ~CFidToQh716();
	bool FidToQh716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);

private:
	bool Transform();
	void GetQh716HeadFromFid();
	void SetStartStopRlev();

private:
	CQh716W *Qh716W;
	CFidRead *FidRead;
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	int m_nConvEncode; //常规曲线编码方式
	float m_FixRlev; //常规曲线采样间隔
	FILE *fp;
	CString sOutFileName; //当前输出文件
	CWordArray CurveIndex; //选择的在源文件中的索引号
	CProgressDlg* pGuage;	
};

#endif 
