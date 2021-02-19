// FidToXtf.h: interface for the CFidToXtf class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOXTF_H__
#define __FIDTOXTF_H__

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"
#include "FidRead.h"
#include "XtfWrite.h"

class CFidToXtf  
{
public:
	CFidToXtf();
	virtual ~CFidToXtf();
	bool FidToXtf(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fp);
	bool Transform();
	void GetXtfCurveIndexFromFid();
	void SetXtfHeaderFromFid();
	void SetXtfCurveFromFidHead(int IndexFid,int IndexXtf);
	short FidRepCodeToXtf(short FidRepCode);
	int GetCurveIndex(CString SelCurve);

private:
	CXtfWrite *XtfWrite; //�����
	CFidRead *FidRead;

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
private:
	CString XtfFileName; //��ǰ����ļ�
	CStringArray SourceCurve; //ѡ���Դ������
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif 