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
	m_RecordID = 0;				// ��¼��
	m_StructureName=_T("");		//��������
	m_Alias=_T("");				//�������
	m_Area=0.f;					//�������
	m_Region=_T("");			//����
	m_Oilfield=_T("");			//��������
	m_ViewX=0.f;				//�۲��X����
	m_ViewY=0.f;				//�۲��Y����
	m_Longitude=0.f;			//�۲�㾭��
	m_Latitude=0.f;				//�۲��γ��
	m_GeoDescribe=_T(""); 		//�����������
	m_FormDescribe=_T(""); 		//�����������
	m_DrillDescribe=_T(""); 	//�����꾮�����������
	m_OilDescribe=_T(""); 		//�������������������
	m_ImageType=_T("");			//����ͼ����
	m_ImageSize=0;				//����ͼ�ֽ���
	m_ImageCreDate=_T("");		//����ͼ��������

	m_FillUser = _T("");		// ���
	m_FillDate = _T("");		// �����
	m_Describe = _T("");		// ��ע
	m_UpperID  = 0;				// �����洢��¼��
}

CGeoStructure::~CGeoStructure()
{
	Disconnect();
}

/****************************************************************************************/

//�����ĵ����жϸ��ĵ��Ƿ������ݿ��д���
DWORD CGeoStructure::HaveName(CString cStructureName)
{	
	_RecordsetPtr m_pRecordset = GetRecordSet( "SELECT RecordID FROM Structure WHERE StructureName='" + cStructureName + "'");
	
	if(m_pRecordset->adoEOF)
		return 0;
	else
		return atol((_bstr_t)m_pRecordset->GetCollect("RecordID"));
}


//�����ĵ�ID��ȡ�ĵ�����
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


//�����µ��ĵ���Ϣ
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


//�����ĵ�ID�����ĵ�����Ϣ
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

//���ݽṹ��ϢID�޸���������(���ڴ洢�ռ��ת����ģ����)
BOOL CGeoStructure::sql_updateUpper(DWORD iUpperID)
{
	CString cID;
	cID.Format("%lu",m_RecordID);
	CString sID;
	sID.Format("%lu",iUpperID);
	return Execute("UPDATE Structure SET UpperID=" + sID + " WHERE RecordID=" + cID);	
}

//�����ĵ�ID�޸��ĵ�����StructureByte�ֶ�ֵ
void CGeoStructure::SetStructureByte(DWORD iStructureID,DWORD nLen,char *pBuf)
{
	CString szID;
	szID.Format("%lu",iStructureID);
	//���ĵ���pBuf���浽���ݿ���

	_RecordsetPtr	m_pRs;
	//����ǰ�ĵ���¼��ȡ��m_pRs������
	m_pRs = GetRecordSet("SELECT DataByte FROM Structure WHERE RecordID=" + szID);	

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
	m_pRs->GetFields()->GetItem("DataByte")->AppendChunk(varBLOB);
	//�������ݿ�
	m_pRs->Update();
	//�Ͽ������ݿ������
}

//�����ĵ�ID��ȡ�ĵ�����StructureByte�ֶ�ֵ
DWORD CGeoStructure::GetStructureByte(DWORD iStructureID,DWORD nLen,char *pBuf)
{
	CString szID;
	szID.Format("%lu",iStructureID);
	_RecordsetPtr  m_pRs= GetRecordSet("SELECT DataByte FROM Structure WHERE RecordID=" + szID);
	//��ȡDataByte�ֶε�ʵ�ʴ�С
	DWORD Size = m_pRs->GetFields()->GetItem("DataByte")->ActualSize;		
	BSTR HUGEP *Buffer=NULL;
	if(Size>0)
	{		
		//��ͼ���ֶ��ж�ȡ���ݵ�varBLOB��
		_variant_t varBLOB = m_pRs->GetFields()->GetItem("DataByte")->GetChunk(Size);		
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

//���ݲ⾮��ĿIDɾ������Ŀ�е����гɹ��ĵ�
BOOL CGeoStructure::DeleteProject(DWORD iProjectID)
{
	CString cID;
	cID.Format("%lu",iProjectID);
	return Execute( "DELETE FROM Structure WHERE UpperID=" + cID);	
}

//�����ĵ�IDɾ�����ĵ��������Ϣ
BOOL CGeoStructure::sql_delete(DWORD iStructureID)
{
	CString cID;
	cID.Format("%lu",iStructureID);
	return Execute( "DELETE FROM Structure WHERE RecordID=" + cID);	
}

//���ݱ�Ŷ�ȡ�����ֶ�ֵ
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

//���������洢�ձ�Ŷ�ȡ����Ϣ�����ֶ�ֵ
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
