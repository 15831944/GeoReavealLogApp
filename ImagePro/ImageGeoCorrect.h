// ImageGeoCorrect.h: interface for the CImageGeoCorrect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IIMAGEGEOCORRECT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
#define AFX_IMAGEGEOCORRECT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageCreate.h"
#include "FlowChartDoc.h"
class CImageGeoCorrect : public CImageCreat  
{
	DECLARE_SERIAL(CImageGeoCorrect);
public:
	CImageGeoCorrect();
	virtual ~CImageGeoCorrect();
	CImageGeoCorrect(CString szNodename,BOOL bResult);
// Attributes
public:
	CString m_szImageType;                          //ÕºœÒ¿‡–Õ
	CString m_szInName,m_szOutName;
//  Implementation
public:
	virtual BOOL Run();
	BOOL doRun(CString InName,CString OutName);
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL GeometryCorrect(CString InName,CString OutName);
	BOOL AzimuthCorret(CString Path,CString Name);
};

#endif // !defined(AFX_IMAGEGEOCORRECT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
