// FwdLasW.h: interface for the CFwdLasW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FWDLASW_H_INCLUDED_)
#define AFX_FWDLASW_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FwdLas_Head.h"
#include "CodeSP.h"

class AFX_EXT_CLASS CFwdLasW  
{
public:
	CFwdLasW();
	virtual ~CFwdLasW();
	FWDLAS_HEAD LasHd;
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
	FILE *fLas;
	void WriteData();
	void StartWrite();
	CString sMidFile;
	int BlockSize;
	CFile fTmp;
};

#endif // !defined(AFX_FWDLasW_H_INCLUDED_)
