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
	m_ProjectID = 0;
	m_ProjectName= _T("");		//��Ŀ����
	m_ProjectAlias= _T("");		//��Ŀ����
	m_LogDate= _T("");  		//�⾮����
	m_LogStDep=0.f;  			//�⾮��ʼ���
	m_LogEnDep=0.f;  			//�⾮�������
	m_DepRatio=200.f;			//��ȱ���
	m_LogDevice= _T("");  	    //�⾮�豸
	m_LogTeam= _T("");  		//�⾮С��
	m_LogCompany= _T("");  	    //�⾮��˾
	m_LogLeader= _T("");  	    //�⾮С�ӳ�
	m_LogEngineer= _T("");  	//�⾮����ʦ
	m_CasingNote= _T("");  	    //�׹�����
	m_BitNote= _T("");  		//��ͷ����
	m_FluidType= _T("");		//�ཬ����
	m_FluidDen=-999.25f;		//�ཬ�ܶ�
	m_FluidVis=-999.25f;		//�ཬճ��
	m_FluidRm=-999.25f;			//�ཬ������
	m_FluidTem=-999.25f;		//�ཬ�¶�
	m_BottomDep=0.f;			//�������
	m_BottomTem=-999.25f;		//�����¶�
	m_DeviceNote= _T("");  	    //�������
	m_AuditUser= _T("");		//�����
	m_AuditDate= _T("");		//�������
	m_FillUser= _T("");			//�ύ��
	m_FillDate= _T("");			//�ύ����
	m_Describe= _T("");			//��������
	m_UpperID = 0;				//�洢�ռ���
}

CGeoProject::~CGeoProject()
{
	Disconnect();
}

void CGeoProject::Disconnect()
{
	m_ado.Disconnect();
}

BOOL CGeoProject::InitData(CString szCon)
{
	return m_ado.InitData(szCon);
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

int CGeoProject::GetUpperID()
{
	return m_UpperID;
}

void CGeoProject::SetUpperID(int iUpperID)
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
	if(!m_ado.Execute(vSQL))
		return 0;

	_RecordsetPtr pRS = m_ado.GetRS("SELECT MAX(ProjectID) AS MaxID FROM Project");
	DWORD ProjectID;
	if(pRS->adoEOF)
		ProjectID = 0;
	else 
		ProjectID = atol((LPCTSTR)(_bstr_t)pRS->GetCollect("MaxID"));

	return ProjectID;
}

BOOL CGeoProject::sql_update(int iProjectID)
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
	cID.Format("%d",iProjectID);
	CString str("UPDATE Project SET	  ProjectName='"	+		m_ProjectName		//��Ŀ����
									+ "', ProjectAlias='"	+		m_ProjectAlias		//��Ŀ����
									+ "', LogDate='"  		+		m_LogDate 			//�⾮����
									+ "', LogStDep='"  		+		sLogStDep			//�⾮��ʼ���
									+ "', LogEnDep='"  		+		sLogEnDep  			//�⾮�������
									+ "', DepRatio='"  		+		sDepRatio  			//��ȱ���
									+ "', LogDevice='"  	+		m_LogDevice			//�⾮�豸
									+ "', LogTeam='"  		+		m_LogTeam  			//�⾮С��
									+ "', LogCompany='"  	+		m_LogCompany 		//�⾮��˾
									+ "', LogLeader='"  	+		m_LogLeader  		//�⾮С�ӳ�
									+ "', LogEngineer='" 	+		m_LogEngineer		//�⾮����ʦ
									+ "', CasingNote='"  	+		m_CasingNote		//�׹�����
									+ "', BitNote='"  		+		m_BitNote  			//��ͷ����
									+ "', FluidType='"		+		m_FluidType			//�ཬ����
									+ "', FluidDen='"		+		sFluidDen			//�ཬ�ܶ�
									+ "', FluidVis='"		+		sFluidVis			//�ཬճ��
									+ "', FluidRm='"		+		sFluidRm			//�ཬ������
									+ "', FluidTem='"		+		sFluidTem			//�ཬ�¶�
									+ "', BottomDep='"		+		sBottomDep			//�������
									+ "', BottomTem='"		+		sBottomTem			//�����¶�
									+ "', DeviceNote='"  	+		m_DeviceNote		//�������			
									+ "', AuditUser='"		+		m_Describe			//�����
									+ "', AuditDate='"		+		m_FillUser			//�������
									+ "', Describe='"		+ 		m_FillDate			//��Ŀ����
									+ "', FillUser='"		+		m_AuditUser			//�ύ��
									+ "', FillDate='"		+		m_AuditDate			//�ύ����
									+ "', UpperID="			+		sUpperID			//�����洢�ռ���
				+ "' WHERE ProjectID=" + cID );
	return m_ado.Execute(str);	
}

