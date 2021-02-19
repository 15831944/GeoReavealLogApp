// Slc716ToSlc716.h: interface for the CSlc716ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716TOSLC716_H_INCLUDED_)
#define AFX_SLC716TOSLC716_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "Slc716R.h"

class CSlc716ToSlc716  
{
public:
	CSlc716ToSlc716();
	virtual ~CSlc716ToSlc716();
	bool Slc716ToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void Slc716HeadToSlc716Head();

private:
	CSlc716W *Slc716W; //SLC716�ļ����
	CSlc716R *Slc716R; //Slc716�ļ�����
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
	CString sOutFileName; //��ǰ����ļ�
	CWordArray CurveIndex; //ѡ�����Դ�ļ��е�������
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_SLC716TOSLC716_H_INCLUDED_)
