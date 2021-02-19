// MinThickCp.h: interface for the CMinThickCp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MINTHICKCP_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_)
#define AFX_MINTHICKCP_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlowNode.h"
class CFlowChartDoc;
class CMinThickCp : public CFlowNode  
{
	DECLARE_SERIAL(CMinThickCp);
public:
	CMinThickCp();
	virtual ~CMinThickCp();
	CMinThickCp(CString szNodename,BOOL bResult);
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
	float m_fRadius;
	float m_HighValue,m_LowValue,m_BoxLength,m_TubeThick,m_NormalThick;
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive&ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	void FitCirclePro();
	BOOL ComputeMinThick(CString InName);
	BOOL GetCCLDepList();
};

#endif // !defined(AFX_MinThickCp_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_)
