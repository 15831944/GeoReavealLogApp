/*********************************************************************/

#include "stdafx.h"
#include "GeoLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGeoLayer::CGeoLayer()
{
	m_RecordID = 0;
	m_LayerName= _T("");			//层位名称
	m_LayerAlias= _T("");			//层位别名

	m_DepLog=-999.25f;  			//测井深度
	m_TVLog=-999.25f;  				//测井垂深
	m_DepGeo=-999.25f;				//地质深度
	m_TVGeo=-999.25f;				//地质垂深
	m_RemOption= _T("实线底界");	//标注选项               实线底界 虚线底界 断层标识 垂直定位 水平定位	
	m_Describe= _T("");			//解释描述
	m_FillUser= _T("");			//提交人
	m_FillDate= _T("");			//提交日期
	m_UpperID = 0;				//存储空间编号
}

CGeoLayer::~CGeoLayer()
{
	Disconnect();
}

CString CGeoLayer::GetDescribe()
{
	return m_Describe;
}

void CGeoLayer::SetDescribe(CString cDescribe)
{
	m_Describe = cDescribe;
}

CString CGeoLayer::GetFillUser()
{
	return m_FillUser;
}

void CGeoLayer::SetFillUser(CString cUser)
{
	m_FillUser = cUser;
}

CString CGeoLayer::GetFillDate()
{
	return m_FillDate;
}

void CGeoLayer::SetFillDate(CString cDate)
{
	m_FillDate = cDate;
}

DWORD CGeoLayer::GetUpperID()
{
	return m_UpperID;
}

void CGeoLayer::SetUpperID(DWORD iUpperID)
{
	m_UpperID = iUpperID;
}

DWORD CGeoLayer::sql_insert()
{	
	CString sDepLog;  	sDepLog.Format("%.2f",m_DepLog);  	
	CString sTVLog;  	sTVLog.Format("%.2f",m_TVLog);  	
	CString sDepGeo;	sDepGeo.Format("%.2f",m_DepGeo);	
	CString sTVGeo;		sTVGeo.Format("%.2f",m_TVGeo);			
	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);
	CString vSQL;
	vSQL = "INSERT INTO Layer( LayerName,		\
									LayerAlias,		\
									DepLog,  		\
									TVLog,  		\
									DepGeo,			\
									TVGeo,			\
									RemOption,		\
									Describe,		\
									FillUser,		\
									FillDate,		\
									UpperID)		\
		VALUES('"   + m_LayerName	+ "','"
					+ m_LayerAlias	+ "','" 
					+ sDepLog   		+ "','"
					+ sTVLog  		+ "','"
					+ sDepGeo		+ "','"
					+ sTVGeo		+ "','"
					+ m_RemOption	+ "','"			
					+ m_Describe 	+ "','"
					+ m_FillUser	+ "','"
					+ m_FillDate	+ "','"
					+ sUpperID		+ "')";
	if(!Execute(vSQL))
		return 0;
	return GetMaxRecordID("Layer");
}

BOOL CGeoLayer::sql_update(DWORD iLayerID)
{	
	CString sDepLog;  	sDepLog.Format("%.2f",m_DepLog);  	
	CString sTVLog;  	sTVLog.Format("%.2f",m_TVLog);  	
	CString sDepGeo;	sDepGeo.Format("%.2f",m_DepGeo);	
	CString sTVGeo;		sTVGeo.Format("%.2f",m_TVGeo);			
	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);
	CString cID;
	cID.Format("%d",iLayerID);
	CString str("UPDATE Layer SET	  LayerName='"		+		m_LayerName				 //层位名称   
									+ "', LayerAlias='"		+		m_LayerAlias		 //层位别名	
									+ "', DepLog='"  		+		sDepLog   			 //测井深度
									+ "', TVLog='"  		+		sTVLog  			 //测井垂深
									+ "', DepGeo='"			+		sDepGeo				 //地质深度
									+ "', TVGeo='"			+		sTVGeo				 //地质垂深
									+ "', RemOption='"		+		m_RemOption			 //标注选项					
									+ "', Describe='"		+		m_Describe 			 //层位描述
									+ "', FillUser='"		+		m_FillUser			 //提交人
									+ "', FillDate='"		+		m_FillDate			 //提交日期
									+ "', UpperID="			+		sUpperID			 //存储空间编号
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

BOOL CGeoLayer::sql_delete(DWORD iLayerID)
{
	CString cID;
	cID.Format("%d",iLayerID);
	return Execute("DELETE FROM Layer WHERE RecordID=" + cID);
}

//根据层位信息编号读取所有字段值
BOOL CGeoLayer::GetData(DWORD iLayerID)
{
	CString cID;
	cID.Format("%d",iLayerID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Layer WHERE RecordID=" + cID);

	//返回各列的值
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_RecordID=iLayerID;
		m_LayerName	=pRS->GetCollect("LayerName");	
		m_LayerAlias=pRS->GetCollect("LayerAlias");	
		m_DepLog	=(float)atof((_bstr_t)pRS->GetCollect("DepLog"));   	
		m_TVLog		=(float)atof((_bstr_t)pRS->GetCollect("TVLog"));  	
		m_DepGeo	=(float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("DepGeo"));		
		m_TVGeo		=(float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("TVGeo"));		
		m_RemOption	=pRS->GetCollect("RemOption");
		m_Describe	=pRS->GetCollect("Describe");
		m_FillUser	=pRS->GetCollect("FillUser");
		m_FillDate	=pRS->GetCollect("FillDate");
		m_UpperID	=atoi((_bstr_t)pRS->GetCollect("UpperID"));
		return TRUE;
	}
}
