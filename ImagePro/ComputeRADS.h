// ComputeRADS.h: interface for the CComputeRADS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPUTERADS_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_)
#define AFX_COMPUTERADS_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlowNode.h"
class CFlowChartDoc;
class CComputeRADS : public CFlowNode  
{
	DECLARE_SERIAL(CComputeRADS);
public:
	CComputeRADS();
	virtual ~CComputeRADS();
	CComputeRADS(CString szNodename,BOOL bResult);
// Attributes
public:
	float *m_Value;
	CString m_szBufPath;                  //���ݻ�����ȫ·����
	CString m_szWellName;                 //����·���ľ���
	CString	m_szSeries;                   //��������
	int m_PadNum;                         //������
	int m_ButtonNum;                      //ÿ����������
	int m_DataNum;                        //�ܵ����
	int	m_ButtonMid;                      //ÿ�������м���λ��
	int m_CalNum;                         //����������
	float m_StProDep,m_EnProDep,m_Rlev;   //������
	float m_fRadius,m_CenOff,m_Bits;
	float m_ScaleDep,m_ScaleRadius,m_ScaleK;
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	void FitCirclePro();
	BOOL ScaleRADS(CString InName,CString OutName,BOOL bFit);
};

#endif // !defined(AFX_ComputeRADS_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_)
