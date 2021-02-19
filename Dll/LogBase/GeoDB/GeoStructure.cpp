// GeoStructure.cpp: implementation of the CGeoStructure class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoStructure.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGeoStructure::CGeoStructure()
{	
	m_RecordID = 0;				// 记录号
	m_StructureName=_T("");		//构造名称
	m_Alias=_T("");				//构造别名
	m_Area=0.f;					//构造面积
	m_Region=_T("");			//地区
	m_Oilfield=_T("");			//所属油田
	m_ViewX=0.f;				//观察点X坐标
	m_ViewY=0.f;				//观察点Y坐标
	m_Longitude=0.f;			//观察点经度
	m_Latitude=0.f;				//观察点纬度
	m_GeoDescribe=_T(""); 		//构造地质描述
	m_FormDescribe=_T(""); 		//构造成因描述
	m_DrillDescribe=_T(""); 	//邻区钻井情况文字描述
	m_OilDescribe=_T(""); 		//邻区试油情况文字描述
	m_ImageType=_T("");			//构造图类型
	m_ImageSize=0;				//构造图字节数
	m_ImageCreDate=_T("");		//构造图创建日期

	m_FillUser = _T("");		// 填报人
	m_FillDate = _T("");		// 填报日期
	m_Describe = _T("");		// 备注
	m_UpperID  = 0;				// 所属存储记录号
}

CGeoStructure::~CGeoStructure()
{
	Disconnect();
}

/****************************************************************************************/

