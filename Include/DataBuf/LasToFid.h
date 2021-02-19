// LasToFid.h: interface for the CLasToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LASTOFID_H_INCLUDED_)
#define AFX_LASTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ProgDlg.h"
#include "CodeSP.h"
#include "FidWrite.h"
#include "FwdLasR.h"
#include "ReCurNamDlg.h"

class CLasToFid
{
public:
	CLasToFid();
	virtual ~CLasToFid();
	bool LasToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //转换控制
	void SelectCurves(); //选择曲线
	void SetFidHead(); //设置标题信息
	void DataNormalization(int NumPoint,int NumLog,float *buf); //数据深度正规化

	CFidWrite *FidWrite; 
	CFwdLasR *FwdLasR;

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

#endif // !defined(AFX_LASTOFID_H_INCLUDED_)