BOOL CGeoProject::sql_delete(int iProjectID)
{
	CString cID;
	cID.Format("%d",iProjectID);
	return m_ado.Execute("DELETE FROM Project WHERE ProjectID=" + cID);
}

//���ݾ���Ϣ��Ŷ�ȡ�����ֶ�ֵ
BOOL CGeoProject::GetData(int iProjectID)
{
	CString cID;
	cID.Format("%d",iProjectID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Project WHERE ProjectID=" + cID);

	//���ظ��е�ֵ
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_ProjectName=(LPCTSTR)(_bstr_t)pRS->GetCollect("ProjectName");	
		m_ProjectAlias=(LPCTSTR)(_bstr_t)pRS->GetCollect("ProjectAlias");	
		m_LogDate=(LPCTSTR)(_bstr_t)pRS->GetCollect("LogDate");  		
		m_LogStDep=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("LogStDep"));  	
		m_LogEnDep=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("LogEnDep"));
		m_DepRatio=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("DepRatio"));
		m_LogDevice=(LPCTSTR)(_bstr_t)pRS->GetCollect("LogDevice");  	
		m_LogTeam=(LPCTSTR)(_bstr_t)pRS->GetCollect("LogTeam");  		
		m_LogCompany=(LPCTSTR)(_bstr_t)pRS->GetCollect("LogCompany");  	
		m_LogLeader=(LPCTSTR)(_bstr_t)pRS->GetCollect("LogLeader");  	
		m_LogEngineer=(LPCTSTR)(_bstr_t)pRS->GetCollect("LogEngineer");  	
		m_CasingNote=(LPCTSTR)(_bstr_t)pRS->GetCollect("CasingNote");  	
		m_BitNote=(LPCTSTR)(_bstr_t)pRS->GetCollect("BitNote");  		
		m_FluidType=(LPCTSTR)(_bstr_t)pRS->GetCollect("FluidType");		
		m_FluidDen=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("FluidDen"));		
		m_FluidVis=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("FluidVis"));		
		m_FluidRm=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("FluidRm"));		
		m_FluidTem=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("FluidTem"));		
		m_BottomDep=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("BottomDep"));		
		m_BottomTem=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("BottomTem"));		
		m_DeviceNote=(LPCTSTR)(_bstr_t)pRS->GetCollect("DeviceNote");  	
		m_AuditUser=(LPCTSTR)(_bstr_t)pRS->GetCollect("AuditUser");		
		m_AuditDate=(LPCTSTR)(_bstr_t)pRS->GetCollect("AuditDate");		
		m_FillUser=(LPCTSTR)(_bstr_t)pRS->GetCollect("FillUser");
		m_FillDate=(LPCTSTR)(_bstr_t)pRS->GetCollect("FillDate");
		m_Describe=(LPCTSTR)(_bstr_t)pRS->GetCollect("Describe");
		m_UpperID =atoi( (LPCTSTR)(_bstr_t)pRS->GetCollect("UpperID"));
		return TRUE;
	}
}
