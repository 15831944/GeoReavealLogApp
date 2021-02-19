// Skh2000ToFid.h: interface for the CSkh2000ToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKH2000TOFID_H_INCLUDED_)
#define AFX_SKH2000TOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "FidWrite.h"
#include "Skh2000R.h"

class CSkh2000ToFid  
{
public:
	CSkh2000ToFid();
	virtual ~CSkh2000ToFid();
	bool Skh2000ToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SetFidHeadFromSkh2000Head(); //���ñ�����Ϣ
	void SetFidChannelFromSkh2000(int IndexFid,int IndexSkh2000); //����ͨ����Ϣ

private:
	CFidWrite *FidWrite; 
	CSkh2000R *Skh2000R;

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_SKH2000TOFID_H_INCLUDED_)
