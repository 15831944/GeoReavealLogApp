// GeoResult.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Result
(
	ResultID			bigint  PRIMARY KEY IDENTITY,	--������ͳɹ����
	LayerName			varchar(64) NOT NULL,			--��λ����				
	LayerAlias			varchar(64),					--��λ����
	Number				varchar(16),					--��ע���				
	DepSt  				float,			        		--��ʼ���				
	DepEn  				float,			        		--�������				
	DepHD				float,			        		--���					
	DepHD1				float,			        		--��Ч���
	POR					float,			        		--��϶��				
	PERMA				float,			        		--��͸��				
	SW					float,			        		--��ˮ���Ͷ�			
	SH					float,			        		--���ʺ���				
	Conclusion			varchar(64),					--���ͽ���				
	SP					float,			        		--��Ȼ��λ				
	GR					float,			        		--��Ȼ٤��		
	CGR					float,			        		--����٤��	
	DEN					float,			        		--�ܶ�					
	CNL					float,			        		--����					
	AC					float,			        		--����					
	RT					float,			        		--�������				
	RXO					float,			        		--�е�����				
	RS					float,			        		--ǳ������				
	PorMax;				float,							--����϶��
	SwMax;				float,							--���ˮ���Ͷ�
	HoleNum;			float,							--�ܿ׶���
	HolePor;			float,							--�������
	HoleDia;			float,							--ƽ���׾�
	CrackNum;			float,							--�ѷ�����
	CrackLength;		float,							--�ѷ쳤��
	CrackWidth;			float,							--�ѷ���
	CrackWaterWidth;	float,							--�ѷ�ˮ�������
	CrackPor;			float,							--�ѷ������
	Describe 			varchar(512),					--��������
	FillUser			varchar(64),					--�ύ��
	FillDate			datetime,						--�ύ����				
	UpperID				bigint NOT NULL					--�洢�ռ���
)
*/
class AFX_EXT_CLASS CGeoResult :public CGeoAdo
{
public:
	CString		m_LayerName;		//��λ����
	CString		m_LayerAlias;		//��λ����
	CString		m_Number;			//��ע���
	float		m_DepSt;  			//��ʼ���
	float		m_DepEn;  			//�������
	float		m_DepHD;			//���
	float		m_DepHD1;			//��Ч���
	float		m_POR;				//��϶��	
	float		m_PERMA;			//��͸��
	float		m_SW;				//��ˮ���Ͷ�	
	float		m_SH;				//���ʺ���
	CString		m_Conclusion;		//���ͽ���
	float		m_SP;				//��Ȼ��λ
	float		m_GR;				//��Ȼ٤��
	float		m_CGR;				//����٤��
	float		m_DEN;				//�ܶ�
	float		m_CNL;				//����
	float		m_AC;				//����
	float		m_RT;				//�������
	float		m_RXO;				//�е�����
	float		m_RS;				//ǳ������
	float		m_PorMax;			//����϶��
	float		m_SwMax;			//���ˮ���Ͷ�
	float		m_HoleNum;			//�ܿ׶���
	float		m_HolePor;			//�������
	float		m_HoleDia;			//ƽ���׾�
	float		m_CrackNum;			//�ѷ�����
	float		m_CrackLength;		//�ѷ쳤��
	float		m_CrackWidth;		//�ѷ���
	float		m_CrackWaterWidth;	//�ѷ�ˮ�������
	float		m_CrackPor;			//�ѷ������
public:
	CGeoResult();
	virtual ~CGeoResult();

public:	
	CString GetDescribe();
	void	SetDescribe(CString cDescribe);
	DWORD		GetUpperID();
	void	SetUpperID(DWORD iUpperID);
	CString GetFillUser();
	void	SetFillUser(CString cUser);
	CString GetFillDate();
	void	SetFillDate(CString cDate);

public:		//���ݿ����	 
	//�����²����¼�ı��
	DWORD	sql_insert();					 
	
	BOOL	sql_update(DWORD iResultID);
	BOOL	sql_delete(DWORD iResultlID);
	//���ݾ���Ϣ��Ŷ�ȡ�����ֶ�ֵ
	BOOL GetData(DWORD iResultID);			 
};

