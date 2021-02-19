// ImagePoisePro.h: interface for the CImagePoisePro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ImagePRO_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_)
#define AFX_ImagePRO_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
#include "DataUnite.h"
class CFlowChartDoc;
class CDataPoisePro : public CDataUnite  
{
	DECLARE_SERIAL(CDataPoisePro);
public:
	CDataPoisePro();
	virtual ~CDataPoisePro();
	CDataPoisePro(CString szNodename,BOOL bResult);
// Attributes
public:
	CString m_szButNo;                              //�쳣��ۺ�
	CString m_szPadNo;                              //�쳣�����
	float   m_MaxFre;                               //�쳣���Ƶ��ֵ
	float   m_ScaleMin;                             //�쳣��ۼ�Сֵ
	float   m_ScaleMax;                             //�쳣��ۼ���ֵ
	float   m_PadMax;                               //�쳣���弫��ֵ
	float	m_ButMin;								//������С���ֵ
	BOOL    m_bFilter;                              //ƽ���˲�����
	BOOL	m_bLogScal;                             //�����̶�
	BOOL    m_bAutoPad;                             //������⴦��
	BOOL	m_bAutoBut;                             //��۾��⴦��

	float m_Bits;									//��ͷֱ��
//  Implementation
public:
	
	BOOL ButBalancePro(CString Name);               //ָ������쳣����
	BOOL PadBalancePro(CString Name);               //ָ�������쳣���� 	
	BOOL DataMaxMinPro(CString CurveName,float Max,float Min);
	//����ƽ���˲�����
	BOOL DataFilterPro(CString Path,CString Name);
	BOOL DataScalePro(CString Path,CString Name);
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);

	BOOL MITGeometryCorrect(CString InName,CString OutName);
	BOOL AutoButBalancePro(CString Name);
};

#endif // !defined(AFX_DATAPRO_H__8CE6570C_B2E8_47AA_973A_C67640A7F2D0__INCLUDED_)
