/*********************************************************************/

#include "stdafx.h"
#include "GeoOilTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGeoOilTest::CGeoOilTest()
{
	m_RecordID = 0;
	m_LayerName= _T("");		//��λ����
	m_LayerAlias= _T("");		//��λ����

	m_TestMethod= _T("");		//���ͷ�ʽ	
	m_OutMethod= _T("");		//������ʽ
	m_Conclusion= _T("");		//���ͽ���
	m_TestDate= _T("");			//��������
	m_Reference= _T("");		//��������
	m_ReferDate= _T("");		//��������

	m_DepSt=-999.25f;  			//��ʼ���				
	m_DepEn=-999.25f;  			//�������								
	m_PorePlate=-999.25f;		//�װ�
	m_Glib=-999.25f;			//����
	m_CasingPres=-999.25f;		//��ѹ
	m_OilPres=-999.25f;			//��ѹ
	m_FlowPres=-999.25f;		//��ѹ
	m_TopPres=-999.25f;			//��ѹ
	m_TopTemp=-999.25f;			//����
	m_ClosePres=-999.25f;		//�ؾ�ѹ��
	m_LayerPres=-999.25f;		//�ز�ѹ��
	m_OilOut=-999.25f;			//�ղ���
	m_GasOut=-999.25f;			//�ղ���
	m_WaterOut=-999.25f;		//�ղ�ˮ

	m_Describe= _T("");			//��������
	m_FillUser= _T("");			//�ύ��
	m_FillDate= _T("");			//�ύ����
	m_UpperID = 0;				//�洢�ռ���

	a_OpenDepSt.SetSize(100);
	a_OpenDepEn.SetSize(100);
}

CGeoOilTest::~CGeoOilTest()
{
	Disconnect();
}

CString CGeoOilTest::GetDescribe()
{
	return m_Describe;
}

void CGeoOilTest::SetDescribe(CString cDescribe)
{
	m_Describe = cDescribe;
}

CString CGeoOilTest::GetFillUser()
{
	return m_FillUser;
}

void CGeoOilTest::SetFillUser(CString cUser)
{
	m_FillUser = cUser;
}

CString CGeoOilTest::GetFillDate()
{
	return m_FillDate;
}

void CGeoOilTest::SetFillDate(CString cDate)
{
	m_FillDate = cDate;
}

DWORD CGeoOilTest::GetUpperID()
{
	return m_UpperID;
}

void CGeoOilTest::SetUpperID(DWORD iUpperID)
{
	m_UpperID = iUpperID;
}

DWORD CGeoOilTest::sql_insert()
{	
	CString sDepSt;			sDepSt.Format("%.2f",  		m_DepSt);  			//��ʼ���	
	CString sDepEn;			sDepEn.Format("%.2f",  		m_DepEn);  			//�������	
	CString sPorePlate;		sPorePlate.Format("%.2f",	m_PorePlate);		//�װ�
	CString sGlib;			sGlib.Format("%.2f",		m_Glib);			//����
	CString sCasingPres;	sCasingPres.Format("%.2f",	m_CasingPres);		//��ѹ
	CString sOilPres;		sOilPres.Format("%.2f",		m_OilPres);			//��ѹ
	CString sFlowPres;		sFlowPres.Format("%.2f",	m_FlowPres);		//��ѹ
	CString sTopPres;		sTopPres.Format("%.2f",		m_TopPres);			//��ѹ
	CString sTopTemp;		sTopTemp.Format("%.2f",		m_TopTemp);			//����
	CString sClosePres;		sClosePres.Format("%.2f",	m_ClosePres);		//�ؾ�ѹ��
	CString sLayerPres;		sLayerPres.Format("%.2f",	m_LayerPres);		//�ز�ѹ��
	CString sOilOut;		sOilOut.Format("%.2f",		m_OilOut);			//�ղ���
	CString sGasOut;		sGasOut.Format("%.2f",		m_GasOut);			//�ղ���
	CString sWaterOut;		sWaterOut.Format("%.2f",	m_WaterOut);		//�ղ�ˮ	
			
	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);
	CString vSQL;
	vSQL = "INSERT INTO OilTest( LayerName,		\
									LayerAlias,		\
									TestMethod,		\
									OutMethod,		\
									Conclusion,		\
									TestDate,		\
									Reference,		\
									ReferDate,		\
									DepSt,  		\
									DepEn,  		\
									PorePlate,		\
									Glib,			\
									CasingPres,		\
									OilPres,		\
									FlowPres,		\
									TopPres,		\
									TopTemp,		\
									ClosePres,		\
									LayerPres,		\
									OilOut,			\
									GasOut,			\
									WaterOut,		\
									Describe,		\
									FillUser,		\
									FillDate,		\
									UpperID)		\
		VALUES('"   + m_LayerName	+ "','"
					+ m_LayerAlias	+ "','"
					+ m_TestMethod	+ "','"
					+ m_OutMethod	+ "','"
					+ m_Conclusion	+ "','"
					+ m_TestDate	+ "','"
					+ m_Reference	+ "','"
					+ m_ReferDate	+ "','"
					+ sDepSt		+ "','"
					+ sDepEn		+ "','"
					+ sPorePlate	+ "','"
					+ sGlib			+ "','"
					+ sCasingPres	+ "','"
					+ sOilPres		+ "','"
					+ sFlowPres		+ "','"
					+ sTopPres		+ "','"
					+ sTopTemp		+ "','"
					+ sClosePres	+ "','"
					+ sLayerPres	+ "','"
					+ sOilOut		+ "','"
					+ sGasOut		+ "','"
					+ sWaterOut		+ "','"
					+ m_Describe 	+ "','"
					+ m_FillUser	+ "','"
					+ m_FillDate	+ "','"
					+ sUpperID		+ "')";
	if(!Execute(vSQL))
		return 0;
	return GetMaxRecordID("OilTest");
}

