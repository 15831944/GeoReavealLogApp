// Slc716ToWis.h: interface for the CSlc716ToWis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716TOWIS_H_INCLUDED_)
#define AFX_SLC716TOWIS_H_INCLUDED_

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"

#include "WisWrite.h"
#include "Slc716R.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSlc716ToWis  
{
public:
	CSlc716ToWis();
	virtual ~CSlc716ToWis();
	bool Slc716ToWis(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE* fo);
	bool Transform();
	void GetWisObjectIndexFromSlc716();
	void SetWisChannelFromSlc716Head(int IndexFid,int IndexWis);


private:
	CWisWrite *WisWrite; //�����
	CSlc716R *Slc716R;

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

#endif // !defined(AFX_SLC716TOWIS_H_INCLUDED_)
