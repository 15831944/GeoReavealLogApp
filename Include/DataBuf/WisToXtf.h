// WisToXtf.h: interface for the CWisToXtf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISTOXTF_H_INCLUDED_)
#define AFX_WISTOXTF_H_INCLUDED_

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"

#include "XtfWrite.h"
#include "WisRead.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWisToXtf  
{
public:
	CWisToXtf();
	virtual ~CWisToXtf();
	bool WisToXtf(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE* fo);
	bool Transform();

private:
	void GetChannelIndexFromWis();
	void GetChannelFromWin();
	void SetXtfHeaderFromWis();
	void SetXtfCurveFromWis(int IndexWis,int IndexXtf);
	BYTE WisRepCToXtf(WORD WisRepC);
	CXtfWrite *XtfWrite; //�����
	CWisRead *WisRead;

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
	double IndexSacle;
private:
	CString XtfFileName; //��ǰ����ļ�
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_WISTOXTF_H_INCLUDED_)
