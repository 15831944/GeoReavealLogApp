// Yuangong.cpp: implementation of the CGeoCurve class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoCurve.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGeoCurve::CGeoCurve()
{
	m_CurveID=0;		
	m_CurveName=_T("");   
	m_CurveAlias=_T("");	
	m_Dimension=2;	
	m_DepUnit="m";    	
	m_DepSt=0.f;		
	m_DepEn=0.f;		
	m_DepRlev=0.125f;		
	m_XName=_T("");    	
	m_XUnit=_T("");    	
	m_XMin=0.f;		
	m_XMax=0.f;		
	m_XRlev=0.f;		
	m_XNPS=1;		
	m_DataType=4;	
	m_DataUnit=_T("none");    	
	m_CreateDate=_T("");
	m_UpperID=0;
	m_DataZip=0;
	m_DataSize=0;
}

CGeoCurve::~CGeoCurve()
{
	Disconnect();
}

void CGeoCurve::Disconnect()
{
	m_ado.Disconnect();
}

BOOL CGeoCurve::InitData(CString szCon)
{
	return m_ado.InitData(szCon);
}

/****************************************************************************************/

//根据曲线名判断该曲线是否在数据库中存在
DWORD CGeoCurve::HaveName(CString cCurveName)
{	
	_RecordsetPtr m_pRecordset = m_ado.GetRS( "SELECT * FROM Curve WHERE CurveName='" + cCurveName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("CurveID"));
}


