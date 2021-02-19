// XtfToWis.h: interface for the CXtfToWis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTFTOWIS_H_INCLUDED_)
#define AFX_XTFTOWIS_H_INCLUDED_

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"

#include "WisWrite.h"
#include "XtfRead.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXtfToWis  
{
public:
	CXtfToWis();
	virtual ~CXtfToWis();
	bool XtfToWis(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE* fo);
	bool Transform();

private:
	void GetWisObjectIndexFromXtf();
	void SetWisChannelFromXtfChannel(int IndexXtf,int IndexWis);
	short XtfRepCToWis(short XtfRepC);
	CWisWrite *WisWrite; //�����
	CXtfRead *XtfRead;

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	//CString m_sInFormat; //�����ļ����ݸ�ʽ
	int m_nCurvesRadio; //���˿���
	FILE *fp;
	double IndexSacle;
private:
	CString WisFileName; //��ǰ����ļ�
	//CStringArray SourceCurve; //ѡ���Դ������
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_XTFTOWIS_H_INCLUDED_)
