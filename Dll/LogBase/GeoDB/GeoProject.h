// GeoProject.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Project
(
	ProjectID		int PRIMARY KEY IDENTITY,		--项目编号
	ProjectName		varchar(64) NOT NULL,			--项目名称			TM
	ProjectAlias	varchar(64),					--项目别名
	LogDate  		datetime,						--测井日期			CJRQ
	LogStDep  		float,			        		--测井起始深度		SD1
	LogEnDep  		float,			        		--测井结束深度		SD2
	DepRatio		float,							--深度比例			SDBL
	LogDevice  		varchar(64),			        --测井设备			YQXH
	LogTeam  		varchar(128),			        --测井小队			CJDH
	LogCompany  	varchar(256),			        --测井公司			CJGS
	LogLeader  		varchar(64),			        --测井小队长		DZ
	LogEngineer  	varchar(64),			        --测井工程师		CZY
	CasingNote  	varchar(128),			        --套管描述
	BitNote  		varchar(128),			        --钻头描述
	FluidType		varchar(64),			        --泥浆类型			ZJYLX
	FluidDen		float,			        		--泥浆密度			ZJYMD
	FluidVis		float,			        		--泥浆粘度			ZJYND
	FluidRm			float,			        		--泥浆电阻率		ZJYDZL
	FluidTem		float,			        		--泥浆温度			18
	BottomDep		float,			        		--井底深度			CSJS
	BottomTem		float,			        		--井底温度			JDWD
	DeviceNote  	varchar(256),			        --仪器编号
	AuditUser		varchar(64),					--审核人
	AuditDate		datetime,						--审核日期
	Describe 		varchar(512),					--项目描述			JSSM+CSXZ
	FillUser		varchar(64),					--提交人
	FillDate		datetime,						--提交日期
	UpperID			int NOT NULL					--所属存储空间编号
)
*/
class AFX_EXT_CLASS CGeoProject :public CGeoAdo
{
public:
	CString		m_ProjectName;		//项目名称
	CString		m_ProjectAlias;		//项目别名
	CString		m_LogDate;  		//测井日期
	float		m_LogStDep;  		//测井起始深度
	float		m_LogEnDep;  		//测井结束深度
	float		m_DepRatio;			//深度比例
	CString		m_LogDevice;  	    //测井设备
	CString		m_LogTeam;  		//测井小队
	CString		m_LogCompany;  	    //测井公司
	CString		m_LogLeader;  	    //测井小队长
	CString		m_LogEngineer;  	//测井工程师
	CString		m_CasingNote;  	    //套管描述
	CString		m_BitNote;  		//钻头描述
	CString		m_FluidType;		//泥浆类型
	float		m_FluidDen;			//泥浆密度
	float		m_FluidVis;			//泥浆粘度
	float		m_FluidRm;			//泥浆电阻率
	float		m_FluidTem;			//泥浆温度
	float		m_BottomDep;		//井底深度
	float		m_BottomTem;		//井底温度
	CString		m_DeviceNote;  	    //仪器编号
	CString		m_AuditUser;		//审核人
	CString		m_AuditDate;		//审核日期
public:
	CGeoProject();
	virtual ~CGeoProject();

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
	
	BOOL	sql_update(DWORD iProjectID);
	BOOL	sql_delete(DWORD iProjectID);
	//根据井信息编号读取所有字段值
	BOOL GetData(DWORD iProjectID);	
	//根据所属存储空编号读取井信息所有字段值
	BOOL GetDataReferToUpperID(DWORD iUpperID);
};

