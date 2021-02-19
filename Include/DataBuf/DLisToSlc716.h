// DLisToSlc716.h: interface for the CDLisToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISTOSLC716_H_INCLUDED_)
#define AFX_DLISTOSLC716_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DLisRead.h"
#include "Slc716W.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CDLisToSlc716  
{
public:
	CDLisToSlc716();
	virtual ~CDLisToSlc716();
	bool DLisToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);
	bool LogicalToSlc716(CString WellName,int nf);

private:
	bool Transform();
	void SetSlc716HeadFromDlisHead();
	bool DlisCurvesFilter();
	void SelectChannelFromWin();
	void SetStartStopRlev(); //��������ָ����ȼ��������

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	CString sOutFileName; //����ļ���
	int m_nCurvesRadio; //���˿���
	int m_nConvEncode; //�������߱��뷽ʽ
	float m_FixRlev; //�������߲������
	FILE *fp;

private:
	CDlisRead *DlisRead;
	CSlc716W *Slc716W;

private:
	CStringArray NewCurveName;
	CProgressDlg* pGuage;

};

#endif // !defined(AFX_DLISTOSLC716_H_INCLUDED_)
