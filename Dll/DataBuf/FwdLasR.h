// FwdLasR.h: interface for the CFwdLasR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FWDLASR_H_INCLUDED_)
#define AFX_FWDLASR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FwdLas_head.h"

class AFX_EXT_CLASS CFwdLasR  
{
public:
	CFwdLasR();
	virtual ~CFwdLasR();
	CStdioFile fFwdLas;
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	bool ReadHead(); //��������Ϣ
	int ReadCurve(float *buf); //����������
	void Prt_Header(FILE *fp); //��ӡ������Ϣ

public:
	bool GetSubStr(const char *str,int &StartPos,char *SubStr);
	int FileSize; //�ļ���С,�ļ�ͷ����
	FWDLAS_HEAD FwdLasHd;
private:
	float StringToFloat(CString sVal);
	void FwdLasHdReset();
	void GetCurveName(CString sVal);

};
bool _declspec(dllexport) IsFwdLas(CString sDataFile);

#endif // !defined(AFX_FWDLASR_H_INCLUDED_)
