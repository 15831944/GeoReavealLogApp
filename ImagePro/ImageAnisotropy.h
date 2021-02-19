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
	float   m_AzMax,m_RxyMin;      //��λ������  ����������С���ֵ

	float	m_fWin;                   //��������
	float	m_fRlev;                  //��������
	float	m_NumMin;                 //������
	int		m_iImage;				  //ͼ��ѡ��1-��̬  2-��̬  3-�̶�
//  Implementation
public:
	BOOL ImgAniso();
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
};

#endif // !defined(AFX_IMAGEANISOTROPY_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
