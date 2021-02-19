/*********************************************************************/

#include "stdafx.h"
#include "GeoStorage.h"
#include "BusyMessage.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGeoStorage::CGeoStorage()
{
	m_RecordID = 0;
	m_StorageName = "";
	m_StorageAlias= "";
	m_StorageKind= "";
	m_FillUser= "";
	m_FillDate= "";
	m_Describe = "";
	m_UpperID = 0;
}

CGeoStorage::~CGeoStorage()
{
	Disconnect();
}

//读取和设置成员变量
DWORD CGeoStorage::GetStorageID()
{
	return m_RecordID;
}

void CGeoStorage::SetStorageID(DWORD iStorageID)
{
	m_RecordID = iStorageID;
}

CString CGeoStorage::GetStorageName()
{
	return m_StorageName;
}

void CGeoStorage::SetStorageName(CString cStorageName)
{
	m_StorageName = cStorageName;
}

CString CGeoStorage::GetStorageKind()
{
	return m_StorageKind;
}

void CGeoStorage::SetStorageKind(CString cStorageKind)
{
	m_StorageKind = cStorageKind;
}

CString CGeoStorage::GetStorageAlias()
{
	return m_StorageAlias;
}

void CGeoStorage::SetStorageAlias(CString cStorageAlias)
{
	m_StorageAlias = cStorageAlias;
}

CString CGeoStorage::GetDescribe()
{
	return m_Describe;
}

void CGeoStorage::SetDescribe(CString cDescribe)
{
	m_Describe = cDescribe;
}

CString CGeoStorage::GetFillUser()
{
	return m_FillUser;
}

void CGeoStorage::SetFillUser(CString cUser)
{
	m_FillUser = cUser;
}

CString CGeoStorage::GetFillDate()
{
	return m_FillDate;
}

void CGeoStorage::SetFillDate(CString cDate)
{
	m_FillDate = cDate;
}

DWORD CGeoStorage::GetUpperID()
{
	return m_UpperID;
}

void CGeoStorage::SetUpperID(DWORD iUpperID)
{
	m_UpperID = iUpperID;
}


/*********************************************************************/
//判断指定名称的存储空间是否存在(用于插入模块中)
DWORD CGeoStorage::HaveName(CString cStorageName)
{	
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Storage WHERE StorageName='" + cStorageName + "'");

	if(pRS->adoEOF)
		return 0;
	else
		return atol((_bstr_t)pRS->GetCollect("RecordID"));
}

//判断指定的存储空间号是否存有上级存储空间
DWORD CGeoStorage::HaveUpper(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",iStorageID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Storage WHERE RecordID=" + cID);
	if(pRS->adoEOF)
		return 0;
	else
		return atol((_bstr_t)pRS->GetCollect("UpperID"));
}

//根据存储空间ID判断该存储空间下是否有子存储空间(用于删除模块中)
BOOL CGeoStorage::HaveSon(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",iStorageID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Storage WHERE UpperID=" + cID);

	if(pRS->adoEOF)
		return false;
	else
		return true;

}

//判断指定的存储空间下是否有曲线
BOOL CGeoStorage::HaveCurve(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",iStorageID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Curve WHERE RecordID=" + cID);
	
	if(pRS->adoEOF)
		return false;
	else
		return true;
}

//加载全部存储空间信息到字符串数组中
void CGeoStorage::LoadStorageInfo()
{	
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Storage");

	//初始化数组
	a_StorageName.RemoveAll();
	a_StorageAlias.RemoveAll();
	a_StorageKind.RemoveAll();
	a_StorageID.RemoveAll();
	a_UpperID.RemoveAll();
	DWORD ID;
	//执行SELECT语句
	while (pRS->adoEOF == 0)
	{
		ID=atol((_bstr_t)pRS->GetCollect("RecordID"));
		a_StorageID.Add(ID);
		a_StorageAlias.Add((_bstr_t)pRS->GetCollect("StorageAlias"));
		a_StorageName.Add((_bstr_t)pRS->GetCollect("StorageName"));
		a_StorageKind.Add((_bstr_t)pRS->GetCollect("StorageKind"));
		ID=atol((_bstr_t)pRS->GetCollect("UpperID"));
		a_UpperID.Add(ID);
		pRS->MoveNext();
	}
}

