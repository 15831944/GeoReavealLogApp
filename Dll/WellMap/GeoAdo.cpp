#include "stdafx.h"
#include "GeoAdo.h"

/*********************************************************************/
#if !defined CATCH_ERROR
#define CATCH_ERROR														\
		{																\
			CString strComError;										\
			strComError.Format("������: %08lx\n������Ϣ: %s			\
								\n����Դ: %s\n��������: %s",			\
								e.Error(),								\
								e.ErrorMessage(),						\
								(LPCSTR) e.Source(),					\
								(LPCSTR) e.Description());				\
			::MessageBox(NULL,strComError,"����",MB_ICONEXCLAMATION);	\
		}
#endif
/*********************************************************************/

CGeoAdo::CGeoAdo()
{
	m_szSever=_T("");
	m_pRS = NULL;
	m_pConn=NULL;
}

CGeoAdo::~CGeoAdo()
{
	Disconnect();
}

void CGeoAdo::Disconnect()
{
	if(m_pRS != NULL)				// �رռ�¼��������
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
		//����XdData
		HRESULT hr = m_pConn.CreateInstance(__uuidof(Connection));
		sprintf(m_szConnect,"provider = sqloledb;server=%s;database=%s;", m_szHost, m_szDef);
		hr=m_pConn->Open(_bstr_t(m_szConnect),_bstr_t(m_szUser),_bstr_t(m_szPwd),-1);
	}
	catch(_com_error e)					//��׽�쳣
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
		//����XdData
		HRESULT hr = m_pConn.CreateInstance(__uuidof(Connection));
		sprintf(m_szConnect,"provider = sqloledb;server=%s;database=%s;", m_szHost, m_szDef);
		hr=m_pConn->Open(_bstr_t(m_szConnect),_bstr_t(m_szUser),_bstr_t(m_szPwd),-1);
	}
	catch(_com_error e)					//��׽�쳣
	{
		CATCH_ERROR;
		return FALSE;
	}
	m_szSever=szCon;
	return TRUE;
}

_RecordsetPtr&  CGeoAdo::GetRS(CString strSQL)
{
	try
	{
		m_pRS.CreateInstance(__uuidof(Recordset));
		m_pRS->Open((_bstr_t)strSQL,m_pConn.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);	// ȡ�ñ��еļ�¼
	}
	catch(_com_error e)
	{
		CATCH_ERROR	
	}
	return m_pRS;
}

 
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