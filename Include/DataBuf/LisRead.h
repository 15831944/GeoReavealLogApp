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
	bool Open(CString sDataFile); //打开输入文件
	void Close(); //关闭输入文件
	void ResetTapeHeader(); //带头信息初始化
	void ReadTapeHeader(); //读带头信息
	void ScanLisFile(int NumPrtFdata,int NumPrtSample,BOOL bPrtBaseInfo,FILE *fp); //扫描一个文件信息
	bool ScanLisOneLogicalFile(FILE *fp); //扫描一个逻辑文件信息
	int GetLogicalRecord(DWORD &Count); //取得一个逻辑记录内容
	DWORD GetLogicalRecPos(); //取得一个逻辑记录的超始位置
	DWORD GetLogicalFilePos(); //取得一个逻辑文件的起始位置
	void SetLisPos(DWORD pos); //设置文件的起始位置
	void ReadLogicalFile(); //读一个逻辑文件
	bool ReadCurve(int index,int Num,float *buffer); //读常规数据
	bool ReadWave(int index,float depth,float *buffer); //读波形数据
	void GetTapeProducer(CString &sProducer,CString &DeviceName); //取得数据生产者名称

public:
	DWORD FileSize; //文件大小
	DWORD NumDatRec; //数据逻辑记录数
	DWORD LisDatStartPos; //逻辑文件数据开始位置
	DWORD LisLogicalFileStartPos; //逻辑文件起始位置
	DWORD LisLogicalFileStopPos; //逻辑文件结束位置
	DWORD LisRecPos; //逻辑记录所在起始位置
	DWORD LisPos; //LIS文件当前位置
	float DepScale; //深度转换系数
	float Rlev; //采样间隔
	float StDep; //起始深度
	float EnDep; //终止深度
	int NumLog; //曲线数
	int NumPoint; //采样点数
	int m_NumPrtFdata; //显示帧数据数
	int m_NumPrtSample; //显示采样数(一点)
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
	int MaxPhyLen; //最大物理记录长度

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
	BYTE LisBuf[MAX_LIS_BLOCKLEN]; //一个逻辑记录数据体
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
	CString sMidFile; //中间数据文件
	DWORD nTempBlockSize; //临时文件块长度
	CFile fTemp; //临时文件指针

};
bool _declspec(dllexport) IsLIS(CString sDataFile);

#endif // !defined(AFX_LISREAD_H_INCLUDED_)
