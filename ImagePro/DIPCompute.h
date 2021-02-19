// DIPCompute.h: interface for the CDIPCompute class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIPCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
#define AFX_DIPCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageCreate.h"
class CDIPCompute : public CImageCreat  
{
	DECLARE_SERIAL(CDIPCompute);
public:
	CDIPCompute();
	CDIPCompute(CString szNodename,BOOL bResult);
	virtual ~CDIPCompute();
	// Attributes

public:
	float	m_WinLength;             //��ضԱȴ���
	float	m_StepLength;            //����������ߵĲ���
	float	m_SearchLength;          //����̽������
	float	m_DangMax;               //������
	float	m_RMin;                  //�μӼ�����ǵ���С���ϵ��
	float   m_GradCons;
	float   m_AzMax;
	float	m_NumMin;                //������
	//������������м����
	float m_RMAX;
	int IWL,ISL,ISTEP;
	float X[5000],Y[5000];
	float vr[10];//�뾶

	int  CORREL();                             //�������ϵ��
	BOOL DIPPro();                             //����ز����
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	//���ܷ������ά��ֵͼ��ָ�
	BOOL CreateFullImage(CString InName,CString OutName);
	//ƽ������ϳ�ͼ��ָ�
	BOOL TextureRecover(CString InName,CString OutName);
	float KrigingCompute(int Num,float *X);
};

#endif // !defined(AFX_DIPCOMPUTE_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
