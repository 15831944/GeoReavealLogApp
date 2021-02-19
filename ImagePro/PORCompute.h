// PORCompute.h: interface for the CPORCompute class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
#define AFX_PORCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ImageCreate.h"
class CFlowChartDoc;

class CPORCompute : public CImageCreat  
{
	DECLARE_SERIAL(CPORCompute);
public:
	CPORCompute();
	virtual ~CPORCompute();
	CPORCompute(CString szNodename,BOOL bResult);
// Attributes
public:
	int		m_SpcNum;
	BOOL    m_bSpectFilter;                //区间孔隙度滤波
	BOOL    m_bResistance;                 //电导率成像数据
	float   m_XM,m_XN,m_XA,m_XB;
	float   m_StatWin,m_StatRlev,m_PORMax,m_MinStat;
	CString m_szExtPorWell,m_szExtPorName;
//  Implementation
public:
	
	BOOL Curve_Filter(int nFilter);
	BOOL CreatStacData(CString InName,CString OutName);
	BOOL CreatSpect(CString InName,CString OutName);
	BOOL StaticPor(CString InName);
	BOOL StaticSectionPor(CString InName);

	int kmeans(float **x, int p, int n, int k, float **c, int *z, float *work);
	BOOL StaticKmeansPor(CString InName);
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
};

#endif // !defined(AFX_PORCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