BOOL CGeoOilTest::sql_update(DWORD iOilTestID)
{	
	CString    sDepSt;			sDepSt.Format("%.2f",  		m_DepSt);  		//��ʼ���	
	CString    sDepEn;			sDepEn.Format("%.2f",  		m_DepEn);  		//�������	
	CString    sPorePlate;		sPorePlate.Format("%.2f",	m_PorePlate);	//�װ�		
	CString    sGlib;			sGlib.Format("%.2f",		m_Glib);		//����			
	CString    sCasingPres;		sCasingPres.Format("%.2f",	m_CasingPres);	//��ѹ		
	CString    sOilPres;		sOilPres.Format("%.2f",		m_OilPres);		//��ѹ		
	CString    sFlowPres;		sFlowPres.Format("%.2f",	m_FlowPres);	//��ѹ			
	CString    sTopPres;		sTopPres.Format("%.2f",		m_TopPres);		//��ѹ		
	CString    sTopTemp;		sTopTemp.Format("%.2f",		m_TopTemp);		//����		
	CString    sClosePres;		sClosePres.Format("%.2f",	m_ClosePres);	//�ؾ�ѹ��	
	CString    sLayerPres;		sLayerPres.Format("%.2f",	m_LayerPres);	//�ز�ѹ��	
	CString    sOilOut;			sOilOut.Format("%.2f",		m_OilOut);		//�ղ���	
	CString    sGasOut;			sGasOut.Format("%.2f",		m_GasOut);		//�ղ���	
	CString    sWaterOut;		sWaterOut.Format("%.2f",	m_WaterOut);	//�ղ�ˮ		

	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);
	CString cID;
	cID.Format("%lu",iOilTestID);
	CString str("UPDATE OilTest SET	  LayerName='"		+		m_LayerName				//��λ����   
									+ "', LayerAlias='"		+		m_LayerAlias		//��λ����	
									+ "', TestMethod='"  	+		m_TestMethod		//���ͷ�ʽ
									+ "', OutMethod='"  	+		m_OutMethod			//������ʽ
									+ "', Conclusion='"  	+		m_Conclusion		//���ͽ���
									+ "', TestDate='"  		+		m_TestDate			//��������
									+ "', Reference='"  	+		m_Reference			//��������
									+ "', ReferDate='"  	+		m_ReferDate			//��������
									+ "', DepSt='"  		+		sDepSt		  		//��ʼ���	
									+ "', DepEn='"  		+		sDepEn		  		//�������	
									+ "', PorePlate='"  	+		sPorePlate			//�װ�
									+ "', Glib='"  			+		sGlib				//����	
									+ "', CasingPres='"  	+		sCasingPres			//��ѹ
									+ "', OilPres='"  		+		sOilPres			//��ѹ
									+ "', FlowPres='"  		+		sFlowPres			//��ѹ
									+ "', TopPres='"  		+		sTopPres			//��ѹ
									+ "', TopTemp='"  		+		sTopTemp			//����
									+ "', ClosePres='"  	+		sClosePres			//�ؾ�ѹ��
									+ "', LayerPres='"  	+		sLayerPres			//�ز�ѹ��
									+ "', OilOut='"  		+		sOilOut				//�ղ���
									+ "', GasOut='"  		+		sGasOut				//�ղ���
									+ "', WaterOut='"  		+		sWaterOut			//�ղ�ˮ	
									+ "', Describe='"		+		m_Describe 			//��������
									+ "', FillUser='"		+		m_FillUser			//�ύ��
									+ "', FillDate='"		+		m_FillDate			//�ύ����
									+ "', UpperID="			+		sUpperID			//�洢�ռ���
				+ "' WHERE RecordID=" + cID );
	return Execute(str);
}

BOOL CGeoOilTest::UpdateOpenDep(DWORD iOilTestID)
{
	if(DeleteOpenDep(iOilTestID))
	{
		InsertOpenDep(iOilTestID);
		return TRUE;
	}
	return FALSE;
}

