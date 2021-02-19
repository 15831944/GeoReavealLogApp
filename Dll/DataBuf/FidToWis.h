// FidToWis.h: interface for the CFidToWis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIDTOWIS_H_INCLUDED_)
#define AFX_FIDTOWIS_H_INCLUDED_

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"

#include "WisWrite.h"
#include "FidRead.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFidToWis  
{
public:
	CFidToWis();
	virtual ~CFidToWis();
	bool FidToWis(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE* fo);
	bool Transform();
	void GetWisObjectIndexFromFid();
	void SetWisChannelFromFidHead(int IndexFid,int IndexWis);
	int GetCurveIndex(CString SelCurve);


private:
	CWisWrite *WisWrite; //�����
	CFidRead *FidRead;

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	//CString m_sInFormat; //�����ļ����ݸ�ʽ
	int m_nCurvesRadio; //���˿���
	FILE *fp;
private:
	CString WisFileName; //��ǰ����ļ�
	//CStringArray SourceCurve; //ѡ���Դ������
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_FIDTOWIS_H_INCLUDED_)
