#include "stdafx.h"
#include "GeoAdo.h"

/*********************************************************************/
#if !defined CATCH_ERROR
#define CATCH_ERROR														\
		{																\
			CString strComError;										\
			strComError.Format("错误编号: %08lx\n错误信息: %s			\
								\n错误源: %s\n错误描述: %s",			\
								e.Error(),								\
								e.ErrorMessage(),						\
								(LPCSTR) e.Source(),					\
								(LPCSTR) e.Description());				\
			::MessageBox(NULL,strComError,"错误",MB_ICONEXCLAMATION);	\
		}
#endif
/*********************************************************************/

CGeoAdo::CGeoAdo()
{
	m_szSever=_T("");
	m_pRS = NULL;
	m_pConn=NULL;

	m_RecordID = 0;				// 记录号
	m_FillUser = _T("");		// 填报人
	m_FillDate = _T("");		// 填报日期
	m_Describe = _T("");		// 备注
	m_UpperID  = 0;				// 所属存储记录号
}

CGeoAdo::~CGeoAdo()
{
	Disconnect();
}

void CGeoAdo::Disconnect()
{
	// 关闭记录集和连接
	// 使用记录集操作完毕后要关闭记录集
	// 直接用Close()方法关闭记录集并赋予其空值
	if(m_pRS != NULL)				
	{
		m_pRS->Close();
		m_pRS=NULL;
	}
	if(m_pConn!=NULL)
	{
		m_pConn->Close();
		m_pConn=NULL;
	}
}

BOOL CGeoAdo::InitData()
{
	if(m_szSever.IsEmpty())
		return FALSE;
	char m_szConnect[512];
	char m_szTmp[1024]="" ;
	char m_szHost[20], m_szUser[20], m_szPwd[20], m_szDef[20] ;
	CString str0;
	CString str=m_szSever;
	
	int x=str.Find(";");
	str0=str.Left(x);str=str.Mid(x+1);
	strcpy(m_szHost,str0);
	x=str.Find(";");
	str0=str.Left(x);str=str.Mid(x+1);
	strcpy(m_szUser,str0);
	x=str.Find(";");
	str0=str.Left(x);str=str.Mid(x+1);
	strcpy(m_szPwd,str0);
	strcpy(m_szDef,str);		
	try
	{
		HRESULT hr = m_pConn.CreateInstance(__uuidof(Connection));
		sprintf(m_szConnect,"provider = sqloledb;server=%s;database=%s;", m_szHost, m_szDef);
		hr=m_pConn->Open(_bstr_t(m_szConnect),_bstr_t(m_szUser),_bstr_t(m_szPwd),-1);
	}
	catch(_com_error e)
	{
		CATCH_ERROR;
		return FALSE;
	}
	return TRUE;
}

BOOL CGeoAdo::InitData(CString szCon)
{
	char m_szConnect[512];
	char m_szTmp[1024]="" ;
	char m_szHost[20], m_szUser[20], m_szPwd[20], m_szDef[20] ;
	CString str0;
	CString str=szCon;
	
	int x=str.Find(";");
	str0=str.Left(x);str=str.Mid(x+1);
	strcpy(m_szHost,str0);
	x=str.Find(";");
	str0=str.Left(x);str=str.Mid(x+1);
	strcpy(m_szUser,str0);
	x=str.Find(";");
	str0=str.Left(x);str=str.Mid(x+1);
	strcpy(m_szPwd,str0);
	strcpy(m_szDef,str);		
	try
	{
		HRESULT hr = m_pConn.CreateInstance(__uuidof(Connection));
		sprintf(m_szConnect,"provider = sqloledb;server=%s;database=%s;", m_szHost, m_szDef);
		hr=m_pConn->Open(_bstr_t(m_szConnect),_bstr_t(m_szUser),_bstr_t(m_szPwd),-1);
	}
	catch(_com_error e)
	{
		CATCH_ERROR;
		return FALSE;
	}
	m_szSever=szCon;
	return TRUE;
}
// 执行SQL语句  返回记录集
_RecordsetPtr&  CGeoAdo::GetRecordSet(CString strSQL)
{
	//_RecordsetPtr智能指针
	// 它是专门为通过记录集操作数据库而设立的指针，
	// 通过该接口可以对数据库的表内的记录、字段等进行各种操作
	try
	{
		//先创建记录集对象
		m_pRS.CreateInstance(__uuidof(Recordset));
		//创建记录集对象后，只是为它分配内存空间，记录集中不含任何数据。
		//记录集对象是用来获得数据库中的数据并对其操作的，所以还要打开记录集，从数据库中取得数据记录
		m_pRS->Open((_bstr_t)strSQL,m_pConn.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch(_com_error e)
	{
		CATCH_ERROR	
	}
	return m_pRS;
}
// 执行SQL语句  无记录集返回
BOOL CGeoAdo::Execute(CString strSQL)
{
	try
	{
		m_pConn->Execute((_bstr_t)strSQL,NULL,adCmdText);
		return true;
	}
	catch(_com_error e)
	{
		CATCH_ERROR		
		return false;
	}
}
// 获取某张表中的最大记录
DWORD CGeoAdo::GetMaxRecordID(CString TableName)
{
	CString szSQL="SELECT MAX(RecordID) AS MaxID FROM "+TableName;
	_RecordsetPtr pRS = GetRecordSet(szSQL);
	if(pRS->adoEOF)
		return 0;
	else 
		return atol((_bstr_t)pRS->GetCollect("MaxID"));
}