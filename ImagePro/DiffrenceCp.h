// DiffrenceCp.h: interface for the CDiffrenceCp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIFFRENCECP_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
#define AFX_DIFFRENCECP_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageCreate.h"
#include "FlowChartDoc.h"
class CDiffrenceCp : public CImageCreat  
{
	DECLARE_SERIAL(CDiffrenceCp);
public:
	CDiffrenceCp();
	virtual ~CDiffrenceCp();
	CDiffrenceCp(CString szNodename,BOOL bResult);
// 生成井眼图像三维重建，生成4个方位垂直截面图
	BOOL CreateSectionImage(CString InName,CString OutName,CString SectionName);
// Attributes
public:
	CString m_szImageType;                          //图像类型
	float m_DaltaDep;
	CString m_szDifName;
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL DiffrencePro(CString InName,CString OutName);
	BOOL Template(CString InName,CString OutName,int iTempH, int iTempW, int iTempCX, int iTempCY, FLOAT *fpTempArray);
	//图像中值滤波
	float FindMedianValue(float *lpbArray, int iArrayLen);
	BOOL MedianFilter(CString InName,CString OutName,int iFilterH, int iFilterW, int iFilterCX, int iFilterCY);
	//探测法边界跟踪
	BOOL TraceEdge(CString InName,CString OutName);
	//k-均值聚类法图像分割
	BOOL KAveIncise(CString InName,CString OutName,int k);
};

#endif // !defined(AFX_DiffrenceCp_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
