// Skc92ToSlc716.h: interface for the CSkc92ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC92TOSLC716_H_INCLUDED_)
#define AFX_SKC92TOSLC716_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "Skc92R.h"


class CSkc92ToSlc716  
{
public:
	CSkc92ToSlc716();
	virtual ~CSkc92ToSlc716();
	bool Skc92ToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void Skc92HeadToSlc716Head();

private:
	CSlc716W *Slc716W;
	CSkc92R *Skc92R;
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
	CString sOutFileName; //��ǰ����ļ�
	CWordArray CurveIndex; //ѡ�����Դ�ļ��е�������
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_SKC92TOSLC716_H_INCLUDED_)
