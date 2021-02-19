// GeoResult.h
/*********************************************************************/
#pragma once
#include <afxtempl.h>
#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE OilTest
(
	OilTestID		bigint PRIMARY KEY IDENTITY,	--层位编号
	LayerName		varchar(64) NOT NULL,			--层位名称				
	LayerAlias		varchar(64),					--层位别名
	TestMethod		varchar(10),					--试油方式	
	OutMethod		varchar(10),					--产出方式
	Conclusion		varchar(10),					--试油结论
	TestDate		datetime,						--试油日期
	Reference		varchar(128),					--资料依据
	ReferDate		datetime,						--资料日期

	DepSt  			float,			        		--起始深度				
	DepEn  			float,			        		--结束深度								
	PorePlate		float,			        		--孔板
	Glib			float,			        		--油嘴
	CasingPres		float,			        		--套压
	OilPres			float,			        		--油压
	FlowPres		float,			        		--流压
	TopPres			float,			        		--上压
	TopTemp			float,			        		--上温
	ClosePres		float,			        		--关井压力
	LayerPres		float,			        		--地层压力
	OilOut			float,			        		--日产油
	GasOut			float,			        		--日产气
	WaterOut		float,			        		--日产水
	Describe 		varchar(1024),					--试油描述
	FillUser		varchar(64),					--提交人
	FillDate		datetime,						--提交日期			
	UpperID			bigint NOT NULL					--存储空间编号
)
*/
class AFX_EXT_CLASS CGeoOilTest :public  CGeoAdo
{
public:
	CString		m_LayerName;		//层位名称
	CString		m_LayerAlias;		//层位别名

	CString		m_TestMethod;		//试油方式	
	CString		m_OutMethod;		//产出方式
	CString		m_Conclusion;		//试油结论
	CString		m_TestDate;			//试油日期
	CString		m_Reference;		//资料依据
	CString		m_ReferDate;		//资料日期

	float		m_DepSt;  			//起始深度				
	float		m_DepEn;  			//结束深度								
	float		m_PorePlate;		//孔板
	float		m_Glib;				//油嘴
	float		m_CasingPres;		//套压
	float		m_OilPres;			//油压
	float		m_FlowPres;			//流压
	float		m_TopPres;			//上压
	float		m_TopTemp;			//上温
	float		m_ClosePres;		//关井压力
	float		m_LayerPres;		//地层压力
	float		m_OilOut;			//日产油
	float		m_GasOut;			//日产气
	float		m_WaterOut;			//日产水	

//	试油打开井段
	CArray<float,float&> a_OpenDepSt;
	CArray<float,float&> a_OpenDepEn;
public:
	CGeoOilTest();
	virtual ~CGeoOilTest();

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
	BOOL	sql_update(DWORD iOilTestID);
	BOOL	sql_delete(DWORD iOilTestID);
	//根据试油信息编号读取所有字段值
	BOOL	GetData(DWORD iOilTestID);	
	BOOL	GetOpenDep(DWORD iOilTestID);
	DWORD	InsertOpenDep(DWORD iOilTestID);
	BOOL	DeleteOpenDep(DWORD iOilTestID);
	BOOL	UpdateOpenDep(DWORD iOilTestID);
};

