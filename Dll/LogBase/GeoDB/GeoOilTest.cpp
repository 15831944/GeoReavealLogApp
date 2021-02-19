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
	m_LayerName= _T("");		//层位名称
	m_LayerAlias= _T("");		//层位别名

	m_TestMethod= _T("");		//试油方式	
	m_OutMethod= _T("");		//产出方式
	m_Conclusion= _T("");		//试油结论
	m_TestDate= _T("");			//试油日期
	m_Reference= _T("");		//资料依据
	m_ReferDate= _T("");		//资料日期

	m_DepSt=-999.25f;  			//起始深度				
	m_DepEn=-999.25f;  			//结束深度								
	m_PorePlate=-999.25f;		//孔板
	m_Glib=-999.25f;			//油嘴
	m_CasingPres=-999.25f;		//套压
	m_OilPres=-999.25f;			//油压
	m_FlowPres=-999.25f;		//流压
	m_TopPres=-999.25f;			//上压
	m_TopTemp=-999.25f;			//上温
	m_ClosePres=-999.25f;		//关井压力
	m_LayerPres=-999.25f;		//地层压力
	m_OilOut=-999.25f;			//日产油
	m_GasOut=-999.25f;			//日产气
	m_WaterOut=-999.25f;		//日产水

	m_Describe= _T("");			//试油描述
	m_FillUser= _T("");			//提交人
	m_FillDate= _T("");			//提交日期
	m_UpperID = 0;				//存储空间编号

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
	CString sDepSt;			sDepSt.Format("%.2f",  		m_DepSt);  			//起始深度	
	CString sDepEn;			sDepEn.Format("%.2f",  		m_DepEn);  			//结束深度	
	CString sPorePlate;		sPorePlate.Format("%.2f",	m_PorePlate);		//孔板
	CString sGlib;			sGlib.Format("%.2f",		m_Glib);			//油嘴
	CString sCasingPres;	sCasingPres.Format("%.2f",	m_CasingPres);		//套压
	CString sOilPres;		sOilPres.Format("%.2f",		m_OilPres);			//油压
	CString sFlowPres;		sFlowPres.Format("%.2f",	m_FlowPres);		//流压
	CString sTopPres;		sTopPres.Format("%.2f",		m_TopPres);			//上压
	CString sTopTemp;		sTopTemp.Format("%.2f",		m_TopTemp);			//上温
	CString sClosePres;		sClosePres.Format("%.2f",	m_ClosePres);		//关井压力
	CString sLayerPres;		sLayerPres.Format("%.2f",	m_LayerPres);		//地层压力
	CString sOilOut;		sOilOut.Format("%.2f",		m_OilOut);			//日产油
	CString sGasOut;		sGasOut.Format("%.2f",		m_GasOut);			//日产气
	CString sWaterOut;		sWaterOut.Format("%.2f",	m_WaterOut);		//日产水	
			
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
	CString    sDepSt;			sDepSt.Format("%.2f",  		m_DepSt);  		//起始深度	
	CString    sDepEn;			sDepEn.Format("%.2f",  		m_DepEn);  		//结束深度	
	CString    sPorePlate;		sPorePlate.Format("%.2f",	m_PorePlate);	//孔板		
	CString    sGlib;			sGlib.Format("%.2f",		m_Glib);		//油嘴			
	CString    sCasingPres;		sCasingPres.Format("%.2f",	m_CasingPres);	//套压		
	CString    sOilPres;		sOilPres.Format("%.2f",		m_OilPres);		//油压		
	CString    sFlowPres;		sFlowPres.Format("%.2f",	m_FlowPres);	//流压			
	CString    sTopPres;		sTopPres.Format("%.2f",		m_TopPres);		//上压		
	CString    sTopTemp;		sTopTemp.Format("%.2f",		m_TopTemp);		//上温		
	CString    sClosePres;		sClosePres.Format("%.2f",	m_ClosePres);	//关井压力	
	CString    sLayerPres;		sLayerPres.Format("%.2f",	m_LayerPres);	//地层压力	
	CString    sOilOut;			sOilOut.Format("%.2f",		m_OilOut);		//日产油	
	CString    sGasOut;			sGasOut.Format("%.2f",		m_GasOut);		//日产气	
	CString    sWaterOut;		sWaterOut.Format("%.2f",	m_WaterOut);	//日产水		

	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);
	CString cID;
	cID.Format("%lu",iOilTestID);
	CString str("UPDATE OilTest SET	  LayerName='"		+		m_LayerName				//层位名称   
									+ "', LayerAlias='"		+		m_LayerAlias		//层位别名	
									+ "', TestMethod='"  	+		m_TestMethod		//试油方式
									+ "', OutMethod='"  	+		m_OutMethod			//产出方式
									+ "', Conclusion='"  	+		m_Conclusion		//试油结论
									+ "', TestDate='"  		+		m_TestDate			//试油日期
									+ "', Reference='"  	+		m_Reference			//资料依据
									+ "', ReferDate='"  	+		m_ReferDate			//资料日期
									+ "', DepSt='"  		+		sDepSt		  		//起始深度	
									+ "', DepEn='"  		+		sDepEn		  		//结束深度	
									+ "', PorePlate='"  	+		sPorePlate			//孔板
									+ "', Glib='"  			+		sGlib				//油嘴	
									+ "', CasingPres='"  	+		sCasingPres			//套压
									+ "', OilPres='"  		+		sOilPres			//油压
									+ "', FlowPres='"  		+		sFlowPres			//流压
									+ "', TopPres='"  		+		sTopPres			//上压
									+ "', TopTemp='"  		+		sTopTemp			//上温
									+ "', ClosePres='"  	+		sClosePres			//关井压力
									+ "', LayerPres='"  	+		sLayerPres			//地层压力
									+ "', OilOut='"  		+		sOilOut				//日产油
									+ "', GasOut='"  		+		sGasOut				//日产气
									+ "', WaterOut='"  		+		sWaterOut			//日产水	
									+ "', Describe='"		+		m_Describe 			//试油描述
									+ "', FillUser='"		+		m_FillUser			//提交人
									+ "', FillDate='"		+		m_FillDate			//提交日期
									+ "', UpperID="			+		sUpperID			//存储空间编号
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
	//删除射孔井段
	if(DeleteOpenDep(iOilTestID))
		return Execute("DELETE FROM OilTest WHERE RecordID=" + cID);
	else 
		return FALSE;
}

