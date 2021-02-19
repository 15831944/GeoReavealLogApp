/*********************************************************************/

#include "stdafx.h"
#include "GeoProject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGeoProject::CGeoProject()
{
	m_RecordID = 0;
	m_ProjectName= _T("ZH");	//项目名称
	m_ProjectAlias= _T("");		//项目别名
	m_LogDate= _T("");  		//测井日期
	m_LogStDep=0.f;  			//测井起始深度
	m_LogEnDep=0.f;  			//测井结束深度
	m_DepRatio=200.f;			//深度比例
	m_LogDevice= _T("");  	    //测井设备
	m_LogTeam= _T("");  		//测井小队
	m_LogCompany= _T("");  	    //测井公司
	m_LogLeader= _T("");  	    //测井小队长
	m_LogEngineer= _T("");  	//测井工程师
	m_CasingNote= _T("");  	    //套管描述
	m_BitNote= _T("");  		//钻头描述
	m_FluidType= _T("");		//泥浆类型
	m_FluidDen=-999.25f;		//泥浆密度
	m_FluidVis=-999.25f;		//泥浆粘度
	m_FluidRm=-999.25f;			//泥浆电阻率
	m_FluidTem=-999.25f;		//泥浆温度
	m_BottomDep=0.f;			//井底深度
	m_BottomTem=-999.25f;		//井底温度
	m_DeviceNote= _T("");  	    //仪器编号
	m_AuditUser= _T("");		//审核人
	m_AuditDate= _T("");		//审核日期
	m_FillUser= _T("");			//提交人
	m_FillDate= _T("");			//提交日期
	m_Describe= _T("");			//解释描述
	m_UpperID = 0;				//存储空间编号
}

CGeoProject::~CGeoProject()
{
	Disconnect();
}

CString CGeoProject::GetDescribe()
{
	return m_Describe;
}

void CGeoProject::SetDescribe(CString cDescribe)
{
	m_Describe = cDescribe;
}

CString CGeoProject::GetFillUser()
{
	return m_FillUser;
}

void CGeoProject::SetFillUser(CString cUser)
{
	m_FillUser = cUser;
}

CString CGeoProject::GetFillDate()
{
	return m_FillDate;
}

void CGeoProject::SetFillDate(CString cDate)
{
	m_FillDate = cDate;
}

DWORD CGeoProject::GetUpperID()
{
	return m_UpperID;
}

void CGeoProject::SetUpperID(DWORD iUpperID)
{
	m_UpperID = iUpperID;
}

DWORD CGeoProject::sql_insert()
{	
	CString sLogStDep;	sLogStDep.Format("%.2f",m_LogStDep);  	
	CString sLogEnDep;	sLogEnDep.Format("%.2f",m_LogEnDep);
	CString sDepRatio;	sDepRatio.Format("%.2f",m_DepRatio);
	CString sFluidDen;	sFluidDen.Format("%.3f",m_FluidDen); 		
	CString sFluidVis;	sFluidVis.Format("%.2f",m_FluidVis); 		
	CString sFluidRm;	sFluidRm.Format("%.2f",m_FluidRm); 		
	CString sFluidTem;	sFluidTem.Format("%.2f",m_FluidTem); 		
	CString sBottomDep;	sBottomDep.Format("%.2f",m_BottomDep); 		
	CString sBottomTem;	sBottomTem.Format("%.2f",m_BottomTem); 	
	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);

	CString vSQL;
	vSQL = "INSERT INTO Project(ProjectName,	\
									ProjectAlias,	\
									LogDate, 		\
									LogStDep,		\
									LogEnDep,  		\
									DepRatio,  		\
									LogDevice,		\
									LogTeam,  		\
									LogCompany, 	\
									LogLeader,  	\
									LogEngineer,	\
									CasingNote,		\
									BitNote,  		\
									FluidType,		\
									FluidDen,		\
									FluidVis,		\
									FluidRm,		\
									FluidTem,		\
									BottomDep,		\
									BottomTem,		\
									DeviceNote,		\
									AuditUser,		\
									AuditDate,		\
									Describe,		\
									FillUser,		\
									FillDate,		\
									UpperID)		\
		VALUES('"   + m_ProjectName		+ "','"
					+ m_ProjectAlias	+ "','" 
					+ m_LogDate 		+ "','"
					+ sLogStDep			+ "','"
					+ sLogEnDep  		+ "','"
					+ sDepRatio  		+ "','"
					+ m_LogDevice		+ "','"
					+ m_LogTeam  		+ "','"
					+ m_LogCompany 		+ "','"
					+ m_LogLeader  		+ "','"
					+ m_LogEngineer		+ "','"
					+ m_CasingNote		+ "','"
					+ m_BitNote  		+ "','"
					+ m_FluidType		+ "','"
					+ sFluidDen			+ "','"
					+ sFluidVis			+ "','"
					+ sFluidRm			+ "','"
					+ sFluidTem			+ "','"
					+ sBottomDep		+ "','"
					+ sBottomTem		+ "','"
					+ m_DeviceNote		+ "','"					
					+ m_AuditUser		+ "','"
					+ m_AuditDate		+ "','"
					+ m_Describe		+ "','"
					+ m_FillUser		+ "','"		
					+ m_FillDate		+ "','"		
					+ sUpperID		+ "')";
	if(!Execute(vSQL))
		return 0;
	return GetMaxRecordID("Project");
}

