// BitToFid.h: interface for the CBitToFid class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BITTOFID_H__
#define __BITTOFID_H__

#include "CodeSP.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"
#include "FidWrite.h"
#include "BitR.h"

class CBitToFid  
{
public:
	CBitToFid();
	virtual ~CBitToFid();
	bool BitToFid(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,FILE *fo);

private:
	void SetFidHeadFromBitHead(); //����FID�ļ��������
	void SetFidChannelFromBit(int IndexFid,int IndexBit); //����FID��ͨ������
	bool Transform(); //����ת������

private:
	CFidWrite *FidWrite; 
	CBitR *BitR;

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CProgressDlg* pGuage;
};

#endif // !defined(AFX_BITTOFID_H_INCLUDED_)
