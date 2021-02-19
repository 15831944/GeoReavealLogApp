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

//�����ĵ����жϸ��ĵ��Ƿ������ݿ��д���
DWORD CGeoProduction::HaveName(CString cDocName)
{	
	_RecordsetPtr m_pRecordset = m_ado.GetRS( "SELECT * FROM Production WHERE DocName='" + cDocName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((LPCTSTR)(_bstr_t)m_pRecordset->GetCollect("ProductionID"));
}


//�����ĵ�ID��ȡ�ĵ�����
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


//�����µ��ĵ���Ϣ
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
	//��ȡ�ղŲ���Ĵ洢�ռ���Ϣ�е�StorageIDֵ(���ݿ���StorageID�ֶ����Զ����ӵ�)
	DWORD CurID;
	_RecordsetPtr pRS = m_ado.GetRS("SELECT MAX(ProductionID) AS MaxID FROM Production");

	if(pRS->adoEOF)
		CurID = 0;
	else 
		CurID = atol((LPCTSTR)(_bstr_t)pRS->GetCollect("MaxID"));

	return CurID;
}


//�����ĵ�ID�����ĵ�����Ϣ
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

//�����ĵ�ID�޸��ĵ����ڵĲ���(���ڴ洢�ռ��ת����ģ����)
BOOL CGeoProduction::sql_updateStorage(int iStorageID)
{
	CString cID;
	cID.Format("%d",m_ProductionID);
	CString sID;
	sID.Format("%d",iStorageID);
	return m_ado.Execute("UPDATE Production SET UpperID=" + sID + " WHERE ProductionID=" + cID);	
}

//�����ĵ�ID�޸��ĵ�����DocByte�ֶ�ֵ
void CGeoProduction::SetDocByte(DWORD iDocID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH])
{
	CString cID;
	cID.Format("%d",iDocID);
	//@@@@@@@@@@@  ���ĵ���pBuf���浽���ݿ���  @@@@@@@@@@@@@

	//����SELECT���
	CString vSQL;
	vSQL = "SELECT * FROM Production WHERE ProductionID=" + cID;
	//ִ��SELETE���
	_RecordsetPtr	m_pRs;
	//����ǰ�ĵ���¼��ȡ��m_pRs������
	m_pRs = m_ado.GetRS(vSQL);	

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
	//����AppendChunk()������ͼ������д��Photo�ֶ�
	m_pRs->GetFields()->GetItem(szBlob)->AppendChunk(varBLOB);
	//�������ݿ�
	m_pRs->Update();
	//�Ͽ������ݿ������
}

//�����ĵ�ID��ȡ�ĵ�����DocByte�ֶ�ֵ
DWORD CGeoProduction::GetDocByte(DWORD iDocID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH])
{
	CString cID;
	cID.Format("%d",iDocID);
	//@@@@@@@@@@@  �����߾����ݿ���DataByte��ȡ��pBuf  @@@@@@@@@@@@@
	CString vSQL("SELECT * FROM Production WHERE ProductionID=" + cID);
	_RecordsetPtr  m_pRs= m_ado.GetRS(vSQL);
	//��ȡDataByte�ֶε�ʵ�ʴ�С
	DWORD Size = m_pRs->GetFields()->GetItem(szBlob)->ActualSize;		
	char *Buffer=NULL;
	if(Size>0)
	{		
		//��ͼ���ֶ��ж�ȡ���ݵ�varBLOB��
		_variant_t varBLOB = m_pRs->GetFields()->GetItem(szBlob)->GetChunk(Size);		
		if(varBLOB.vt == (VT_ARRAY | VT_UI1))
		{
			//�����Ҫ�Ĵ洢�ռ�
			if(Buffer = new char[Size+1])								
			{	
				SafeArrayAccessData(varBLOB.parray,(void **)&Buffer); 
				//�������ݵ�������pBuf
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

//���ݲ⾮��ĿIDɾ������Ŀ�е����гɹ��ĵ�
BOOL CGeoProduction::DeleteProject(DWORD iProjectID)
{
	CString cID;
	cID.Format("%d",iProjectID);
	return m_ado.Execute( "DELETE FROM Production WHERE UpperID=" + cID);	
}

//�����ĵ�IDɾ�����ĵ��������Ϣ
BOOL CGeoProduction::sql_delete(DWORD iDocID)
{
	CString cID;
	cID.Format("%d",iDocID);
	return m_ado.Execute( "DELETE FROM Production WHERE ProductionID=" + cID);	
}

//�����ĵ���Ŷ�ȡ�����ֶ�ֵ
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

