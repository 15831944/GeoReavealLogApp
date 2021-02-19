// GeoUser.h: interface for the CGeoUser class.
//////////////////////////////////////////////////////////////////////

#pragma once
#include "GeoAdo.h"
/*
CREATE TABLE GeoUser
(
	GeoUserID		int PRIMARY KEY IDENTITY,		--�û����
    UserName  		varchar(64) NOT NULL,			--�û�����
    Pwd				varchar(64) NOT NULL,			--�û�����
    UserType  		varchar(8),						--�û�Ȩ��
    Telephone		varchar(16),					--������
	Mobilephone		varchar(32),					--�ֻ���
	Email			varchar(64),					--�����ַ
	HomeAddress		varchar(128),					--��ͥסַ
	Department		varchar(64),					--��λ����
	Job				varchar(64)						--����
)

�û�Ȩ�޷����

��ɫ	1	2	3	4	5	6	7	8
��ѯ    1
����		1	
�ύ			1
�༭				1

����	1	1	1	1	1	1	1	1
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

public:	//���ݿ����
	//�ж�ָ���û����Ƿ����
	BOOL	HaveName(CString cUserName); 
	//�����û�����ȡ�û�����Ϣ
	BOOL	GetData(CString cUserName);
	//����һ�û���¼
	void	sql_insert(); 
	//�����û����޸��û����� 
	void	sql_updatePwd(CString cPwd);
	//�����û���ɾ�����û�
	void	sql_delete(CString cUserName);
	//��ȡ�����û���Ϣ������
	void    LoadGeoUserInfo();
};

