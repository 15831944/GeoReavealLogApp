// XtfRead.h: interface for the CXtfRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTFREAD_H_INCLUDED_)
#define AFX_XTFREAD_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "Xtf_Head.h"

class AFX_EXT_CLASS CXtfRead  
{
public:
	CXtfRead();
	virtual ~CXtfRead();
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	void GetHead(); //���ļ�ͷ��Ϣ
	int OpenCurve(CString sCurveName); //����������
	int OpenCurve(int Index); //����������
	int ReadCurveHead(); //������ͷ
	DWORD ReadCurve(int index, DWORD count, float* buffer); //��������������
	bool ReadWave(int index,float depth,float *buffer); //����������
	bool ReadWaveFromMatrix(int index,int subindex,float depth,float *buffer); //�����λ���������
	bool ReadMatrix(int index,float depth,float *buffer); //����������

	DWORD FileSize; //�ļ���С
	XTF_HEAD XtfHead;
	CURVE_HEAD XtfCurve;
private:
	CFile fXtf;
	void ReadData(DWORD pos,DWORD count,float *buffer); //������ͨ�ú���
	void SunXtfHead2PC(); //�ļ�ͷ����ת��
	void SunCurveHead2PC(); //����ͷ����ת��
	int CheckCurveHeader(); //�������ͷ�Ƿ�Ϸ�
	void ClearHeaderPack(); //����ļ�ͷ���쳣����


private:
	int GetCurveIndex(CString sCurveName);
	int index; //��ǰ����������

};
bool _declspec(dllexport) IsXTF(CString sDataFile);

#endif // !defined(AFX_XTFREAD_H_INCLUDED_)
