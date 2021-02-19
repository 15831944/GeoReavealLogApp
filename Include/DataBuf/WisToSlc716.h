// WisToSlc716.h: interface for the CWisToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISTOSLC716_H_INCLUDED_)
#define AFX_WISTOSLC716_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "Slc716W.h"
#include "WisRead.h"

class CWisToSlc716  
{
public:
	CWisToSlc716();
	virtual ~CWisToSlc716();
	bool WisToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo);
private:
	bool Transform();
	void SetSlc716Channel();
	void SelectChannelFromWin();
	void GetCurvesAndDepth();
	void SetStartStopRlev();
	CSlc716W *Slc716W; 
	CWisRead *WisRead;

private:

	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	CString sOutFileName; //����ļ���
	int m_nConvEncode;
	float m_FixRlev;
	FILE *fp;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_WISTOSLC716_H_INCLUDED_)
