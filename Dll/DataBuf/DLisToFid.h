// DLisToFid.h: interface for the CDLisToFid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISTOFID_H_INCLUDED_)
#define AFX_DLISTOFID_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DLisRead.h"
#include "FidWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CDLisToFid  
{
public:
	CDLisToFid();
	virtual ~CDLisToFid();
	bool DLisToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,BOOL bCurveSplit,BOOL bDelSplitCur,FILE *fo);

private:
	bool Transform(); //ת�����Ƴ���
	bool LogicalToFid(CString WellName,int nf); //ת��һ���߼��ļ�
	void SetFidHeadFromDlisHead(); //����FID�ļ�������Ϣ
	bool DlisCurvesFilter(); //ѡ������
	BYTE PcCodeToFid(BYTE PcCode); //��ʾ��ת��
	void SelectChannelFromWin(); //����ʽѡ�����ߡ��޸�������Ϣ

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	BOOL m_bCurveSplit; //�������߲��
	BOOL m_bDelSplitCur; //ɾ�����������
	FILE *fp;

private:
	CDlisRead *DlisRead;
	CFidWrite *FidWrite;

private:
	CStringArray NewCurveName;
	CProgressDlg* pGuage;

};

#endif // !defined(AFX_DLISTOFID_H_INCLUDED_)
