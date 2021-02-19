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
	m_RecordID=0;							//曲线编号
	m_CurveName=_T("");						//曲线名称
	m_CurveAlias=_T("");					//曲线别名
	m_Dimension=2;							//曲线维数
	m_DepUnit="m";    						//深度单位
	m_DepSt=0.f;							//起始深度
	m_DepEn=0.f;							//结束深度
	m_DepRlev=0.125f;						//深度步长
	m_XName=_T("");    						//X轴名称
	m_XUnit=_T("");    						//X轴单位
	m_XMin=0.f;								//X轴最小值
	m_XMax=0.f;								//X轴最大值
	m_XRlev=0.f;							//X轴采样步长
	m_XNPS=1;								//X轴采样数
	m_DataType=4;							//数据类型
	m_DataUnit=_T("none");    				//数据单位
	m_CreateDate=_T("");					//填写日期
	m_DataSize=0;							//数据体大小
	m_PieceSize=0;							//数据块大小
	m_DataZip=0;							//数据压缩标识 0-未压缩 16-标准6级Zlib压缩
	m_DataStID=0;							//数据块起始记录号
	m_DataPieceNum=0;						//数据块数
	m_FillUser=_T("");						//提交人
	m_FillDate=_T("");						//提交日期
	m_UpperID=0;							//所属测井项目编号
}

CGeoCurve::~CGeoCurve()
{
	Disconnect();
}

/****************************************************************************************/

//根据曲线名判断该曲线是否在数据库中存在
DWORD CGeoCurve::HaveName(CString cCurveName)
{	
	_RecordsetPtr m_pRecordset = GetRecordSet( "SELECT * FROM Curve WHERE CurveName='" + cCurveName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((_bstr_t)m_pRecordset->GetCollect("RecordID"));
}


//根据曲线ID获取曲线姓名
CString CGeoCurve::GetName(DWORD iCurveID)
{	
	CString cID;
	cID.Format("%lu",iCurveID);
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT CurveName FROM Curve WHERE RecordID=" + cID);

	if (m_pRecordset->adoEOF)
		return "";
	else
		return m_pRecordset->GetCollect("CurveName");
}


//插入新的曲线信息
DWORD CGeoCurve::sql_insert()
{	
	CString sDim;				sDim.Format("%d",m_Dimension);	
	CString sDepSt;				sDepSt.Format("%.2f",m_DepSt);		
	CString sDepEn;				sDepEn.Format("%.2f",m_DepEn);
	CString sDepRlev;			sDepRlev.Format("%g",m_DepRlev);		  	
	CString sXMin;				sXMin.Format("%.2f",m_XMin);		
	CString sXMax;				sXMax.Format("%.2f",m_XMax);		
	CString sXRlev;				sXRlev.Format("%g",m_XRlev);		
	CString sXNPS;				sXNPS.Format("%d",m_XNPS);		
	CString sDataType;			sDataType.Format("%d",m_DataType);
	
	CString sDataSize;			sDataSize.Format("%lu",m_DataSize);
	CString sPieceSize;			sPieceSize.Format("%lu",m_PieceSize);		
	CString sDataZip;			sDataZip.Format("%d",m_DataZip);			
	CString sDataStID;			sDataStID.Format("%lu",m_DataStID);		
	CString sDataPieceNum;		sDataPieceNum.Format("%d",m_DataPieceNum);	
	CString sUpperID;			sUpperID.Format("%lu",m_UpperID);
	

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
								PieceSize,		\
								DataZip,		\
								DataStID,		\
								DataPieceNum,	\
								FillUser,		\
								FillDate,		\
								UpperID)		\
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
					  + sDataSize		+ "','" 
					  + sPieceSize		+ "','" 	
				   	  + sDataZip		+ "','" 
					  + sDataStID		+ "','" 
					  + sDataPieceNum	+ "','" 
					  + m_FillUser		+ "','" 
					  + m_FillDate		+ "','"
					  + sUpperID		+ "')";

	if(!Execute(vSQL))
		return 0;
	return GetMaxRecordID("Curve");
}


