// GeoCurve.h: interface for the CCurve class.
//////////////////////////////////////////////////////////////////////
#pragma once

#include "GeoAdo.h"
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
	DataSize		bigint							--数据体大小
	PieceSize		bigint							--数据块大小
	DataZip			tinyint,						--数据压缩标识 0-未压缩 16-标准6级Zlib压缩
	DataStID		bigint							--数据块起始记录号
	DataPieceNum	int								--数据块数
	FillUser		varchar(32),					--提交人
	FillDate		datetime						--提交日期
	UpperID			int NOT NULL					--所属测井项目编号
)*/
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CGeoCurve :public CGeoAdo
{	

public://曲线属性
	DWORD		m_CurveID;				//曲线编号
	CString		m_CurveName;			//曲线名称
	CString		m_CurveAlias;			//曲线别名
	int			m_Dimension;			//曲线维数
	CString		m_DepUnit;    			//深度单位
	float		m_DepSt;				//起始深度
	float		m_DepEn;				//结束深度
	float		m_DepRlev;				//深度步长
	CString		m_XName;    			//X轴名称
	CString		m_XUnit;    			//X轴单位
	float		m_XMin;					//X轴最小值
	float		m_XMax;					//X轴最大值
	float		m_XRlev;				//X轴采样步长
	int			m_XNPS;					//X轴采样数
	int			m_DataType;				//数据类型
	CString		m_DataUnit;    			//数据单位
	CString		m_CreateDate;			//填写日期
	DWORD		m_DataSize;				//数据体大小
	DWORD		m_PieceSize;			//数据块大小
	int			m_DataZip;				//数据压缩标识 0-未压缩 16-标准6级Zlib压缩
	DWORD		m_DataStID;				//数据块起始记录号
	WORD		m_DataPieceNum;			//数据块数

	CGeoCurve();
	virtual ~CGeoCurve();

public:	//对员工属性的操作函数
	

public:  //数据库操作函数
	// 判断指定的曲线名是否存在
	DWORD	HaveName(CString cCurveName);			
	// 根据指定的曲线编号返回曲线姓名
	CString GetName(DWORD iCurveID);					
	// 根据曲线编号读取所有字段值
	BOOL	GetData(DWORD iCurveID);					
	// 根据曲线ID修改曲线数据DataByte字段值
	DWORD	SetDataByte(DWORD iCurveID,int Number,DWORD nLen,char *pBuf);
	// 根据曲线ID读取曲线数据DataByte字段值
	DWORD	GetDataByte(DWORD iCurveID,DWORD nLen,char *pBuf);
	// 根据测井项目ID删除该项目中的所有曲线
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iCurveID);
	//更新存储空间信息，在调转存储空间时调用
	BOOL	sql_updateStorage(DWORD iStorageID);		
	BOOL	sql_delete(DWORD CurveID);	

};

