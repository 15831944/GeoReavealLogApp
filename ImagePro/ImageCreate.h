// ImageCreate.h: interface for the CImageCreat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGECREAT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
#define AFX_IMAGECREAT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DataUnite.h"
#include "FlowChartDoc.h"
class CImageCreat : public CDataUnite  
{
	DECLARE_SERIAL(CImageCreat);
public:
	CImageCreat();
	virtual ~CImageCreat();
	CImageCreat(CString szNodename,BOOL bResult);
// Attributes
public:
	BOOL  m_bRepair;							 //ͼ���޸�			 
	float m_radius;								 //�޸��뾶

	BOOL  m_bCompose;							 //ͼ��ϳ�
	BOOL  m_bPadPro;                             //������⴦��
	BOOL  m_bFilter;                             //�˲�����
	BOOL  m_bBalance;                            //������⴦��
	float m_nClass;                              //Kmeans��������4-512
	float m_PadMin;                              //������СƵ��ֵ0-30
	BOOL  m_bLogScal;                            //������� TRUE--Log  FALSE--Line
	BOOL  m_bButPro;                             //��۾��⴦��
	float m_WinLongth;                           //��������
	float m_Bits;                                //��ͷֱ��
//  Implementation	
public:
	//�Զ�ͳ�Ƶ��Ƶ����̶�����
	BOOL DataFreqScale(CString Path,CString Name,float PadMin);
	//�������ݽ�����ǿ����
	/*BOOL CopyCurve(CString InName,CString OutName);  */
	//������⴦��
	BOOL  AutoPadBalancePro(CString Name);
	//��۾��⴦��
	BOOL  AutoButBalancePro(CString Name);
	//ͼ��ϳ� ��۶�̬KMeans������̶�����
	BOOL ImageCompose(CString Path,CString Name);
	//ͼ���˲�����
	BOOL ImageFilterPro(CString Path,CString Name);
	//ͼ���޸�
	void DataRepair(CString Path,CString Name);
	BOOL AllDataExtentPro(CString Path,CString Name);
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
	void OnWellRefresh();
};

#endif // !defined(AFX_IMAGECREAT_H__7230FED6_99E1_48EB_B363_C5225C30C19C__INCLUDED_)
