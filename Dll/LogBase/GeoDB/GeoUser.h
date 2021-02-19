// GeoUser.h: interface for the CGeoUser class.
//////////////////////////////////////////////////////////////////////

#pragma once
#include "GeoAdo.h"
/*
CREATE TABLE GeoUser
(
	GeoUserID		int PRIMARY KEY IDENTITY,		--用户编号
    UserName  		varchar(64) NOT NULL,			--用户姓名
    Pwd				varchar(64) NOT NULL,			--用户密码
    UserType  		varchar(8),						--用户权限
    Telephone		varchar(16),					--坐机号
	Mobilephone		varchar(32),					--手机号
	Email			varchar(64),					--邮箱地址
	HomeAddress		varchar(128),					--家庭住址
	Department		varchar(64),					--单位名称
	Job				varchar(64)						--工作
)

用户权限分配表

角色	1	2	3	4	5	6	7	8
查询    1
下载		1	
提交			1
编辑				1

超级	1	1	1	1	1	1	1	1
*/
class AFX_EXT_CLASS CGeoUser :public CGeoAdo
{			
public:
	DWORD		m_UserID;
	CString		m_UserName;
	CString		m_RealName;
	CString		m_Password;
  	
	CString		m_Telephone;	
	CString		m_Email;		
	CString		m_Department;
	CString		m_LastActive;
	int			m_Role;
	CString		m_szRole;

	CStringArray a_UserName;
	CStringArray a_RealName;

public:
	
	CString GetUserName();
	void	SetUserName(CString cUserName);
	
	CString GetPwd();
	void	SetPwd(CString cPwd);
	
	int	GetUserType();
	void	SetUserType(CString iUserType);

public:
	CGeoUser();
	virtual ~CGeoUser();

public:	//数据库操作
	//判断指定用户名是否存在
	BOOL	HaveName(CString cUserName); 
	//根据用户名读取用户的信息
	BOOL	GetData(CString cUserName);
	//插入一用户记录
	void	sql_insert(); 
	//根据用户名修改用户密码 
	void	sql_updatePwd(CString cPwd);
	//根据用户名删除该用户
	void	sql_delete(CString cUserName);
	//读取所有用户信息到数组
	void    LoadGeoUserInfo();
};

