// GeoWell.h
/*********************************************************************/
#pragma once
#include <afxtempl.h>
#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Well
(
	WellID			bigint PRIMARY KEY IDENTITY,	--井编号							
	WellName		varchar(64) NOT NULL,			--井名称									JM
	Alias			varchar(64),					--井别名									BM
	Structure		varchar(64),					--所属构造									GZ
	Oilfield		varchar(64),			        --所属油田									DM
	Owner			varchar(64),			        --业主单位									DW				
    Region			varchar(128),			        --地区									
    Location		varchar(128),					--构造位置
    Line_Location	varchar(128),					--测线位置
    ElevationGd		float,							--地面海拔									HBGD
    Elevation0		float,							--补心海拔
    DepOffset		float,							--补心高度									BXGD
    OilOffset		float,							--油补距
    PipeOffset		float,							--套补距
    MagneticV		float,							--磁偏角
	WellDepth		float,							--完井井深									JS
	WellBottom		float,							--人工井底
	StartDate		datetime,						--开钻日期									RQ
	EndDate			datetime,						--完钻日期									WZRQ
	WellX			float,							--井位X坐标									X
	WellY			float,						    --井位Y坐标									Y
	Longitude		float,						    --井位经度
	Latitude		float,						    --井位纬度
	WellType  		varchar(32),			        --勘探分类(探井、预探井、评价井、开发井)	JB
	WellKind  		varchar(32),			        --工程分类(直井、斜井、水平井、丛式井)
	DrillAim		varchar(4096),					--钻探目的
	AimLayer		varchar(64),					--目标层位
	ReservoirType	varchar(32),					--储层类型(常规油气、煤层气、页岩气)
	Describe 		varchar(512),					--井描述									BZ
	FillUser		varchar(64),					--提交人
	FillDate		datetime,						--提交日期
	UpperID			bigint NOT NULL					--存储空间编号
)
*/
class AFX_EXT_CLASS CGeoWell  :public CGeoAdo
{

public:
	CString		m_WellName;	         //井名称               
	CString		m_Alias;			 //井别名
	CString		m_Structure;		 //所属构造
	CString		m_Oilfield;			 //所属油田
	CString		m_Owner;			 //业主单位
	CString		m_Region;			 //地区
	CString		m_Location;			 //构造位置
	CString		m_Line_Location;	 //测线位置
	float		m_ElevationGd;		 //地面海拔
	float		m_Elevation0;		 //基准面海拔
	float		m_DepOffset;		 //补心高度
	float		m_OilOffset;		 //油补距
	float		m_PipeOffset;		 //套补距
	float		m_MagneticV;		 //磁偏角
	float		m_WellDepth;		 //完井井深
	float		m_WellBottom;		 //人工井底
	CString		m_StartDate;		 //开钻日期
	CString		m_EndDate;			 //完钻日期
	float		m_WellX;			 //井位X坐标
	float		m_WellY;			 //井位Y坐标
	double		m_Longitude;		 //井位经度
	double		m_Latitude;			 //井位纬度
	CString		m_WellType;			 //勘探分类(探井、预探井、评价井、开发井)
	CString		m_WellKind;			 //工程分类(直井、斜井、水平井、丛式井)
	CString		m_DrillAim;			 //钻探目的
	CString		m_AimLayer;			 //目标层位
	CString		m_ReservoirType;	 //储层类型(常规油气层、煤层气、页岩气)

	// 钻具信息
	CArray<float,float&> a_BitSize;
	CArray<float,float&> a_BitDep;
	CArray<float,float&> a_CasingSize;
	CArray<float,float&> a_CasingDep;
public:
	CGeoWell();
	virtual ~CGeoWell();

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
	
	BOOL	sql_update(DWORD iWellID);
	BOOL	sql_delete(DWORD iWellID);
	//根据井信息编号读取所有字段值
	BOOL GetData(DWORD iWellID);
	//根据井信息ID修所属构造(用于存储空间调转功能模块中)
	BOOL	sql_updateUpper(DWORD iUpperID);
	//根据所属存储空编号读取井信息所有字段值
	BOOL GetDataReferToUpperID(DWORD iUpperID);

	BOOL	GetDrillTool(DWORD iWellID);
	DWORD	InsertDrillTool(DWORD iWellID);
	BOOL	DeleteDrillTool(DWORD iWellID);
	BOOL	UpdateDrillTool(DWORD iWellID);
};

