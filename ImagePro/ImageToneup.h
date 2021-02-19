// ImageToneup.h: interface for the CImageToneup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGETONEUP_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
#define AFX_IMAGETONEUP_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageCreate.h"
class CFlowChartDoc;
class CImageToneup : public CImageCreat  
{
	DECLARE_SERIAL(CImageToneup);
public:
	CImageToneup();
	virtual ~CImageToneup();
	CImageToneup(CString szNodename,BOOL bResult);
// Attributes
public:
	BOOL	m_bGrads;             //梯度增强
	BOOL	m_bFilter;            //平滑滤波处理
	BOOL	m_bSquare;            //指数增强
	BOOL    m_bFreq;              //频率直方图增强
	BOOL    m_bMusic;             //小波技术图像动态增强
	int		m_nLevel;
	short   m_MaxPos,m_MinVal;
	float	m_WinLongth,m_RlevLongth;
//  Implementation
public:
	virtual BOOL Run();
	BOOL ACDataPro();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL PadDataDynFreq(CString Path,CString Name);     //分窗长对各极进行直方图均衡
	BOOL AllDataDynFreq(CString Path,CString Name);     //直方图处理
	BOOL AllDataDynSquare(CString Path,CString Name);   //图像平方指数增强
	BOOL AllMultiScale(CString Path,CString Name);      //多刻度图像对比增强
};

#endif // !defined(AFX_IMAGETONEUP_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
