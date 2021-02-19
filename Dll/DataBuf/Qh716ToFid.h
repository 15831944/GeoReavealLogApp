// Qh716ToFid.h: interface for the CQh716ToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QH716TOFID_H_INCLUDED_)
#define AFX_QH716TOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FidWrite.h"
#include "Qh716R.h"
#include "ReCurNamDlg.h"

class CQh716ToFid
{
public:
	CQh716ToFid();
	virtual ~CQh716ToFid();
	bool Qh716ToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void SetFidHead(); //��������
	void DataNormalization(int NumPoint,int NumLog,float *buf); //�������������

	CFidWrite *FidWrite; 
	CQh716R *Qh716R;

private:

	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;

private:
	CStringArray SelCurveName; //ѡ����������
	CStringArray NewCurveName; //����������
	CStringArray NewUnits; ////�µ�λ����
	CWordArray CurveIndex; //Դ����������

	CProgressDlg* pGuage;
};

#endif // !defined(AFX_QH716TOFID_H_INCLUDED_)
