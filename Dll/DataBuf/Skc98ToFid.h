// Skc98ToFid.h: interface for the CSkc98ToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC98TOFID_H_INCLUDED_)
#define AFX_SKC98TOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "FidWrite.h"
#include "Skc98R.h"

class CSkc98ToFid  
{
public:
	CSkc98ToFid();
	virtual ~CSkc98ToFid();
	bool Skc98ToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SetFidHeadFromSkc98Head(); //���ñ�����Ϣ
	void SetFidChannelFromSkc98(int IndexFid,int IndexSkc98); //������������
	bool IsConvCurve(int index); //�ж��Ƿ�Ϊ��������

private:
	CFidWrite *FidWrite; 
	CSkc98R *Skc98R;

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_SKC98TOFID_H_INCLUDED_)
