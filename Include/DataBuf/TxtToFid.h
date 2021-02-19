// TxtToFid.h: interface for the CTxtToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TXTTOFID_H_INCLUDED_)
#define AFX_TXTTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ProgDlg.h"
#include "CodeSP.h"
#include "FidWrite.h"
#include "FwdTxtR.h"
#include "ReCurNamDlg.h"

class CTxtToFid
{
public:
	CTxtToFid();
	virtual ~CTxtToFid();
	bool TxtToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void SetFidHead(); //���ñ�����Ϣ
	void DataNormalization(int NumPoint,int NumLog,float *buf); //����������滯

	CFidWrite *FidWrite; 
	CFwdTxtR *FwdTxtR;

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

#endif // !defined(AFX_TXTTOFID_H_INCLUDED_)
