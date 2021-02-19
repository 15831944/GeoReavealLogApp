/*********************************************************************/

#include "stdafx.h"
#include "GeoResult.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGeoResult::CGeoResult()
{
	m_ResultID = 0;
	m_LayerName= _T("");		//��λ����
	m_LayerAlias= _T("");		//��λ����
	m_Number= _T("");			//��ע���
	m_DepSt=0.f;  				//��ʼ���
	m_DepEn=0.f;  				//�������
	m_DepHD=0.f;				//���
	m_DepHD1=0.f;				//��Ч���
	m_POR=-999.25f;				//��϶��
	m_PERMA=-999.25f;			//��͸��
	m_SW=-999.25f;				//��ˮ���Ͷ�
	m_SH=-999.25f;				//���ʺ���
	m_Conclusion= _T("");		//���ͽ���
	m_SP=-999.25f;				//��Ȼ��λ
	m_GR=-999.25f;				//��Ȼ٤��
	m_DEN=-999.25f;				//�ܶ�
	m_CNL=-999.25f;				//����
	m_AC=-999.25f;				//����
	m_RT=-999.25f;				//�������
	m_RXO=-999.25f;				//�е�����
	m_RS=-999.25f;				//ǳ������
	m_FillUser= _T("");			//�ύ��
	m_FillDate= _T("");			//�ύ����
	m_Describe= _T("");			//��������
	m_UpperID = 0;				//�洢�ռ���
}

CGeoResult::~CGeoResult()
{
	Disconnect();
}

void CGeoResult::Disconnect()
{
	m_ado.Disconnect();
}

BOOL CGeoResult::InitData(CString szCon)
{
	return m_ado.InitData(szCon);
}

CString CGeoResult::GetDescribe()
{
	return m_Describe;
}

void CGeoResult::SetDescribe(CString cDescribe)
{
	m_Describe = cDescribe;
}

CString CGeoResult::GetFillUser()
{
	return m_FillUser;
}

void CGeoResult::SetFillUser(CString cUser)
{
	m_FillUser = cUser;
}

CString CGeoResult::GetFillDate()
{
	return m_FillDate;
}

void CGeoResult::SetFillDate(CString cDate)
{
	m_FillDate = cDate;
}

int CGeoResult::GetUpperID()
{
	return m_UpperID;
}

void CGeoResult::SetUpperID(int iUpperID)
{
	m_UpperID = iUpperID;
}

DWORD CGeoResult::sql_insert()
{	
	CString sDepSt;  	sDepSt.Format("%.2f",m_DepSt);  	
	CString sDepEn;  	sDepEn.Format("%.2f",m_DepEn);  	
	CString sDepHD;		sDepHD.Format("%.2f",m_DepHD);	
	CString sDepHD1;	sDepHD1.Format("%.2f",m_DepHD1);	
	CString sPOR;		sPOR.Format("%.2f",m_POR);		
	CString sPERMA;		sPERMA.Format("%.2f",m_PERMA);	
	CString sSW;		sSW.Format("%.2f",m_SW);		
	CString sSH;		sSH.Format("%.2f",m_SH);		
	CString sSP;		sSP.Format("%.2f",m_SP);	
	CString sGR;		sGR.Format("%.2f",m_GR);	
	CString sDEN;		sDEN.Format("%.3f",m_DEN);
	CString sCNL;		sCNL.Format("%.2f",m_CNL);
	CString sAC;		sAC.Format("%.2f",m_AC);	
	CString sRT;		sRT.Format("%.2f",m_RT);	
	CString sRXO;		sRXO.Format("%.2f",m_RXO);
	CString sRS;		sRS.Format("%.2f",m_RS);			
	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);
	CString vSQL;
	vSQL = "INSERT INTO Result( LayerName,		\
									LayerAlias,		\
									Number,			\
									DepSt,  		\
									DepEn,  		\
									DepHD,			\
									DepHD1,			\
									POR,			\
									PERMA,			\
									SW,				\
									SH,				\
									Conclusion,		\
									SP,				\
									GR,				\
									DEN,			\
									CNL,			\
									AC,				\
									RT,				\
									RXO,			\
									RS,				\
									Describe,		\
									FillUser,		\
									FillDate,		\
									UpperID)		\
		VALUES('"   + m_LayerName	+ "','"
					+ m_LayerAlias	+ "','" 
					+ m_Number		+ "','"
					+ sDepSt   		+ "','"
					+ sDepEn  		+ "','"
					+ sDepHD		+ "','"
					+ sDepHD1		+ "','"
					+ sPOR			+ "','"
					+ sPERMA		+ "','"
					+ sSW			+ "','"
					+ sSH			+ "','"
					+ m_Conclusion	+ "','"
					+ sSP			+ "','"
					+ sGR			+ "','"
					+ sDEN			+ "','"
					+ sCNL			+ "','"
					+ sAC			+ "','"
					+ sRT			+ "','"
					+ sRXO			+ "','"
					+ sRS			+ "','"					
					+ m_Describe 	+ "','"
					+ m_FillUser	+ "','"
					+ m_FillDate	+ "','"
					+ sUpperID		+ "')";
	if(!m_ado.Execute(vSQL))
		return 0;
	//��ȡ�ղŲ���Ĵ洢�ռ���Ϣ�е�StorageIDֵ(���ݿ���StorageID�ֶ����Զ����ӵ�)
	_RecordsetPtr pRS = m_ado.GetRS("SELECT MAX(ResultID) AS MaxID FROM Result");
	DWORD ResultID;
	if(pRS->adoEOF)
		ResultID = 0;
	else 
		ResultID = atol((LPCTSTR)(_bstr_t)pRS->GetCollect("MaxID"));

	return ResultID;
}

