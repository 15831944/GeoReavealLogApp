// CentOffCorrect.h: interface for the CCentOffCorrect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CENTOFFCORRECT_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_)
#define AFX_CENTOFFCORRECT_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlowNode.h"
class CFlowChartDoc;
class CCentOffCorrect : public CFlowNode  
{
	DECLARE_SERIAL(CCentOffCorrect);
public:
	CCentOffCorrect();
	virtual ~CCentOffCorrect();
	CCentOffCorrect(CString szNodename,BOOL bResult);
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
	float fScale;                         //���Բ�ο�����
	float fX,fY;                          //���Բ��
	float m_fRadius,m_CenOff;             //���Բ�뾶��ƫ�ľ�
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	void FitCirclePro();
	BOOL MITCorrect(CString Path,CString InName,BOOL bWrite);
	void OnWellRefresh();
};

#endif // !defined(AFX_CentOffCorrect_H__3384DD47_D6F3_4AE4_A8B5_D429501813C7__INCLUDED_)
