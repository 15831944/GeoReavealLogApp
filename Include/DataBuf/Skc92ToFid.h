// Skc92ToFid.h: interface for the CSkc92ToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC92TOFID_H_INCLUDED_)
#define AFX_SKC92TOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "FidWrite.h"
#include "Skc92R.h"

class CSkc92ToFid  
{
public:
	CSkc92ToFid();
	virtual ~CSkc92ToFid();
	bool Skc92ToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);
	
private:
	bool Transform(); //ת������
	void SetFidHeadFromSkc92Head(); //���ñ�����Ϣ
	void SetFidChannelFromSkc92(int IndexFid,int IndexSkc92); //����ͨ������
	bool IsConvCurve(int index); //�ж��Ƿ�Ϊ��������
	bool IsCCLCurve(int index); //�ж��Ƿ�Ϊ�ӹ�����
	
private:
	CFidWrite *FidWrite; 
	CSkc92R *Skc92R;
	
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

#endif // !defined(AFX_SKC92TOFID_H_INCLUDED_)
