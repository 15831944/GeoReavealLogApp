// Xb716R.h: interface for the CQb716R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XB716R_H_INCLUDED_)
#define AFX_XB716R_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Xb716_Head.h"

class AFX_EXT_CLASS CXb716R  
{
public:
	CXb716R();
	virtual ~CXb716R();
	XB716_HEAD Head;
	int FileSize; //�ļ���С
	CFile fXb716;
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	bool ReadHead(); //��������Ϣ
	void Prt_Header(FILE *fp); //��ӡ������Ϣ
	void ReadCurve(int index,float *buf); //����������

public:
	int NumPoint; //����
	int NumBlock; //����
	int BlockSize; //�鳤
	int HeadSize; //�ļ�ͷ����

};
bool _declspec(dllexport) IsXB716(CString sDataFile);

#endif // !defined(AFX_XB716R_H_INCLUDED_)
