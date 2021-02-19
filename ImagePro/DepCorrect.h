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
	CString m_szBufPath;                  //数据缓冲区全路径名
	CString m_szWellName;                 //包括路径的井名
	CString	m_szSeries;                   //仪器类型
	int m_PadNum;                         //极板数
	int m_ButtonNum;                      //每个极板电扣数
	int	m_ButtonMid;                      //每个极板中间电扣位置
	int m_CalNum;                         //井径曲线数
	float m_StProDep,m_EnProDep,m_Rlev;   //处理井段
//  Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
};

#endif // !defined(AFX_DEPCORRECT_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_)
