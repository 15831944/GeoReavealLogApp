// FidRead.h: interface for the CFidRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIDREAD_H_INCLUDED_)
#define AFX_FIDREAD_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Fid_Head.h"

class AFX_EXT_CLASS CFidRead  
{
public:
	CFidRead();
	virtual ~CFidRead();
	bool ReadFidInfo(const CString FidFileName); //��FID�ļ���������Ϣ
	bool ReWriteFidFromData(const CString FidFileName); //����Ŀ¼������Ϣ��дFID�ļ�
	bool ReadFidChannelInfo(int Index,CString sCurveName); //��ͨ����Ϣ
	bool ReadGenData(const CString sCurveName,int NumR,float *buf); //��������������
	bool OpenWave(CString sCurveName); //�򿪲�������
	bool ReadWave(float Depth,float *buf); //����������һ������������
	void CloseWave(); //�رղ����ļ�
	bool OpenArray(CString sCurveName); //����������
	bool ReadArray(float Depth,float *buf); //����������һ������������
	void CloseArray(); //�ر������ļ�
	bool IsIndexFile(CString sCurIndexFile); //�ж��ļ��Ƿ�Ϊ�����ļ�
	int GetIndexFromCurveName(const CString sCurveName); //��ѯ��Ӧ���ߵ���������
	void CleanArrayCurve(); //�����������
	void CleanWaveCurve(); //�����������
	void CleanConvCurve(); //�����������
private:
	void ReadDataFromFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,float *buffer); //�������ļ��ж���Ӧ���͵�����
	void InitFidChannel(); //��ʼ��������Ϣ
	void ResetHead();
	bool ReadIndex(); //�ҳ�Ŀ¼��������������
	int RepCodeStrToInt(CString sRepCode,int CodeLen); //�ô���ʾ�ı�ʾ��ת��Ϊ���ͱ�ʾ��

public:
	CString m_sInWellPath; //FID������������Ŀ¼
	CString m_sInWellName; //��ǰ��ȡ���ݵľ���
	FID_HEAD FidHead; //�ļ�ͷ��Ϣ
	FID_CHANNEL *FidChannel; //����ͷ��Ϣ

	CString sWaveFile; //��ǰ��ȡ���������ļ�
	bool bWaveOpen; //�����ļ��Ƿ��ѱ���
	int IndexCurve; //��ǰ��������
	CFile fR; //��ȡ�����ļ�ָ��
};
bool _declspec(dllexport) IsFID(CString sDataFile);
void _declspec(dllexport) GetorkSpace(CString &m_sWorkSpace);

#endif // !defined(AFX_FIDREAD_H_INCLUDED_)
