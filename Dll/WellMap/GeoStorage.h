// GeoStorage.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Storage
(
	StorageID       int PRIMARY KEY IDENTITY,		--存储空间编号
	StorageName		varchar(64) NOT NULL,			--存储空间名称
	StorageAlias	varchar(64),					--存储空间别名
	StorageKind  	varchar(32),			        --存储空间分类(工区、构造、井、测井项目)
	FillUser		varchar(64),					--管理员
	FillDate		DateTime,						--填写日期
	Describe 		varchar(512),					--存储空间描述
	UpperID			int NOT NULL					--上级存储空间编号
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

public:		//数据库操作
	//判断指定的存储空间名称是否存在
	DWORD	HaveName(CString cStorageName);
	//判断指定的存储空间号是否存有上级存储空间
	DWORD	HaveUpper(int iStorageID);
	//判断指定的存储空间是否包含下级存储空间（如果包含则不能删除）
	BOOL	HaveSon(int iStorageID);	 
	//判断指定的存储空间中是否包含曲线（如果有则不能删除）
	BOOL	HaveCurve(int iStorageID);	
	//读取所有存储空间编号、名称和上级存储空间编号等信息到数组对象CStringArray
	void	LoadStorageInfo();

	//加载指定ID号全部子存储空间信息到字符串数组中
	void	LoadSonInfo(DWORD UpperID);
	void	LoadStorageInfo(DWORD UpperID);

	//加载除测井项目和油气井外存储空间信息到字符串数组中
	void	LoadWorkSpaceInfo(CBusyMessage *pBusy);
	void	LoadSonInfo(DWORD UpperID,CBusyMessage *pBusy);
	void	PumpMessages();

	//返回新插入记录的编号
	long	sql_insert();					 
	
	BOOL	sql_update(int iStorageID);
	BOOL	sql_delete(int iStorageID);
	//根据存储空间编号读取所有字段值
	BOOL GetData(int iStorageID);			 
};

