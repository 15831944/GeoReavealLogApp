// GeoResult.h
/*********************************************************************/
#pragma once
#include <afxtempl.h>
#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE OilTest
(
	OilTestID		bigint PRIMARY KEY IDENTITY,	--��λ���
	LayerName		varchar(64) NOT NULL,			--��λ����				
	LayerAlias		varchar(64),					--��λ����
	TestMethod		varchar(10),					--���ͷ�ʽ	
	OutMethod		varchar(10),					--������ʽ
	Conclusion		varchar(10),					--���ͽ���
	TestDate		datetime,						--��������
	Reference		varchar(128),					--��������
	ReferDate		datetime,						--��������

	DepSt  			float,			        		--��ʼ���				
	DepEn  			float,			        		--�������								
	PorePlate		float,			        		--�װ�
	Glib			float,			        		--����
	CasingPres		float,			        		--��ѹ
	OilPres			float,			        		--��ѹ
	FlowPres		float,			        		--��ѹ
	TopPres			float,			        		--��ѹ
	TopTemp			float,			        		--����
	ClosePres		float,			        		--�ؾ�ѹ��
	LayerPres		float,			        		--�ز�ѹ��
	OilOut			float,			        		--�ղ���
	GasOut			float,			        		--�ղ���
	WaterOut		float,			        		--�ղ�ˮ
	Describe 		varchar(1024),					--��������
	FillUser		varchar(64),					--�ύ��
	FillDate		datetime,						--�ύ����			
	UpperID			bigint NOT NULL					--�洢�ռ���
)
*/
class AFX_EXT_CLASS CGeoOilTest :public  CGeoAdo
{
public:
	CString		m_LayerName;		//��λ����
	CString		m_LayerAlias;		//��λ����

	CString		m_TestMethod;		//���ͷ�ʽ	
	CString		m_OutMethod;		//������ʽ
	CString		m_Conclusion;		//���ͽ���
	CString		m_TestDate;			//��������
	CString		m_Reference;		//��������
	CString		m_ReferDate;		//��������

	float		m_DepSt;  			//��ʼ���				
	float		m_DepEn;  			//�������								
	float		m_PorePlate;		//�װ�
	float		m_Glib;				//����
	float		m_CasingPres;		//��ѹ
	float		m_OilPres;			//��ѹ
	float		m_FlowPres;			//��ѹ
	float		m_TopPres;			//��ѹ
	float		m_TopTemp;			//����
	float		m_ClosePres;		//�ؾ�ѹ��
	float		m_LayerPres;		//�ز�ѹ��
	float		m_OilOut;			//�ղ���
	float		m_GasOut;			//�ղ���
	float		m_WaterOut;			//�ղ�ˮ	

//	���ʹ򿪾���
	CArray<float,float&> a_OpenDepSt;
	CArray<float,float&> a_OpenDepEn;
public:
	CGeoOilTest();
	virtual ~CGeoOilTest();

public:	
	CString GetDescribe();
	void	SetDescribe(CString cDescribe);
	DWORD	GetUpperID();
	void	SetUpperID(DWORD iUpperID);
	CString GetFillUser();
	void	SetFillUser(CString cUser);
	CString GetFillDate();
	void	SetFillDate(CString cDate);

public:		//���ݿ����	 
	//�����²����¼�ı��
	DWORD	sql_insert();					 
	BOOL	sql_update(DWORD iOilTestID);
	BOOL	sql_delete(DWORD iOilTestID);
	//����������Ϣ��Ŷ�ȡ�����ֶ�ֵ
	BOOL	GetData(DWORD iOilTestID);	
	BOOL	GetOpenDep(DWORD iOilTestID);
	DWORD	InsertOpenDep(DWORD iOilTestID);
	BOOL	DeleteOpenDep(DWORD iOilTestID);
	BOOL	UpdateOpenDep(DWORD iOilTestID);
};

