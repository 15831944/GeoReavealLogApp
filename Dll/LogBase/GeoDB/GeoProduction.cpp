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
	m_RecordID=0;					//文档编号
	m_DocName=_T("none");					//文档名称
	m_DocType=_T("处理成果");			//文档类型	
	m_DocTitle=_T("");					//文档标题
	m_DocAuthor=_T("");					//文档作者
	m_DocCreDate=_T("");				//创建日期
	m_Describe=_T("");					//文档描述
	m_FileFormat=_T("");				//文件格式
	m_FileName=_T("");					//文件名	
	m_StorageMode=_T("数据库");			//数据存储方式
	m_FilePath=_T("");					//文件存储路径
	m_DataSize=0;						//数据体大小
	m_PieceSize=0;						//数据块大小
	m_DataZip=0;						//数据压缩标识 0-未压缩 16-标准6级Zlib压缩
	m_DataStID=0;						//数据块起始记录号
	m_DataPieceNum=0;					//数据块数
	m_FillUser=_T("");					//提交人
	m_FillDate=_T("");					//提交日期
	m_UpperID=0;						//所属测井项目编号
}

CGeoProduction::~CGeoProduction()
{
	Disconnect();
}


/****************************************************************************************/

//根据文档名判断该文档是否在数据库中存在
DWORD CGeoProduction::HaveName(CString cDocName)
{	
	_RecordsetPtr m_pRecordset = GetRecordSet( "SELECT * FROM Production WHERE DocName='" + cDocName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((_bstr_t)m_pRecordset->GetCollect("RecordID"));
}


//根据文档ID获取文档姓名
CString CGeoProduction::GetName(DWORD iDocID)
{	
	CString cID;
	cID.Format("%lu",iDocID);
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT DocName FROM Production WHERE RecordID=" + cID);

	if (m_pRecordset->adoEOF)
		return "";
	else
		return m_pRecordset->GetCollect("DocName");
}


//插入新的文档信息
DWORD CGeoProduction::sql_insert()
{		
	CString sUpperID;			sUpperID.Format("%lu",m_UpperID);
	CString sDataSize; 			sDataSize.Format("%lu",m_DataSize); 	
	CString sPieceSize; 		sPieceSize.Format("%lu",m_PieceSize); 	
	CString sDataZip; 			sDataZip.Format("%d",m_DataZip); 	
	CString sDataStID; 			sDataStID.Format("%d",m_DataStID); 	
	CString sDataPieceNum;		sDataPieceNum.Format("%lu",m_DataPieceNum);

	CString vSQL;
	vSQL = "INSERT INTO Production( DocName,   		\
									DocType,		\
									DocTitle,		\
									DocAuthor,		\
									DocCreDate,		\
									Describe,		\
									FileFormat,		\
									FileName,		\
									StorageMode,	\
									FilePath,		\
									DataSize,		\
									PieceSize,		\
									DataZip,		\
									DataStID,		\
									DataPieceNum,	\
									FillUser,		\
									FillDate,		\
									UpperID)		\
			VALUES('" + m_DocName 		+ "','" 
					  + m_DocType		+ "','" 
					  + m_DocTitle		+ "','"	
					  + m_DocAuthor		+ "','"	
					  + m_DocCreDate 	+ "','" 
					  + m_Describe 		+ "','" 
					  + m_FileFormat 	+ "','" 	
					  + m_FileName 		+ "','" 	
					  + m_StorageMode 	+ "','" 
					  + m_FilePath 		+ "','" 
					  + sDataSize 		+ "','"	
					  + sPieceSize 		+ "','"	
					  + sDataZip 		+ "','" 	
					  + sDataStID 		+ "','"	
					  + sDataPieceNum 	+ "','"		  
					  + m_FillUser		+ "','" 
					  + m_FillDate 		+ "','" 
					  + sUpperID 		+ "')"; 

	if(!Execute(vSQL))
		return 0;
	return GetMaxRecordID("Production");
}


//根据文档ID更新文档的信息
BOOL CGeoProduction::sql_update(DWORD iDocID)
{		
	CString sUpperID;			sUpperID.Format("%lu",m_UpperID);
	CString sDataSize; 			sDataSize.Format("%lu",m_DataSize); 	
	CString sPieceSize; 		sPieceSize.Format("%lu",m_PieceSize); 	
	CString sDataZip; 			sDataZip.Format("%d",m_DataZip); 	
	CString sDataStID; 			sDataStID.Format("%lu",m_DataStID); 	
	CString sDataPieceNum;		sDataPieceNum.Format("%d",m_DataPieceNum);

	CString cID;
	cID.Format("%lu",iDocID);
	CString str("UPDATE Production SET	  DocName='"		+		m_DocName 
									+ "', DocType='"		+		m_DocType
									+ "', DocTitle='"		+		m_DocTitle	
									+ "', DocAuthor='"		+		m_DocAuthor
									+ "', DocCreDate='"		+		m_DocCreDate
									+ "', Describe='"		+		m_Describe 	
									+ "', FileFormat='"		+		m_FileFormat 			
									+ "', FileName='"		+		m_FileName 			
									+ "', StorageMode='"	+		m_StorageMode 		
									+ "', FilePath='"		+		m_FilePath 			
									+ "', DataSize='"		+		sDataSize 			
									+ "', PieceSize='"		+		sPieceSize 				
									+ "', DataZip='"		+		sDataZip 			
									+ "', DataStID='"		+		sDataStID 			
									+ "', DataPieceNum='"	+		sDataPieceNum 				
									+ "', FillUser='"		+		m_FillUser	
									+ "', FillDate='"		+		m_FillDate 
									+ "', UpperID='"		+		sUpperID 
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

//根据文档ID修改文档所在的部门(用于存储空间调转功能模块中)
BOOL CGeoProduction::sql_updateStorage(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",m_RecordID);
	CString sID;
	sID.Format("%lu",iStorageID);
	return Execute("UPDATE Production SET UpperID=" + sID + " WHERE RecordID=" + cID);	
}

//根据文档ID修改文档数据DocByte字段值
DWORD CGeoProduction::SetDataByte(DWORD iDocID,int Number,DWORD nLen,char *pBuf)
{
	// 在数据体表中插入一个记录	
	CString sDataSize;	sDataSize.Format("%lu",nLen);
	CString sNumber;	sNumber.Format("%d",Number);
	CString sUpperID;	sUpperID.Format("%lu",iDocID);
	CString vSQL;
	vSQL = "INSERT INTO DocBody(  Number,	\
									DataSize,	\
									UpperID,	\
									DataByte)	\
	VALUES( '"  + sNumber	+ "','"	+ sDataSize	+ "','"	+ sUpperID	+ "',0x)";
	if(!Execute(vSQL))
		return 0;

	DWORD DataID=GetMaxRecordID("DocBody");
	if(DataID<1)
		return 0;
	// 将曲线数据pBuf写入CurveBody表中的DataByte大字段
	CString szID;
	szID.Format("%lu",DataID);
	//执行SELETE语句,将当前曲线记录读取到m_pRs对象中
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM DocBody WHERE RecordID=" + szID);	

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
	//调用AppendChunk()函数将数据写入DataByte字段
	pRS->GetFields()->GetItem("DataByte")->AppendChunk(varBLOB);
	//更新数据库
	pRS->Update();
	return DataID;
}

//根据文档ID读取文档数据DocByte字段值
DWORD CGeoProduction::GetDataByte(DWORD iDocID,DWORD nLen,char *pBuf)
{
	CString szID;
	szID.Format("%lu",iDocID);
	_RecordsetPtr m_pRs = GetRecordSet("SELECT * FROM DocBody WHERE UpperID=" + szID);

	int Number;
	DWORD DataID;
	_RecordsetPtr pRs=NULL;
	BSTR HUGEP	*Buffer=NULL;
	_variant_t	varBLOB;
	DWORD Size,len0,pos=0;
	while(m_pRs->adoEOF == 0)
	{	
		Number=atoi((_bstr_t)m_pRs->GetCollect("Number"));
		DataID=atol((_bstr_t)m_pRs->GetCollect("RecordID"));	

		szID.Format("%lu",DataID);
		// 将据数据体中DataByte读取到pBuf
		pRs= GetRecordSet("SELECT * FROM DocBody WHERE RecordID=" + szID);
		// 获取DataByte字段的实际大小
		Size = pRs->GetFields()->GetItem("DataByte")->ActualSize;		
		if(Size>0)
		{		
			// 从DataByte字段中读取数据到varBLOB中
			_variant_t varBLOB = pRs->GetFields()->GetItem("DataByte")->GetChunk(Size);		
			if(varBLOB.vt == (VT_ARRAY | VT_UI1))
			{
				SafeArrayAccessData(varBLOB.parray,(void **)&Buffer); 
				//复制数据到缓冲区pBuf
				if(pos+Size>nLen)
					len0=nLen-pos;
				else
					len0=Size;
				memcpy(pBuf,Buffer,len0);
				SafeArrayUnaccessData(varBLOB.parray);
				pos+=len0;
				pBuf+=len0;
			}
		}
		m_pRs->MoveNext();
	}
	return pos;
}

//根据测井项目ID删除该项目中的所有成果文档
BOOL CGeoProduction::DeleteProject(DWORD iProjectID)
{
	CString cID;
	cID.Format("%lu",iProjectID);
	return Execute( "DELETE FROM Production WHERE UpperID=" + cID);	
}

//根据文档ID删除该文档的相关信息
BOOL CGeoProduction::sql_delete(DWORD iDocID)
{
	CString cID;
	cID.Format("%lu",iDocID);
	return Execute( "DELETE FROM Production WHERE RecordID=" + cID);	
}

//根据文档编号读取所有字段值
BOOL CGeoProduction::GetData(DWORD iDocID)
{
	CString cID;
	cID.Format("%lu",iDocID);
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT * FROM Production WHERE RecordID=" + cID);

	if(m_pRecordset->adoEOF)
		return FALSE;
	else
	{
		m_RecordID	= iDocID;
		m_DocName    	= m_pRecordset->GetCollect("DocName");   
		m_DocType		= m_pRecordset->GetCollect("DocType");  	
		m_DocTitle		= m_pRecordset->GetCollect("DocTitle");	
		m_DocAuthor		= m_pRecordset->GetCollect("DocAuthor");
		m_DocCreDate	= m_pRecordset->GetCollect("DocCreDate");
		m_Describe 		= m_pRecordset->GetCollect("Describe");
		m_FileFormat	= m_pRecordset->GetCollect("FileFormat"); 	
		m_FileName 		= m_pRecordset->GetCollect("FileName"); 		
		m_StorageMode	= m_pRecordset->GetCollect("StorageMode"); 	
		m_FilePath 		= m_pRecordset->GetCollect("FilePath"); 		
		m_DataSize 		= atol((_bstr_t)m_pRecordset->GetCollect("DataSize"));  		
		m_PieceSize 	= atol((_bstr_t)m_pRecordset->GetCollect("PieceSize"));  	
		m_DataZip 		= atol((_bstr_t)m_pRecordset->GetCollect("DataZip"));  		
		m_DataStID 		= atol((_bstr_t)m_pRecordset->GetCollect("DataStID"));  		
		m_DataPieceNum	= atol((_bstr_t)m_pRecordset->GetCollect("DataPieceNum"));  
		m_FillUser		= m_pRecordset->GetCollect("FillUser");
		m_FillDate 		= m_pRecordset->GetCollect("FillDate");
		m_UpperID		= atol((_bstr_t)m_pRecordset->GetCollect("UpperID"));  
		return TRUE;
	}
}

