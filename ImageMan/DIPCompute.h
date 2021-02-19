// DIPCompute.h: interface for the CDIPCompute class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIPCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
#define AFX_DIPCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDIPCompute 
{
public:
	CDIPCompute();
	virtual ~CDIPCompute();
	// Attributes
public:
	CString m_szSeries,m_szWellName,m_szBufPath;
	float   m_StProDep,m_EnProDep;
	int     m_PadNum;

	float	m_WinLength;             //相关对比窗长
	float	m_StepLength;            //计算倾角曲线的步长
	float	m_SearchLength;          //上下探索长度
	float	m_DangMax;               //最大倾角
	float	m_RMin;                  //参加计算倾角的最小相关系数
	float   m_GradCons;
	float   m_GradMin;

	//计算倾角所需中间变量
	float m_RMAX;
	int IWL,ISL,ISTEP;
	float X[5000],Y[5000];
	float   vr[10];//半径

	int  CORREL();                             //计算相关系数
	BOOL DIPPro();                             //计算地层倾角
public:
	BOOL Run();
};

#endif // !defined(AFX_DIPCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
