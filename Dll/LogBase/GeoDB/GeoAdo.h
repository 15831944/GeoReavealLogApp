#pragma once
//当不需要数据库时，将数据库分离
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF") rename("BOF","adoBOF")

class AFX_EXT_CLASS CGeoAdo  
{
private:
	_ConnectionPtr m_pConn;
	_RecordsetPtr  m_pRS;

public:
	CGeoAdo();
	virtual ~CGeoAdo();
	//数据库链接字符串
	//如：10.132.204.16;sa;log;LogBase
	CString m_szSever;

	DWORD		m_RecordID;		// 记录号
	CString		m_FillUser;		// 填报人
	CString		m_FillDate;		// 填报日期
	CString		m_Describe;		// 备注
	DWORD		m_UpperID;		// 所属存储记录号
public:							
	//初始化和建立数据库链接	 
	BOOL InitData();			
	BOOL InitData(CString szCon);
	void Disconnect();
public:
	_RecordsetPtr&	GetRecordSet(CString strSQL);				// 执行SQL语句  返回记录集
	BOOL			Execute(CString strSQL);					// 执行SQL语句  无记录集返回
	DWORD			GetMaxRecordID(CString TableName);			// 获取某张表中的最大记录
};

