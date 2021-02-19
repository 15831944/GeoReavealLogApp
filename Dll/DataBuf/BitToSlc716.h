// BitToSlc716.h: interface for the CBitToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BITTOSLC716_H__
#define __BITTOSLC716_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "BitR.h"

class CBitToSlc716  
{
public:
	CBitToSlc716();
	virtual ~CBitToSlc716();
	bool BitToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void BitHeadToSlc716Head();

private:
	CSlc716W *Slc716W; //SLC716�ļ����
	CBitR *BitR; //Slc716�ļ�����
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
	CString sOutFileName; //��ǰ����ļ�
	CWordArray CurveIndex; //ѡ�����Դ�ļ��е�������
	CProgressDlg* pGuage;
};

#endif 