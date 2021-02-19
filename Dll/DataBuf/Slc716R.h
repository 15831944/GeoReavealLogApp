// Slc716R.h: interface for the CSlc716R class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716R_H__INCLUDED_)
#define AFX_SLC716R_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Slc716_Head.h"

class AFX_EXT_CLASS CSlc716R  
{
public:
	CSlc716R();
	virtual ~CSlc716R();
	CFile fSlc716;
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	bool ReadHead(); //��������Ϣ
	void ClearHead(); //�������Ϣ
	bool ReadChannel(int Index,float *buf); //����������
	bool ReadChannel(CString sCurveName,float *buf); //����������
	int GetCurveIndex(CString sCurveName); //�����������

	int FileSize; //�ļ���С
	SLC716_HEAD Slc716Head;
	int NumPoint; //һ�����ߵĲ�������
	int NumBlock; //������

};
bool _declspec(dllexport) IsSLC716(CString sDataFile);
#endif // !defined(AFX_SLC716R_H_INCLUDED_)
