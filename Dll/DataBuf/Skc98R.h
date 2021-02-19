// Skc98R.h: interface for the CSkc98R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC98R_H_INCLUDED_)
#define AFX_SKC98R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Skc98_Head.h"

class AFX_EXT_CLASS CSkc98R  
{
public:
	CSkc98R();
	virtual ~CSkc98R();
	CFile fSkc98;
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	bool ReadHead(); //���������Ϣ
	void ClearHead(); //��������Ϣ
	bool ReadChannel(int Index,float *buf); //����������
	bool ReadVDL(int Index,float Depth,short *buf); //�����ܶ�����
	bool IsConvCurve(int index); //�Ƿ�Ϊ��������

public:
	int FileSize; //�ļ���С
	SKC98_HEAD Skc98Head;
	int NumPoint; //һ�����ߵĲ�������
	int NumBlock; //���ݼ�¼��
	int nBlockSize; //һ����Ĵ�С
	int NumVdlSamp; //VDL����ʱ�������
	float yStep[30]; //�����߲����������������ռ�ÿռ�����
};
bool _declspec(dllexport) IsSKC98(CString sDataFile);

#endif // !defined(AFX_SKC98R_H_INCLUDED_)
