// FidToQh716.h: interface for the CFidToQh716 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOQH716_H__
#define __FIDTOQH716_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "Qh716W.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"

class CFidToQh716  
{
public:
	CFidToQh716();
	virtual ~CFidToQh716();
	bool FidToQh716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);

private:
	bool Transform();
	void GetQh716HeadFromFid();
	void SetStartStopRlev();

private:
	CQh716W *Qh716W;
	CFidRead *FidRead;
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
