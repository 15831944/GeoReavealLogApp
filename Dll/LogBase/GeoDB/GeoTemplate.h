// Production.h: interface for the CDocument class.
//////////////////////////////////////////////////////////////////////

#pragma once
#include "GeoAdo.h"

/***************************图形展示模板信息表*************************************
CREATE TABLE Template
(
	RecordID		int PRIMARY KEY IDENTITY,		--模板编号
	TempName		varchar(128) NOT NULL,			--模板名称
	TempType		varchar(16),					--模板类型
	TempKind		varchar(64),					--模板分类
	TempSize		int,							--模板字节数
	TempCreDate		datetime,						--模板创建日期
	Describe 		varchar(512),					--模板描述
	FillUser		varchar(64),					--提交人
	FillDate		datetime,						--提交日期
	DataZip			int,							--数压缩标识 0-没压缩 16-Zlib标准6级压缩
	DataByte		image							--数据空间
)*/
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CGeoTemplate :public CGeoAdo
{	
public:
	CString		m_Name;		
	CString		m_Type;
	CString		m_Kind;
	DWORD		m_DataSize;		
	CString		m_CreDate;	
		
	CString		m_Describe; 	
	CString		m_FillUser;	
	CString		m_FillDate;		
	int			m_DataZip;
	CGeoTemplate();
	virtual ~CGeoTemplate();
	
public:	//对员工属性的操作函数
	

public:  //数据库操作函数
	//判断指定的曲线名是否存在
	DWORD	HaveName(CString cDocName);			
	//根据指定的曲线编号返回曲线姓名
	CString GetName(DWORD iDocID);					
	//根据曲线编号读取所有字段值
	BOOL	GetData(DWORD iDocID);					
	//根据文档ID修改文档数据DocByte字段值
	void	SetDataByte(DWORD iDocID,DWORD nLen,char *pBuf);
	//根据文档ID读取文档数据DocByte字段值
	DWORD	GetDataByte(DWORD iDocID,DWORD nLen,char *pBuf);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iDocID);	
	BOOL	sql_delete(DWORD iDocID);	

};
