// BitR.h: interface for the CBitR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITR_H_INCLUDED_)
#define AFX_BITR_H_INCLUDED_

#include "Bit_Head.h"
#include "CodeSP.h"
#include <math.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class _declspec(dllexport) CBitR  
{
public:
	CBitR();
	virtual ~CBitR();
	CFile fBit;
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	bool ReadHead(); //��������Ϣ
	void ClearHead(); //�������Ϣ
	bool ReadChannel(int Index,float *buf); //����������
	bool ReadWaveform(float Depth,char *buf); //��ȫ����������
	bool ReadWaveform(int nWave,float Depth,char *buf); //��һ����������


	int FileSize; //�ļ���С
	BIT_HEAD BitHead;
	int NumPoint; //һ�����ߵĲ�������
	int NumBlock; //���ݼ�¼��
	int nBlockSize; //һ����Ĵ�С
	bool bWaveform; //�Ƿ�Ϊ�������ݣ�1��ʾ����
	int NumConvLog;//����������
	int NumWaveLog; //����������

};
bool _declspec(dllexport) IsBIT(CString sDataFile);

#endif // !defined(AFX_BITR_H_INCLUDED_)
