// GeoStorage.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Storage
(
	StorageID       int PRIMARY KEY IDENTITY,		--�洢�ռ���
	StorageName		varchar(64) NOT NULL,			--�洢�ռ�����
	StorageAlias	varchar(64),					--�洢�ռ����
	StorageKind  	varchar(32),			        --�洢�ռ����(���������졢�����⾮��Ŀ)
	FillUser		varchar(64),					--����Ա
	FillDate		DateTime,						--��д����
	Describe 		varchar(512),					--�洢�ռ�����
	UpperID			int NOT NULL					--�ϼ��洢�ռ���
)
*/
class CBusyMessage;
class CGeoStorage  
{
private:
	int		m_StorageID;
	CString m_StorageName;
	CString m_StorageAlias;
	CString m_StorageKind;
	CString m_FillUser;
	CString	m_FillDate;
	CString m_Describe;
	int		m_UpperID;

	CGeoAdo		m_ado;
public:
	CStringArray a_StorageName;
	CStringArray a_StorageKind;
	CStringArray a_StorageID;
	CStringArray a_UpperID;

public:
	CGeoStorage();
	virtual ~CGeoStorage();
	
	BOOL InitData(CString szCon);
	void Disconnect();
public:	
	int		GetStorageID();
	void	SetStorageID(int iStorageID);
	CString GetStorageName();
	void	SetStorageName(CString cStorageKind);
	CString GetStorageKind();
	void	SetStorageKind(CString cStorageKind);
	CString GetStorageAlias();
	void	SetStorageAlias(CString cStorageAlias);
	CString GetDescribe();
	void	SetDescribe(CString cDescribe);
	int		GetUpperID();
	void	SetUpperID(int iUpperID);
	CString GetFillUser();
	void	SetFillUser(CString cUser);
	CString GetFillDate();
	void	SetFillDate(CString cDate);

public:		//���ݿ����
	//�ж�ָ���Ĵ洢�ռ������Ƿ����
	DWORD	HaveName(CString cStorageName);
	//�ж�ָ���Ĵ洢�ռ���Ƿ�����ϼ��洢�ռ�
	DWORD	HaveUpper(int iStorageID);
	//�ж�ָ���Ĵ洢�ռ��Ƿ�����¼��洢�ռ䣨�����������ɾ����
	BOOL	HaveSon(int iStorageID);	 
	//�ж�ָ���Ĵ洢�ռ����Ƿ�������ߣ����������ɾ����
	BOOL	HaveCurve(int iStorageID);	
	//��ȡ���д洢�ռ��š����ƺ��ϼ��洢�ռ��ŵ���Ϣ���������CStringArray
	void	LoadStorageInfo();

	//����ָ��ID��ȫ���Ӵ洢�ռ���Ϣ���ַ���������
	void	LoadSonInfo(DWORD UpperID);
	void	LoadStorageInfo(DWORD UpperID);

	//���س��⾮��Ŀ����������洢�ռ���Ϣ���ַ���������
	void	LoadWorkSpaceInfo(CBusyMessage *pBusy);
	void	LoadSonInfo(DWORD UpperID,CBusyMessage *pBusy);
	void	PumpMessages();

	//�����²����¼�ı��
	long	sql_insert();					 
	
	BOOL	sql_update(int iStorageID);
	BOOL	sql_delete(int iStorageID);
	//���ݴ洢�ռ��Ŷ�ȡ�����ֶ�ֵ
	BOOL GetData(int iStorageID);			 
};

