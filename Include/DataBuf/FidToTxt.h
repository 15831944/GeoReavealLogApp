// FidToTxt.h: interface for the CFidToTxt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FIDTOTXT_H__
#define __FIDTOTXT_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FwdTxtW.h"
#include "FidRead.h"
#include "ReCurNamDlg.h"

class CFidToTxt  
{
public:
	CFidToTxt();
	virtual ~CFidToTxt();
	bool FidToTxt(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);

private:
	bool Transform();
	void GetTxtHeadFromFid();
	void SetStartStopRlev();

private:
	CFwdTxtW *TxtW; //TXT�ļ����
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

#endif // !defined(AFX_FIDTOTXT_H_INCLUDED_)
