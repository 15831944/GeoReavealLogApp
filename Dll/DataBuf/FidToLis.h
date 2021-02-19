// FidToLis.h: interface for the CFidToLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIDTOLIS_H_INCLUDED_)
#define AFX_FIDTOLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "CodeSP.h"
#include "FidRead.h"
#include "LisWrite.h"
#include "FwdTxtW.h"

class CFidToLis  
{
public:
	CFidToLis();
	virtual ~CFidToLis();
	bool FidToLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void GetLisFileIndexFromFid();
	void Prt_CurveItemIndex();
	void SetReelHeader(); //设置卷头记录信息
	void SetFileHeader(int FileNo);
	void SetEB(int nf);
	int GetLogicalCurveIndex(int nf);
	unsigned char FidRepCodeToLis(unsigned char RepCode);
	void WriteDataToLis(int nf);
	void SelectCurves();
	void Prt_FileInfo(int nf);
private:
	CWordArray CurveItem; //分组曲线索引，其结构为：组数->第１组项数及索引值->第２组项数及索引...


private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;

private:
	CFidRead *FidRead;
	CLisWrite *LisWrite;

private:
	CString LisFileName; //当前输出文件
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;

};

#endif // !defined(AFX_FIDTOLIS_H_INCLUDED_)
