// LisToFid.h: interface for the CLisToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTOFID_H_INCLUDED_)
#define AFX_LISTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LisRead.h"
#include "FidWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CLisToFid  
{
public:
	CLisToFid();
	virtual ~CLisToFid();
	bool LisToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SetFidHeadFromLisHead(); //������������
	bool LogicalToFid(CString WellName); //ת��һ���߼��ļ�

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
	CString sProducerName; //��˾��
	CString OriginOfdata; //������Դ

private:
	CLisRead *LisRead;
	CFidWrite *FidWrite;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;

	CProgressDlg* pGuage;

};

#endif // !defined(AFX_LISTOFID_H_INCLUDED_)
