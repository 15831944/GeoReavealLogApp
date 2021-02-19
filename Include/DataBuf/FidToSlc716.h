// FidToSlc716.h: interface for the CFidToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOSLC716_H__
#define __FIDTOSLC716_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"

class CFidToSlc716  
{
public:
	CFidToSlc716();
	virtual ~CFidToSlc716();
	bool FidToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);

private:
	bool Transform();
	void GetSlc716HeadFromFid();
	void SetStartStopRlev();

private:
	CSlc716W *Slc716W; //SLC716文件输出
	CFidRead *FidRead; //SKC98输入类
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
