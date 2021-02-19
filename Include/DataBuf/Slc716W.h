// Slc716W.h: interface for the CSlc716W class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716W_H_INCLUDED_)
#define AFX_SLC716W_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Slc716_Head.h"

class AFX_EXT_CLASS CSlc716W  
{
public:
	CSlc716W();
	virtual ~CSlc716W();
	CFile fSlc716;
	bool Open(CString sDataFile); //������ļ�
	void Close(); //�ر�����ļ�
	void ClearHead(); //�������Ϣ
	void ResetHead(); //������Ϣʼ����
	bool WriteHead(); //���������Ϣ
	void WriteChannel(int Index,float *buf); //�����������
	void PrintHead(FILE *fp); //��ʾ������Ϣ

	SLC716_HEAD Slc716Head;
	int NumPoint; //һ�����ߵĲ�������
	int NumBlock; //���ݼ�¼��
};

#endif // !defined(AFX_SLC716W_H_INCLUDED_)
