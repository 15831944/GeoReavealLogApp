// ImageAnisotropy.h: interface for the CImageAnisotropy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEANISOTROPY_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
#define AFX_IMAGEANISOTROPY_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageCreate.h"
class CFlowChartDoc;
class CImageAnisotropy : public CImageCreat  
{
	DECLARE_SERIAL(CImageAnisotropy);
public:
	CImageAnisotropy();
	virtual ~CImageAnisotropy();
	CImageAnisotropy(CString szNodename,BOOL bResult);
// Attributes
public:
	float   m_AzMax,m_RxyMin;      //方位最大误差  各向异性最小检测值

	float	m_fWin;                   //滑动窗长
	float	m_fRlev;                  //滑动步长
	float	m_NumMin;                 //样本率
	int		m_iImage;				  //图像选择1-静态  2-动态  3-刻度
//  Implementation
public:
	BOOL ImgAniso();
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
};

#endif // !defined(AFX_IMAGEANISOTROPY_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
