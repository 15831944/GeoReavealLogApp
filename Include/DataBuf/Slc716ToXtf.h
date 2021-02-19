// Slc716ToXtf.h: interface for the CSlc716ToXtf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716TOXTF_H_INCLUDED_)
#define AFX_SLC716TOXTF_H_INCLUDED_

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"
#include "XtfWrite.h"
#include "Slc716R.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSlc716ToXtf  
{
public:
	CSlc716ToXtf();
	virtual ~CSlc716ToXtf();
	bool Slc716ToXtf(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE* fo);
	bool Transform();

private:
	void GetXtfCurveIndexFromSlc716();
	int GetCurveIndex(CString SelCurve);
	void SetXtfHeaderFromSlc716();
	void SetXtfCurveFromSlc716Head(int IndexFid,int IndexXtf);
	CXtfWrite *XtfWrite; //�����
	CSlc716R *Slc716R;

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
private:
	CString XtfFileName; //��ǰ����ļ�
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_SLC716TOXTF_H_INCLUDED_)
