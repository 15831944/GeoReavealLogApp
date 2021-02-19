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
	m_RecordID=0;					//�ĵ����
	m_DocName=_T("none");					//�ĵ�����
	m_DocType=_T("����ɹ�");			//�ĵ�����	
	m_DocTitle=_T("");					//�ĵ�����
	m_DocAuthor=_T("");					//�ĵ�����
	m_DocCreDate=_T("");				//��������
	m_Describe=_T("");					//�ĵ�����
	m_FileFormat=_T("");				//�ļ���ʽ
	m_FileName=_T("");					//�ļ���	
	m_StorageMode=_T("���ݿ�");			//���ݴ洢��ʽ
	m_FilePath=_T("");					//�ļ��洢·��
	m_DataSize=0;						//�������С
	m_PieceSize=0;						//���ݿ��С
	m_DataZip=0;						//����ѹ����ʶ 0-δѹ�� 16-��׼6��Zlibѹ��
	m_DataStID=0;						//���ݿ���ʼ��¼��
	m_DataPieceNum=0;					//���ݿ���
	m_FillUser=_T("");					//�ύ��
	m_FillDate=_T("");					//�ύ����
	m_UpperID=0;						//�����⾮��Ŀ���
}

CGeoProduction::~CGeoProduction()
{
	Disconnect();
}


/****************************************************************************************/

//�����ĵ����жϸ��ĵ��Ƿ������ݿ��д���
DWORD CGeoProduction::HaveName(CString cDocName)
{	
	_RecordsetPtr m_pRecordset = GetRecordSet( "SELECT * FROM Production WHERE DocName='" + cDocName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((_bstr_t)m_pRecordset->GetCollect("RecordID"));
}


//�����ĵ�ID��ȡ�ĵ�����
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


//�����µ��ĵ���Ϣ
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


//�����ĵ�ID�����ĵ�����Ϣ
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

//�����ĵ�ID�޸��ĵ����ڵĲ���(���ڴ洢�ռ��ת����ģ����)
BOOL CGeoProduction::sql_updateStorage(DWORD iStorageID)
{
	CString cID;
	cID.Format("%lu",m_RecordID);
	CString sID;
	sID.Format("%lu",iStorageID);
	return Execute("UPDATE Production SET UpperID=" + sID + " WHERE RecordID=" + cID);	
}

//�����ĵ�ID�޸��ĵ�����DocByte�ֶ�ֵ
DWORD CGeoProduction::SetDataByte(DWORD iDocID,int Number,DWORD nLen,char *pBuf)
{
	// ����������в���һ����¼	
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
	// ����������pBufд��CurveBody���е�DataByte���ֶ�
	CString szID;
	szID.Format("%lu",DataID);
	//ִ��SELETE���,����ǰ���߼�¼��ȡ��m_pRs������
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM DocBody WHERE RecordID=" + szID);	

	VARIANT			varBLOB;
	SAFEARRAY		*psa;  //��������
    SAFEARRAYBOUND	rgsabound[1];

	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = nLen;
	psa = SafeArrayCreate(VT_UI1, 1, rgsabound);  //��������
	for(long i = 0; i <nLen; i++)  //��pBuf�е�ͼ������д������psa
		SafeArrayPutElement (psa, &i, pBuf++);
	varBLOB.vt = VT_ARRAY | VT_UI1;
	varBLOB.parray = psa;
	//����AppendChunk()����������д��DataByte�ֶ�
	pRS->GetFields()->GetItem("DataByte")->AppendChunk(varBLOB);
	//�������ݿ�
	pRS->Update();
	return DataID;
}

//�����ĵ�ID��ȡ�ĵ�����DocByte�ֶ�ֵ
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
		// ������������DataByte��ȡ��pBuf
		pRs= GetRecordSet("SELECT * FROM DocBody WHERE RecordID=" + szID);
		// ��ȡDataByte�ֶε�ʵ�ʴ�С
		Size = pRs->GetFields()->GetItem("DataByte")->ActualSize;		
		if(Size>0)
		{		
			// ��DataByte�ֶ��ж�ȡ���ݵ�varBLOB��
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

//���ݲ⾮��ĿIDɾ������Ŀ�е����гɹ��ĵ�
BOOL CGeoProduction::DeleteProject(DWORD iProjectID)
{
	CString cID;
	cID.Format("%lu",iProjectID);
	return Execute( "DELETE FROM Production WHERE UpperID=" + cID);	
}

//�����ĵ�IDɾ�����ĵ��������Ϣ
BOOL CGeoProduction::sql_delete(DWORD iDocID)
{
	CString cID;
	cID.Format("%lu",iDocID);
	return Execute( "DELETE FROM Production WHERE RecordID=" + cID);	
}

//�����ĵ���Ŷ�ȡ�����ֶ�ֵ
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

