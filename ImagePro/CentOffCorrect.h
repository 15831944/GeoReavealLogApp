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
	CString m_szBufPath;                  //数据缓冲区全路径名
	CString m_szWellName;                 //包括路径的井名
	CString	m_szSeries;                   //仪器类型
	int m_PadNum;                         //极板数
	int m_ButtonNum;                      //每个极板电扣数
	int m_DataNum;                        //总电扣数
	int	m_ButtonMid;                      //每个极板中间电扣位置
	int m_CalNum;                         //井径曲线数
	float m_StProDep,m_EnProDep,m_Rlev;   //处理井段
	float fScale;                         //拟合圆参考比例
	float fX,fY;                          //拟合圆点
	float m_fRadius,m_CenOff;             //拟合圆半径、偏心距
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
