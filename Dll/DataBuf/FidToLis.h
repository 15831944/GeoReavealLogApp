// FidToLis.h: interface for the CFidToLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIDTOLIS_H_INCLUDED_)
#define AFX_FIDTOLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "CodeSP.h"
#include "FidRead.h"
#include "LisWrite.h"
#include "FwdTxtW.h"

class CFidToLis  
{
public:
	CFidToLis();
	virtual ~CFidToLis();
	bool FidToLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo);

private:
	bool Transform();
	void GetLisFileIndexFromFid();
	void Prt_CurveItemIndex();
	void SetReelHeader(); //���þ�ͷ��¼��Ϣ
	void SetFileHeader(int FileNo);
	void SetEB(int nf);
	int GetLogicalCurveIndex(int nf);
	unsigned char FidRepCodeToLis(unsigned char RepCode);
	void WriteDataToLis(int nf);
	void SelectCurves();
	void Prt_FileInfo(int nf);
private:
	CWordArray CurveItem; //����������������ṹΪ������->�ڣ�������������ֵ->�ڣ�������������...


private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;

private:
	CFidRead *FidRead;
	CLisWrite *LisWrite;

private:
	CString LisFileName; //��ǰ����ļ�
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;

};

#endif // !defined(AFX_FIDTOLIS_H_INCLUDED_)
