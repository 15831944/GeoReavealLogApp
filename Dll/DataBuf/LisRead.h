// LisRead.h: interface for the CLisRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISREAD_H_INCLUDED_)
#define AFX_LISREAD_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Lis_Head.h"
#include "FID_Head.h"
#include "CodeSp.h"

class AFX_EXT_CLASS CLisRead  
{
public:
	CLisRead();
	virtual ~CLisRead();
	bool Open(CString sDataFile); //�������ļ�
	void Close(); //�ر������ļ�
	void ResetTapeHeader(); //��ͷ��Ϣ��ʼ��
	void ReadTapeHeader(); //����ͷ��Ϣ
	void ScanLisFile(int NumPrtFdata,int NumPrtSample,BOOL bPrtBaseInfo,FILE *fp); //ɨ��һ���ļ���Ϣ
	bool ScanLisOneLogicalFile(FILE *fp); //ɨ��һ���߼��ļ���Ϣ
	int GetLogicalRecord(DWORD &Count); //ȡ��һ���߼���¼����
	DWORD GetLogicalRecPos(); //ȡ��һ���߼���¼�ĳ�ʼλ��
	DWORD GetLogicalFilePos(); //ȡ��һ���߼��ļ�����ʼλ��
	void SetLisPos(DWORD pos); //�����ļ�����ʼλ��
	void ReadLogicalFile(); //��һ���߼��ļ�
	bool ReadCurve(int index,int Num,float *buffer); //����������
	bool ReadWave(int index,float depth,float *buffer); //����������
	void GetTapeProducer(CString &sProducer,CString &DeviceName); //ȡ����������������

public:
	DWORD FileSize; //�ļ���С
	DWORD NumDatRec; //�����߼���¼��
	DWORD LisDatStartPos; //�߼��ļ����ݿ�ʼλ��
	DWORD LisLogicalFileStartPos; //�߼��ļ���ʼλ��
	DWORD LisLogicalFileStopPos; //�߼��ļ�����λ��
	DWORD LisRecPos; //�߼���¼������ʼλ��
	DWORD LisPos; //LIS�ļ���ǰλ��
	float DepScale; //���ת��ϵ��
	float Rlev; //�������
	float StDep; //��ʼ���
	float EnDep; //��ֹ���
	int NumLog; //������
	int NumPoint; //��������
	int m_NumPrtFdata; //��ʾ֡������
	int m_NumPrtSample; //��ʾ������(һ��)
	BOOL m_bPrtBaseInfo;
public:
	CString m_sDataFile;
	PhyRecordHeader PhyHead; 
	RecordHeader RecHead;
	TapeHeaderRec ReelHead;
	TapeHeaderRec ReelTrail;
	TapeHeaderRec TapeHead;
	TapeHeaderRec TapeTrail;
	FileHeadRec FileHead;
	FileHeadRec FileTrail;
	ComponentBlocks CompBlock;
	Entry_Block Entry;
	EntryValue EnVa;
	DataSpeBlockRec Dsbr;
	CurveInfo *Curve;
	int MaxPhyLen; //��������¼����

private:
	CFile fLis;
	void ResetEntryValue(); 
	void LisGetPhyHead(BYTE *PhyBuf);
	int LisGetTrailLen();
	void LisBufToReelHeader();
	void LisBufToReelTrailer();
	void LisBufToTapeHeader();
	void LisBufToTapeTrailer();
	void LisBufToFileHeader();
	void LisBufToFileTrailer();
	void LisBufToComponentBlocks(DWORD nCount,FILE *fp);
	void LisBufToEntryValue(DWORD nCount,FILE *fp);
	void Prt_LisData(DWORD nCount,DWORD &NumPrtFra,FILE *fp);
	void LisBufToDepth(DWORD nCount);
	void LisBufToData(BYTE *buf,BYTE RepC,short nItem,float *vf);
	void LisBufToData(BYTE *buf,BYTE RepC,short nItem,int *vi);
	void LisBufToData(BYTE *buf,BYTE RepC,short nItem,short *vh);
	void LisBufToData(BYTE *buf,BYTE RepC,short nItem,BYTE *vb);
	void LisBufToData(BYTE *buf,BYTE RepC,short nItem,char *vc);

	void Prt_LisReelHeader(FILE *fp);
	void Prt_LisReelTrailer(FILE *fp);
	void Prt_LisTapeHeader(FILE *fp);
	void Prt_LisTapeTrailer(FILE *fp);
	void Prt_LisFileHeader(FILE *fp);
	void Prt_LisFileTrailer(FILE *fp);
	void Prt_EntryBlock(FILE *fp);
	void Prt_WellInfo(FILE *fp);

private:
	BYTE LisBuf[MAX_LIS_BLOCKLEN]; //һ���߼���¼������
	int GetCodeLength(unsigned char RepCode);
	int GetObjCodeLen(unsigned char RepCode);

	float LisFloat16(unsigned char *buf);
	float LisFloat32Low(unsigned char *buf);
	float LisFloat32(unsigned char *buf);
	float LisFloat32Fix(unsigned char *buf);
	int Lis32Integer(unsigned char *buf);
	short Lis16Integer(unsigned char *buf);
	short Lis8Integer(unsigned char *buf);
	short Lis8Unsigned(unsigned char *buf);
	BYTE LisRepCToPcRepC(BYTE Code);
	float GetDepScaleFromLisUnits(char *Units);

	float LisBufToFloat(BYTE Code,BYTE *buf);
	int LisBufToInt(BYTE Code,BYTE *buf);
	short LisBufToShort(BYTE Code,BYTE *buf);
	BYTE LisBufToByte(BYTE Code,BYTE *buf);
	char LisBufToChar(BYTE Code,BYTE *buf);
	bool ReadData(DWORD pos,BYTE RepCode,BYTE CodeLen,DWORD count,float *buffer);
	void ResetHeader(); 
private:
	CString sMidFile; //�м������ļ�
	DWORD nTempBlockSize; //��ʱ�ļ��鳤��
	CFile fTemp; //��ʱ�ļ�ָ��

};
bool _declspec(dllexport) IsLIS(CString sDataFile);

#endif // !defined(AFX_LISREAD_H_INCLUDED_)