//根据曲线ID获取曲线姓名
CString CGeoCurve::GetName(DWORD iCurveID)
{	
	CString cID;
	cID.Format("%d",iCurveID);
	_RecordsetPtr m_pRecordset = m_ado.GetRS("SELECT CurveName FROM Curve WHERE CurveID=" + cID);

	if (m_pRecordset->adoEOF)
		return "";
	else
		return (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("CurveName");
}


//插入新的曲线信息
DWORD CGeoCurve::sql_insert()
{	
	CString sDim;sDim.Format("%d",m_Dimension);	
	CString sDepSt;sDepSt.Format("%.2f",m_DepSt);		
	CString sDepEn;sDepEn.Format("%.2f",m_DepEn);
	CString sDepRlev;sDepRlev.Format("%g",m_DepRlev);		  	
	CString sXMin;sXMin.Format("%.2f",m_XMin);		
	CString sXMax;sXMax.Format("%.2f",m_XMax);		
	CString sXRlev;sXRlev.Format("%g",m_XRlev);		
	CString sXNPS;sXNPS.Format("%d",m_XNPS);		
	CString sDataType;sDataType.Format("%d",m_DataType);
	CString sDataZip;sDataZip.Format("%d",m_DataZip);
	CString sUpperID;sUpperID.Format("%lu",m_UpperID);
	CString sDataSize;sDataSize.Format("%d",m_DataSize);

	CString vSQL;
	vSQL = "INSERT INTO Curve ( CurveName,   	\
								CurveAlias,		\
								Dimension,		\
								DepUnit,		\
								DepSt,			\
								DepEn,			\
								DepRlev,		\
								XName,    		\
								XUnit,    		\
								XMin,			\
								XMax,			\
								XRlev,			\
								XNPS,			\
								DataType,		\
								DataUnit,    	\
								CreateDate,		\
								DataSize,		\
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
			VALUES('" + m_CurveName 	+ "','" 
					  + m_CurveAlias	+ "','"  
					  + sDim			+ "','" 
					  + m_DepUnit 		+ "','" 
					  + sDepSt 			+ "','" 
					  + sDepEn			+ "','" 
					  + sDepRlev 		+ "','" 
					  + m_XName			+ "','" 
					  + m_XUnit 		+ "','" 
					  + sXMin			+ "','" 
					  + sXMax 			+ "','" 
					  + sXRlev			+ "','" 
					  + sXNPS 			+ "','" 
					  + sDataType 		+ "','" 
					  + m_DataUnit 		+ "','" 
					  + m_CreateDate 	+ "','" 
					  + sDataSize	 	+ "','" 
					  + sDataZip	 	+ "','" 
					  + sUpperID 		+ "','NULL','NULL','NULL','NULL','NULL','NULL','NULL','NULL','NULL','NULL')"; 

	if(!m_ado.Execute(vSQL))
		return 0;
	//获取刚才插入的存储空间信息中的StorageID值(数据库中StorageID字段是自动增加的)
	DWORD CurID;
	_RecordsetPtr pRS = m_ado.GetRS("SELECT MAX(CurveID) AS MaxID FROM Curve");

	if(pRS->adoEOF)
		CurID = 0;
	else 
		CurID = atol((LPCTSTR)(_bstr_t)pRS->GetCollect("MaxID"));

	return CurID;
}


//根据曲线ID更新曲线的信息
BOOL CGeoCurve::sql_update(DWORD iCurveID)
{		
	CString sDim;sDim.Format("%d",m_Dimension);	
	CString sDepSt;sDepSt.Format("%.2f",m_DepSt);		
	CString sDepEn;sDepEn.Format("%.2f",m_DepEn);
	CString sDepRlev;sDepRlev.Format("%g",m_DepRlev);		  	
	CString sXMin;sXMin.Format("%.2f",m_XMin);		
	CString sXMax;sXMax.Format("%.2f",m_XMax);		
	CString sXRlev;sXRlev.Format("%g",m_XRlev);		
	CString sXNPS;sXNPS.Format("%d",m_XNPS);		
	CString sDataType;sDataType.Format("%d",m_DataType);	
	CString sUpperID;sUpperID.Format("%lu",m_UpperID);
	CString sDataZip;sDataZip.Format("%d",m_DataZip);
	CString sDataSize;sDataSize.Format("%d",m_DataSize);

	CString cID;
	cID.Format("%d",iCurveID);
	CString str("UPDATE Curve SET	  CurveName='" +			m_CurveName 
									+ "', CurveAlias='" +		m_CurveAlias
									+ "', Dimension='" +		sDim
									+ "', DepUnit='" +			m_DepUnit 
									+ "', DepSt='" +			sDepSt 
									+ "', DepEn='"+				sDepEn
									+ "', DepRlev='" +			sDepRlev 	
									+ "', XName='" +			m_XName
									+ "', XUnit='" +			m_XUnit 
									+ "', XMin='" +				sXMin
									+ "', XMax='" +				sXMax 
									+ "', XRlev='" +			sXRlev					
									+ "', XNPS='" +				sXNPS 
									+ "', DataType='" +			sDataType 
									+ "', DataUnit='"+			m_DataUnit 
									+ "', CreateDate='" +		m_CreateDate 
									+ "', DataSize='" +			sDataSize
									+ "', DataZip='" +			sDataZip 
									+ "', UpperID=" +			sUpperID 
				+ "' WHERE CurveID=" + cID );
	return m_ado.Execute(str);	
}

//根据曲线ID修改曲线所在的部门(用于存储空间调转功能模块中)
BOOL CGeoCurve::sql_updateStorage(int iStorageID)
{
	CString cID;
	cID.Format("%d",m_CurveID);
	CString sID;
	sID.Format("%d",iStorageID);
	return m_ado.Execute("UPDATE Curve SET UpperID=" + sID + " WHERE CurveID=" + cID);	
}

//根据曲线ID修改曲线数据DataByte字段值
void CGeoCurve::SetDataByte(DWORD iCurveID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH])
{
	CString cID;
	cID.Format("%d",iCurveID);
	//@@@@@@@@@@@  将曲线据pBuf保存到数据库中  @@@@@@@@@@@@@

	//设置SELECT语句
	CString vSQL;
	vSQL = "SELECT * FROM Curve WHERE CurveID=" + cID;
	//执行SELETE语句
	_RecordsetPtr	m_pRs;
	//将当前曲线记录读取到m_pRs对象中
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

//根据曲线ID读取曲线数据DataByte字段值
DWORD CGeoCurve::GetDataByte(DWORD iCurveID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH])
{
	CString cID;
	cID.Format("%d",iCurveID);
	//@@@@@@@@@@@  将曲线据数据库中DataByte读取到pBuf  @@@@@@@@@@@@@
	CString vSQL("SELECT * FROM Curve WHERE CurveID=" + cID);
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

//根据测井项目ID删除该项目中的所有曲线
BOOL CGeoCurve::DeleteProject(DWORD iProjectID)
{
	CString cID;
	cID.Format("%d",iProjectID);
	return m_ado.Execute("DELETE FROM Curve WHERE UpperID=" + cID);	
}

//根据曲线ID删除该曲线的相关信息
BOOL CGeoCurve::sql_delete(DWORD iCurveID)
{
	CString cID;
	cID.Format("%d",iCurveID);
	return m_ado.Execute( "DELETE FROM Curve WHERE CurveID=" + cID);	
}

//根据曲线编号读取所有字段值
BOOL CGeoCurve::GetData(DWORD iCurveID)
{
	CString cID;
	cID.Format("%d",iCurveID);
	_RecordsetPtr m_pRecordset = m_ado.GetRS("SELECT * FROM Curve WHERE CurveID=" + cID);

	if(m_pRecordset->adoEOF)
		return FALSE;
	else
	{
		m_CurveID			= iCurveID;
		m_CurveName    		= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("CurveName");   
		m_CurveAlias		= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("CurveAlias");  	
		m_Dimension			= atoi((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("Dimension"));  	
		m_DepUnit    		= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DepUnit");      	
		m_DepSt				= (float)atof((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DepSt"));  		
		m_DepEn				= (float)atof((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DepEn"));  		
		m_DepRlev			= (float)atof((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DepRlev"));  		
		m_XName    			= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("XName");      	
		m_XUnit    			= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("XUnit");      	
		m_XMin				= (float)atof((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("XMin"));  		
		m_XMax				= (float)atof((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("XMax"));  		
		m_XRlev				= (float)atof((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("XRlev"));  		
		m_XNPS				= atoi((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("XNPS"));  		
		m_DataType			= atoi((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DataType"));  	
		m_DataUnit    		= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DataUnit");      
		m_CreateDate		= (LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("CreateDate");
		m_DataSize			= atol((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DataSize"));
		m_DataZip			= atoi((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("DataZip"));
		m_UpperID			= atoi((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("UpperID"));  	
		return TRUE; 	
	}
}

