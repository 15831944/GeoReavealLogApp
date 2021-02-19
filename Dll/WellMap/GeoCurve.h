// GeoCurve.h: interface for the CCurve class.
//////////////////////////////////////////////////////////////////////
#if !defined _GEOCURVE_H__
#define _GEOCURVE_H__

#include "GeoAdo.h"

#pragma once
/***************************创建测井曲线信息表************************************
CREATE TABLE Curve
(
	CurveID			int PRIMARY KEY IDENTITY,		--曲线编号
	CurveName    	varchar(32) NOT NULL,			--曲线名称
	CurveAlias		varchar(32),					--曲线别名
	Dimension		tinyint,						--曲线维数
	DepUnit    		varchar(8),						--深度单位
	DepSt			float,							--起始深度
	DepEn			float,							--结束深度
	DepRlev			float,							--深度步长
	XName    		varchar(32),					--X轴名称
	XUnit    		varchar(8),						--X轴单位
	XMin			float,							--X轴最小值
	XMax			float,							--X轴最大值
	XRlev			float,							--X轴采样步长
	XNPS			int,							--X轴采样数
	DataType		tinyint,						--数据类型
	DataUnit    	varchar(8),						--数据单位
	CreateDate		varchar(32),					--填写日期
	DataZip			tinyint,						--数据压缩标识 0-未压缩 16-标准6级Zlib压缩
	UpperID			int NOT NULL,					--存储空间编号
	DataByte		image,							--数据空间
)*/
//////////////////////////////////////////////////////////////////////
class CGeoCurve  
{	
private:
	CGeoAdo		m_ado;

public://曲线属性
	DWORD		m_CurveID;		
	CString	m_CurveName;   
	CString	m_CurveAlias;	
	int		m_Dimension;	
	CString	m_DepUnit;    	
	float	m_DepSt;		
	float	m_DepEn;		
	float	m_DepRlev;		
	CString	m_XName;    	
	CString	m_XUnit;    	
	float	m_XMin;		
	float	m_XMax;		
	float	m_XRlev;		
	int		m_XNPS;		
	int		m_DataType;	
	CString	m_DataUnit;    	
	CString	m_CreateDate;
	int		m_UpperID;
	int		m_DataZip;
	DWORD	m_DataSize;
	CGeoCurve();
	virtual ~CGeoCurve();
	
	
	BOOL InitData(CString szCon);
	void Disconnect();
public:	//对员工属性的操作函数
	

public:  //数据库操作函数
	//判断指定的曲线名是否存在
	DWORD	HaveName(CString cCurveName);			
	//根据指定的曲线编号返回曲线姓名
	CString GetName(DWORD iCurveID);					
	//根据曲线编号读取所有字段值
	BOOL	GetData(DWORD iCurveID);					
	//根据曲线ID修改曲线数据DataByte字段值
	void	SetDataByte(DWORD iCurveID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH]);
	//根据曲线ID读取曲线数据DataByte字段值
	DWORD	GetDataByte(DWORD iCurveID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH]);
	//根据测井项目ID删除该项目中的所有曲线
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iCurveID);
	//更新存储空间信息，在调转存储空间时调用
	BOOL	sql_updateStorage(int iStorageID);		
	BOOL	sql_delete(DWORD CurveID);	

};


//////////////////////////////////////////////////////////////////////
#endif 
