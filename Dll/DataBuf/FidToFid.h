// FidToFid.h: interface for the CFidToFid class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOFID_H__
#define __FIDTOFID_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FidWrite.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"


class CFidToFid  
{
public:
	CFidToFid();
	virtual ~CFidToFid();
	bool FidToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SetFidHeadFromFidHead(); //������Ϣ����
	CFidWrite *FidWrite; 
	CFidRead *FidRead;

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

#endif // !defined(AFX_FIDTOFID_H_INCLUDED_)
