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
	bool Open(CString sDataFile); //打列输出文件
	void Close(); //关闭输出文件
	void InitWisHead(int MaxObjectNumber); //文件头初始化
	void WriteWisHead(); //输出文件头信息
	DWORD GetWisLength(); //取得文件长度
	void WriteChannel(); //输出通道信息
	void WriteGendata(int NumR,float *buffer); //输出常规曲线数据
	void WriteWave(float depth,float *buffer); //输出波形数据
	void WriteWaveArray(float depth,float *buffer); //输出阵列数据
	void PrintHead(FILE *fp); //打印文件头信息
	void InitWisObjectEntry(); //初始化对象入口
	void InitWisChannel(); //初始化通道入口

	WIS_HEAD WisHead; //头信息
	WIS_OBJECT_ENTRY *m_ObjectEntry; //对象入口
	WIS_CHANNEL *WisChannel; //通道信息
private:
	CFile fWis;
	void WriteDataToWis(DWORD pos,WORD nRepCode,DWORD count,float *buffer); //数据输出通用函数

};

#endif // !defined(AFX_WISWRITE_H_INCLUDED_)
