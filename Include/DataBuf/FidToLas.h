// FidToLas.h: interface for the CFidToLas class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOLAS_H__
#define __FIDTOLAS_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FwdLasW.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"

class CFidToLas  
{
public:
	CFidToLas();
	virtual ~CFidToLas();
	bool FidToLas(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);

private:
	bool Transform();
	void GetLasHeadFromFid();
	void SetStartStopRlev();

private:
	CFwdLasW *LasW; //Las�ļ����
	CFidRead *FidRead; //SKC98������
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	int m_nConvEncode; //�������߱��뷽ʽ
	float m_FixRlev; //�������߲������
	CString sOutFileName; //��ǰ����ļ�
	CWordArray CurveIndex; //ѡ�����Դ�ļ��е�������
	CProgressDlg* pGuage;
	FILE *fp;
};

#endif // !defined(AFX_FIDTOLas_H_INCLUDED_)
