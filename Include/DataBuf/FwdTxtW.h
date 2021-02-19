// FwdTxtW.h: interface for the CFwdTxtW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FWDTXTW_H_INCLUDED_)
#define AFX_FWDTXTW_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FwdTxt_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CFwdTxtW  
{
public:
	CFwdTxtW();
	virtual ~CFwdTxtW();
	FWDTXT_HEAD TxtHd;
	bool Open(CString sDataFile); //������ļ�
	void Close(); //�ر�����ļ�
	void WriteHead(); //���������Ϣ
	void WriteChannel(int index,float *buf); //�����������
	void Prt_Header(FILE *fp); //��ӡ������Ϣ
	void ResetHead(); //������Ϣ��ʼ��
public:
	int FileSize; //�ļ���С
	CString m_sFileName;
private:
	FILE *fTxt;
	void WriteData();
	void StartWrite();
	CString sMidFile;
	int BlockSize;
	CFile fTmp;
};

#endif // !defined(AFX_FWDTXTW_H_INCLUDED_)
