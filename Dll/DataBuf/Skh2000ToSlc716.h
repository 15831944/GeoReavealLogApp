// Skh2000ToSlc716.h: interface for the CSkh2000ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKH2000TOSLC716_H_INCLUDED_)
#define AFX_SKH2000TOSLC716_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "Skh2000R.h"

class CSkh2000ToSlc716  
{
public:
	CSkh2000ToSlc716();
	virtual ~CSkh2000ToSlc716();
	bool Skh2000ToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void Skh2000ToSlc716Head();

private:
	CSlc716W *Slc716W; //SLC716�ļ����
	CSkh2000R *Skh2000R; //SKC98������
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
	CString sOutFileName; //��ǰ����ļ�
	CWordArray CurveIndex; //ѡ�����Դ�ļ��е�������
	CProgressDlg* pGuage;	
};

#endif // !defined(AFX_SKH2000TOSLC716_H_INCLUDED_)
