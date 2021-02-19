// Qh716R.h: interface for the CQh716R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QH716R_H_INCLUDED_)
#define AFX_QH716R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Qh716_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CQh716R  
{
public:
	CQh716R();
	virtual ~CQh716R();
	int FileSize; //�ļ���С
	QH716_HEAD Head;
	CFile fQh716;
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	bool ReadHead(); //��������Ϣ
	void ReadCurve(int index,float *buf); //����������
	void Prt_Header(FILE *fp); //��ӡ������Ϣ

public:
	int NumPoint; //����
	int NumBlock; //����
	int BlockSize; //�鳤
	int HeadSize; //�ļ�ͷ����

};
bool _declspec(dllexport) IsQH716(CString sDataFile);

#endif // !defined(AFX_QH716R_H_INCLUDED_)
