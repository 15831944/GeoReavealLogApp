// Production.h: interface for the CDocument class.
//////////////////////////////////////////////////////////////////////
#pragma once
#include "GeoAdo.h"
/***************************创建成果文档信息表************************************
CREATE TABLE Production
(
	ProductionID	int PRIMARY KEY IDENTITY,		--文档编号
	DocName			varchar(64) NOT NULL,			--文档名称
	DocType			varchar(16),					--文档类型
	DocTitle		varchar(64)						--文档标题
	DocAuthor		varchar(32)						--文档作者
	DocCreDate		datetime						--创建日期
	Describe 		varchar(512),					--文档描述
	FileFormat		varchar(32)						--文件格式
	FileName		varchar(256)					--文件名	
	StorageMode		varchar(32)						--数据存储方式
	FilePath		varchar(256)					--文件存储路径
	DataSize		bigint							--数据体大小
	PieceSize		bigint							--数据块大小
	DataZip			tinyint							--数据压缩标识 0-未压缩 16-标准6级Zlib压缩
	DataStID		bigint							--数据块起始记录号
	DataPieceNum	int								--数据块数
	FillUser		varchar(64),					--提交人
	FillDate		varchar(32),					--提交日期
	UpperID			int NOT NULL,					--所属测井项目编号
)*/
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CGeoProduction :public CGeoAdo
{	
public:
	CString		m_DocName;							//文档名称
	CString		m_DocType;							//文档类型
	CString		m_DocTitle;							//文档标题
	CString		m_DocAuthor;						//文档作者
	CString		m_DocCreDate;						//创建日期

	CString		m_FileFormat;						//文件格式
	CString		m_FileName;							//文件名	
	CString		m_StorageMode;						//数据存储方式
	CString		m_FilePath;							//文件存储路径
	DWORD		m_DataSize;							//数据体大小
	DWORD		m_PieceSize;						//数据块大小
	int			m_DataZip;							//数据压缩标识 0-未压缩 16-标准6级Zlib压缩
	DWORD		m_DataStID;							//数据块起始记录号
	int			m_DataPieceNum;						//数据块数

	CGeoProduction();
	virtual ~CGeoProduction();

public:	//对员工属性的操作函数
	

public:  //数据库操作函数
	//判断指定的曲线名是否存在
	DWORD	HaveName(CString cDocName);			
	//根据指定的曲线编号返回曲线姓名
	CString GetName(DWORD iDocID);					
	//根据曲线编号读取所有字段值
	BOOL	GetData(DWORD iDocID);					
	//根据文档ID修改文档数据DataByte字段值
	DWORD	SetDataByte(DWORD iDocID,int Number,DWORD nLen,char *pBuf);
	//根据文档ID读取文档数据DataByte字段值
	DWORD	GetDataByte(DWORD iDocID,DWORD nLen,char *pBuf);
	//根据测井项目ID删除该项目中的所有成果文档
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iDocID);
	//更新存储空间信息，在调转存储空间时调用
	BOOL	sql_updateStorage(DWORD iStorageID);		
	BOOL	sql_delete(DWORD iDocID);	

};
