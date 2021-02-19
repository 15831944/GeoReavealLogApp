// GeoResult.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Layer
(
	LayerID			bigint PRIMARY KEY IDENTITY,	--��λ���
	LayerName		varchar(64) NOT NULL,			--��λ����				
	LayerAlias		varchar(64),					--��λ����
	DepLog  		float,			        		--�⾮���				
	TVLog  			float,			        		--�⾮����				
	DepGeo			float,			        		--�������					
	TVGeo			float,			        		--���ʴ���
	RemOption		varchar(10),					--��עѡ��                ʵ�ߵ׽� ���ߵ׽� �ϲ��ʶ ��ֱ��λ ˮƽ��λ			
	Describe 		varchar(512),					--��λ����
	FillUser		varchar(64),					--�ύ��
	FillDate		datetime,						--�ύ����			
	UpperID			int NOT NULL					--�洢�ռ���
)
*/
class AFX_EXT_CLASS CGeoLayer :public CGeoAdo
{

public:
	CString		m_LayerName;		//��λ����
	CString		m_LayerAlias;		//��λ����

	float		m_DepLog;  			//�⾮���
	float		m_TVLog;  			//�⾮����
	float		m_DepGeo;			//�������
	float		m_TVGeo;			//���ʴ���
	CString		m_RemOption;		//��עѡ��               ʵ�ߵ׽� ���ߵ׽� �ϲ��ʶ ��ֱ��λ ˮƽ��λ	

public:
	CGeoLayer();
	virtual ~CGeoLayer();

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
	
	BOOL	sql_update(DWORD iLayerID);
	BOOL	sql_delete(DWORD iLayerID);
	//���ݲ�λ��Ϣ��Ŷ�ȡ�����ֶ�ֵ
	BOOL GetData(DWORD iLayerID);			 
};