//根据文档名判断该文档是否在数据库中存在
DWORD CGeoStructure::HaveName(CString cStructureName)
{	
	_RecordsetPtr m_pRecordset = GetRecordSet( "SELECT RecordID FROM Structure WHERE StructureName='" + cStructureName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((_bstr_t)m_pRecordset->GetCollect("RecordID"));
}


//根据文档ID获取文档姓名
CString CGeoStructure::GetName(DWORD iStructureID)
{	
	CString cID;
	cID.Format("%lu",iStructureID);
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT StructureName FROM Structure WHERE RecordID=" + cID);

	if (m_pRecordset->adoEOF)
		return "";
	else
		return m_pRecordset->GetCollect("StructureName");
}


//插入新的文档信息
DWORD CGeoStructure::sql_insert()
{		
	CString sArea;sArea.Format("%.2f",m_Area);	
	CString sViewX;sViewX.Format("%.2f",m_ViewX);
	CString sViewY;sViewY.Format("%.2f",m_ViewY);
	CString sLongitude;sLongitude.Format("%.2f",m_Longitude);
	CString sLatitude;sLatitude.Format("%.2f",m_Latitude);
	CString sImageSize;sImageSize.Format("%lu",m_ImageSize);
	CString sUpperID;sUpperID.Format("%lu",m_UpperID);
	CString vSQL;
	vSQL = "INSERT INTO Structure( StructureName,    \
								Alias				,\
								Area  				,\
								Region				,\
								Oilfield			,\
								ViewX				,\
								ViewY				,\
								Longitude			,\
								Latitude			,\
								GeoDescribe 		,\
								FormDescribe 		,\
								DrillDescribe 		,\
								OilDescribe 		,\
								ImageType			,\
								ImageSize			,\
								ImageCreDate		,\
								FillUser			,\
								FillDate			,\
								UpperID				,\
								DataByte)			\
			VALUES('" + m_StructureName 		+ "','" 
								+ m_Alias				+ "','"  
								+ sArea  				+ "','" 
								+ m_Region				+ "','" 
								+ m_Oilfield			+ "','" 
								+ sViewX				+ "','" 
								+ sViewY				+ "','" 
								+ sLongitude			+ "','"
								+ sLatitude				+ "','"
								+ m_GeoDescribe 		+ "','"
								+ m_FormDescribe 		+ "','"
								+ m_DrillDescribe 		+ "','"
								+ m_OilDescribe 		+ "','"
								+ m_ImageType			+ "','"
								+ sImageSize			+ "','"
								+ m_ImageCreDate		+ "','"
								+ m_FillUser			+ "','"
								+ m_FillDate			+ "','"
								+ sUpperID 				+ "',0x)"; 

	if(!Execute(vSQL))
		return 0;
	return GetMaxRecordID("Structure");
}


//根据文档ID更新文档的信息
BOOL CGeoStructure::sql_update(DWORD iStructureID)
{		
	CString sArea;sArea.Format("%.2f",m_Area);	
	CString sViewX;sViewX.Format("%.2f",m_ViewX);
	CString sViewY;sViewY.Format("%.2f",m_ViewY);
	CString sLongitude;sLongitude.Format("%.2f",m_Longitude);
	CString sLatitude;sLatitude.Format("%.2f",m_Latitude);
	CString sImageSize;sImageSize.Format("%lu",m_ImageSize);
	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);

	CString cID;
	cID.Format("%lu",iStructureID);
	CString str("UPDATE Structure SET	  StructureName='"	+		m_StructureName 
									+ "', Alias				='"		+		m_Alias			
									+ "', Area  			='"		+		sArea  			
									+ "', Region			='"		+		m_Region			
									+ "', Oilfield			='"		+		m_Oilfield		
									+ "', ViewX				='"		+		sViewX			
									+ "', ViewY				='"		+		sViewY			
									+ "', Longitude			='"		+		sLongitude		
									+ "', Latitude			='"		+		sLatitude			
									+ "', GeoDescribe		='"		+ 		m_GeoDescribe 	
									+ "', FormDescribe 		='"		+		m_FormDescribe 	
									+ "', DrillDescribe 	='"		+		m_DrillDescribe 	
									+ "', OilDescribe 		='"		+		m_OilDescribe 	
									+ "', ImageType			='"		+		m_ImageType		
									+ "', ImageSize			='"		+		sImageSize		
									+ "', ImageCreDate		='"		+		m_ImageCreDate	
									+ "', FillUser			='"		+		m_FillUser		
									+ "', FillDate			='"		+		m_FillDate	
									+ "', UpperID			='"		+		sUpperID
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

//根据结构信息ID修改所属工区(用于存储空间调转功能模块中)
BOOL CGeoStructure::sql_updateUpper(DWORD iUpperID)
{
	CString cID;
	cID.Format("%lu",m_RecordID);
	CString sID;
	sID.Format("%lu",iUpperID);
	return Execute("UPDATE Structure SET UpperID=" + sID + " WHERE RecordID=" + cID);	
}

//根据文档ID修改文档数据StructureByte字段值
void CGeoStructure::SetStructureByte(DWORD iStructureID,DWORD nLen,char *pBuf)
{
	CString szID;
	szID.Format("%lu",iStructureID);
	//将文档据pBuf保存到数据库中

	_RecordsetPtr	m_pRs;
	//将当前文档记录读取到m_pRs对象中
	m_pRs = GetRecordSet("SELECT DataByte FROM Structure WHERE RecordID=" + szID);	

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
	m_pRs->GetFields()->GetItem("DataByte")->AppendChunk(varBLOB);
	//更新数据库
	m_pRs->Update();
	//断开与数据库的连接
}

//根据文档ID读取文档数据StructureByte字段值
DWORD CGeoStructure::GetStructureByte(DWORD iStructureID,DWORD nLen,char *pBuf)
{
	CString szID;
	szID.Format("%lu",iStructureID);
	_RecordsetPtr  m_pRs= GetRecordSet("SELECT DataByte FROM Structure WHERE RecordID=" + szID);
	//读取DataByte字段的实际大小
	DWORD Size = m_pRs->GetFields()->GetItem("DataByte")->ActualSize;		
	BSTR HUGEP *Buffer=NULL;
	if(Size>0)
	{		
		//从图像字段中读取数据到varBLOB中
		_variant_t varBLOB = m_pRs->GetFields()->GetItem("DataByte")->GetChunk(Size);		
		if(varBLOB.vt == (VT_ARRAY | VT_UI1))
		{
			SafeArrayAccessData(varBLOB.parray,(void **)&Buffer); 
			//复制数据到缓冲区pBuf
			if(nLen>Size)
				memcpy(pBuf,Buffer,Size);
			else
				memcpy(pBuf,Buffer,nLen);
			SafeArrayUnaccessData(varBLOB.parray);
		}
	}
	return Size;
}

//根据测井项目ID删除该项目中的所有成果文档
BOOL CGeoStructure::DeleteProject(DWORD iProjectID)
{
	CString cID;
	cID.Format("%lu",iProjectID);
	return Execute( "DELETE FROM Structure WHERE UpperID=" + cID);	
}

//根据文档ID删除该文档的相关信息
BOOL CGeoStructure::sql_delete(DWORD iStructureID)
{
	CString cID;
	cID.Format("%lu",iStructureID);
	return Execute( "DELETE FROM Structure WHERE RecordID=" + cID);	
}

//根据编号读取所有字段值
BOOL CGeoStructure::GetData(DWORD iStructureID)
{
	CString cID;
	cID.Format("%lu",iStructureID);
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT * FROM Structure WHERE RecordID=" + cID);

	if(m_pRecordset->adoEOF)
		return FALSE;
	else
	{
		m_RecordID			= iStructureID;
		m_StructureName    	= m_pRecordset->GetCollect("StructureName"); 
		m_Alias				= m_pRecordset->GetCollect("Alias");
		m_Area				= (float)atof((_bstr_t)m_pRecordset->GetCollect("Area"));
		m_Region			= m_pRecordset->GetCollect("Region");
		m_Oilfield 			= m_pRecordset->GetCollect("Oilfield");
		m_ViewX				= (float)atof((_bstr_t)m_pRecordset->GetCollect("ViewX"));
		m_ViewY 			= (float)atof((_bstr_t)m_pRecordset->GetCollect("ViewY"));
		m_Longitude			= (float)atof((_bstr_t)m_pRecordset->GetCollect("Longitude"));
		m_Latitude			= (float)atof((_bstr_t)m_pRecordset->GetCollect("Latitude"));
		m_GeoDescribe 		= m_pRecordset->GetCollect("GeoDescribe");
		m_FormDescribe		= m_pRecordset->GetCollect("FormDescribe");
		m_DrillDescribe		= m_pRecordset->GetCollect("DrillDescribe");
		m_OilDescribe 		= m_pRecordset->GetCollect("OilDescribe");
		m_ImageType			= m_pRecordset->GetCollect("ImageType");
		m_ImageSize			= atol((_bstr_t)m_pRecordset->GetCollect("ImageSize"));
		m_ImageCreDate		= m_pRecordset->GetCollect("ImageCreDate");
		m_FillUser			= m_pRecordset->GetCollect("FillUser");
		m_FillDate			= m_pRecordset->GetCollect("FillDate");
		m_UpperID			= atol((_bstr_t)m_pRecordset->GetCollect("UpperID"));  
		return TRUE;
	}
}

//根据所属存储空编号读取井信息所有字段值
BOOL CGeoStructure::GetDataReferToUpperID(DWORD iUpperID)
{

	CString szID;
	szID.Format("%lu",iUpperID);
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT * FROM Structure WHERE UpperID=" + szID);

	if(m_pRecordset->adoEOF)
		return FALSE;
	else
	{
		m_RecordID			= atol((_bstr_t)m_pRecordset->GetCollect("RecordID"));
		m_StructureName    	= m_pRecordset->GetCollect("StructureName"); 
		m_Alias				= m_pRecordset->GetCollect("Alias");
		m_Area				= (float)atof((_bstr_t)m_pRecordset->GetCollect("Area"));
		m_Region			= m_pRecordset->GetCollect("Region");
		m_Oilfield 			= m_pRecordset->GetCollect("Oilfield");
		m_ViewX				= (float)atof((_bstr_t)m_pRecordset->GetCollect("ViewX"));
		m_ViewY 			= (float)atof((_bstr_t)m_pRecordset->GetCollect("ViewY"));
		m_Longitude			= (float)atof((_bstr_t)m_pRecordset->GetCollect("Longitude"));
		m_Latitude			= (float)atof((_bstr_t)m_pRecordset->GetCollect("Latitude"));
		m_GeoDescribe 		= m_pRecordset->GetCollect("GeoDescribe");
		m_FormDescribe		= m_pRecordset->GetCollect("FormDescribe");
		m_DrillDescribe		= m_pRecordset->GetCollect("DrillDescribe");
		m_OilDescribe 		= m_pRecordset->GetCollect("OilDescribe");
		m_ImageType			= m_pRecordset->GetCollect("ImageType");
		m_ImageSize			= atol((_bstr_t)m_pRecordset->GetCollect("ImageSize"));
		m_ImageCreDate		= m_pRecordset->GetCollect("ImageCreDate");
		m_FillUser			= m_pRecordset->GetCollect("FillUser");
		m_FillDate			= m_pRecordset->GetCollect("FillDate");
		m_UpperID			= atol((_bstr_t)m_pRecordset->GetCollect("UpperID"));  
		return TRUE;
	}
}
