// GeoResult.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Result
(
	ResultID			bigint  PRIMARY KEY IDENTITY,	--常规解释成果编号
	LayerName			varchar(64) NOT NULL,			--层位名称				
	LayerAlias			varchar(64),					--层位别名
	Number				varchar(16),					--标注序号				
	DepSt  				float,			        		--起始深度				
	DepEn  				float,			        		--结束深度				
	DepHD				float,			        		--厚度					
	DepHD1				float,			        		--有效厚度
	POR					float,			        		--孔隙度				
	PERMA				float,			        		--渗透率				
	SW					float,			        		--含水饱和度			
	SH					float,			        		--泥质含量				
	Conclusion			varchar(64),					--解释结论				
	SP					float,			        		--自然电位				
	GR					float,			        		--自然伽玛		
	CGR					float,			        		--无铀伽玛	
	DEN					float,			        		--密度					
	CNL					float,			        		--中子					
	AC					float,			        		--声波					
	RT					float,			        		--深电阻率				
	RXO					float,			        		--中电阻率				
	RS					float,			        		--浅电阻率				
	PorMax;				float,							--最大孔隙度
	SwMax;				float,							--最大含水饱和度
	HoleNum;			float,							--总孔洞数
	HolePor;			float,							--总面孔率
	HoleDia;			float,							--平均孔径
	CrackNum;			float,							--裂缝条数
	CrackLength;		float,							--裂缝长度
	CrackWidth;			float,							--裂缝宽度
	CrackWaterWidth;	float,							--裂缝水动力宽度
	CrackPor;			float,							--裂缝面孔率
	Describe 			varchar(512),					--解释描述
	FillUser			varchar(64),					--提交人
	FillDate			datetime,						--提交日期				
	UpperID				bigint NOT NULL					--存储空间编号
)
*/
class AFX_EXT_CLASS CGeoResult :public CGeoAdo
{
public:
	CString		m_LayerName;		//层位名称
	CString		m_LayerAlias;		//层位别名
	CString		m_Number;			//标注序号
	float		m_DepSt;  			//起始深度
	float		m_DepEn;  			//结束深度
	float		m_DepHD;			//厚度
	float		m_DepHD1;			//有效厚度
	float		m_POR;				//孔隙度	
	float		m_PERMA;			//渗透率
	float		m_SW;				//含水饱和度	
	float		m_SH;				//泥质含量
	CString		m_Conclusion;		//解释结论
	float		m_SP;				//自然电位
	float		m_GR;				//自然伽玛
	float		m_CGR;				//无铀伽玛
	float		m_DEN;				//密度
	float		m_CNL;				//中子
	float		m_AC;				//声波
	float		m_RT;				//深电阻率
	float		m_RXO;				//中电阻率
	float		m_RS;				//浅电阻率
	float		m_PorMax;			//最大孔隙度
	float		m_SwMax;			//最大含水饱和度
	float		m_HoleNum;			//总孔洞数
	float		m_HolePor;			//总面孔率
	float		m_HoleDia;			//平均孔径
	float		m_CrackNum;			//裂缝条数
	float		m_CrackLength;		//裂缝长度
	float		m_CrackWidth;		//裂缝宽度
	float		m_CrackWaterWidth;	//裂缝水动力宽度
	float		m_CrackPor;			//裂缝面孔率
public:
	CGeoResult();
	virtual ~CGeoResult();

public:	
	CString GetDescribe();
	void	SetDescribe(CString cDescribe);
	DWORD		GetUpperID();
	void	SetUpperID(DWORD iUpperID);
	CString GetFillUser();
	void	SetFillUser(CString cUser);
	CString GetFillDate();
	void	SetFillDate(CString cDate);

public:		//数据库操作	 
	//返回新插入记录的编号
	DWORD	sql_insert();					 
	
	BOOL	sql_update(DWORD iResultID);
	BOOL	sql_delete(DWORD iResultlID);
	//根据井信息编号读取所有字段值
	BOOL GetData(DWORD iResultID);			 
};

