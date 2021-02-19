// ImageCreate.h: interface for the CImageCreat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGECREAT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
#define AFX_IMAGECREAT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DataUnite.h"
#include "FlowChartDoc.h"
class CImageCreat : public CDataUnite  
{
	DECLARE_SERIAL(CImageCreat);
public:
	CImageCreat();
	virtual ~CImageCreat();
	CImageCreat(CString szNodename,BOOL bResult);
// Attributes
public:
	BOOL  m_bRepair;							 //图像修复			 
	float m_radius;								 //修复半径

	BOOL  m_bCompose;							 //图象合成
	BOOL  m_bPadPro;                             //极板均衡处理
	BOOL  m_bFilter;                             //滤波处理
	BOOL  m_bBalance;                            //极板均衡处理
	float m_nClass;                              //Kmeans聚类族数4-512
	float m_PadMin;                              //极板最小频率值0-30
	BOOL  m_bLogScal;                            //标刻类型 TRUE--Log  FALSE--Line
	BOOL  m_bButPro;                             //电扣均衡处理
	float m_WinLongth;                           //滑动窗长
	float m_Bits;                                //钻头直径
//  Implementation	
public:
	//自动统计电扣频率求刻度区间
	BOOL DataFreqScale(CString Path,CString Name,float PadMin);
	//备份数据进行增强处理
	/*BOOL CopyCurve(CString InName,CString OutName);  */
	//极板均衡处理
	BOOL  AutoPadBalancePro(CString Name);
	//电扣均衡处理
	BOOL  AutoButBalancePro(CString Name);
	//图像合成 电扣动态KMeans聚类求刻度区间
	BOOL ImageCompose(CString Path,CString Name);
	//图像滤波处理
	BOOL ImageFilterPro(CString Path,CString Name);
	//图像修复
	void DataRepair(CString Path,CString Name);
	BOOL AllDataExtentPro(CString Path,CString Name);
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	void OnWellRefresh();
};

#endif // !defined(AFX_IMAGECREAT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
