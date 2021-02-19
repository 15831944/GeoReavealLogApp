// WisWrite.h: interface for the CWisWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISWRITE_H_INCLUDED_)
#define AFX_WISWRITE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wis_Head.h"

class AFX_EXT_CLASS CWisWrite  
{
public:
	CWisWrite();
	virtual ~CWisWrite();
	bool Open(CString sDataFile); //��������ļ�
	void Close(); //�ر�����ļ�
	void InitWisHead(int MaxObjectNumber); //�ļ�ͷ��ʼ��
	void WriteWisHead(); //����ļ�ͷ��Ϣ
	DWORD GetWisLength(); //ȡ���ļ�����
	void WriteChannel(); //���ͨ����Ϣ
	void WriteGendata(int NumR,float *buffer); //���������������
	void WriteWave(float depth,float *buffer); //�����������
	void WriteWaveArray(float depth,float *buffer); //�����������
	void PrintHead(FILE *fp); //��ӡ�ļ�ͷ��Ϣ
	void InitWisObjectEntry(); //��ʼ���������
	void InitWisChannel(); //��ʼ��ͨ�����

	WIS_HEAD WisHead; //ͷ��Ϣ
	WIS_OBJECT_ENTRY *m_ObjectEntry; //�������
	WIS_CHANNEL *WisChannel; //ͨ����Ϣ
private:
	CFile fWis;
	void WriteDataToWis(DWORD pos,WORD nRepCode,DWORD count,float *buffer); //�������ͨ�ú���

};

#endif // !defined(AFX_WISWRITE_H_INCLUDED_)
