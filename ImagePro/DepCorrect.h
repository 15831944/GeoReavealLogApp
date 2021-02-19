// DepCorrect.h: interface for the CDepCorrect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPCORRECT_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_)
#define AFX_DEPCORRECT_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlowNode.h"
class CFlowChartDoc;
class CDepCorrect : public CFlowNode  
{
	DECLARE_SERIAL(CDepCorrect);
public:
	CDepCorrect();
	virtual ~CDepCorrect();
// Attributes
public:
	CString m_szBufPath;                  //���ݻ�����ȫ·����
	CString m_szWellName;                 //����·���ľ���
	CString	m_szSeries;                   //��������
	int m_PadNum;                         //������
	int m_ButtonNum;                      //ÿ����������
	int	m_ButtonMid;                      //ÿ�������м���λ��
	int m_CalNum;                         //����������
	float m_StProDep,m_EnProDep,m_Rlev;   //������
//  Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
};

#endif // !defined(AFX_DEPCORRECT_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_)
