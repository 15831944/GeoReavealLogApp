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
	bool Open(CString sDataFile); //打开输出文件
	void Close(); //关闭输出文件
	void WriteRTHT(int type);  //输出卷头、带头、卷尾、带尾记录
	void WriteFHR(int type); //输出文件头记录
	void WriteEB(); //输出格式说明记录
	int GetCodeLength(unsigned char RepCode); //取得代码长度
	void StartWriteData(); //开始输出数据
	void StopWriteData(); //结束输出数据
	void WriteCurveData(int index,float sdep,float edep,float rlev,int NumR,float *buf); //输出常规数据
	void WriteWaveData(int index,float depth,int NumX,float *buf); //输出波形数据

public:
	PhyRecordHeader PhyHead; 
	TapeHeaderRec TapeHead; //卷头、带头、带尾、卷尾信息
	FileHeadRec FileHead;
	EntryValue EnVa;
	DataSpeBlockRec *Dsbr;
	int NumLog; //逻辑文件曲线数
	float StDep;  //逻辑文件起始深度
	float EnDep; //逻辑文件结束深度
	float Rlev; //逻辑文件采样间隔

private:
	CFile fLis;
	void WriteEOF(); //EOF
	BYTE LisBuf[MAX_LIS_BLOCKLEN]; //一个逻辑记录数据体
	void Float32ToLIS68F(float vf,BYTE *str);
	void Float32ToLIS73I(float vf,BYTE *str);
	void Float32ToLIS79I(float vf,BYTE *str);
	void Float32ToLIS65C(float vf,BYTE *str);

	void Int32ToLis73I(int vi,BYTE *str);
	void Int16ToLis79I(short vh,BYTE *str);
	void WriteBuf(int NumBytes,unsigned char *buf);

	int NumRecLen; //逻辑记录长度
	int LogicalRecLen; //逻辑记录长度
	int NumFra; //输出帧数
	CFile fTmp;
	CString sMidFile;
};

#endif // !defined(AFX_LISWRITE_H_INCLUDED_)
