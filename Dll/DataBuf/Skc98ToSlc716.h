// Skc98ToSlc716.h: interface for the CSkc98ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC98TOSLC716_H_INCLUDED_)
#define AFX_SKC98TOSLC716_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "Skc98R.h"

class CSkc98ToSlc716  
{
public:
	CSkc98ToSlc716();
	virtual ~CSkc98ToSlc716();
	bool Skc98ToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void Skc98ToSlc716Head();

private:
	CSlc716W *Slc716W; //SLC716文件输出
	CSkc98R *Skc98R; //SKC98输入类
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;
	CString sOutFileName; //当前输出文件
	CWordArray CurveIndex; //选择的在源文件中的索引号
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_SKC98TOSLC716_H_INCLUDED_)
