// GeoResult.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Layer
(
	LayerID			bigint PRIMARY KEY IDENTITY,	--层位编号
	LayerName		varchar(64) NOT NULL,			--层位名称				
	LayerAlias		varchar(64),					--层位别名
	DepLog  		float,			        		--测井深度				
	TVLog  			float,			        		--测井垂深				
	DepGeo			float,			        		--地质深度					
	TVGeo			float,			        		--地质垂深
	RemOption		varchar(10),					--标注选项                实线底界 虚线底界 断层标识 垂直定位 水平定位			
	Describe 		varchar(512),					--层位描述
	FillUser		varchar(64),					--提交人
	FillDate		datetime,						--提交日期			
	UpperID			int NOT NULL					--存储空间编号
)
*/
class AFX_EXT_CLASS CGeoLayer :public CGeoAdo
{

public:
	CString		m_LayerName;		//层位名称
	CString		m_LayerAlias;		//层位别名

	float		m_DepLog;  			//测井深度
	float		m_TVLog;  			//测井垂深
	float		m_DepGeo;			//地质深度
	float		m_TVGeo;			//地质垂深
	CString		m_RemOption;		//标注选项               实线底界 虚线底界 断层标识 垂直定位 水平定位	

public:
	CGeoLayer();
	virtual ~CGeoLayer();

public:	
	CString GetDescribe();
	void	SetDescribe(CString cDescribe);
	DWORD	GetUpperID();
	void	SetUpperID(DWORD iUpperID);
	CString GetFillUser();
	void	SetFillUser(CString cUser);
	CString GetFillDate();
	void	SetFillDate(CString cDate);

public:		//数据库操作	 
	//返回新插入记录的编号
	DWORD	sql_insert();					 
	
	BOOL	sql_update(DWORD iLayerID);
	BOOL	sql_delete(DWORD iLayerID);
	//根据层位信息编号读取所有字段值
	BOOL GetData(DWORD iLayerID);			 
};

