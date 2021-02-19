// Yuangong.cpp: implementation of the CGeoProduction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoProduction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGeoProduction::CGeoProduction()
{
	m_ProductionID=0;	
	m_DocName=_T("");		
	m_DocType=_T("");		
	m_DocSize=0;		
	m_DocCreDate=_T("");	
	
	m_Describe=_T(""); 	
	m_FillUser=_T("");	
	m_FillDate=_T("");	
	m_UpperID=0;
	m_DataZip=0;
}

CGeoProduction::~CGeoProduction()
{
	Disconnect();
}

void CGeoProduction::Disconnect()
{
	m_ado.Disconnect();
}

BOOL CGeoProduction::InitData(CString szCon)
{
	return m_ado.InitData(szCon);
}
/****************************************************************************************/

//根据文档名判断该文档是否在数据库中存在
DWORD CGeoProduction::HaveName(CString cDocName)
{	
	_RecordsetPtr m_pRecordset = m_ado.GetRS( "SELECT * FROM Production WHERE DocName='" + cDocName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("ProductionID"));
}


//根据文档ID获取文档姓名
CString CGeoProduction::GetName(DWORD iDocID)
{	
	CString cID;
	cID.Format("%d",iDocID);
	_RecordsetPtr m_pRecordset = m_ado.GetRS("SELECT DocName FROM Production WHERE ProductionID=" + cID);

	if (m_pRecordset->adoEOF)
		return "";
	else
		return (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DocName");
}


//插入新的文档信息
DWORD CGeoProduction::sql_insert()
{		
	CString sDocSize;sDocSize.Format("%d",m_DocSize);	
	CString sUpperID;sUpperID.Format("%lu",m_UpperID);
	CString sDataZip;sDataZip.Format("%d",m_DataZip);
	CString vSQL;
	vSQL = "INSERT INTO Production( DocName,   	\
								DocType,		\
								DocSize,		\
								DocCreDate,		\
								Describe,		\
								FillUser,		\
								FillDate,		\
								DataZip,		\
								UpperID,		\
								DataByte1,		\
								DataByte2,		\
								DataByte3,		\
								DataByte4,		\
								DataByte5,		\
								DataByte6,		\
								DataByte7,		\
								DataByte8,		\
								DataByte9,		\
								DataByte10)		\
			VALUES('" + m_DocName 		+ "','" 
					  + m_DocType		+ "','"  
					  + sDocSize		+ "','" 
					  + m_DocCreDate 	+ "','" 
					  + m_Describe 		+ "','" 
					  + m_FillUser		+ "','" 
					  + m_FillDate 		+ "','" 
					  + sDataZip 		+ "','" 
					  + sUpperID 		+ "','NULL','NULL','NULL','NULL','NULL','NULL','NULL','NULL','NULL','NULL')"; 

	if(!m_ado.Execute(vSQL))
		return 0;
	//获取刚才插入的存储空间信息中的StorageID值(数据库中StorageID字段是自动增加的)
	DWORD CurID;
	_RecordsetPtr pRS = m_ado.GetRS("SELECT MAX(ProductionID) AS MaxID FROM Production");

	if(pRS->adoEOF)
		CurID = 0;
	else 
		CurID = atol((LPCTSTR)(_bstr_t)pRS->GetCollect("MaxID"));

	return CurID;
}


//根据文档ID更新文档的信息
BOOL CGeoProduction::sql_update(DWORD iDocID)
{		
	CString sDocSize;sDocSize.Format("%d",m_DocSize);	
	CString sUpperID;sUpperID.Format("%lu",m_UpperID);
	CString sDataZip;sDataZip.Format("%d",m_DataZip);

	CString cID;
	cID.Format("%d",iDocID);
	CString str("UPDATE Production SET	  DocName='"	+		m_DocName 
									+ "', DocType='"	+		m_DocType
									+ "', DocSize='"	+		sDocSize	
									+ "', DocCreDate='" +		m_DocCreDate
									+ "', Describe='"	+		m_Describe 	
									+ "', FillUser='"	+		m_FillUser	
									+ "', FillDate='"	+		m_FillDate 
									+ "', DataZip='"	+		sDataZip 
									+ "', UpperID="		+		sUpperID 
				+ "' WHERE ProductionID=" + cID );
	return m_ado.Execute(str);	
}

//根据文档ID修改文档所在的部门(用于存储空间调转功能模块中)
BOOL CGeoProduction::sql_updateStorage(int iStorageID)
{
	CString cID;
	cID.Format("%d",m_ProductionID);
	CString sID;
	sID.Format("%d",iStorageID);
	return m_ado.Execute("UPDATE Production SET UpperID=" + sID + " WHERE ProductionID=" + cID);	
}

//根据文档ID修改文档数据DocByte字段值
void CGeoProduction::SetDocByte(DWORD iDocID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH])
{
	CString cID;
	cID.Format("%d",iDocID);
	//@@@@@@@@@@@  将文档据pBuf保存到数据库中  @@@@@@@@@@@@@

	//设置SELECT语句
	CString vSQL;
	vSQL = "SELECT * FROM Production WHERE ProductionID=" + cID;
	//执行SELETE语句
	_RecordsetPtr	m_pRs;
	//将当前文档记录读取到m_pRs对象中
	m_pRs = m_ado.GetRS(vSQL);	

	VARIANT			varBLOB;
	SAFEARRAY		*psa;  //定义数组
    SAFEARRAYBOUND	rgsabound[1];

	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = nLen;
	psa = SafeArrayCreate(VT_UI1, 1, rgsabound);  //创建数组
	for(long i = 0; i <nLen; i++)  //将pBuf中的图像数据写入数组psa
		SafeArrayPutElement (psa, &i, pBuf++);
	varBLOB.vt = VT_ARRAY | VT_UI1;
	varBLOB.parray = psa;
	//调用AppendChunk()函数将图像数据写入Photo字段
	m_pRs->GetFields()->GetItem(szBlob)->AppendChunk(varBLOB);
	//更新数据库
	m_pRs->Update();
	//断开与数据库的连接
}

