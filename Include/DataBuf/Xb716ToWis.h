// Xb716ToWis.h: interface for the CXb716ToWis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XB716TOWIS_H_INCLUDED_)
#define AFX_XB716TOWIS_H_INCLUDED_

#include "CodeSP.h"
#include "ReCurNamDlg.h"
#include "ProgDlg.h"

#include "WisWrite.h"
#include "Xb716R.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXb716ToWis  
{
public:
	CXb716ToWis();
	virtual ~CXb716ToWis();
	bool Xb716ToWis(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE* fo);
	bool Transform();
	void GetWisObjectIndexFromXb716();
	void SetWisChannelFromXb716Head(int IndexFid,int IndexWis);


private:
	CWisWrite *WisWrite; //�����
	CXb716R *Xb716R;

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

#endif // !defined(AFX_XB716TOWIS_H_INCLUDED_)
