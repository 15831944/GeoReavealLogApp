// CrackCompute.h: interface for the CCrackCompute class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRACKCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
#define AFX_CRACKCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSinPoint : public CObject
{                  
public:
	CSinPoint(float Dep,float Dip,float Dir,CString Flag);
	CString m_Flag;
	int m_Hight;
	float m_Dep,m_Dip,m_Dir,m_Dip0,m_Dir0,FVTL,FVA,FVAH,FVPA;
};

#include "ImageCreate.h"
class CFlowChartDoc;
class CCrackCompute : public CImageCreat  
{
	DECLARE_SERIAL(CCrackCompute);
public:
	CCrackCompute();
	virtual ~CCrackCompute();
	CCrackCompute(CString szNodename,BOOL bResult);
// Attributes
public:
	CObList m_SinPoint;
	void CreatePoint();
	CString m_szCp;
	float m_Win,m_WinRlev;
	float m_WinWide,m_flagValue,m_ParA,m_ParB,m_FlagFre,m_XM,m_XN;
//  Implementation
public:
	BOOL StaticCrack(CString InName);
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
};

#endif // !defined(AFX_CRACKCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