//根据文档ID读取文档数据DocByte字段值
DWORD CGeoProduction::GetDocByte(DWORD iDocID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH])
{
	CString cID;
	cID.Format("%d",iDocID);
	//@@@@@@@@@@@  将曲线据数据库中DataByte读取到pBuf  @@@@@@@@@@@@@
	CString vSQL("SELECT * FROM Production WHERE ProductionID=" + cID);
	_RecordsetPtr  m_pRs= m_ado.GetRS(vSQL);
	//读取DataByte字段的实际大小
	DWORD Size = m_pRs->GetFields()->GetItem(szBlob)->ActualSize;		
	char *Buffer=NULL;
	if(Size>0)
	{		
		//从图像字段中读取数据到varBLOB中
		_variant_t varBLOB = m_pRs->GetFields()->GetItem(szBlob)->GetChunk(Size);		
		if(varBLOB.vt == (VT_ARRAY | VT_UI1))
		{
			//分配必要的存储空间
			if(Buffer = new char[Size+1])								
			{	
				SafeArrayAccessData(varBLOB.parray,(void **)&Buffer); 
				//复制数据到缓冲区pBuf
				if(nLen>Size)
					memcpy(pBuf,Buffer,Size);
				else
					memcpy(pBuf,Buffer,nLen);
				SafeArrayUnaccessData(varBLOB.parray);
			//	delete	Buffer;	
			}
		}
	}
	return Size;
}

//根据测井项目ID删除该项目中的所有成果文档
BOOL CGeoProduction::DeleteProject(DWORD iProjectID)
{
	CString cID;
	cID.Format("%d",iProjectID);
	return m_ado.Execute( "DELETE FROM Production WHERE UpperID=" + cID);	
}

//根据文档ID删除该文档的相关信息
BOOL CGeoProduction::sql_delete(DWORD iDocID)
{
	CString cID;
	cID.Format("%d",iDocID);
	return m_ado.Execute( "DELETE FROM Production WHERE ProductionID=" + cID);	
}

//根据文档编号读取所有字段值
BOOL CGeoProduction::GetData(DWORD iDocID)
{
	CString cID;
	cID.Format("%d",iDocID);
	_RecordsetPtr m_pRecordset = m_ado.GetRS("SELECT * FROM Production WHERE ProductionID=" + cID);

	if(m_pRecordset->adoEOF)
		return FALSE;
	else
	{
		m_ProductionID	= iDocID;
		m_DocName    	= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DocName");   
		m_DocType		= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DocType");  	
		m_DocSize		= atol((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DocSize"));
		m_DocCreDate	= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DocCreDate");
		m_Describe 		= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("Describe");
		m_FillUser		= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("FillUser");
		m_FillDate 		= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("FillDate");
		m_DataZip		= atoi((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DataZip"));
		m_UpperID		= atol((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("UpperID"));  
		return TRUE;
	}
}

