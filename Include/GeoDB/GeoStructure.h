// GeoStructure.h: interface for the CGeoStructur class.
//////////////////////////////////////////////////////////////////////

#pragma once
#include "GeoAdo.h"

/***************************创建构造信息表************************************
CREATE TABLE COM_Structure
(
	StructureID     int PRIMARY KEY IDENTITY,		--构造编号
	StructureName	varchar(128) NOT NULL,			--构造名称
	Alias			varchar(128),					--构造别名
	Area  			float,							--构造面积
	Region			varchar(256),					--地区
	Oilfield		varchar(64),			        --所属油田
	ViewX			float,							--观察点X坐标
	ViewY			float,							--观察点Y坐标
	Longitude		float,							--观察点经度
	Latitude		float,							--观察点纬度
	GeoDescribe 	varchar(2048),					--构造地质描述
	FormDescribe 	varchar(2048),					--构造成因描述
	DrillDescribe 	varchar(2048),					--邻区钻井情况文字描述
	OilDescribe 	varchar(2048),					--邻区试油情况文字描述
	ImageType		varchar(16),					--构造图类型
	ImageSize		int,							--构造图字节数
	ImageCreDate	datetime,						--构造图创建日期
	FillUser		varchar(64),					--提交人
	FillDate		datetime,						--提交日期
	UpperID			int NOT NULL,					--存储空间编号
	DataByte		image							--构造图
)
*/
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CGeoStructure :public CGeoAdo 
{	
public:
	CString		m_StructureName;					//构造名称
	CString		m_Alias;							//构造别名
	float		m_Area;  							//构造面积
	CString		m_Region;							//地区
	CString		m_Oilfield;							//所属油田
	float		m_ViewX;							//观察点X坐标
	float		m_ViewY;							//观察点Y坐标
	float		m_Longitude;						//观察点经度
	float		m_Latitude;							//观察点纬度
	CString		m_GeoDescribe; 						//构造地质描述
	CString		m_FormDescribe; 					//构造成因描述
	CString		m_DrillDescribe; 					//邻区钻井情况文字描述
	CString		m_OilDescribe; 						//邻区试油情况文字描述
	CString		m_ImageType;						//构造图类型
	DWORD		m_ImageSize;						//构造图字节数
	CString		m_ImageCreDate;						//构造图创建日期


public: 
	CGeoStructure();
	virtual ~CGeoStructure();


public:  //数据库操作函数
	//判断指定的曲线名是否存在
	DWORD	HaveName(CString cStructureName);			
	//根据指定的曲线编号返回曲线姓名
	CString GetName(DWORD iStructureID);					
	//根据构造信息号读取所有字段值
	BOOL	GetData(DWORD iStructureID);
	//根据所属存储空编号读取构造信息所有字段值
	BOOL GetDataReferToUpperID(DWORD iUpperID);

	//根据文档ID修改文档数据StructureByte字段值
	void	SetStructureByte(DWORD iStructureID,DWORD nLen,char *pBuf);
	//根据文档ID读取文档数据StructureByte字段值
	DWORD	GetStructureByte(DWORD iStructureID,DWORD nLen,char *pBuf);
	//根据测井项目ID删除该项目中的所有成果文档
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iStructureID);	
	BOOL	sql_delete(DWORD iStructureID);
	//根据结构信息ID修改所属工区(用于存储空间调转功能模块中)
	BOOL	sql_updateUpper(DWORD iUpperID);	
};

