// DataUnite.h: interface for the CDataUnite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAUNITE_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_)
#define AFX_DATAUNITE_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlowNode.h"
class CFlowChartDoc;
class CDataUnite : public CFlowNode  
{
	DECLARE_SERIAL(CDataUnite);
public:
	CDataUnite();
	virtual ~CDataUnite();
	CDataUnite(CString szNodename,BOOL bResult);
// Attributes
public:
	
	CString m_szBufPath;                  //���ݻ�����ȫ·����
	CString m_szWellName;                 //����·���ľ���
	CString	m_szSeries;                   //��������
	int m_PadNum;                         //������
	int m_ButtonNum;                      //ÿ����������
	int m_DataNum;                        //�ܵ����
	int	m_ButtonMid;                      //ÿ�������м���λ��
	int m_CalNum;                         //����������
	float m_StProDep,m_EnProDep,m_Rlev;   //������
	float m_Rm;
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL FMI8UnitData();
	BOOL FMI4UnitData();
	BOOL EMIUnitData();
	BOOL StarIIUnitData();
	BOOL CMIUnitData();

	BOOL DataCopy(CString InName,CString OutName);
};

#endif // !defined(AFX_DATAUNITE_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_)
