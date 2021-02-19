// BitToSlc716.h: interface for the CBitToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BITTOSLC716_H__
#define __BITTOSLC716_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "BitR.h"

class CBitToSlc716  
{
public:
	CBitToSlc716();
	virtual ~CBitToSlc716();
	bool BitToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void BitHeadToSlc716Head();

private:
	CSlc716W *Slc716W; //SLC716文件输出
	CBitR *BitR; //Slc716文件输入
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;
	CString sOutFileName; //当前输出文件
	CWordArray CurveIndex; //选择的在源文件中的索引号
	CProgressDlg* pGuage;
};

#endif 