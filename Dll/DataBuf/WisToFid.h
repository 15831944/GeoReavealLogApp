// WisToFid.h: interface for the CWisToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISTOFID_H_INCLUDED_)
#define AFX_WISTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "ProgDlg.h"
#include "FidWrite.h"
#include "WisRead.h"

class CWisToFid  
{
public:
	CWisToFid();
	virtual ~CWisToFid();
	bool WisToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	bool Transform(); //ת������
	void SetFidHeadFromWisChannel(); //���ñ�����Ϣ
	void SetFidChannelFromWisChannel(int index); //����ͨ����Ϣ
	CFidWrite *FidWrite;
	CWisRead *WisRead;

private:

	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;

	CProgressDlg* pGuage;
};

#endif // !defined(AFX_WISTOFID_H_INCLUDED_)
