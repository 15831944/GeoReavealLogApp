// LasToFid.h: interface for the CLasToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LASTOFID_H_INCLUDED_)
#define AFX_LASTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ProgDlg.h"
#include "CodeSP.h"
#include "FidWrite.h"
#include "FwdLasR.h"
#include "ReCurNamDlg.h"

class CLasToFid
{
public:
	CLasToFid();
	virtual ~CLasToFid();
	bool LasToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void SetFidHead(); //���ñ�����Ϣ
	void DataNormalization(int NumPoint,int NumLog,float *buf); //����������滯

	CFidWrite *FidWrite; 
	CFwdLasR *FwdLasR;

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

#endif // !defined(AFX_LASTOFID_H_INCLUDED_)
