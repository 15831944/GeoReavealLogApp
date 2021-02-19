/*********************************************************************/

#include "stdafx.h"
#include "GeoWell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGeoWell::CGeoWell()
{
	m_RecordID = 0;
	m_WellName=_T("");	        //井名称               
	m_Alias=_T("");				//井别名
	m_Structure=_T("");			//所属构造
	m_Oilfield=_T("");			//所属油田
	m_Owner=_T("");				//业主单位
	m_Region=_T("");			//地区
	m_Location=_T("");			//构造位置
	m_Line_Location=_T("");	 	//测线位置
	m_ElevationGd=0.f;		 	//地面海拔
	m_Elevation0=0.f;		 	//基准面海拔
	m_DepOffset=0.f;		 	//补心高度
	m_OilOffset=0.f;		 	//油补距
	m_PipeOffset=0.f;		 	//套补距
	m_MagneticV=0.f;		 	//磁偏角
	m_WellDepth=0.f;		 	//完井井深
	m_WellBottom=0.f;		 	//人工井底
	m_StartDate=_T("");		 	//开钻日期
	m_EndDate=_T("");			//完钻日期
	m_WellX=0.f;			 	//井位X坐标
	m_WellY=0.f;			 	//井位Y坐标
	m_Longitude=0.;		 		//井位经度
	m_Latitude=0.;			 	//井位纬度
	m_WellType=_T("");		 	//勘探分类(探井、预探井、评价井、开发井)
	m_WellKind=_T("");		 	//工程分类(直井、斜井、水平井、丛式井)
	m_DrillAim=_T("");			//钻探目的
	m_AimLayer=_T("");			//目标层位
	m_ReservoirType=_T("");		//储层类型(常规油气层、煤层气、页岩气)
	m_FillUser= _T("");			//提交人
	m_FillDate= _T("");			//提交日期
	m_Describe= _T("");			//井描述
	m_UpperID = 0;				//存储空间编号
}

CGeoWell::~CGeoWell()
{
	Disconnect();
}

CString CGeoWell::GetDescribe()
{
	return m_Describe;
}

void CGeoWell::SetDescribe(CString cDescribe)
{
	m_Describe = cDescribe;
}

CString CGeoWell::GetFillUser()
{
	return m_FillUser;
}

void CGeoWell::SetFillUser(CString cUser)
{
	m_FillUser = cUser;
}

CString CGeoWell::GetFillDate()
{
	return m_FillDate;
}

void CGeoWell::SetFillDate(CString cDate)
{
	m_FillDate = cDate;
}

DWORD CGeoWell::GetUpperID()
{
	return m_UpperID;
}

void CGeoWell::SetUpperID(DWORD iUpperID)
{
	m_UpperID = iUpperID;
}

DWORD CGeoWell::sql_insert()
{	
	CString sWellX;			sWellX.Format("%.3f",m_WellX);		
	CString sWellY;			sWellY.Format("%.3f",m_WellY);		
	CString sLongitude;		sLongitude.Format("%f",m_Longitude);	
	CString sLatitude;		sLatitude.Format("%f",m_Latitude);	
	CString sElevationGd;	sElevationGd.Format("%.2f",m_ElevationGd);
	CString sElevation;		sElevation.Format("%.2f",m_Elevation0);	
	CString sDepOffset;		sDepOffset.Format("%.2f",m_DepOffset);		
	CString sOilOffset;		sOilOffset.Format("%.2f",m_OilOffset);		
	CString sPipeOffset;	sPipeOffset.Format("%.2f",m_PipeOffset);	
	CString sMagneticV;		sMagneticV.Format("%.2f",m_MagneticV);		
	CString sWellDepth;		sWellDepth.Format("%.2f",m_WellDepth);		
	CString sWellBottom;	sWellBottom.Format("%.2f",m_WellBottom);				
	CString sUpperID;		sUpperID.Format("%lu",m_UpperID);
	CString vSQL;
	vSQL = "INSERT INTO Well( WellName,			\
									Alias,			\
									Structure,		\
									Oilfield,		\
									Owner,			\
									Region,			\
									Location,		\
									Line_Location,	\
									ElevationGd,	\
									Elevation0,		\
									DepOffset,		\
									OilOffset,		\
									PipeOffset,		\
									MagneticV,		\
									WellDepth,		\
									WellBottom,		\
									StartDate,		\
									EndDate,		\
									WellX,			\
									WellY,			\
									Longitude,		\
									Latitude,		\
									WellType,		\
									WellKind,		\
									DrillAim,		\
									AimLayer,		\
									ReservoirType,	\
									Describe,		\
									FillUser,		\
									FillDate,		\
									UpperID)		\
			VALUES('" + m_WellName      	+ "','"
					+ m_Alias				+ "','" 
					+ m_Structure			+ "','"
					+ m_Oilfield		 	+ "','"
					+ m_Owner				+ "','"
					+ m_Region				+ "','"
					+ m_Location			+ "','"
					+ m_Line_Location		+ "','"
					+ sElevationGd			+ "','"
					+ sElevation			+ "','"
					+ sDepOffset			+ "','"
					+ sOilOffset			+ "','"
					+ sPipeOffset			+ "','"
					+ sMagneticV			+ "','"
					+ sWellDepth			+ "','"
					+ sWellBottom			+ "','"
					+ m_StartDate			+ "','"
					+ m_EndDate				+ "','"
					+ sWellX				+ "','"
					+ sWellY				+ "','"
					+ sLongitude			+ "','"
					+ sLatitude				+ "','"
					+ m_WellType			+ "','"
					+ m_WellKind			+ "','"
					+ m_DrillAim			+ "','"
					+ m_AimLayer			+ "','"
					+ m_ReservoirType		+ "','"
					+ m_Describe 			+ "','"
					+ m_FillUser			+ "','"
					+ m_FillDate			+ "','"
					+ sUpperID		+ "')";
	if(!Execute(vSQL))
		return 0;
	return GetMaxRecordID("Well");
}