void CGeoStorage::LoadStorageInfo(DWORD UpperID,CString szFlag)
{
	//初始化数组
	a_StorageName.RemoveAll();
	a_StorageAlias.RemoveAll();
	a_StorageKind.RemoveAll();
	a_StorageID.RemoveAll();
	a_UpperID.RemoveAll();
	LoadSonInfo(UpperID,szFlag);
}
//加载指定UpperID号满足szFlag条件的所有子存储空间信息到字符串数组中
void CGeoStorage::LoadSonInfo(DWORD UpperID,CString szFlag)
{
	CString cID;
	cID.Format("%lu",UpperID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Storage WHERE StorageKind='"+szFlag+"' AND UpperID=" + cID);
	DWORD ID;
	//执行SELECT语句
	while(pRS->adoEOF == 0)
	{
		a_StorageAlias.Add((_bstr_t)pRS->GetCollect("StorageAlias"));
		a_StorageName.Add((_bstr_t)pRS->GetCollect("StorageName"));
		a_StorageKind.Add((_bstr_t)pRS->GetCollect("StorageKind"));
		ID=atol((_bstr_t)pRS->GetCollect("UpperID"));
		a_UpperID.Add(ID);
		ID=atol((_bstr_t)pRS->GetCollect("RecordID"));
		a_StorageID.Add(ID);
		LoadSonInfo(ID,szFlag);
		pRS->MoveNext();
	}
}

//加载指定ID号全部子存储空间信息到字符串数组中
void CGeoStorage::LoadStorageInfo(DWORD UpperID)
{	
	//初始化数组
	a_StorageName.RemoveAll();
	a_StorageAlias.RemoveAll();
	a_StorageKind.RemoveAll();
	a_StorageID.RemoveAll();
	a_UpperID.RemoveAll();
	LoadSonInfo(UpperID);
}

void CGeoStorage::LoadSonInfo(DWORD UpperID)
{	
	CString cID;
	cID.Format("%lu",UpperID);
	_RecordsetPtr pRS = GetRecordSet("SELECT RecordID,StorageAlias,StorageName,StorageKind,UpperID FROM Storage WHERE UpperID=" + cID);
	DWORD ID;
	//执行SELECT语句
	while(pRS->adoEOF == 0)
	{
		a_StorageAlias.Add(pRS->GetCollect("StorageAlias"));
		a_StorageName.Add(pRS->GetCollect("StorageName"));
		a_StorageKind.Add(pRS->GetCollect("StorageKind"));
		ID=atol((_bstr_t)pRS->GetCollect("UpperID"));
		a_UpperID.Add(ID);
		ID=atol((_bstr_t)pRS->GetCollect("RecordID"));
		a_StorageID.Add(ID);
		LoadSonInfo(ID);
		pRS->MoveNext();
	}
}
//加载指定ID号全部井信息到字符串数组中
void CGeoStorage::LoadWellInfo(DWORD UpperID)
{	
	//初始化数组
	a_StorageName.RemoveAll();
	a_StorageAlias.RemoveAll();
	a_StorageKind.RemoveAll();
	a_StorageID.RemoveAll();
	a_UpperID.RemoveAll();
	CString cID;
	cID.Format("%lu",UpperID);
	_RecordsetPtr pRS = GetRecordSet("SELECT RecordID,WellName,Alias,WellType,UpperID FROM Well WHERE UpperID=" + cID);
	DWORD ID;
	//执行SELECT语句
	while(!pRS->adoEOF)
	{
		a_StorageAlias.Add(pRS->GetCollect("Alias"));
		a_StorageName.Add(pRS->GetCollect("WellName"));
		a_StorageKind.Add(pRS->GetCollect("WellType"));
		ID=atol((_bstr_t)pRS->GetCollect("RecordID"));
		a_StorageID.Add(ID);
		ID=atol((_bstr_t)pRS->GetCollect("UpperID"));
		a_UpperID.Add(ID);
		pRS->MoveNext();
	}
}
//加载指定ID号全部测井项目信息到字符串数组中
void CGeoStorage::LoadProjectInfo(DWORD UpperID)
{	
	//初始化数组
	a_StorageName.RemoveAll();
	a_StorageAlias.RemoveAll();
	a_StorageKind.RemoveAll();
	a_StorageID.RemoveAll();
	a_UpperID.RemoveAll();
	CString cID;
	cID.Format("%lu",UpperID);
	_RecordsetPtr pRS = GetRecordSet("SELECT SELECT RecordID,ProjectName,ProjectAlias,LogDevice,UpperID FROM Project WHERE UpperID=" + cID);
	DWORD ID;
	//执行SELECT语句
	while(!pRS->adoEOF)
	{
		a_StorageAlias.Add(pRS->GetCollect("ProjectAlias"));
		a_StorageName.Add(pRS->GetCollect("ProjectName"));
		a_StorageKind.Add(pRS->GetCollect("LogDevice"));
		ID=atol((_bstr_t)pRS->GetCollect("RecordID"));
		a_StorageID.Add(ID);
		ID=atol((_bstr_t)pRS->GetCollect("UpperID"));
		a_UpperID.Add(ID);
		pRS->MoveNext();
	}
}
//加载除测井项目和油气井外存储空间信息到字符串数组中
void CGeoStorage::LoadWorkSpaceInfo(CBusyMessage *pBusy)
{	
	//初始化数组
	a_StorageName.RemoveAll();
	a_StorageKind.RemoveAll();
	a_StorageID.RemoveAll();
	a_UpperID.RemoveAll();
	LoadSonInfo(0,pBusy);
}
void CGeoStorage::LoadSonInfo(DWORD UpperID,CBusyMessage *pBusy)
{	
	CString cID;
	cID.Format("%lu",UpperID);
	_RecordsetPtr pRS = GetRecordSet("SELECT StorageKind,RecordID,StorageAlias,StorageName,UpperID FROM Storage WHERE UpperID=" + cID);
	//执行SELECT语句
	CString str;
	DWORD ID;
	while(pRS->adoEOF == 0)
	{
		str=pRS->GetCollect("StorageKind");
		str.Trim();
		if(str!="测井项目"&&str!="油气井")
		{
			a_StorageAlias.Add((_bstr_t)pRS->GetCollect("StorageAlias"));
			a_StorageName.Add((_bstr_t)pRS->GetCollect("StorageName"));
			a_StorageKind.Add(str);
			ID=atol((_bstr_t)pRS->GetCollect("UpperID"));
			a_UpperID.Add(ID);
			ID=atol((_bstr_t)pRS->GetCollect("RecordID"));
			a_StorageID.Add(ID);
			LoadSonInfo(ID,pBusy);
		}
		pRS->MoveNext();
		pBusy->m_Busy.Invalidate();
		PumpMessages();
	}
}

void CGeoStorage::PumpMessages()
{
    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);  
    }
}

