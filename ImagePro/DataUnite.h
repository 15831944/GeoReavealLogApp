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
	
	CString m_szBufPath;                  //数据缓冲区全路径名
	CString m_szWellName;                 //包括路径的井名
	CString	m_szSeries;                   //仪器类型
	int m_PadNum;                         //极板数
	int m_ButtonNum;                      //每个极板电扣数
	int m_DataNum;                        //总电扣数
	int	m_ButtonMid;                      //每个极板中间电扣位置
	int m_CalNum;                         //井径曲线数
	float m_StProDep,m_EnProDep,m_Rlev;   //处理井段
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
