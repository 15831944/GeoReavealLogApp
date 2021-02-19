// Skc98ToFid.h: interface for the CSkc98ToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC98TOFID_H_INCLUDED_)
#define AFX_SKC98TOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "FidWrite.h"
#include "Skc98R.h"

class CSkc98ToFid  
{
public:
	CSkc98ToFid();
	virtual ~CSkc98ToFid();
	bool Skc98ToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //转换控制
	void SetFidHeadFromSkc98Head(); //设置标题信息
	void SetFidChannelFromSkc98(int IndexFid,int IndexSkc98); //设置曲线索引
	bool IsConvCurve(int index); //判断是否为常规曲线

private:
	CFidWrite *FidWrite; 
	CSkc98R *Skc98R;

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

#endif // !defined(AFX_SKC98TOFID_H_INCLUDED_)