//根据试油信息编号读取所有字段值
BOOL CGeoOilTest::GetData(DWORD iOilTestID)
{
	CString cID;
	cID.Format("%lu",iOilTestID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM OilTest WHERE RecordID=" + cID);

	//返回各列的值
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
		//初始化数组
		a_OpenDepSt.RemoveAll();
		a_OpenDepEn.RemoveAll();
		GetOpenDep(iOilTestID);
		return TRUE;
	}
}
//根据试油信息编号读取所有射孔井段
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
//根据试油信息编号增加射孔井段
DWORD CGeoOilTest::InsertOpenDep(DWORD iOilTestID)
{
	int Num=a_OpenDepSt.GetCount();
	if(Num<1) return 0;

	CString sDepSt,sDepEn,sUpperID;
	sUpperID.Format("%lu",iOilTestID);
	CString vSQL;
	for(int i=0;i<Num;i++)
	{
		sDepSt.Format("%.2f",  		a_OpenDepSt.GetAt(i));  			//起始深度	
		sDepEn.Format("%.2f",  		a_OpenDepEn.GetAt(i));  			//结束深度	
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
//根据试油信息编号删除所有射孔井段
BOOL CGeoOilTest::DeleteOpenDep(DWORD iOilTestID)
{
	CString cID;
	cID.Format("%lu",iOilTestID);
	//删除射孔井段
	return Execute("DELETE FROM OilTestOpenDep WHERE UpperID=" + cID);
}