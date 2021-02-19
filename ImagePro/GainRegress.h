// GainRegress.h
//
//////////////////////////////////////////////////////////////////////

#if !defined _GAINREGRESS_H__
#define _GAINREGRESS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlowNode.h"
class CFlowChartDoc;
class CGainRegress : public CFlowNode  
{
	DECLARE_SERIAL(CGainRegress);
public:
	CGainRegress();
	virtual ~CGainRegress();
	CGainRegress(CString szNodename,BOOL bResult);
// Attributes
public:
	
	CString m_szBufPath;                  //数据缓冲区全路径名
	CString	m_szSeries;                   //仪器类型
	int m_PadNum;                         //极板数
	int m_ButtonNum;                      //每个极板电扣数
	float m_StProDep,m_EnProDep,m_Rlev;   //处理井段
	float m_Rm;                           //泥浆电阻率      
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL GainRenew();
};
#endif 