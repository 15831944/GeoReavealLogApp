// WisRead.h: interface for the CWisRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISREAD_H_INCLUDED_)
#define AFX_WISREAD_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wis_Head.h"

class AFX_EXT_CLASS CWisRead  
{
public:
	CWisRead();
	virtual ~CWisRead();
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	bool GetHead(); //���ļ�ͷ��Ϣ
	bool GetObjectEntry(); //�����������Ϣ
	int OpenChannel(const char *name); //��ͨ����Ϣ
	DWORD ReadCurve(int index, float depth, DWORD count, float* buffer); //��������������
	bool ReadWave(int index,float depth,float *buffer); //����������
	bool ReadWaveArray(int index,float depth,float *buffer); //����������
	int GetCurveDim(int index); //ȡ��ͨ����ά��
	void Prt_HeaderInfo(FILE *fp); //��ӡ�ļ�ͷ��Ϣ
	void Prt_ObjectInfo(FILE *fp); //��ӡ������Ϣ
	void Prt_ChannelInfo(FILE *fp); //��ӡͨ����Ϣ
	void Prt_TableInfo(FILE *fp); //��ӡͨ����Ϣ
	void Prt_StreamInfo(FILE *fp); //��ӡͨ����Ϣ
	int FileSize; //�ļ���С
	WIS_HEAD WisHead; //ͷ��Ϣ
	WIS_OBJECT_ENTRY* m_ObjectEntry; //�������
	WIS_CHANNEL WisChannel; //ͨ����Ϣ
private:
	CFile fWis;
	DWORD ReadData(DWORD pos,int RepCode,DWORD count,float *buffer); //�����ݹ��ú���

};
bool _declspec(dllexport) IsWIS(CString sDataFile);

#endif // !defined(AFX_WISREAD_H_INCLUDED_)
