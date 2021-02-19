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
	m_RecordID=0;							//���߱��
	m_CurveName=_T("");						//��������
	m_CurveAlias=_T("");					//���߱���
	m_Dimension=2;							//����ά��
	m_DepUnit="m";    						//��ȵ�λ
	m_DepSt=0.f;							//��ʼ���
	m_DepEn=0.f;							//�������
	m_DepRlev=0.125f;						//��Ȳ���
	m_XName=_T("");    						//X������
	m_XUnit=_T("");    						//X�ᵥλ
	m_XMin=0.f;								//X����Сֵ
	m_XMax=0.f;								//X�����ֵ
	m_XRlev=0.f;							//X���������
	m_XNPS=1;								//X�������
	m_DataType=4;							//��������
	m_DataUnit=_T("none");    				//���ݵ�λ
	m_CreateDate=_T("");					//��д����
	m_DataSize=0;							//�������С
	m_PieceSize=0;							//���ݿ��С
	m_DataZip=0;							//����ѹ����ʶ 0-δѹ�� 16-��׼6��Zlibѹ��
	m_DataStID=0;							//���ݿ���ʼ��¼��
	m_DataPieceNum=0;						//���ݿ���
	m_FillUser=_T("");						//�ύ��
	m_FillDate=_T("");						//�ύ����
	m_UpperID=0;							//�����⾮��Ŀ���
}

CGeoCurve::~CGeoCurve()
{
	Disconnect();
}

/****************************************************************************************/

//�����������жϸ������Ƿ������ݿ��д���
DWORD CGeoCurve::HaveName(CString cCurveName)
{	
	_RecordsetPtr m_pRecordset = GetRecordSet( "SELECT * FROM Curve WHERE CurveName='" + cCurveName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((_bstr_t)m_pRecordset->GetCollect("RecordID"));
}


//��������ID��ȡ��������
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


//�����µ�������Ϣ
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


//��������ID�������ߵ���Ϣ
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

//��������ID�޸����������⾮��Ŀ
BOOL CGeoCurve::sql_updateStorage(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",m_RecordID);
	CString sID;
	sID.Format("%lu",iStorageID);
	return Execute("UPDATE Curve SET UpperID=" + sID + " WHERE RecordID=" + cID);	
}

//��������ID�޸���������DataByte�ֶ�ֵ
DWORD CGeoCurve::SetDataByte(DWORD iCurveID,int Number,DWORD nLen,char *pBuf)
{
	// ��������������в���һ����¼	
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
	
	// ����������pBufд��CurveBody���е�DataByte���ֶ�
	CString szID;
	szID.Format("%lu",DataID);
	//ִ��SELETE���,����ǰ���߼�¼��ȡ��pRs������
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM CurveBody WHERE RecordID=" + szID);	

	VARIANT			varBLOB;
	SAFEARRAY		*psa;  //��������
    SAFEARRAYBOUND	rgsabound[1];

	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = nLen;
	psa = SafeArrayCreate(VT_UI1, 1, rgsabound);  //��������
	for(long i = 0; i <nLen; i++)  //��pBuf�е�����д������psa
		SafeArrayPutElement (psa, &i, pBuf++);
	varBLOB.vt = VT_ARRAY | VT_UI1;
	varBLOB.parray = psa;
	//����AppendChunk()����������д��DataByte�ֶ�
	pRS->GetFields()->GetItem("DataByte")->AppendChunk(varBLOB);
	//�������ݿ�
	pRS->Update();
	return DataID;
}

//��������ID��ȡ��������DataByte�ֶ�ֵ
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
		// ������������DataByte��ȡ��pBuf
		pRs= GetRecordSet("SELECT * FROM CurveBody WHERE RecordID=" + szID);
		//��ȡDataByte�ֶε�ʵ�ʴ�С
		Size = pRs->GetFields()->GetItem("DataByte")->ActualSize;		
		if(Size>0)
		{		
			//��DataByte�ֶ��ж�ȡ���ݵ�varBLOB��
			_variant_t varBLOB = pRs->GetFields()->GetItem("DataByte")->GetChunk(Size);		
			if(varBLOB.vt == (VT_ARRAY | VT_UI1))
			{
				SafeArrayAccessData(varBLOB.parray,(void **)&Buffer); 
				//�������ݵ�������pBuf
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

//���ݲ⾮��ĿIDɾ������Ŀ�е���������
BOOL CGeoCurve::DeleteProject(DWORD iProjectID)
{
	CString cID;
	cID.Format("%lu",iProjectID);
	return Execute("DELETE FROM Curve WHERE UpperID=" + cID);	
}

//��������IDɾ�������ߵ������Ϣ
BOOL CGeoCurve::sql_delete(DWORD iCurveID)
{
	CString cID;
	cID.Format("%lu",iCurveID);
	return Execute( "DELETE FROM Curve WHERE RecordID=" + cID);	
}

//�������߱�Ŷ�ȡ�����ֶ�ֵ
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