BOOL CGeoProject::sql_update(DWORD iProjectID)
{	
	CString sLogStDep;	sLogStDep.Format("%.2f",m_LogStDep);  	
	CString sLogEnDep;	sLogEnDep.Format("%.2f",m_LogEnDep);
	CString sDepRatio;	sDepRatio.Format("%.2f",m_DepRatio);
	CString sFluidDen;	sFluidDen.Format("%.3f",m_FluidDen); 		
	CString sFluidVis;	sFluidVis.Format("%.2f",m_FluidVis); 		
	CString sFluidRm;	sFluidRm.Format("%.2f",m_FluidRm); 		
	CString sFluidTem;	sFluidTem.Format("%.2f",m_FluidTem); 		
	CString sBottomDep;	sBottomDep.Format("%.2f",m_BottomDep); 		
	CString sBottomTem;	sBottomTem.Format("%.2f",m_BottomTem); 	
	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);			

	CString cID;
	cID.Format("%lu",iProjectID);
	CString str("UPDATE Project SET	  ProjectName='"	+		m_ProjectName		//项目名称
									+ "', ProjectAlias='"	+		m_ProjectAlias		//项目别名
									+ "', LogDate='"  		+		m_LogDate 			//测井日期
									+ "', LogStDep='"  		+		sLogStDep			//测井起始深度
									+ "', LogEnDep='"  		+		sLogEnDep  			//测井结束深度
									+ "', DepRatio='"  		+		sDepRatio  			//深度比例
									+ "', LogDevice='"  	+		m_LogDevice			//测井设备
									+ "', LogTeam='"  		+		m_LogTeam  			//测井小队
									+ "', LogCompany='"  	+		m_LogCompany 		//测井公司
									+ "', LogLeader='"  	+		m_LogLeader  		//测井小队长
									+ "', LogEngineer='" 	+		m_LogEngineer		//测井工程师
									+ "', CasingNote='"  	+		m_CasingNote		//套管描述
									+ "', BitNote='"  		+		m_BitNote  			//钻头描述
									+ "', FluidType='"		+		m_FluidType			//泥浆类型
									+ "', FluidDen='"		+		sFluidDen			//泥浆密度
									+ "', FluidVis='"		+		sFluidVis			//泥浆粘度
									+ "', FluidRm='"		+		sFluidRm			//泥浆电阻率
									+ "', FluidTem='"		+		sFluidTem			//泥浆温度
									+ "', BottomDep='"		+		sBottomDep			//井底深度
									+ "', BottomTem='"		+		sBottomTem			//井底温度
									+ "', DeviceNote='"  	+		m_DeviceNote		//仪器编号			
									+ "', AuditUser='"		+		m_Describe			//审核人
									+ "', AuditDate='"		+		m_FillUser			//审核日期
									+ "', Describe='"		+ 		m_FillDate			//项目描述
									+ "', FillUser='"		+		m_AuditUser			//提交人
									+ "', FillDate='"		+		m_AuditDate			//提交日期
									+ "', UpperID="			+		sUpperID			//所属存储空间编号
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

BOOL CGeoProject::sql_delete(DWORD iProjectID)
{
	CString cID;
	cID.Format("%lu",iProjectID);
	return Execute("DELETE FROM Project WHERE RecordID=" + cID);
}

//根据井信息编号读取所有字段值
BOOL CGeoProject::GetData(DWORD iProjectID)
{
	CString cID;
	cID.Format("%lu",iProjectID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Project WHERE RecordID=" + cID);

	//返回各列的值
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_ProjectName	=pRS->GetCollect("ProjectName");	
		m_ProjectAlias	=pRS->GetCollect("ProjectAlias");	
		m_LogDate		=pRS->GetCollect("LogDate");  		
		m_LogStDep		=(float)atof((_bstr_t)pRS->GetCollect("LogStDep"));  	
		m_LogEnDep		=(float)atof((_bstr_t)pRS->GetCollect("LogEnDep"));
		m_DepRatio		=(float)atof((_bstr_t)pRS->GetCollect("DepRatio"));
		m_LogDevice		=pRS->GetCollect("LogDevice");  	
		m_LogTeam		=pRS->GetCollect("LogTeam");  		
		m_LogCompany	=pRS->GetCollect("LogCompany");  	
		m_LogLeader		=pRS->GetCollect("LogLeader");  	
		m_LogEngineer	=pRS->GetCollect("LogEngineer");  	
		m_CasingNote	=pRS->GetCollect("CasingNote");  	
		m_BitNote		=pRS->GetCollect("BitNote");  		
		m_FluidType		=pRS->GetCollect("FluidType");		
		m_FluidDen		=(float)atof((_bstr_t)pRS->GetCollect("FluidDen"));		
		m_FluidVis		=(float)atof((_bstr_t)pRS->GetCollect("FluidVis"));		
		m_FluidRm		=(float)atof((_bstr_t)pRS->GetCollect("FluidRm"));		
		m_FluidTem		=(float)atof((_bstr_t)pRS->GetCollect("FluidTem"));		
		m_BottomDep		=(float)atof((_bstr_t)pRS->GetCollect("BottomDep"));		
		m_BottomTem		=(float)atof((_bstr_t)pRS->GetCollect("BottomTem"));		
		m_DeviceNote	=pRS->GetCollect("DeviceNote");  	
		m_AuditUser		=pRS->GetCollect("AuditUser");		
		m_AuditDate		=pRS->GetCollect("AuditDate");		
		m_FillUser		=pRS->GetCollect("FillUser");
		m_FillDate		=pRS->GetCollect("FillDate");
		m_Describe		=pRS->GetCollect("Describe");
		m_UpperID		=atol((_bstr_t)pRS->GetCollect("UpperID"));
		return TRUE;
	}
}
//根据所属存储空编号读取井信息所有字段值
BOOL CGeoProject::GetDataReferToUpperID(DWORD iUpperID)
{
	CString cID;
	cID.Format("%lu",iUpperID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Project WHERE UpperID=" + cID);

	//返回各列的值
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_RecordID		=atol((_bstr_t)pRS->GetCollect("RecordID"));
		m_ProjectName	=pRS->GetCollect("ProjectName");	
		m_ProjectAlias	=pRS->GetCollect("ProjectAlias");	
		m_LogDate		=pRS->GetCollect("LogDate");  		
		m_LogStDep		=(float)atof((_bstr_t)pRS->GetCollect("LogStDep"));  	
		m_LogEnDep		=(float)atof((_bstr_t)pRS->GetCollect("LogEnDep"));
		m_DepRatio		=(float)atof((_bstr_t)pRS->GetCollect("DepRatio"));
		m_LogDevice		=pRS->GetCollect("LogDevice");  	
		m_LogTeam		=pRS->GetCollect("LogTeam");  		
		m_LogCompany	=pRS->GetCollect("LogCompany");  	
		m_LogLeader		=pRS->GetCollect("LogLeader");  	
		m_LogEngineer	=pRS->GetCollect("LogEngineer");  	
		m_CasingNote	=pRS->GetCollect("CasingNote");  	
		m_BitNote		=pRS->GetCollect("BitNote");  		
		m_FluidType		=pRS->GetCollect("FluidType");		
		m_FluidDen		=(float)atof((_bstr_t)pRS->GetCollect("FluidDen"));		
		m_FluidVis		=(float)atof((_bstr_t)pRS->GetCollect("FluidVis"));		
		m_FluidRm		=(float)atof((_bstr_t)pRS->GetCollect("FluidRm"));		
		m_FluidTem		=(float)atof((_bstr_t)pRS->GetCollect("FluidTem"));		
		m_BottomDep		=(float)atof((_bstr_t)pRS->GetCollect("BottomDep"));		
		m_BottomTem		=(float)atof((_bstr_t)pRS->GetCollect("BottomTem"));		
		m_DeviceNote	=pRS->GetCollect("DeviceNote");  	
		m_AuditUser		=pRS->GetCollect("AuditUser");		
		m_AuditDate		=pRS->GetCollect("AuditDate");		
		m_FillUser		=pRS->GetCollect("FillUser");
		m_FillDate		=pRS->GetCollect("FillDate");
		m_Describe		=pRS->GetCollect("Describe");
		m_UpperID		=atol((_bstr_t)pRS->GetCollect("UpperID"));
		return TRUE;
	}
}