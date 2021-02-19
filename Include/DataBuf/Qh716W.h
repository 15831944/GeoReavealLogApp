// Qh716W.h: interface for the CQh716W class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QH716W_H_INCLUDED_)
#define AFX_QH716W_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Qh716_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CQh716W  
{
public:
	CQh716W();
	virtual ~CQh716W();
	CFile fQh716;
	bool Open(CString sDataFile); //������ļ�
	void Close(); //�ر�����ļ�
	void ResetHead(); //ͷ������Ϣ
	bool WriteHead(); //���������Ϣ
	void WriteChannel(int Index,float *buf); //�����������
	void PrintHead(FILE *fp); //��ӡ������Ϣ

	QH716_HEAD Head;
	int NumPoint; //һ�����ߵĲ�������
	int NumBlock; //���ݼ�¼��
	int BlockSize; //��ĳ���
	int FileSize; //�ļ���С
};

#endif // !defined(AFX_QH716W_H_INCLUDED_)