BOOL CGeoResult::sql_update(int iResultID)
{	
	CString sDepSt;  	sDepSt.Format("%.2f",m_DepSt);  	
	CString sDepEn;  	sDepEn.Format("%.2f",m_DepEn);  	
	CString sDepHD;		sDepHD.Format("%.2f",m_DepHD);	
	CString sDepHD1;	sDepHD1.Format("%.2f",m_DepHD1);	
	CString sPOR;		sPOR.Format("%.2f",m_POR);		
	CString sPERMA;		sPERMA.Format("%.2f",m_PERMA);	
	CString sSW;		sSW.Format("%.2f",m_SW);		
	CString sSH;		sSH.Format("%.2f",m_SH);		
	CString sSP;		sSP.Format("%.2f",m_SP);	
	CString sGR;		sGR.Format("%.2f",m_GR);	
	CString sDEN;		sDEN.Format("%.3f",m_DEN);
	CString sCNL;		sCNL.Format("%.2f",m_CNL);
	CString sAC;		sAC.Format("%.2f",m_AC);	
	CString sRT;		sRT.Format("%.2f",m_RT);	
	CString sRXO;		sRXO.Format("%.2f",m_RXO);
	CString sRS;		sRS.Format("%.2f",m_RS);			
	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);
	CString cID;
	cID.Format("%d",iResultID);
	CString str("UPDATE Result SET	  LayerName='"		+		m_LayerName	          //��λ����   
									+ "', LayerAlias='"		+		m_LayerAlias		  //��λ����	
									+ "', Number='"			+		m_Number			  //��ע���
									+ "', DepSt='"  		+		sDepSt   			  //��ʼ���
									+ "', DepEn='"  		+		sDepEn  			  //�������
									+ "', DepHD='"			+		sDepHD				  //���
									+ "', DepHD1='"			+		sDepHD1				  //��Ч���
									+ "', POR='"			+		sPOR				  //��϶��		
									+ "', PERMA='"			+		sPERMA				  //��͸��
									+ "', SW='"				+		sSW					  //��ˮ���Ͷ�
									+ "', SH='"				+		sSH					  //���ʺ���
									+ "', Conclusion='"		+		m_Conclusion		  //���ͽ���
									+ "', SP='"				+		sSP					  //��Ȼ��λ
									+ "', GR='"				+		sGR					  //��Ȼ٤��
									+ "', DEN='"			+		sDEN				  //�ܶ�
									+ "', CNL='"			+		sCNL				  //����
									+ "', AC='"				+		sAC					  //����
									+ "', RT='"				+		sRT					  //�������
									+ "', RXO='"			+		sRXO				  //�е�����
									+ "', RS='"				+		sRS					 //ǳ������						
									+ "', Describe='"		+		m_Describe 			 //��������
									+ "', FillUser='"		+		m_FillUser			 //�ύ��
									+ "', FillDate='"		+		m_FillDate			 //�ύ����
									+ "', UpperID="			+		sUpperID			 //�洢�ռ���
				+ "' WHERE ResultID=" + cID );
	return m_ado.Execute(str);	
}

BOOL CGeoResult::sql_delete(int iResultID)
{
	CString cID;
	cID.Format("%d",iResultID);
	return m_ado.Execute("DELETE FROM Result WHERE ResultID=" + cID);
}

//���ݾ���Ϣ��Ŷ�ȡ�����ֶ�ֵ
BOOL CGeoResult::GetData(int iResultID)
{
	CString cID;
	cID.Format("%d",iResultID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Result WHERE ResultID=" + cID);

	//���ظ��е�ֵ
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_LayerName	=(LPCTSTR)(_bstr_t)pRS->GetCollect("LayerName");	
		m_LayerAlias=(LPCTSTR)(_bstr_t)pRS->GetCollect("LayerAlias");	
		m_Number=(LPCTSTR)(_bstr_t)pRS->GetCollect("Number");
		m_DepSt =(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("DepSt"));   	
		m_DepEn =(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("DepEn"));  	
		m_DepHD =(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("DepHD"));		
		m_DepHD1=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("DepHD1"));		
		m_POR	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("POR"));		
		m_PERMA =(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("PERMA"));		
		m_SW	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("SW"));			
		m_SH	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("SH"));			
		m_Conclusion=(LPCTSTR)(_bstr_t)pRS->GetCollect("Conclusion");
		m_SP	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("SP"));	 	
		m_GR	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("GR"));		
		m_DEN	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("DEN"));		
		m_CNL	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("CNL"));		
		m_AC	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("AC"));		
		m_RT	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("RT"));		
		m_RXO	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("RXO"));		
		m_RS	=(float)atof( (LPCTSTR)(_bstr_t)pRS->GetCollect("RS"));	
		m_FillUser=(LPCTSTR)(_bstr_t)pRS->GetCollect("FillUser");
		m_FillDate=(LPCTSTR)(_bstr_t)pRS->GetCollect("FillDate");
		m_Describe=(LPCTSTR)(_bstr_t)pRS->GetCollect("Describe");
		m_UpperID =atoi( (LPCTSTR)(_bstr_t)pRS->GetCollect("UpperID"));
		return TRUE;
	}
}
