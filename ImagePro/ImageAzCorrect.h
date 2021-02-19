// ImageAzCorrect.h: interface for the CImageAzCorrect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEAZCORREC_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
#define AFX_IMAGEAZCORREC_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageCreate.h"
#include "FlowChartDoc.h"
class CImageAzCorrect : public CImageCreat  
{
	DECLARE_SERIAL(CImageAzCorrect);
public:
	CImageAzCorrect();
	virtual ~CImageAzCorrect();
	CImageAzCorrect(CString szNodename,BOOL bResult);
// Attributes
public:
	CString m_szImageType;                //ÕºœÒ¿‡–Õ
public:
	virtual BOOL Run();
	BOOL AzimuthCorret(CString Name);
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
};

#endif // !defined(AFX_IMAGEAZCORRECT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
