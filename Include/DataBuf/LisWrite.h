// LisWrite.h: interface for the CLisWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISWRITE_H_INCLUDED_)
#define AFX_LISWRITE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "Lis_Head.h"
#include "CodeSp.h"

class AFX_EXT_CLASS CLisWrite  
{
public:
	CLisWrite();
	virtual ~CLisWrite();
	bool Open(CString sDataFile); //������ļ�
	void Close(); //�ر�����ļ�
	void WriteRTHT(int type);  //�����ͷ����ͷ����β����β��¼
	void WriteFHR(int type); //����ļ�ͷ��¼
	void WriteEB(); //�����ʽ˵����¼
	int GetCodeLength(unsigned char RepCode); //ȡ�ô��볤��
	void StartWriteData(); //��ʼ�������
	void StopWriteData(); //�����������
	void WriteCurveData(int index,float sdep,float edep,float rlev,int NumR,float *buf); //�����������
	void WriteWaveData(int index,float depth,int NumX,float *buf); //�����������

public:
	PhyRecordHeader PhyHead; 
	TapeHeaderRec TapeHead; //��ͷ����ͷ����β����β��Ϣ
	FileHeadRec FileHead;
	EntryValue EnVa;
	DataSpeBlockRec *Dsbr;
	int NumLog; //�߼��ļ�������
	float StDep;  //�߼��ļ���ʼ���
	float EnDep; //�߼��ļ��������
	float Rlev; //�߼��ļ��������

private:
	CFile fLis;
	void WriteEOF(); //EOF
	BYTE LisBuf[MAX_LIS_BLOCKLEN]; //һ���߼���¼������
	void Float32ToLIS68F(float vf,BYTE *str);
	void Float32ToLIS73I(float vf,BYTE *str);
	void Float32ToLIS79I(float vf,BYTE *str);
	void Float32ToLIS65C(float vf,BYTE *str);

	void Int32ToLis73I(int vi,BYTE *str);
	void Int16ToLis79I(short vh,BYTE *str);
	void WriteBuf(int NumBytes,unsigned char *buf);

	int NumRecLen; //�߼���¼����
	int LogicalRecLen; //�߼���¼����
	int NumFra; //���֡��
	CFile fTmp;
	CString sMidFile;
};

#endif // !defined(AFX_LISWRITE_H_INCLUDED_)
