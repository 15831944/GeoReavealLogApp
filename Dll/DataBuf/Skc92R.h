// Skc92R.h: interface for the CSkc92R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC92R_H_INCLUDED_)
#define AFX_SKC92R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Skc92_Head.h"

class AFX_EXT_CLASS CSkc92R  
{
public:
	CSkc92R();
	virtual ~CSkc92R();
	CFile fSkc92;
	bool Open(CString sDataFile); //�������ļ�
	void Close();  //�ر������ļ�
	bool ReadHead(); //���������Ϣ
	void ClearHead(); //��������Ϣ
	bool ReadChannel(int Index,float *buf); //��������������
	bool ReadVDL(int Index,float Depth,short *buf); //��������
	bool IsCCLHasFourPoint(); //�ж�CCL�����Ƿ��м��ܲ���
	bool ExistVDL(); //�Ƿ����VDL����

public:
	int FileSize; //�ļ���С
	SKC92_HEAD Skc92Head;
	int NumPoint; //һ�����ߵĲ�������
	int NumBlock; //���ݼ�¼��
	int nBlockSize; //һ����Ĵ�С
	int CurveTypes[20]; //һ����������ֽ���
	bool bVdl; //VDL�⾮�Ҵ���VDL����
	bool bCCLFour; //CCL�����Ƿ���ܲ���

};
bool _declspec(dllexport) IsSKC92(CString sDataFile);

#endif // !defined(AFX_SKC92R_H_INCLUDED_)
