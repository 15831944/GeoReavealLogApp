// ImageRMFScale.h: interface for the CImageRMFScale class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGERMFSCALE_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
#define AFX_IMAGERMFSCALE_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageCreate.h"
class CFlowChartDoc;
class CImageRMFScale : public CImageCreat  
{
	DECLARE_SERIAL(CImageRMFScale);
public:
	CImageRMFScale();
	virtual ~CImageRMFScale();
	CImageRMFScale(CString szNodename,BOOL bResult);
// Attributes
public:
	BOOL    m_bResistance;          //电阻率记录
	CString	m_szRMFWell;            //标准电阻率井名
	CString	m_szRMFName;            //浅侧向刻度 用于生成高分辨率电阻率曲线
	float   m_fRadius,m_CenOff;

	float m_fRlev;					//滑动步长
	float m_fWin;                   //滑动窗长
	float m_LeftScal,m_RightScal;   //浅侧向刻度范围
	int	  m_iImage;                 //选择刻度图像0--静态图像   1--动态图像

//  Implementation
public:
	BOOL CreateCrackImg(CString FullRTName);
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL CurveToBuf(CString m_szInWell,CString InName,CString OutName,float NewRlev,BOOL bAz);
};

#endif // !defined(AFX_IMAGERMFSCALE_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
