// Slc716ToWis.h: interface for the CSlc716ToWis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716TOWIS_H_INCLUDED_)
#define AFX_SLC716TOWIS_H_INCLUDED_

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"

#include "WisWrite.h"
#include "Slc716R.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSlc716ToWis  
{
public:
	CSlc716ToWis();
	virtual ~CSlc716ToWis();
	bool Slc716ToWis(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE* fo);
	bool Transform();
	void GetWisObjectIndexFromSlc716();
	void SetWisChannelFromSlc716Head(int IndexFid,int IndexWis);


private:
	CWisWrite *WisWrite; //输出类
	CSlc716R *Slc716R;

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	//CString m_sInFormat; //输入文件数据格式
	int m_nCurvesRadio; //过滤开关
	FILE *fp;
private:
	CString WisFileName; //当前输出文件
	//CStringArray SourceCurve; //选择的源曲线名
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_SLC716TOWIS_H_INCLUDED_)
