// DLisToWis.h: interface for the CDLisToWis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISTOWIS_H_INCLUDED_)
#define AFX_DLISTOWIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DLisRead.h"
#include "WisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CDLisToWis  
{
public:
	CDLisToWis();
	virtual ~CDLisToWis();
	bool DLisToWis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void SetWisHeadFromDlisHead();
	bool DlisCurvesFilter();
	BYTE PcCodeToFid(BYTE PcCode);
	void SelectChannelFromWin();
	void SetWisChannelFromFidHead(int IndexWis,int nFra,int nCur);
	BYTE DlisRepCToWis(BYTE RepCode);
	bool LogicalToWis(CString WellName,CString WellPath,int nf);
private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	CString sOutFileName; //����ļ���
	int m_nCurvesRadio; //���˿���
	FILE *fp;

private:
	CDlisRead *DlisRead;
	CWisWrite *WisWrite;

private:
	CStringArray NewCurveName;
	CProgressDlg* pGuage;

};

#endif // !defined(AFX_DLISTOWIS_H_INCLUDED_)
