// Skc98ToSlc716.h: interface for the CSkc98ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC98TOSLC716_H_INCLUDED_)
#define AFX_SKC98TOSLC716_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "Skc98R.h"

class CSkc98ToSlc716  
{
public:
	CSkc98ToSlc716();
	virtual ~CSkc98ToSlc716();
	bool Skc98ToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void Skc98ToSlc716Head();

private:
	CSlc716W *Slc716W; //SLC716�ļ����
	CSkc98R *Skc98R; //SKC98������
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
	CString sOutFileName; //��ǰ����ļ�
	CWordArray CurveIndex; //ѡ�����Դ�ļ��е�������
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_SKC98TOSLC716_H_INCLUDED_)
