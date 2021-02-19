// Production.h: interface for the CDocument class.
//////////////////////////////////////////////////////////////////////
#if !defined _GEOPRODUCTION_H__
#define _GEOPRODUCTION_H__

#include "GeoAdo.h"

#pragma once
/***************************�����ɹ��ĵ���Ϣ��************************************
CREATE TABLE Production
(
	ProductionID	int PRIMARY KEY IDENTITY,		--�ĵ����
	DocName			varchar(64) NOT NULL,			--�ĵ�����
	DocType			varchar(16),					--�ĵ�����
	DocSize			int,							--�ĵ��ֽ���
	DocCreDate		varchar(32),					--�ĵ���������
	Describe 		varchar(512),					--�ĵ�����
	FillUser		varchar(64),					--����Ա
	FillDate		varchar(32),					--��д����
	DataZip			tinyint,						--����ѹ����ʶ 0-δѹ�� 16-��׼6��Zlibѹ��
	UpperID			int NOT NULL,					--�洢�ռ���
	DocByte			image							--���ݿռ�
)*/
//////////////////////////////////////////////////////////////////////
class CGeoProduction  
{	
private:
	CGeoAdo		m_ado;
public:
	DWORD		m_ProductionID;	
	CString		m_DocName;		
	CString		m_DocType;		
	DWORD		m_DocSize;		
	CString		m_DocCreDate;	
		
	CString		m_Describe; 	
	CString		m_FillUser;	
	CString		m_FillDate;	
	DWORD		m_UpperID;		
	int			m_DataZip;
	CGeoProduction();
	virtual ~CGeoProduction();
	
	
	BOOL InitData(CString szCon);
	void Disconnect();
public:	//��Ա�����ԵĲ�������
	

public:  //���ݿ��������
	//�ж�ָ�����������Ƿ����
	DWORD	HaveName(CString cDocName);			
	//����ָ�������߱�ŷ�����������
	CString GetName(DWORD iDocID);					
	//�������߱�Ŷ�ȡ�����ֶ�ֵ
	BOOL	GetData(DWORD iDocID);					
	//�����ĵ�ID�޸��ĵ�����DocByte�ֶ�ֵ
	void	SetDocByte(DWORD iDocID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH]);
	//�����ĵ�ID��ȡ�ĵ�����DocByte�ֶ�ֵ
	DWORD	GetDocByte(DWORD iDocID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH]);
	//���ݲ⾮��ĿIDɾ������Ŀ�е����гɹ��ĵ�
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iDocID);
	//���´洢�ռ���Ϣ���ڵ�ת�洢�ռ�ʱ����
	BOOL	sql_updateStorage(int iStorageID);		
	BOOL	sql_delete(DWORD iDocID);	

};


//////////////////////////////////////////////////////////////////////
#endif 