BOOL CGeoWell::sql_update(DWORD iWellID)
{	
	CString sWellX;			sWellX.Format("%.3f",m_WellX);		
	CString sWellY;			sWellY.Format("%.3f",m_WellY);		
	CString sLongitude;		sLongitude.Format("%f",m_Longitude);	
	CString sLatitude;		sLatitude.Format("%f",m_Latitude);	

	CString sElevationGd;	sElevationGd.Format("%.2f",m_ElevationGd);
	CString sElevation0;	sElevation0.Format("%.2f",m_Elevation0);	
	CString sDepOffset;		sDepOffset.Format("%.2f",m_DepOffset);		
	CString sOilOffset;		sOilOffset.Format("%.2f",m_OilOffset);		
	CString sPipeOffset;	sPipeOffset.Format("%.2f",m_PipeOffset);	
	CString sMagneticV;		sMagneticV.Format("%.2f",m_MagneticV);		
	CString sWellDepth;		sWellDepth.Format("%.2f",m_WellDepth);		
	CString sWellBottom;	sWellBottom.Format("%.2f",m_WellBottom);				
	CString sUpperID;		sUpperID.Format("%lu",m_UpperID);
	CString cID;
	cID.Format("%lu",iWellID);
	CString str("UPDATE Well SET	  WellName='"		+		m_WellName           //井名称               
									+ "', Alias='"			+		m_Alias				 //井别名
									+ "', Structure='"		+		m_Structure			 //所属构造
									+ "', Oilfield='"		+		m_Oilfield			 //所属油田
									+ "', Owner='"			+		m_Owner				 //业主单位
									+ "', Region='"			+		m_Region			 //地区
									+ "', Location='"		+		m_Location			 //构造位置
									+ "', Line_Location='"	+		m_Line_Location		 //测线位置
									+ "', ElevationGd='"	+		sElevationGd		 //地面海拔
									+ "', Elevation0='"		+		sElevation0			 //基准面海拔
									+ "', DepOffset='"		+		sDepOffset			 //补心高度
									+ "', OilOffset='"		+		sOilOffset			 //油补距
									+ "', PipeOffset='"		+		sPipeOffset			 //套补距
									+ "', MagneticV='"		+		sMagneticV			 //磁偏角
									+ "', WellDepth='"		+		sWellDepth			 //完井井深
									+ "', WellBottom='"		+		sWellBottom			 //人工井底
									+ "', StartDate='"		+		m_StartDate			 //开钻日期
									+ "', EndDate='"		+		m_EndDate			 //完钻日期
									+ "', WellX='"			+		sWellX				 //井位X坐标
									+ "', WellY='"			+		sWellY				 //井位Y坐标
									+ "', Longitude='"		+		sLongitude			 //井位经度
									+ "', Latitude='"		+		sLatitude			 //井位纬度
									+ "', WellType='"		+		m_WellType			 //勘探分类(探井、预探井、评价井、开发井)
									+ "', WellKind='"		+		m_WellKind			 //工程分类(直井、斜井、水平井、丛式井)
									+ "', DrillAim='"		+		m_DrillAim			 //钻探目的
									+ "', AimLayer='"		+		m_AimLayer			 //目标层位
									+ "', ReservoirType='"	+		m_ReservoirType		 //储层类型(常规油气层、煤层气、页岩气)
									+ "', Describe='"		+		m_Describe 			 //井描述
									+ "', FillUser='"		+		m_FillUser			 //提交人
									+ "', FillDate='"		+		m_FillDate			 //提交日期
									+ "', UpperID='"		+		sUpperID			 //存储空间编号
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

BOOL CGeoWell::sql_delete(DWORD iWellID)
{
	CString cID;
	cID.Format("%lu",iWellID);
	return Execute("DELETE FROM Well WHERE RecordID=" + cID);
}

//根据井信息编号读取所有字段值
BOOL CGeoWell::GetData(DWORD iWellID)
{
	CString cID;
	cID.Format("%lu",iWellID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Well WHERE RecordID=" + cID);

	//返回各列的值
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_WellName		=pRS->GetCollect("WellName");		
		m_Alias			=pRS->GetCollect("Alias");			
		m_Structure		=pRS->GetCollect("Structure");		
		m_Oilfield		=pRS->GetCollect("Oilfield");		
		m_Owner			=pRS->GetCollect("Owner");			
		m_Region		=pRS->GetCollect("Region");			
		m_Location		=pRS->GetCollect("Location");		
		m_Line_Location	=pRS->GetCollect("Line_Location");	
		m_ElevationGd	=(float)atof((_bstr_t)pRS->GetCollect("ElevationGd"));			
		m_Elevation0	=(float)atof((_bstr_t)pRS->GetCollect("Elevation0"));		
		m_DepOffset		=(float)atof((_bstr_t)pRS->GetCollect("DepOffset"));		
		m_OilOffset		=(float)atof((_bstr_t)pRS->GetCollect("OilOffset"));		
		m_PipeOffset	=(float)atof((_bstr_t)pRS->GetCollect("PipeOffset"));		
		m_MagneticV		=(float)atof((_bstr_t)pRS->GetCollect("MagneticV"));		
		m_WellDepth		=(float)atof((_bstr_t)pRS->GetCollect("WellDepth"));		
		m_WellBottom	=(float)atof((_bstr_t)pRS->GetCollect("WellBottom"));	
		m_StartDate		=pRS->GetCollect("StartDate");		
		m_EndDate		=pRS->GetCollect("EndDate");			
		m_WellX			=(float)atof((_bstr_t)pRS->GetCollect("WellX"));			
		m_WellY			=(float)atof((_bstr_t)pRS->GetCollect("WellY"));			
		m_Longitude		=atof((_bstr_t)pRS->GetCollect("Longitude"));		
		m_Latitude		=atof((_bstr_t)pRS->GetCollect("Latitude"));		
		m_WellType		=pRS->GetCollect("WellType");  		
		m_WellKind		=pRS->GetCollect("WellKind");
		m_DrillAim		=pRS->GetCollect("DrillAim");		
		m_AimLayer		=pRS->GetCollect("AimLayer");		
		m_ReservoirType =pRS->GetCollect("ReservoirType");		
		m_FillUser		=pRS->GetCollect("FillUser");
		m_FillDate		=pRS->GetCollect("FillDate");
		m_Describe		=pRS->GetCollect("Describe");
		m_UpperID		=atol((_bstr_t)pRS->GetCollect("UpperID"));
		//初始化数组
		a_BitSize.RemoveAll();
		a_BitDep.RemoveAll();
		a_CasingSize.RemoveAll();
		a_CasingDep.RemoveAll();
		GetDrillTool(iWellID);
		return TRUE;
	}
}
//根据井信息ID修结构信息所属构造(用于存储空间调转功能模块中)
BOOL CGeoWell::sql_updateUpper(DWORD iUpperID)
{
	CString cID;
	cID.Format("%lu",m_RecordID);
	CString sID;
	sID.Format("%lu",iUpperID);
	return Execute("UPDATE Well SET UpperID=" + sID + " WHERE RecordID=" + cID);	
}

//根据所属存储空编号读取井信息所有字段值
BOOL CGeoWell::GetDataReferToUpperID(DWORD iUpperID)
{
	CString cID;
	cID.Format("%lu",iUpperID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Well WHERE UpperID=" + cID);

	//返回各列的值
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_RecordID		=atol((_bstr_t)pRS->GetCollect("RecordID"));
		m_WellName		=pRS->GetCollect("WellName");		
		m_Alias			=pRS->GetCollect("Alias");			
		m_Structure		=pRS->GetCollect("Structure");		
		m_Oilfield		=pRS->GetCollect("Oilfield");		
		m_Owner			=pRS->GetCollect("Owner");			
		m_Region		=pRS->GetCollect("Region");			
		m_Location		=pRS->GetCollect("Location");		
		m_Line_Location	=pRS->GetCollect("Line_Location");	
		m_ElevationGd	=(float)atof((_bstr_t)pRS->GetCollect("ElevationGd"));			
		m_Elevation0	=(float)atof((_bstr_t)pRS->GetCollect("Elevation0"));		
		m_DepOffset		=(float)atof((_bstr_t)pRS->GetCollect("DepOffset"));		
		m_OilOffset		=(float)atof((_bstr_t)pRS->GetCollect("OilOffset"));		
		m_PipeOffset	=(float)atof((_bstr_t)pRS->GetCollect("PipeOffset"));		
		m_MagneticV		=(float)atof((_bstr_t)pRS->GetCollect("MagneticV"));		
		m_WellDepth		=(float)atof((_bstr_t)pRS->GetCollect("WellDepth"));		
		m_WellBottom	=(float)atof((_bstr_t)pRS->GetCollect("WellBottom"));	
		m_StartDate		=pRS->GetCollect("StartDate");		
		m_EndDate		=pRS->GetCollect("EndDate");			
		m_WellX			=(float)atof((_bstr_t)pRS->GetCollect("WellX"));			
		m_WellY			=(float)atof((_bstr_t)pRS->GetCollect("WellY"));			
		m_Longitude		=atof((_bstr_t)pRS->GetCollect("Longitude"));		
		m_Latitude		=atof((_bstr_t)pRS->GetCollect("Latitude"));		
		m_WellType		=pRS->GetCollect("WellType");  		
		m_WellKind		=pRS->GetCollect("WellKind"); 
		m_DrillAim		=pRS->GetCollect("DrillAim");		
		m_AimLayer		=pRS->GetCollect("AimLayer");		
		m_ReservoirType =pRS->GetCollect("ReservoirType");		
		m_FillUser		=pRS->GetCollect("FillUser");
		m_FillDate		=pRS->GetCollect("FillDate");
		m_Describe		=pRS->GetCollect("Describe");
		m_UpperID		=atol((_bstr_t)pRS->GetCollect("UpperID"));
		//初始化数组
		a_BitSize.RemoveAll();
		a_BitDep.RemoveAll();
		a_CasingSize.RemoveAll();
		a_CasingDep.RemoveAll();
		GetDrillTool(m_RecordID);
		return TRUE;
	}
}

//根据井信息编号读取所有钻具信息
BOOL CGeoWell::GetDrillTool(DWORD iWellID)
{
	CString cID;
	cID.Format("%lu",iWellID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM DrillTool WHERE UpperID=" + cID);
	float value;
	while(pRS->adoEOF == 0)
	{	
		value=(float)atof((_bstr_t)pRS->GetCollect("BitSize"));
		a_BitSize.Add(value);
		value=(float)atof((_bstr_t)pRS->GetCollect("BitDep"));
		a_BitDep.Add(value);	
		value=(float)atof((_bstr_t)pRS->GetCollect("CasingSize"));
		a_CasingSize.Add(value);
		value=(float)atof((_bstr_t)pRS->GetCollect("CasingDep"));
		a_CasingDep.Add(value);
		pRS->MoveNext();
	}
	return TRUE;
}
//根据井信息编号增加钻具信息
DWORD CGeoWell::InsertDrillTool(DWORD iWellID)
{
	int Num=a_BitSize.GetCount();
	if(Num<1) return 0;

	CString sBitSize,sBitDep,sCasingSize,sCasingDep;
	CString sUpperID;sUpperID.Format("%lu",iWellID);
	CString vSQL;
	for(int i=0;i<Num;i++)
	{
		sBitSize.Format("%.2f",  		a_BitSize.GetAt(i));  			//钻头大小	
		sBitDep.Format("%.2f",  		a_BitDep.GetAt(i));  			//钻头深度	
		sCasingSize.Format("%.2f",  	a_CasingSize.GetAt(i));  		//套管大小	
		sCasingDep.Format("%.2f",  		a_CasingDep.GetAt(i));  		//套管深度	
		vSQL = "INSERT INTO DrillTool( BitSize,	\
										BitDep,		\
										CasingSize,	\
										CasingDep,	\
										UpperID)	\
		VALUES('"   + sBitSize		+ "','"
					+ sBitDep		+ "','"
					+ sCasingSize	+ "','"
					+ sCasingDep	+ "','"
					+ sUpperID	+ "')";
		Execute(vSQL);
	}
	return GetMaxRecordID("DrillTool");
}
//根据试油信息编号删除所有射孔井段
BOOL CGeoWell::DeleteDrillTool(DWORD iWellID)
{
	CString cID;
	cID.Format("%d",iWellID);
	//删除钻具信息
	return Execute("DELETE FROM DrillTool WHERE UpperID=" + cID);
}

BOOL CGeoWell::UpdateDrillTool(DWORD iWellID)
{
	if(DeleteDrillTool(iWellID))
	{
		InsertDrillTool(iWellID);
		return TRUE;
	}
	return FALSE;
}