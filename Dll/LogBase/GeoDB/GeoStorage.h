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
	FillUser		varchar(64),					--���
	FillDate		DateTime,						--�����
	Describe 		varchar(512),					--�洢�ռ�����
	UpperID			int NOT NULL					--�ϼ��洢�ռ���
)
*/
class CBusyMessage;
class AFX_EXT_CLASS CGeoStorage :public CGeoAdo
{
public:
	CString		m_StorageName;
	CString		m_StorageAlias;
	CString		m_StorageKind;

	CStringArray a_StorageName;
	CStringArray a_StorageAlias;
	CStringArray a_StorageKind;
	CArray<DWORD,DWORD&> a_StorageID;
	CArray<DWORD,DWORD&> a_UpperID;

public:
	CGeoStorage();
	virtual ~CGeoStorage();

public:	
	DWORD	GetStorageID();
	void	SetStorageID(DWORD iStorageID);
	CString GetStorageName();
	void	SetStorageName(CString cStorageKind);
	CString GetStorageKind();
	void	SetStorageKind(CString cStorageKind);
	CString GetStorageAlias();
	void	SetStorageAlias(CString cStorageAlias);
	CString GetDescribe();
	void	SetDescribe(CString cDescribe);
	DWORD	GetUpperID();
	void	SetUpperID(DWORD iUpperID);
	CString GetFillUser();
	void	SetFillUser(CString cUser);
	CString GetFillDate();
	void	SetFillDate(CString cDate);

public:		//���ݿ����
	//�ж�ָ���Ĵ洢�ռ������Ƿ����
	DWORD	HaveName(CString cStorageName);
	//�ж�ָ���Ĵ洢�ռ���Ƿ�����ϼ��洢�ռ�
	DWORD	HaveUpper(DWORD iStorageID);
	//�ж�ָ���Ĵ洢�ռ��Ƿ�����¼��洢�ռ䣨�����������ɾ����
	BOOL	HaveSon(DWORD iStorageID);	 
	//�ж�ָ���Ĵ洢�ռ����Ƿ�������ߣ����������ɾ����
	BOOL	HaveCurve(DWORD iStorageID);	
	//��ȡ���д洢�ռ��š����ƺ��ϼ��洢�ռ��ŵ���Ϣ���������CStringArray
	void	LoadStorageInfo();

	//����ָ��ID��ȫ���Ӵ洢�ռ���Ϣ���ַ���������
	void	LoadSonInfo(DWORD UpperID);
	void	LoadStorageInfo(DWORD UpperID);
	void	LoadWellInfo(DWORD UpperID);
	void	LoadProjectInfo(DWORD UpperID);
	//����ָ��UpperID������szFlag�����������Ӵ洢�ռ���Ϣ���ַ���������
	void	LoadSonInfo(DWORD UpperID,CString szFlag);
	void	LoadStorageInfo(DWORD UpperID,CString szFlag);

	//���س��⾮��Ŀ����������洢�ռ���Ϣ���ַ���������
	void	LoadWorkSpaceInfo(CBusyMessage *pBusy);
	void	LoadSonInfo(DWORD UpperID,CBusyMessage *pBusy);
	void	PumpMessages();
	//�����²����¼�ı��
	DWORD	sql_insert();					 
	
	BOOL	sql_update(DWORD iStorageID);
	BOOL	sql_delete(DWORD iStorageID);
	//���ݴ洢��ID�޸������ϼ�ID(���ڴ洢�ռ��ת����ģ����)
	BOOL	sql_updateUpper(DWORD iUpperID);
	//���ݴ洢�ռ��Ŷ�ȡ�����ֶ�ֵ
	BOOL	 GetData(DWORD iStorageID);			 
};

