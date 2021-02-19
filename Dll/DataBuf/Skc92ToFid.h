// Skc92ToFid.h: interface for the CSkc92ToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC92TOFID_H_INCLUDED_)
#define AFX_SKC92TOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "FidWrite.h"
#include "Skc92R.h"

class CSkc92ToFid  
{
public:
	CSkc92ToFid();
	virtual ~CSkc92ToFid();
	bool Skc92ToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);
	
private:
	bool Transform(); //转换控制
	void SetFidHeadFromSkc92Head(); //设置标题信息
	void SetFidChannelFromSkc92(int IndexFid,int IndexSkc92); //设置通道索引
	bool IsConvCurve(int index); //判断是否为常规曲线
	bool IsCCLCurve(int index); //判断是否为接箍曲线
	
private:
	CFidWrite *FidWrite; 
	CSkc92R *Skc92R;
	
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

#endif // !defined(AFX_SKC92TOFID_H_INCLUDED_)
