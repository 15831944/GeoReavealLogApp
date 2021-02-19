// Production.h: interface for the CDocument class.
//////////////////////////////////////////////////////////////////////
#if !defined _GEOPRODUCTION_H__
#define _GEOPRODUCTION_H__

#include "GeoAdo.h"

#pragma once
/***************************创建成果文档信息表************************************
CREATE TABLE Production
(
	ProductionID	int PRIMARY KEY IDENTITY,		--文档编号
	DocName			varchar(64) NOT NULL,			--文档名称
	DocType			varchar(16),					--文档类型
	DocSize			int,							--文档字节数
	DocCreDate		varchar(32),					--文档创建日期
	Describe 		varchar(512),					--文档描述
	FillUser		varchar(64),					--管理员
	FillDate		varchar(32),					--填写日期
	DataZip			tinyint,						--数据压缩标识 0-未压缩 16-标准6级Zlib压缩
	UpperID			int NOT NULL,					--存储空间编号
	DocByte			image							--数据空间
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
public:	//对员工属性的操作函数
	

public:  //数据库操作函数
	//判断指定的曲线名是否存在
	DWORD	HaveName(CString cDocName);			
	//根据指定的曲线编号返回曲线姓名
	CString GetName(DWORD iDocID);					
	//根据曲线编号读取所有字段值
	BOOL	GetData(DWORD iDocID);					
	//根据文档ID修改文档数据DocByte字段值
	void	SetDocByte(DWORD iDocID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH]);
	//根据文档ID读取文档数据DocByte字段值
	DWORD	GetDocByte(DWORD iDocID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH]);
	//根据测井项目ID删除该项目中的所有成果文档
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iDocID);
	//更新存储空间信息，在调转存储空间时调用
	BOOL	sql_updateStorage(int iStorageID);		
	BOOL	sql_delete(DWORD iDocID);	

};


//////////////////////////////////////////////////////////////////////
#endif 