DWORD CGeoStorage::sql_insert()
{	
	CString strUpperID;
	strUpperID.Format("%lu", m_UpperID);

	if(!Execute("INSERT INTO Storage (StorageName,StorageAlias,StorageKind,FillUser,FillDate,Describe,UpperID) VALUES('" + m_StorageName + "','" + m_StorageAlias + "','" + m_StorageKind + "','" + m_FillUser + "','" + m_FillDate + "','" + m_Describe + "'," + strUpperID + ")"))
		return 0;
	return GetMaxRecordID("Storage");
}

BOOL CGeoStorage::sql_update(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",iStorageID);
	CString cUpperID;
	cUpperID.Format("%lu",m_UpperID);
	return Execute("UPDATE Storage SET StorageName='" +		m_StorageName 
										+ "', StorageKind='" +		m_StorageKind 
										+ "', StorageAlias='" +		m_StorageAlias 
										+ "', FillUser='" +			m_FillUser 
										+ "', FillDate='" +			m_FillDate 
										+ "', Describe='" +			m_Describe
										+ "', UpperID='" +			cUpperID 
										+"' WHERE RecordID=" + cID);	
}
//根据存储体ID修改存储空间所属上级ID(用于存储空间调转功能模块中)
BOOL CGeoStorage::sql_updateUpper(DWORD iUpperID)
{
	CString cID;
	cID.Format("%lu",m_RecordID);
	CString sID;
	sID.Format("%lu",iUpperID);
	return Execute("UPDATE Storage SET UpperID=" + sID + " WHERE RecordID=" + cID);	
}

BOOL CGeoStorage::sql_delete(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",iStorageID);
	return Execute("DELETE FROM Storage WHERE RecordID=" + cID);
}

//根据存储空间编号读取所有字段值
BOOL CGeoStorage::GetData(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",iStorageID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Storage WHERE RecordID=" + cID);

	//返回各列的值
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_RecordID = iStorageID;
		m_StorageName =pRS->GetCollect("StorageName");
		m_StorageAlias=pRS->GetCollect("StorageAlias");
		m_StorageKind=pRS->GetCollect("StorageKind");
		m_FillUser=pRS->GetCollect("FillUser");
		m_FillDate=pRS->GetCollect("FillDate");
		m_Describe =pRS->GetCollect("Describe");
		m_UpperID =atol((_bstr_t)pRS->GetCollect("UpperID"));
	}
	return TRUE;
}
