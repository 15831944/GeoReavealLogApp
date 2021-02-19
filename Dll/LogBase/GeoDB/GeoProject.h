// GeoProject.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Project
(
	ProjectID		int PRIMARY KEY IDENTITY,		--��Ŀ���
	ProjectName		varchar(64) NOT NULL,			--��Ŀ����			TM
	ProjectAlias	varchar(64),					--��Ŀ����
	LogDate  		datetime,						--�⾮����			CJRQ
	LogStDep  		float,			        		--�⾮��ʼ���		SD1
	LogEnDep  		float,			        		--�⾮�������		SD2
	DepRatio		float,							--��ȱ���			SDBL
	LogDevice  		varchar(64),			        --�⾮�豸			YQXH
	LogTeam  		varchar(128),			        --�⾮С��			CJDH
	LogCompany  	varchar(256),			        --�⾮��˾			CJGS
	LogLeader  		varchar(64),			        --�⾮С�ӳ�		DZ
	LogEngineer  	varchar(64),			        --�⾮����ʦ		CZY
	CasingNote  	varchar(128),			        --�׹�����
	BitNote  		varchar(128),			        --��ͷ����
	FluidType		varchar(64),			        --�ཬ����			ZJYLX
	FluidDen		float,			        		--�ཬ�ܶ�			ZJYMD
	FluidVis		float,			        		--�ཬճ��			ZJYND
	FluidRm			float,			        		--�ཬ������		ZJYDZL
	FluidTem		float,			        		--�ཬ�¶�			18
	BottomDep		float,			        		--�������			CSJS
	BottomTem		float,			        		--�����¶�			JDWD
	DeviceNote  	varchar(256),			        --�������
	AuditUser		varchar(64),					--�����
	AuditDate		datetime,						--�������
	Describe 		varchar(512),					--��Ŀ����			JSSM+CSXZ
	FillUser		varchar(64),					--�ύ��
	FillDate		datetime,						--�ύ����
	UpperID			int NOT NULL					--�����洢�ռ���
)
*/
class AFX_EXT_CLASS CGeoProject :public CGeoAdo
{
public:
	CString		m_ProjectName;		//��Ŀ����
	CString		m_ProjectAlias;		//��Ŀ����
	CString		m_LogDate;  		//�⾮����
	float		m_LogStDep;  		//�⾮��ʼ���
	float		m_LogEnDep;  		//�⾮�������
	float		m_DepRatio;			//��ȱ���
	CString		m_LogDevice;  	    //�⾮�豸
	CString		m_LogTeam;  		//�⾮С��
	CString		m_LogCompany;  	    //�⾮��˾
	CString		m_LogLeader;  	    //�⾮С�ӳ�
	CString		m_LogEngineer;  	//�⾮����ʦ
	CString		m_CasingNote;  	    //�׹�����
	CString		m_BitNote;  		//��ͷ����
	CString		m_FluidType;		//�ཬ����
	float		m_FluidDen;			//�ཬ�ܶ�
	float		m_FluidVis;			//�ཬճ��
	float		m_FluidRm;			//�ཬ������
	float		m_FluidTem;			//�ཬ�¶�
	float		m_BottomDep;		//�������
	float		m_BottomTem;		//�����¶�
	CString		m_DeviceNote;  	    //�������
	CString		m_AuditUser;		//�����
	CString		m_AuditDate;		//�������
public:
	CGeoProject();
	virtual ~CGeoProject();

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
	
	BOOL	sql_update(DWORD iProjectID);
	BOOL	sql_delete(DWORD iProjectID);
	//���ݾ���Ϣ��Ŷ�ȡ�����ֶ�ֵ
	BOOL GetData(DWORD iProjectID);	
	//���������洢�ձ�Ŷ�ȡ����Ϣ�����ֶ�ֵ
	BOOL GetDataReferToUpperID(DWORD iUpperID);
};

