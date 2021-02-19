// FwdTxtR.h: interface for the CFwdTxtR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FWDTXTR_H_INCLUDED_)
#define AFX_FWDTXTR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FwdTxt_head.h"

class AFX_EXT_CLASS CFwdTxtR  
{
public:
	CFwdTxtR();
	virtual ~CFwdTxtR();
	CStdioFile fFwdTxt;
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	bool ReadHead(); //��������Ϣ
	int ReadCurve(float *buf); //����������
	void Prt_Header(FILE *fp); //��ӡ������Ϣ

public:
	bool GetSubStr(const char *str,int &StartPos,char *SubStr);
	int FileSize; //�ļ���С,�ļ�ͷ����
	FWDTXT_HEAD FwdTxtHd;
private:
	float StringToFloat(CString sVal);
	void FwdTxtHdReset();
	void GetCurveName(CString sVal);

};
bool _declspec(dllexport) IsFwdTxt(CString sDataFile);

#endif // !defined(AFX_FWDTXTR_H_INCLUDED_)
