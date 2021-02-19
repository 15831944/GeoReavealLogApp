// XtfWrite.h: interface for the CXtfWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTFWRITE_H_INCLUDED_)
#define AFX_XTFWRITE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Xtf_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CXtfWrite  
{
public:
	CXtfWrite();
	virtual ~CXtfWrite();
	bool Open(CString sDataFile); //������ļ�
	void Close(); //�ر�����ļ�
	void InitXtfHeader(); //��ʼ���ļ�ͷ
	void WriteXtfHeader(); //����ļ�ͷ
	void WriteCurveHeader(int index); //�������ͷ
	void WriteGenData(int NumR,float *buf); //�����������
	void WriteWave(float depth,float *buffer); //���������
	void WriteWaveArray(float depth,float *buffer); //������λ���������
	short XtfRepCodeToLen(short FidRepCode); //��ȡ���ݳ���
	short XtfRepCodeToIntFlFix(short XtfRepCode); //ȡ�ô�������
	short XtfRepCodeToSign(short XtfRepCode); //ȡ�ô������

	CURVE_HEAD XtfCurve;
	int Index; //��ǰ��������
	void PrintHead(FILE *fp);
	XTF_HEAD XtfHead; //ͷ��Ϣ
	CString sXtfFile; //�����XTF�����ļ���

private:
	CFile fXtf;
	void PcXtfHead2Sun();
	void WriteHeader();
	void PcCurveHead2Sun();
	void WriteDataToXtf(DWORD pos,DWORD count,float *buffer);

};

#endif // !defined(AFX_XTFWRITE_H_INCLUDED_)
