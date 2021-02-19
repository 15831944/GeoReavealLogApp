// Slc716ToFid.h: interface for the CSlc716ToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716TOFID_H_INCLUDED_)
#define AFX_SLC716TOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FidWrite.h"
#include "Slc716R.h"
#include "ReCurNamDlg.h"

class CSlc716ToFid
{
public:
	CSlc716ToFid();
	virtual ~CSlc716ToFid();
	bool Slc716ToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void SetFidHead(); //���ñ�����Ϣ
	void DataNormalization(int NumPoint,int NumLog,float *buf); //���������

	CFidWrite *FidWrite; 
	CSlc716R *Slc716R;

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

#endif // !defined(AFX_SLC716TOFID_H_INCLUDED_)
