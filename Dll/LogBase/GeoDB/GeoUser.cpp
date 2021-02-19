// GeoUser.cpp:
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoUser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CGeoUser::CGeoUser()
{
	m_UserID=0;
	m_UserName = "";
	m_RealName = "";
	m_Password = "";
	m_Telephone= "";		
	m_Email= "";		
	m_Department= "";
	m_LastActive= "";
	m_Role=3;
	m_szRole="Author";		
}

CGeoUser::~CGeoUser()
{
	Disconnect();
}

/************************************************************/
CString CGeoUser::GetUserName()
{
	return m_UserName;
}

void CGeoUser::SetUserName(CString cUserName)
{
	m_UserName = cUserName;
}

CString CGeoUser::GetPwd()
{
	return m_Password;
}

void CGeoUser::SetPwd(CString cPwd)
{
	m_Password = cPwd;
}

int CGeoUser::GetUserType()
{
	return m_Role;
}

void CGeoUser::SetUserType(CString cUserType)
{	
	m_Role = 3;
	if(cUserType=="SuperAdmin")
		m_Role=0;
	else if(cUserType=="Admin")
		m_Role=1;
}


/******************数据库操作******************************************/
BOOL CGeoUser::HaveName(CString cUserName)
{	
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT RecordID FROM Users WHERE UserName='" + cUserName + "'");
	
	if (m_pRecordset->adoEOF)
		return false;
	else
		return true;
}

	
void CGeoUser::sql_insert()		//插入记录
{	
	Execute("INSERT INTO Users VALUES('" + m_UserName + "','" + m_Password + "'," + m_Email+ ")");	
}

//根据用户名修改用户密码
void CGeoUser::sql_updatePwd(CString cPwd)
{
	Execute("UPDATE Users SET Password='" + cPwd + "' WHERE UserName='" + m_UserName + "'");
}

//根据用户名删除该用户
void CGeoUser::sql_delete(CString cUserName)
{
	Execute("DELETE FROM GeoUser WHERE UserName='" + cUserName	+ "'");	
}

//根据用户名读取用户的信息
BOOL CGeoUser::GetData(CString cUserName)
{
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT * FROM Users WHERE UserName='" + cUserName + "'");
	if(m_pRecordset->adoEOF)
		return FALSE;
	m_UserName	= cUserName;
	m_UserID	= atoi((_bstr_t)m_pRecordset->GetCollect("UserID"));
	m_RealName	= m_pRecordset->GetCollect("RealName");
	m_Password	= m_pRecordset->GetCollect("Password");
	m_Telephone	= m_pRecordset->GetCollect("Telephone");		
	m_Email		= m_pRecordset->GetCollect("UserEmailAddress");		
	m_Department= m_pRecordset->GetCollect("Department");
	m_LastActive= m_pRecordset->GetCollect("LastActivityDate");

	m_Role=3;
	m_szRole="Author";
	if(m_UserID>0)
	{
		CString cID;
		cID.Format("%d",m_UserID);
		m_pRecordset = GetRecordSet("SELECT RoleId FROM UserRoles WHERE UserID=" + cID);
		if(m_pRecordset->adoEOF)
			return FALSE;

		m_Role=atol((_bstr_t)m_pRecordset->GetCollect("RoleId"));
		if(m_Role==0)
			m_szRole="SuperAdmin";
		else if(m_Role==1)
			m_szRole="Admin";
	}
	return TRUE;
}

//读取所有用户信息到数组
void CGeoUser::LoadGeoUserInfo()
{
	a_UserName.RemoveAll();
	a_RealName.RemoveAll();
	_RecordsetPtr pRS = GetRecordSet("SELECT UserName,RealName FROM Users");
	while(pRS->adoEOF == 0)
	{
		a_UserName.Add(pRS->GetCollect("UserName"));
		a_RealName.Add(pRS->GetCollect("RealName"));
		pRS->MoveNext();
	}
}
