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
	m_StorageID = 0;
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

void CGeoStorage::Disconnect()
{
	m_ado.Disconnect();
}

BOOL CGeoStorage::InitData(CString szCon)
{
	return m_ado.InitData(szCon);
}

//读取和设置成员变量
int CGeoStorage::GetStorageID()
{
	return m_StorageID;
}

void CGeoStorage::SetStorageID(int iStorageID)
{
	m_StorageID = iStorageID;
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

int CGeoStorage::GetUpperID()
{
	return m_UpperID;
}

void CGeoStorage::SetUpperID(int iUpperID)
{
	m_UpperID = iUpperID;
}


/*********************************************************************/
//判断指定名称的存储空间是否存在(用于插入模块中)
DWORD CGeoStorage::HaveName(CString cStorageName)
{	
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Storage WHERE StorageName='" + cStorageName + "'");

	if(pRS->adoEOF)
		return 0;
	else
		return atol((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
}

//判断指定的存储空间号是否存有上级存储空间
DWORD CGeoStorage::HaveUpper(int iStorageID)
{
	CString cID;
	cID.Format("%d",iStorageID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Storage WHERE StorageID=" + cID);
	if(pRS->adoEOF)
		return 0;
	else
		return atol((LPCTSTR)(_bstr_t)pRS->GetCollect("UpperID"));
}

//根据存储空间ID判断该存储空间下是否有子存储空间(用于删除模块中)
BOOL CGeoStorage::HaveSon(int iStorageID)
{
	CString cID;
	cID.Format("%d",iStorageID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Storage WHERE UpperID=" + cID);

	if(pRS->adoEOF)
		return false;
	else
		return true;

}

//判断指定的存储空间下是否有曲线
BOOL CGeoStorage::HaveCurve(int iStorageID)
{
	CString cID;
	cID.Format("%d",iStorageID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Curve WHERE StorageID=" + cID);
	
	if(pRS->adoEOF)
		return false;
	else
		return true;
}

//加载全部存储空间信息到字符串数组中
void CGeoStorage::LoadStorageInfo()
{	
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Storage");

	//初始化数组
	a_StorageName.RemoveAll();
	a_StorageKind.RemoveAll();
	a_StorageID.RemoveAll();
	a_UpperID.RemoveAll();

	//执行SELECT语句
	while (pRS->adoEOF == 0)
	{
		a_StorageID.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
		a_StorageName.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageName"));
		a_StorageKind.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageKind"));
		a_UpperID.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("UpperID"));
		pRS->MoveNext();
	}
}

//加载指定ID号全部子存储空间信息到字符串数组中
void CGeoStorage::LoadStorageInfo(DWORD UpperID)
{	
	//初始化数组
	a_StorageName.RemoveAll();
	a_StorageKind.RemoveAll();
	a_StorageID.RemoveAll();
	a_UpperID.RemoveAll();
	LoadSonInfo(UpperID);
}
void CGeoStorage::LoadSonInfo(DWORD UpperID)
{	
	CString cID;
	cID.Format("%d",UpperID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Storage WHERE UpperID=" + cID);
	//执行SELECT语句
	while(pRS->adoEOF == 0)
	{
		a_StorageID.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
		a_StorageName.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageName"));
		a_StorageKind.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageKind"));
		a_UpperID.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("UpperID"));
		LoadSonInfo((WORD)pRS->GetCollect("StorageID"));
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
	cID.Format("%d",UpperID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Storage WHERE UpperID=" + cID);
	//执行SELECT语句
	CString str;
	while(pRS->adoEOF == 0)
	{
		str=(LPCTSTR)(_bstr_t)pRS->GetCollect("StorageKind");
		str.Trim();
		if(str!="测井项目"&&str!="油气井")
		{
			a_StorageID.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
			a_StorageName.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageName"));
			a_StorageKind.Add(str);
			a_UpperID.Add((LPCTSTR)(_bstr_t)pRS->GetCollect("UpperID"));
			LoadSonInfo((WORD)pRS->GetCollect("StorageID"),pBusy);
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

long CGeoStorage::sql_insert()
{	
	CString strUpperID;
	strUpperID.Format("%d", m_UpperID);

	if(!m_ado.Execute("INSERT INTO Storage (StorageName,StorageAlias,StorageKind,FillUser,FillDate,Describe,UpperID	) VALUES('" + m_StorageName + "','" + m_StorageAlias + "','" + m_StorageKind + "','" + m_FillUser + "','" + m_FillDate + "','" + m_Describe + "'," + strUpperID + ")"))
		return 0;
	//获取刚才插入的存储空间信息中的StorageID值(数据库中StorageID字段是自动增加的)
	long lStorageID;
	_RecordsetPtr pRS = m_ado.GetRS("SELECT MAX(StorageID) AS MaxID FROM Storage");

	if (pRS->adoEOF)
		lStorageID = 0;
	else 
		lStorageID = atol((LPCTSTR)(_bstr_t)pRS->GetCollect("MaxID"));

	return lStorageID;
}

BOOL CGeoStorage::sql_update(int iStorageID)
{
	CString cID;
	cID.Format("%d",iStorageID);
	return m_ado.Execute("UPDATE Storage SET StorageName='" + m_StorageName + "', StorageKind='" + m_StorageKind + "', StorageAlias='" + m_StorageAlias + "', FillUser='" + m_FillUser + "', FillDate='" + m_FillDate + "', Describe='" + m_Describe +"' WHERE StorageID=" + cID);	
}

BOOL CGeoStorage::sql_delete(int iStorageID)
{
	CString cID;
	cID.Format("%d",iStorageID);
	return m_ado.Execute("DELETE FROM Storage WHERE StorageID=" + cID);
}

//根据存储空间编号读取所有字段值
BOOL CGeoStorage::GetData(int iStorageID)
{
	CString cID;
	cID.Format("%d",iStorageID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Storage WHERE StorageID=" + cID);

	//返回各列的值
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_StorageID = iStorageID;
		m_StorageName =	(LPCTSTR)(_bstr_t)pRS->GetCollect("StorageName");
		m_StorageAlias=(LPCTSTR)(_bstr_t)pRS->GetCollect("StorageAlias");
		m_StorageKind=(LPCTSTR)(_bstr_t)pRS->GetCollect("StorageKind");
		m_FillUser=(LPCTSTR)(_bstr_t)pRS->GetCollect("FillUser");
		m_FillDate=(LPCTSTR)(_bstr_t)pRS->GetCollect("FillDate");
		m_Describe =(LPCTSTR)(_bstr_t)pRS->GetCollect("Describe");
		m_UpperID =atoi( (LPCTSTR)(_bstr_t)pRS->GetCollect("UpperID"));
	}
	return TRUE;
}
