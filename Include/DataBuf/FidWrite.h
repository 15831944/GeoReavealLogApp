// FidWrite.h: interface for the CFidWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIDWRITE_H_INCLUDED_)
#define AFX_FIDWRITE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Fid_Head.h"

class AFX_EXT_CLASS CFidWrite  
{
public:
	CFidWrite();
	virtual ~CFidWrite();

	void SetFidFileName(CString m_sObjectPath,CString sWellName); //����FID�ļ���
	void InitFidChannel(); //��ʼ������ͨ��
	bool WriteFidInfo(); //���FID�ļ�
	bool FefurbishFidInfo(); //����ˢ��FID�ļ���Ϣ
	bool WriteGenData(int Index,int Num,float *buf); //�����������
	bool WriteIndexFile(int Index); //д�����ļ�
	void PrintHead(FILE *fp); //��ӡ������Ϣ
	//��������
	bool OpenWave(int Index); //�򿪲���ͨ��
	bool WriteWave(float Depth,float *buf); //������㲨������
	bool WriteWave(float Depth,short *buf); //��������Ͳ�������
	bool WriteWave(float Depth,char *buf); //����ֽ��Ͳ�����
	void CloseWave(); //�رղ���ͨ��
	//��������
	bool OpenArray(int Index); //������ͨ��
	bool WriteArray(float Depth,float *buf); //����������
	void CloseArray(); //�ر�����ͨ��
private:
	bool WriteDataFile(int Index,int Num,float *buf); //д���ݵ���
	DWORD GetChannelSize(int Index);
	void ClaerHead();
	CString RepCodeToStr(int RepCode);
	void WriteDataToFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,float *buffer);
	void WriteDataToFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,short *buffer);
	void WriteDataToFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,char *buffer);

public:
	CString sObjectPath; //�������·��
	CString m_sFidFile; //FID�ļ���
	CString m_sOutWellPath; //FID������������Ŀ¼
	CString m_sOutWellName; //����
	FID_HEAD FidHead;
	FID_CHANNEL *FidChannel;

	CFile fw; //������������ļ�ָ��
	CString sWaveFile; //��ǰ������������ļ�
	bool bWaveOpen; //��������ļ��Ƿ��Ѵ�
	int IndexCurve; //��ǰ��������

};

#endif // !defined(AFX_FIDWRITE_H_INCLUDED_)
