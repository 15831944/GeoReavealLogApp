// XtfToFid.h: interface for the CXtfToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTFTOFID_H_INCLUDED_)
#define AFX_XTFTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "FidWrite.h"
#include "XtfRead.h"

class CXtfToFid  
{
public:
	CXtfToFid();
	virtual ~CXtfToFid();
	bool XtfToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SetFidHeadFromXtfHead(); //���ñ�����Ϣ
	void SetFidChannelFromXtfChannel(int index); //����ͨ����Ϣ

private:
	CFidWrite *FidWrite; 
	CXtfRead *XtfRead;

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
	float IndexSacle; //��Ȼ�������

};

#endif // !defined(AFX_XTFTOFID_H_INCLUDED_)
