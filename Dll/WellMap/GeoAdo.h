#pragma once
//当不需要数据库时，将数据库分离
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF") rename("BOF","adoBOF")

class CGeoAdo  
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
	//初始化和建立数据库链接
	BOOL InitData();
	BOOL InitData(CString szCon);
	void Disconnect();
public:
	_RecordsetPtr& GetRS(CString strSQL);	// 执行  返回记录集的SQL语句
	BOOL Execute(CString strSQL);			// 执行不返回记录集的SQL语句
	BOOL GetRegString(CString szSubKey,LPCTSTR lpszEntry,CString& strValue);
};

