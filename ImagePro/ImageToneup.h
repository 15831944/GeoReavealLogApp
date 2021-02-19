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
	BOOL	m_bGrads;             //�ݶ���ǿ
	BOOL	m_bFilter;            //ƽ���˲�����
	BOOL	m_bSquare;            //ָ����ǿ
	BOOL    m_bFreq;              //Ƶ��ֱ��ͼ��ǿ
	BOOL    m_bMusic;             //С������ͼ��̬��ǿ
	int		m_nLevel;
	short   m_MaxPos,m_MinVal;
	float	m_WinLongth,m_RlevLongth;
//  Implementation
public:
	virtual BOOL Run();
	BOOL ACDataPro();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	BOOL PadDataDynFreq(CString Path,CString Name);     //�ִ����Ը�������ֱ��ͼ����
	BOOL AllDataDynFreq(CString Path,CString Name);     //ֱ��ͼ����
	BOOL AllDataDynSquare(CString Path,CString Name);   //ͼ��ƽ��ָ����ǿ
	BOOL AllMultiScale(CString Path,CString Name);      //��̶�ͼ��Ա���ǿ
};

#endif // !defined(AFX_IMAGETONEUP_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
