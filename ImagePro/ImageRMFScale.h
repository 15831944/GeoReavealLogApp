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
	BOOL    m_bResistance;          //�����ʼ�¼
	CString	m_szRMFWell;            //��׼�����ʾ���
	CString	m_szRMFName;            //ǳ����̶� �������ɸ߷ֱ��ʵ���������
	float   m_fRadius,m_CenOff;

	float m_fRlev;					//��������
	float m_fWin;                   //��������
	float m_LeftScal,m_RightScal;   //ǳ����̶ȷ�Χ
	int	  m_iImage;                 //ѡ��̶�ͼ��0--��̬ͼ��   1--��̬ͼ��

//  Implementation
public:
	BOOL CreateCrackImg(CString FullRTName);
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL CurveToBuf(CString m_szInWell,CString InName,CString OutName,float NewRlev,BOOL bAz);
};

#endif // !defined(AFX_IMAGERMFSCALE_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
