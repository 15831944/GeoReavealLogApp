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
	m_WellName=_T("");	        //������               
	m_Alias=_T("");				//������
	m_Structure=_T("");			//��������
	m_Oilfield=_T("");			//��������
	m_Owner=_T("");				//ҵ����λ
	m_Region=_T("");			//����
	m_Location=_T("");			//����λ��
	m_Line_Location=_T("");	 	//����λ��
	m_ElevationGd=0.f;		 	//���溣��
	m_Elevation0=0.f;		 	//��׼�溣��
	m_DepOffset=0.f;		 	//���ĸ߶�
	m_OilOffset=0.f;		 	//�Ͳ���
	m_PipeOffset=0.f;		 	//�ײ���
	m_MagneticV=0.f;		 	//��ƫ��
	m_WellDepth=0.f;		 	//�꾮����
	m_WellBottom=0.f;		 	//�˹�����
	m_StartDate=_T("");		 	//��������
	m_EndDate=_T("");			//��������
	m_WellX=0.f;			 	//��λX����
	m_WellY=0.f;			 	//��λY����
	m_Longitude=0.;		 		//��λ����
	m_Latitude=0.;			 	//��λγ��
	m_WellType=_T("");		 	//��̽����(̽����Ԥ̽�������۾���������)
	m_WellKind=_T("");		 	//���̷���(ֱ����б����ˮƽ������ʽ��)
	m_DrillAim=_T("");			//��̽Ŀ��
	m_AimLayer=_T("");			//Ŀ���λ
	m_ReservoirType=_T("");		//��������(���������㡢ú������ҳ����)
	m_FillUser= _T("");			//�ύ��
	m_FillDate= _T("");			//�ύ����
	m_Describe= _T("");			//������
	m_UpperID = 0;				//�洢�ռ���
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
	CString str("UPDATE Well SET	  WellName='"		+		m_WellName           //������               
									+ "', Alias='"			+		m_Alias				 //������
									+ "', Structure='"		+		m_Structure			 //��������
									+ "', Oilfield='"		+		m_Oilfield			 //��������
									+ "', Owner='"			+		m_Owner				 //ҵ����λ
									+ "', Region='"			+		m_Region			 //����
									+ "', Location='"		+		m_Location			 //����λ��
									+ "', Line_Location='"	+		m_Line_Location		 //����λ��
									+ "', ElevationGd='"	+		sElevationGd		 //���溣��
									+ "', Elevation0='"		+		sElevation0			 //��׼�溣��
									+ "', DepOffset='"		+		sDepOffset			 //���ĸ߶�
									+ "', OilOffset='"		+		sOilOffset			 //�Ͳ���
									+ "', PipeOffset='"		+		sPipeOffset			 //�ײ���
									+ "', MagneticV='"		+		sMagneticV			 //��ƫ��
									+ "', WellDepth='"		+		sWellDepth			 //�꾮����
									+ "', WellBottom='"		+		sWellBottom			 //�˹�����
									+ "', StartDate='"		+		m_StartDate			 //��������
									+ "', EndDate='"		+		m_EndDate			 //��������
									+ "', WellX='"			+		sWellX				 //��λX����
									+ "', WellY='"			+		sWellY				 //��λY����
									+ "', Longitude='"		+		sLongitude			 //��λ����
									+ "', Latitude='"		+		sLatitude			 //��λγ��
									+ "', WellType='"		+		m_WellType			 //��̽����(̽����Ԥ̽�������۾���������)
									+ "', WellKind='"		+		m_WellKind			 //���̷���(ֱ����б����ˮƽ������ʽ��)
									+ "', DrillAim='"		+		m_DrillAim			 //��̽Ŀ��
									+ "', AimLayer='"		+		m_AimLayer			 //Ŀ���λ
									+ "', ReservoirType='"	+		m_ReservoirType		 //��������(���������㡢ú������ҳ����)
									+ "', Describe='"		+		m_Describe 			 //������
									+ "', FillUser='"		+		m_FillUser			 //�ύ��
									+ "', FillDate='"		+		m_FillDate			 //�ύ����
									+ "', UpperID='"		+		sUpperID			 //�洢�ռ���
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

BOOL CGeoWell::sql_delete(DWORD iWellID)
{
	CString cID;
	cID.Format("%lu",iWellID);
	return Execute("DELETE FROM Well WHERE RecordID=" + cID);
}

//���ݾ���Ϣ��Ŷ�ȡ�����ֶ�ֵ
BOOL CGeoWell::GetData(DWORD iWellID)
{
	CString cID;
	cID.Format("%lu",iWellID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Well WHERE RecordID=" + cID);

	//���ظ��е�ֵ
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
		//��ʼ������
		a_BitSize.RemoveAll();
		a_BitDep.RemoveAll();
		a_CasingSize.RemoveAll();
		a_CasingDep.RemoveAll();
		GetDrillTool(iWellID);
		return TRUE;
	}
}
//���ݾ���ϢID�޽ṹ��Ϣ��������(���ڴ洢�ռ��ת����ģ����)
BOOL CGeoWell::sql_updateUpper(DWORD iUpperID)
{
	CString cID;
	cID.Format("%lu",m_RecordID);
	CString sID;
	sID.Format("%lu",iUpperID);
	return Execute("UPDATE Well SET UpperID=" + sID + " WHERE RecordID=" + cID);	
}

//���������洢�ձ�Ŷ�ȡ����Ϣ�����ֶ�ֵ
BOOL CGeoWell::GetDataReferToUpperID(DWORD iUpperID)
{
	CString cID;
	cID.Format("%lu",iUpperID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Well WHERE UpperID=" + cID);

	//���ظ��е�ֵ
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
		//��ʼ������
		a_BitSize.RemoveAll();
		a_BitDep.RemoveAll();
		a_CasingSize.RemoveAll();
		a_CasingDep.RemoveAll();
		GetDrillTool(m_RecordID);
		return TRUE;
	}
}

//���ݾ���Ϣ��Ŷ�ȡ���������Ϣ
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
//���ݾ���Ϣ������������Ϣ
DWORD CGeoWell::InsertDrillTool(DWORD iWellID)
{
	int Num=a_BitSize.GetCount();
	if(Num<1) return 0;

	CString sBitSize,sBitDep,sCasingSize,sCasingDep;
	CString sUpperID;sUpperID.Format("%lu",iWellID);
	CString vSQL;
	for(int i=0;i<Num;i++)
	{
		sBitSize.Format("%.2f",  		a_BitSize.GetAt(i));  			//��ͷ��С	
		sBitDep.Format("%.2f",  		a_BitDep.GetAt(i));  			//��ͷ���	
		sCasingSize.Format("%.2f",  	a_CasingSize.GetAt(i));  		//�׹ܴ�С	
		sCasingDep.Format("%.2f",  		a_CasingDep.GetAt(i));  		//�׹����	
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
//����������Ϣ���ɾ��������׾���
BOOL CGeoWell::DeleteDrillTool(DWORD iWellID)
{
	CString cID;
	cID.Format("%d",iWellID);
	//ɾ�������Ϣ
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