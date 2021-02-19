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
	
	CString m_szBufPath;                  //���ݻ�����ȫ·����
	CString	m_szSeries;                   //��������
	int m_PadNum;                         //������
	int m_ButtonNum;                      //ÿ����������
	float m_StProDep,m_EnProDep,m_Rlev;   //������
	float m_Rm;                           //�ཬ������      
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL GainRenew();
};
#endif 