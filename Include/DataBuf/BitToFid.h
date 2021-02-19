// BitToFid.h: interface for the CBitToFid class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BITTOFID_H__
#define __BITTOFID_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "FidWrite.h"
#include "BitR.h"

class CBitToFid  
{
public:
	CBitToFid();
	virtual ~CBitToFid();
	bool BitToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	void SetFidHeadFromBitHead(); //设置FID文件标题参数
	void SetFidChannelFromBit(int IndexFid,int IndexBit); //设置FID的通道参数
	bool Transform(); //数据转换程序

private:
	CFidWrite *FidWrite; 
	CBitR *BitR;

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

#endif // !defined(AFX_BITTOFID_H_INCLUDED_)
