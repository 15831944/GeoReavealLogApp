// Xb716ToFid.h: interface for the CXb716ToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XB716TOFID_H_INCLUDED_)
#define AFX_XB716TOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FidWrite.h"
#include "Xb716R.h"
#include "ReCurNamDlg.h"

class CXb716ToFid
{
public:
	CXb716ToFid();
	virtual ~CXb716ToFid();
	bool Xb716ToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void SetFidHead(); //���ñ�����Ϣ
	void DataNormalization(int NumPoint,int NumLog,float *buf); //����������滯

	CFidWrite *FidWrite; 
	CXb716R *Xb716R;

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

#endif // !defined(AFX_XB716TOFID_H_INCLUDED_)
