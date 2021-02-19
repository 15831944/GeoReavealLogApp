// ImagePoisePro.h: interface for the CImagePoisePro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ImagePRO_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_)
#define AFX_ImagePRO_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
#include "DataUnite.h"
class CFlowChartDoc;
class CDataPoisePro : public CDataUnite  
{
	DECLARE_SERIAL(CDataPoisePro);
public:
	CDataPoisePro();
	virtual ~CDataPoisePro();
	CDataPoisePro(CString szNodename,BOOL bResult);
// Attributes
public:
	CString m_szButNo;                              //异常电扣号
	CString m_szPadNo;                              //异常极板号
	float   m_MaxFre;                               //异常电扣频率值
	float   m_ScaleMin;                             //异常电扣极小值
	float   m_ScaleMax;                             //异常电扣极大值
	float   m_PadMax;                               //异常极板极大值
	float	m_ButMin;								//处理最小电扣值
	BOOL    m_bFilter;                              //平滑滤波处理
	BOOL	m_bLogScal;                             //对数刻度
	BOOL    m_bAutoPad;                             //极板均衡处理
	BOOL	m_bAutoBut;                             //电扣均衡处理

	float m_Bits;									//钻头直径
//  Implementation
public:
	
	BOOL ButBalancePro(CString Name);               //指定电扣异常处理
	BOOL PadBalancePro(CString Name);               //指定极板异常处理 	
	BOOL DataMaxMinPro(CString CurveName,float Max,float Min);
	//数据平滑滤波处理
	BOOL DataFilterPro(CString Path,CString Name);
	BOOL DataScalePro(CString Path,CString Name);
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);

	BOOL MITGeometryCorrect(CString InName,CString OutName);
	BOOL AutoButBalancePro(CString Name);
};

#endif // !defined(AFX_DATAPRO_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_)