BOOL CGeoOilTest::sql_delete(DWORD iOilTestID)
{
	CString cID;
	cID.Format("%lu",iOilTestID);
	//ɾ����׾���
	if(DeleteOpenDep(iOilTestID))
		return Execute("DELETE FROM OilTest WHERE RecordID=" + cID);
	else 
		return FALSE;
}

//����������Ϣ��Ŷ�ȡ�����ֶ�ֵ
BOOL CGeoOilTest::GetData(DWORD iOilTestID)
{
	CString cID;
	cID.Format("%lu",iOilTestID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM OilTest WHERE RecordID=" + cID);

	//���ظ��е�ֵ
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_RecordID=iOilTestID;
		m_LayerName		=pRS->GetCollect("LayerName");	
		m_LayerAlias	=pRS->GetCollect("LayerAlias");
		m_TestMethod	=pRS->GetCollect("m_TestMethod");	
		m_OutMethod		=pRS->GetCollect("m_OutMethod");
		m_Conclusion	=pRS->GetCollect("m_Conclusion");	
		m_TestDate		=pRS->GetCollect("m_TestDate");	
		m_Reference		=pRS->GetCollect("m_Reference");	
		m_ReferDate		=pRS->GetCollect("m_ReferDate");	
		m_DepSt			=(float)atof((_bstr_t)pRS->GetCollect("DepSt"));  	  		
		m_DepEn			=(float)atof((_bstr_t)pRS->GetCollect("DepEn"));  	  		
		m_PorePlate		=(float)atof((_bstr_t)pRS->GetCollect("PorePlate"));		
		m_Glib			=(float)atof((_bstr_t)pRS->GetCollect("Glib"));				
		m_CasingPres	=(float)atof((_bstr_t)pRS->GetCollect("CasingPres"));		
		m_OilPres		=(float)atof((_bstr_t)pRS->GetCollect("OilPres"));			
		m_FlowPres		=(float)atof((_bstr_t)pRS->GetCollect("FlowPres"));		
		m_TopPres		=(float)atof((_bstr_t)pRS->GetCollect("TopPres"));			
		m_TopTemp		=(float)atof((_bstr_t)pRS->GetCollect("TopTemp"));			
		m_ClosePres		=(float)atof((_bstr_t)pRS->GetCollect("ClosePres"));		
		m_LayerPres		=(float)atof((_bstr_t)pRS->GetCollect("LayerPres"));		
		m_OilOut		=(float)atof((_bstr_t)pRS->GetCollect("OilOut"));				
		m_GasOut		=(float)atof((_bstr_t)pRS->GetCollect("GasOut"));				
		m_WaterOut		=(float)atof((_bstr_t)pRS->GetCollect("WaterOut"));		
		m_Describe		=pRS->GetCollect("Describe");
		m_FillUser		=pRS->GetCollect("FillUser");
		m_FillDate		=pRS->GetCollect("FillDate");
		m_UpperID		=atol((_bstr_t)pRS->GetCollect("UpperID"));
		//��ʼ������
		a_OpenDepSt.RemoveAll();
		a_OpenDepEn.RemoveAll();
		GetOpenDep(iOilTestID);
		return TRUE;
	}
}
//����������Ϣ��Ŷ�ȡ������׾���
BOOL CGeoOilTest::GetOpenDep(DWORD iOilTestID)
{
	CString cID;
	cID.Format("%lu",iOilTestID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM OilTestOpenDep WHERE UpperID=" + cID);
	float dep;
	while(pRS->adoEOF == 0)
	{	
		dep=(float)atof((_bstr_t)pRS->GetCollect("OpenDepSt"));
		a_OpenDepSt.Add(dep);
		dep=(float)atof((_bstr_t)pRS->GetCollect("OpenDEpEn"));
		a_OpenDepEn.Add(dep);		
		pRS->MoveNext();
	}
	return TRUE;
}
//����������Ϣ���������׾���
DWORD CGeoOilTest::InsertOpenDep(DWORD iOilTestID)
{
	int Num=a_OpenDepSt.GetCount();
	if(Num<1) return 0;

	CString sDepSt,sDepEn,sUpperID;
	sUpperID.Format("%lu",iOilTestID);
	CString vSQL;
	for(int i=0;i<Num;i++)
	{
		sDepSt.Format("%.2f",  		a_OpenDepSt.GetAt(i));  			//��ʼ���	
		sDepEn.Format("%.2f",  		a_OpenDepEn.GetAt(i));  			//�������	
		vSQL = "INSERT INTO OilTestOpenDep( OpenDepSt,	\
											OpenDepEn,	\
											UpperID)	\
		VALUES('"   + sDepSt	+ "','"
					+ sDepEn	+ "','"
					+ sUpperID	+ "')";
		Execute(vSQL);
	}
	return GetMaxRecordID("OilTestOpenDep");
}
//����������Ϣ���ɾ��������׾���
BOOL CGeoOilTest::DeleteOpenDep(DWORD iOilTestID)
{
	CString cID;
	cID.Format("%lu",iOilTestID);
	//ɾ����׾���
	return Execute("DELETE FROM OilTestOpenDep WHERE UpperID=" + cID);
}