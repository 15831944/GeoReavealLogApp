// Yuangong.cpp: implementation of the CGeoTemplate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoTemplate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGeoTemplate::CGeoTemplate()
{
	m_RecordID=0;	
	m_Name=_T("");		
	m_Type=_T("");
	m_Kind=_T("");
	m_DataSize=0;		
	m_CreDate=_T("");	
	
	m_Describe=_T(""); 	
	m_FillUser=_T("");	
	m_FillDate=_T("");	
	m_DataZip=0;
}

CGeoTemplate::~CGeoTemplate()
{
	Disconnect();
}

/****************************************************************************************/

//�����ĵ����жϸ��ĵ��Ƿ������ݿ��д���
DWORD CGeoTemplate::HaveName(CString cDocName)
{	
	_RecordsetPtr m_pRecordset = GetRecordSet( "SELECT RecordID FROM Template WHERE DocName='" + cDocName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((_bstr_t)m_pRecordset->GetCollect("RecordID"));
}


//�����ĵ�ID��ȡ�ĵ�����
CString CGeoTemplate::GetName(DWORD iDocID)
{	
	CString cID;
	cID.Format("%lu",iDocID);
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT DocName FROM Template WHERE RecordID=" + cID);

	if (m_pRecordset->adoEOF)
		return "";
	else
		return m_pRecordset->GetCollect("Name");
}


//�����µ��ĵ���Ϣ
DWORD CGeoTemplate::sql_insert()
{		
	CString sDocSize;sDocSize.Format("%lu",m_DataSize);	
	CString sDataZip;sDataZip.Format("%d",m_DataZip);
	CString vSQL;
	vSQL = "INSERT INTO Template( TempName,   	\
								TempType,		\
								TempKind,		\
								DataSize,		\
								TempCreDate,	\
								Describe,		\
								FillUser,		\
								FillDate,		\
								DataZip,		\
								DataByte)		\
			VALUES('" + m_Name 			+ "','" 
					  + m_Type			+ "','"
					  + m_Kind			+ "','"
					  + sDocSize		+ "','" 
					  + m_CreDate 		+ "','" 
					  + m_Describe 		+ "','" 
					  + m_FillUser		+ "','" 
					  + m_FillDate 		+ "','" 
					  + sDataZip 		+ "',0x)"; 

	if(!Execute(vSQL))
		return 0;
	return GetMaxRecordID("Template");
}


//�����ĵ�ID�����ĵ�����Ϣ
BOOL CGeoTemplate::sql_update(DWORD iDocID)
{		
	CString sDocSize;sDocSize.Format("%lu",m_DataSize);	
	CString sDataZip;sDataZip.Format("%d",m_DataZip);

	CString cID;
	cID.Format("%lu",iDocID);
	CString str("UPDATE Template SET	  TempName='"	+		m_Name 
									+ "', TempType='"	+		m_Type
									+ "', TempKind='"	+		m_Kind
									+ "', DataSize='"	+		sDocSize	
									+ "', TempCreDate='"+		m_CreDate
									+ "', Describe='"	+		m_Describe 	
									+ "', FillUser='"	+		m_FillUser	
									+ "', FillDate='"	+		m_FillDate 
									+ "', DataZip='"	+		sDataZip 
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

//�����ĵ�ID�޸��ĵ�����DocByte�ֶ�ֵ
void CGeoTemplate::SetDataByte(DWORD iDocID,DWORD nLen,char *pBuf)
{
	CString szID;
	szID.Format("%lu",iDocID);
	_RecordsetPtr  pRs= GetRecordSet("SELECT DataByte FROM Template WHERE RecordID=" + szID);

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
	pRs->GetFields()->GetItem("DataByte")->AppendChunk(varBLOB);
	//�������ݿ�
	pRs->Update();
}

//�����ĵ�ID��ȡ�ĵ�����DocByte�ֶ�ֵ
DWORD CGeoTemplate::GetDataByte(DWORD iDocID,DWORD nLen,char *pBuf)
{
	CString szID;
	szID.Format("%lu",iDocID);
	_RecordsetPtr  pRs= GetRecordSet("SELECT DataByte FROM Template WHERE RecordID=" + szID);
	//��ȡDataByte�ֶε�ʵ�ʴ�С
	DWORD Size = pRs->GetFields()->GetItem("DataByte")->ActualSize;		
	BSTR HUGEP *Buffer=NULL;
	if(Size>0)
	{		
		//��ͼ���ֶ��ж�ȡ���ݵ�varBLOB��
		_variant_t varBLOB = pRs->GetFields()->GetItem("DataByte")->GetChunk(Size);		
		if(varBLOB.vt == (VT_ARRAY | VT_UI1))
		{
			SafeArrayAccessData(varBLOB.parray,(void **)&Buffer); 
			//�������ݵ�������pBuf
			if(nLen>Size)
				memcpy(pBuf,Buffer,Size);
			else
				memcpy(pBuf,Buffer,nLen);
			SafeArrayUnaccessData(varBLOB.parray);
		}
	}
	return Size;
}

//�����ĵ�IDɾ�����ĵ��������Ϣ
BOOL CGeoTemplate::sql_delete(DWORD iDocID)
{
	CString cID;
	cID.Format("%lu",iDocID);
	return Execute( "DELETE FROM Template WHERE RecordID=" + cID);	
}

//�����ĵ���Ŷ�ȡ�����ֶ�ֵ
BOOL CGeoTemplate::GetData(DWORD iDocID)
{
	CString cID;
	cID.Format("%lu",iDocID);
	_RecordsetPtr m_pRecordset = GetRecordSet("SELECT * FROM Template WHERE RecordID=" + cID);

	if(m_pRecordset->adoEOF)
		return FALSE;
	else
	{
		m_RecordID		= iDocID;
		m_Name    		= m_pRecordset->GetCollect("TempName");   
		m_Type			= m_pRecordset->GetCollect("TempType");
		m_Kind			= m_pRecordset->GetCollect("TempKind");
		m_DataSize			= atol((_bstr_t)m_pRecordset->GetCollect("DataSize"));
		m_CreDate		= m_pRecordset->GetCollect("TempCreDate");
		m_Describe 		= m_pRecordset->GetCollect("Describe");
		m_FillUser		= m_pRecordset->GetCollect("FillUser");
		m_FillDate 		= m_pRecordset->GetCollect("FillDate");
		m_DataZip		= atoi((_bstr_t)m_pRecordset->GetCollect("DataZip")); 
		return TRUE;
	}
}

