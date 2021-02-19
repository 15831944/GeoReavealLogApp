// GeoResult.h
/*********************************************************************/
#pragma once

#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Result
(
	ResultID		int PRIMARY KEY IDENTITY,		--常规解释成果编号
	LayerName		varchar(64) NOT NULL,			--层位名称				CW
	LayerAlias		varchar(64),					--层位别名
	Number			varchar(16),					--标注序号				CH
	DepSt  			float,			        		--起始深度				DEP1
	DepEn  			float,			        		--结束深度				DEP2
	DepHD			float,			        		--厚度					HD
	DepHD1			float,			        		--有效厚度
	POR				float,			        		--孔隙度				POR
	PERMA			float,			        		--渗透率				PERM
	SW				float,			        		--含水饱和度			SW
	SH				float,			        		--泥质含量				SH
	Conclusion		varchar(64),					--解释结论				JL
	SP				float,			        		--自然电位				SP
	GR				float,			        		--自然伽玛				GR
	DEN				float,			        		--密度					DEN
	CNL				float,			        		--中子					CNL
	AC				float,			        		--声波					AC
	RT				float,			        		--深电阻率				RT1
	RXO				float,			        		--中电阻率				RT
	RS				float,			        		--浅电阻率				RXO
	FillUser		varchar(64),					--提交人
	FillDate		datetime,						--提交日期
	Describe 		varchar(1024),					--解释描述				BZ
	UpperID			int NOT NULL					--存储空间编号
)
*/
class CGeoResult  
{
private:
	DWORD		m_ResultID;
	CGeoAdo		m_ado;
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
	float		m_DEN;				//密度
	float		m_CNL;				//中子
	float		m_AC;				//声波
	float		m_RT;				//深电阻率
	float		m_RXO;				//中电阻率
	float		m_RS;				//浅电阻率
	CString		m_Describe;			 //解释描述
	CString		m_FillUser;			 //提交人
	CString		m_FillDate;			 //提交日期
	DWORD		m_UpperID;			 //所属存储空间编号
public:
	CGeoResult();
	virtual ~CGeoResult();
	
	BOOL InitData(CString szCon);
	void Disconnect();
public:	
	CString GetDescribe();
	void	SetDescribe(CString cDescribe);
	int		GetUpperID();
	void	SetUpperID(int iUpperID);
	CString GetFillUser();
	void	SetFillUser(CString cUser);
	CString GetFillDate();
	void	SetFillDate(CString cDate);

public:		//数据库操作	 
	//返回新插入记录的编号
	DWORD	sql_insert();					 
	
	BOOL	sql_update(int iResultID);
	BOOL	sql_delete(int iResultlID);
	//根据井信息编号读取所有字段值
	BOOL GetData(int iResultID);			 
};

