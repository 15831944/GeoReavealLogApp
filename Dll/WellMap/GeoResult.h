// GeoResult.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Result
(
	ResultID		int PRIMARY KEY IDENTITY,		--������ͳɹ����
	LayerName		varchar(64) NOT NULL,			--��λ����				CW
	LayerAlias		varchar(64),					--��λ����
	Number			varchar(16),					--��ע���				CH
	DepSt  			float,			        		--��ʼ���				DEP1
	DepEn  			float,			        		--�������				DEP2
	DepHD			float,			        		--���					HD
	DepHD1			float,			        		--��Ч���
	POR				float,			        		--��϶��				POR
	PERMA			float,			        		--��͸��				PERM
	SW				float,			        		--��ˮ���Ͷ�			SW
	SH				float,			        		--���ʺ���				SH
	Conclusion		varchar(64),					--���ͽ���				JL
	SP				float,			        		--��Ȼ��λ				SP
	GR				float,			        		--��Ȼ٤��				GR
	DEN				float,			        		--�ܶ�					DEN
	CNL				float,			        		--����					CNL
	AC				float,			        		--����					AC
	RT				float,			        		--�������				RT1
	RXO				float,			        		--�е�����				RT
	RS				float,			        		--ǳ������				RXO
	FillUser		varchar(64),					--�ύ��
	FillDate		datetime,						--�ύ����
	Describe 		varchar(1024),					--��������				BZ
	UpperID			int NOT NULL					--�洢�ռ���
)
*/
class CGeoResult  
{
private:
	DWORD		m_ResultID;
	CGeoAdo		m_ado;
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
	float		m_DEN;				//�ܶ�
	float		m_CNL;				//����
	float		m_AC;				//����
	float		m_RT;				//�������
	float		m_RXO;				//�е�����
	float		m_RS;				//ǳ������
	CString		m_Describe;			 //��������
	CString		m_FillUser;			 //�ύ��
	CString		m_FillDate;			 //�ύ����
	DWORD		m_UpperID;			 //�����洢�ռ���
public:
	CGeoResult();
	virtual ~CGeoResult();
	
	BOOL InitData(CString szCon);
	void Disconnect();
public:	
	CString GetDescribe();
	void	SetDescribe(CString cDescribe);
	int		GetUpperID();
	void	SetUpperID(int iUpperID);
	CString GetFillUser();
	void	SetFillUser(CString cUser);
	CString GetFillDate();
	void	SetFillDate(CString cDate);

public:		//���ݿ����	 
	//�����²����¼�ı��
	DWORD	sql_insert();					 
	
	BOOL	sql_update(int iResultID);
	BOOL	sql_delete(int iResultlID);
	//���ݾ���Ϣ��Ŷ�ȡ�����ֶ�ֵ
	BOOL GetData(int iResultID);			 
};

