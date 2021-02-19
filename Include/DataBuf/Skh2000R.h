// Skh2000R.h: interface for the CSkh2000R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKH2000R_H_INCLUDED_)
#define AFX_SKH2000R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Skh2000_Head.h"

class AFX_EXT_CLASS CSkh2000R  
{
public:
	CSkh2000R();
	virtual ~CSkh2000R();
	CFile fSkh2000;
	bool Open(CString sDataFile); //�������ļ�
	void Close();//�ر������ļ�
	bool ReadHead(); //���������Ϣ
	void ClearHead(); //�������Ϣ
	bool ReadChannel(int Index,float *buf); //����������
	bool ReadWAVE(float Depth,short *buf); //�����в�������
	bool ReadWAVE(int index,float Depth,short *buf);//��һ��������

	int FileSize; //�ļ���С
	SKH2000_HEAD Skh2000Head;
	int NumPoint; //һ�����ߵĲ�������
	int NumBlock; //���ݼ�¼��
	int nBlockSize; //һ����Ĵ�С
	int nHeadSize; //������С
	int nTrailSize; //�ļ�β�Ĵ�С
	BOOL bIsWAVE; //�ļ���������
};
bool _declspec(dllexport) IsSKH2000(CString sDataFile);

#endif // !defined(AFX_SKH2000R_H_INCLUDED_)
