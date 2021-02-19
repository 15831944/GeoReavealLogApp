// FidToLas.h: interface for the CFidToLas class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOLAS_H__
#define __FIDTOLAS_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FwdLasW.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"

class CFidToLas  
{
public:
	CFidToLas();
	virtual ~CFidToLas();
	bool FidToLas(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);

private:
	bool Transform();
	void GetLasHeadFromFid();
	void SetStartStopRlev();

private:
	CFwdLasW *LasW; //Las文件输出
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

#endif // !defined(AFX_FIDTOLas_H_INCLUDED_)
