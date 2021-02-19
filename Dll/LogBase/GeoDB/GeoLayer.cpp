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
	m_LayerName= _T("");			//��λ����
	m_LayerAlias= _T("");			//��λ����

	m_DepLog=-999.25f;  			//�⾮���
	m_TVLog=-999.25f;  				//�⾮����
	m_DepGeo=-999.25f;				//�������
	m_TVGeo=-999.25f;				//���ʴ���
	m_RemOption= _T("ʵ�ߵ׽�");	//��עѡ��               ʵ�ߵ׽� ���ߵ׽� �ϲ��ʶ ��ֱ��λ ˮƽ��λ	
	m_Describe= _T("");			//��������
	m_FillUser= _T("");			//�ύ��
	m_FillDate= _T("");			//�ύ����
	m_UpperID = 0;				//�洢�ռ���
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
	CString str("UPDATE Layer SET	  LayerName='"		+		m_LayerName				 //��λ����   
									+ "', LayerAlias='"		+		m_LayerAlias		 //��λ����	
									+ "', DepLog='"  		+		sDepLog   			 //�⾮���
									+ "', TVLog='"  		+		sTVLog  			 //�⾮����
									+ "', DepGeo='"			+		sDepGeo				 //�������
									+ "', TVGeo='"			+		sTVGeo				 //���ʴ���
									+ "', RemOption='"		+		m_RemOption			 //��עѡ��					
									+ "', Describe='"		+		m_Describe 			 //��λ����
									+ "', FillUser='"		+		m_FillUser			 //�ύ��
									+ "', FillDate='"		+		m_FillDate			 //�ύ����
									+ "', UpperID="			+		sUpperID			 //�洢�ռ���
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

BOOL CGeoLayer::sql_delete(DWORD iLayerID)
{
	CString cID;
	cID.Format("%d",iLayerID);
	return Execute("DELETE FROM Layer WHERE RecordID=" + cID);
}

//���ݲ�λ��Ϣ��Ŷ�ȡ�����ֶ�ֵ
BOOL CGeoLayer::GetData(DWORD iLayerID)
{
	CString cID;
	cID.Format("%d",iLayerID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Layer WHERE RecordID=" + cID);

	//���ظ��е�ֵ
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