//根据曲线ID更新曲线的信息
BOOL CGeoCurve::sql_update(DWORD iCurveID)
{		
	CString sDim;				sDim.Format("%d",m_Dimension);	
	CString sDepSt;				sDepSt.Format("%.2f",m_DepSt);		
	CString sDepEn;				sDepEn.Format("%.2f",m_DepEn);
	CString sDepRlev;			sDepRlev.Format("%g",m_DepRlev);		  	
	CString sXMin;				sXMin.Format("%.2f",m_XMin);		
	CString sXMax;				sXMax.Format("%.2f",m_XMax);		
	CString sXRlev;				sXRlev.Format("%g",m_XRlev);		
	CString sXNPS;				sXNPS.Format("%d",m_XNPS);		
	CString sDataType;			sDataType.Format("%d",m_DataType);

	CString sDataSize;			sDataSize.Format("%lu",m_DataSize);
	CString sPieceSize;			sPieceSize.Format("%lu",m_PieceSize);		
	CString sDataZip;			sDataZip.Format("%d",m_DataZip);			
	CString sDataStID;			sDataStID.Format("%lu",m_DataStID);		
	CString sDataPieceNum;		sDataPieceNum.Format("%d",m_DataPieceNum);	
	CString sUpperID;			sUpperID.Format("%lu",m_UpperID);

	CString cID;
	cID.Format("%lu",iCurveID);
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
									+ "', PieceSize='" +		sPieceSize		
									+ "', DataZip='" +			sDataZip		
									+ "', DataStID='" +			sDataStID		
									+ "', DataPieceNum='" +		sDataPieceNum	
									+ "', FillUser='" +			m_FillUser		
									+ "', FillDate='" +			m_FillDate		
									+ "', UpperID='" +			sUpperID 
									+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

//根据曲线ID修改曲线所属测井项目
BOOL CGeoCurve::sql_updateStorage(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",m_RecordID);
	CString sID;
	sID.Format("%lu",iStorageID);
	return Execute("UPDATE Curve SET UpperID=" + sID + " WHERE RecordID=" + cID);	
}

//根据曲线ID修改曲线数据DataByte字段值
DWORD CGeoCurve::SetDataByte(DWORD iCurveID,int Number,DWORD nLen,char *pBuf)
{
	// 在曲线数据体表中插入一个记录	
	CString sDataSize;	sDataSize.Format("%lu",nLen);
	CString sNumber;	sNumber.Format("%d",Number);
	CString sUpperID;	sUpperID.Format("%lu",iCurveID);
	CString vSQL;
	vSQL = "INSERT INTO CurveBody(  Number,	\
									DataSize,	\
									UpperID,	\
									DataByte)	\
	VALUES( '"  + sNumber	+ "','"	+ sDataSize	+ "','"	+ sUpperID	+ "',0x)";
	if(!Execute(vSQL))
		return 0;

	DWORD DataID=GetMaxRecordID("CurveBody");
	if(DataID<1)
		return 0;
	
	// 将曲线数据pBuf写入CurveBody表中的DataByte大字段
	CString szID;
	szID.Format("%lu",DataID);
	//执行SELETE语句,将当前曲线记录读取到pRs对象中
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM CurveBody WHERE RecordID=" + szID);	

	VARIANT			varBLOB;
	SAFEARRAY		*psa;  //定义数组
    SAFEARRAYBOUND	rgsabound[1];

	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = nLen;
	psa = SafeArrayCreate(VT_UI1, 1, rgsabound);  //创建数组
	for(long i = 0; i <nLen; i++)  //将pBuf中的数据写入数组psa
		SafeArrayPutElement (psa, &i, pBuf++);
	varBLOB.vt = VT_ARRAY | VT_UI1;
	varBLOB.parray = psa;
	//调用AppendChunk()函数将数据写入DataByte字段
	pRS->GetFields()->GetItem("DataByte")->AppendChunk(varBLOB);
	//更新数据库
	pRS->Update();
	return DataID;
}

//根据曲线ID读取曲线数据DataByte字段值
DWORD CGeoCurve::GetDataByte(DWORD iCurveID,DWORD nLen,char *pBuf)
{
	CString szID;
	szID.Format("%lu",iCurveID);
	_RecordsetPtr m_pRs = GetRecordSet("SELECT * FROM CurveBody WHERE UpperID=" + szID);

	int Number;
	DWORD DataID;
	_RecordsetPtr  pRs=NULL;
	BSTR HUGEP *Buffer=NULL;
	_variant_t varBLOB;
	DWORD Size,len0,pos=0;
	while(m_pRs->adoEOF == 0)
	{	
		Number=atoi((_bstr_t)m_pRs->GetCollect("Number"));
		DataID=atol((_bstr_t)m_pRs->GetCollect("RecordID"));	

		szID.Format("%lu",DataID);
		// 将据数据体中DataByte读取到pBuf
		pRs= GetRecordSet("SELECT * FROM CurveBody WHERE RecordID=" + szID);
		//获取DataByte字段的实际大小
		Size = pRs->GetFields()->GetItem("DataByte")->ActualSize;		
		if(Size>0)
		{		
			//从DataByte字段中读取数据到varBLOB中
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

//根据测井项目ID删除该项目中的所有曲线
BOOL CGeoCurve::DeleteProject(DWORD iProjectID)
{
	CString cID;
	cID.Format("%lu",iProjectID);
	return Execute("DELETE FROM Curve WHERE UpperID=" + cID);	
}

//根据曲线ID删除该曲线的相关信息
BOOL CGeoCurve::sql_delete(DWORD iCurveID)
{
	CString cID;
	cID.Format("%lu",iCurveID);
	return Execute( "DELETE FROM Curve WHERE RecordID=" + cID);	
}

//根据曲线编号读取所有字段值
BOOL CGeoCurve::GetData(DWORD iCurveID)
{
	CString cID;
	cID.Format("%lu",iCurveID);
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT * FROM Curve WHERE RecordID=" + cID);

	if(m_pRecordset->adoEOF)
		return FALSE;
	else
	{
		m_RecordID			= iCurveID;
		m_CurveName    		= m_pRecordset->GetCollect("CurveName");   
		m_CurveAlias		= m_pRecordset->GetCollect("CurveAlias");  	
		m_Dimension			= atoi((_bstr_t)m_pRecordset->GetCollect("Dimension"));  	
		m_DepUnit    		= m_pRecordset->GetCollect("DepUnit");      	
		m_DepSt				= (float)atof((_bstr_t)m_pRecordset->GetCollect("DepSt"));  		
		m_DepEn				= (float)atof((_bstr_t)m_pRecordset->GetCollect("DepEn"));  		
		m_DepRlev			= (float)atof((_bstr_t)m_pRecordset->GetCollect("DepRlev"));  		
		m_XName    			= m_pRecordset->GetCollect("XName");      	
		m_XUnit    			= m_pRecordset->GetCollect("XUnit");      	
		m_XMin				= (float)atof((_bstr_t)m_pRecordset->GetCollect("XMin"));  		
		m_XMax				= (float)atof((_bstr_t)m_pRecordset->GetCollect("XMax"));  		
		m_XRlev				= (float)atof((_bstr_t)m_pRecordset->GetCollect("XRlev"));  		
		m_XNPS				= atoi((_bstr_t)m_pRecordset->GetCollect("XNPS"));  		
		m_DataType			= atoi((_bstr_t)m_pRecordset->GetCollect("DataType"));
		m_DataUnit    		= m_pRecordset->GetCollect("DataUnit");      
		m_CreateDate		= m_pRecordset->GetCollect("CreateDate");
		m_DataSize			= atoi((_bstr_t)m_pRecordset->GetCollect("DataSize"));			
		m_PieceSize			= atoi((_bstr_t)m_pRecordset->GetCollect("PieceSize"));			
		m_DataZip			= atoi((_bstr_t)m_pRecordset->GetCollect("DataZip"));			
		m_DataStID			= atoi((_bstr_t)m_pRecordset->GetCollect("DataStID"));			
		m_DataPieceNum		= atoi((_bstr_t)m_pRecordset->GetCollect("DataPieceNum"));		
		m_FillUser			= m_pRecordset->GetCollect("FillUser");			
		m_FillDate			= m_pRecordset->GetCollect("FillDate");			
		m_UpperID			= atoi((_bstr_t)m_pRecordset->GetCollect("UpperID"));  	
		return TRUE; 	
	}
}