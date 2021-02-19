// EdegeDetect.h: interface for the CEdgeDetect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDGEDETECT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
#define AFX_EDGEDETECT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageCreate.h"
#include "FlowChartDoc.h"
class CEdgeDetect : public CImageCreat  
{
	DECLARE_SERIAL(CEdgeDetect);
public:
	CEdgeDetect();
	virtual ~CEdgeDetect();
	CEdgeDetect(CString szNodename,BOOL bResult);
// Attributes
public:
	CString m_szEdgeName;
	int m_EdgeType;
	float m_Win,m_WinRlev,m_flagValue,m_MinValue,m_MaxValue;
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL StaticEdge(CString InName,CString OutName);
	BOOL CreatImgData(CString InName);
};

#endif // !defined(AFX_EDGEDETECT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
