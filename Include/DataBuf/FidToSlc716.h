// FidToSlc716.h: interface for the CFidToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOSLC716_H__
#define __FIDTOSLC716_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"

class CFidToSlc716  
{
public:
	CFidToSlc716();
	virtual ~CFidToSlc716();
	bool FidToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);

private:
	bool Transform();
	void GetSlc716HeadFromFid();
	void SetStartStopRlev();

private:
	CSlc716W *Slc716W; //SLC716�ļ����
	CFidRead *FidRead; //SKC98������
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	int m_nConvEncode; //�������߱��뷽ʽ
	float m_FixRlev; //�������߲������
	FILE *fp;
	CString sOutFileName; //��ǰ����ļ�
	CWordArray CurveIndex; //ѡ�����Դ�ļ��е�������
	CProgressDlg* pGuage;	
};

#endif 
