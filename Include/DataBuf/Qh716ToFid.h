// Qh716ToFid.h: interface for the CQh716ToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QH716TOFID_H_INCLUDED_)
#define AFX_QH716TOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FidWrite.h"
#include "Qh716R.h"
#include "ReCurNamDlg.h"

class CQh716ToFid
{
public:
	CQh716ToFid();
	virtual ~CQh716ToFid();
	bool Qh716ToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //转换控制
	void SelectCurves(); //选择曲线
	void SetFidHead(); //设置索引
	void DataNormalization(int NumPoint,int NumLog,float *buf); //数据深度正常化

	CFidWrite *FidWrite; 
	CQh716R *Qh716R;

private:

	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;

private:
	CStringArray SelCurveName; //选择曲线名表
	CStringArray NewCurveName; //新曲线名表
	CStringArray NewUnits; ////新单位名表
	CWordArray CurveIndex; //源曲线索引号

	CProgressDlg* pGuage;
};

#endif // !defined(AFX_QH716TOFID_H_INCLUDED_)
