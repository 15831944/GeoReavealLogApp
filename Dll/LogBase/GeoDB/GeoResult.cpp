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
	m_RecordID = 0;
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
	m_CGR=-999.25f;				//����٤��
	m_DEN=-999.25f;				//�ܶ�
	m_CNL=-999.25f;				//����
	m_AC=-999.25f;				//����
	m_RT=-999.25f;				//�������
	m_RXO=-999.25f;				//�е�����
	m_RS=-999.25f;				//ǳ������
	m_PorMax=-999.25f;			//����϶��
	m_SwMax=-999.25f;			//���ˮ���Ͷ�
	m_HoleNum=-999.25f;			//�ܿ׶���
	m_HolePor=-999.25f;			//�������
	m_HoleDia=-999.25f;			//ƽ���׾�
	m_CrackNum=-999.25f;		//�ѷ�����
	m_CrackLength=-999.25f;		//�ѷ쳤��
	m_CrackWidth=-999.25f;		//�ѷ���
	m_CrackWaterWidth=-999.25f;	//�ѷ�ˮ�������
	m_CrackPor=-999.25f;		//�ѷ������
	m_FillUser= _T("");			//�ύ��
	m_FillDate= _T("");			//�ύ����
	m_Describe= _T("");			//��������
	m_UpperID = 0;				//�洢�ռ���
}

CGeoResult::~CGeoResult()
{
	Disconnect();
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

DWORD CGeoResult::GetUpperID()
{
	return m_UpperID;
}

void CGeoResult::SetUpperID(DWORD iUpperID)
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
	CString sCGR;		sCGR.Format("%.2f",m_CGR);
	CString sDEN;		sDEN.Format("%.3f",m_DEN);
	CString sCNL;		sCNL.Format("%.2f",m_CNL);
	CString sAC;		sAC.Format("%.2f",m_AC);	
	CString sRT;		sRT.Format("%.2f",m_RT);	
	CString sRXO;		sRXO.Format("%.2f",m_RXO);
	CString sRS;		sRS.Format("%.2f",m_RS);
	CString sPorMax;			sPorMax.Format("%.2f",			m_PorMax);			//����϶��
	CString sSwMax;				sSwMax.Format("%.2f",			m_SwMax);			//���ˮ���Ͷ�
	CString sHoleNum;			sHoleNum.Format("%.2f",			m_HoleNum);			//�ܿ׶���
	CString sHolePor;			sHolePor.Format("%.2f",			m_HolePor);			//�������
	CString sHoleDia;			sHoleDia.Format("%.2f",			m_HoleDia);			//ƽ���׾�
	CString sCrackNum;			sCrackNum.Format("%.2f",		m_CrackNum);		//�ѷ�����
	CString sCrackLength;		sCrackLength.Format("%.2f",		m_CrackLength);		//�ѷ쳤��
	CString sCrackWidth;		sCrackWidth.Format("%.2f",		m_CrackWidth);		//�ѷ���
	CString sCrackWaterWidth;	sCrackWaterWidth.Format("%.2f",	m_CrackWaterWidth);	//�ѷ�ˮ�������
	CString sCrackPor;			sCrackPor.Format("%.2f",		m_CrackPor);		//�ѷ������

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
									CGR,			\
									DEN,			\
									CNL,			\
									AC,				\
									RT,				\
									RXO,			\
									RS,				\
									PorMax,			\
									SwMax,			\
									HoleNum,		\
									HolePor,		\
									HoleDia,		\
									CrackNum,		\
									CrackLength,	\
									CrackWidth,		\
									CrackWaterWidth,\
									CrackPor,		\
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
					+ sCGR			+ "','"
					+ sDEN			+ "','"
					+ sCNL			+ "','"
					+ sAC			+ "','"
					+ sRT			+ "','"
					+ sRXO			+ "','"
					+ sRS			+ "','"	
					+ sPorMax		+ "','"				
					+ sSwMax		+ "','"					
					+ sHoleNum		+ "','"				
					+ sHolePor		+ "','"				
					+ sHoleDia		+ "','"				
					+ sCrackNum		+ "','"				
					+ sCrackLength	+ "','"			
					+ sCrackWidth	+ "','"			
					+ sCrackWaterWidth+ "','"		
					+ sCrackPor		+ "','"				
					+ m_Describe 	+ "','"
					+ m_FillUser	+ "','"
					+ m_FillDate	+ "','"
					+ sUpperID		+ "')";
	if(!Execute(vSQL))
		return 0;
	return GetMaxRecordID("Result");
}

BOOL CGeoResult::sql_update(DWORD iResultID)
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
	CString sCGR;		sGR.Format("%.2f",m_CGR);
	CString sDEN;		sDEN.Format("%.3f",m_DEN);
	CString sCNL;		sCNL.Format("%.2f",m_CNL);
	CString sAC;		sAC.Format("%.2f",m_AC);	
	CString sRT;		sRT.Format("%.2f",m_RT);	
	CString sRXO;		sRXO.Format("%.2f",m_RXO);
	CString sRS;		sRS.Format("%.2f",m_RS);
	CString sPorMax;			sPorMax.Format("%.2f",			m_PorMax);			//����϶��
	CString sSwMax;				sSwMax.Format("%.2f",			m_SwMax);			//���ˮ���Ͷ�
	CString sHoleNum;			sHoleNum.Format("%.2f",			m_HoleNum);			//�ܿ׶���
	CString sHolePor;			sHolePor.Format("%.2f",			m_HolePor);			//�������
	CString sHoleDia;			sHoleDia.Format("%.2f",			m_HoleDia);			//ƽ���׾�
	CString sCrackNum;			sCrackNum.Format("%.2f",		m_CrackNum);		//�ѷ�����
	CString sCrackLength;		sCrackLength.Format("%.2f",		m_CrackLength);		//�ѷ쳤��
	CString sCrackWidth;		sCrackWidth.Format("%.2f",		m_CrackWidth);		//�ѷ���
	CString sCrackWaterWidth;	sCrackWaterWidth.Format("%.2f",	m_CrackWaterWidth);	//�ѷ�ˮ�������
	CString sCrackPor;			sCrackPor.Format("%.2f",		m_CrackPor);		//�ѷ������

	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);
	CString cID;
	cID.Format("%lu",iResultID);
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
									+ "', CGR='"			+		sCGR				  //����٤��
									+ "', DEN='"			+		sDEN				  //�ܶ�
									+ "', CNL='"			+		sCNL				  //����
									+ "', AC='"				+		sAC					  //����
									+ "', RT='"				+		sRT					  //�������
									+ "', RXO='"			+		sRXO				  //�е�����
									+ "', RS='"				+		sRS					  //ǳ������	
									+ "', PorMax='"			+		sPorMax				  //����϶��
									+ "', SwMax='"			+		sSwMax				  //���ˮ���Ͷ�		
									+ "', HoleNum='"		+		sHoleNum			  //�ܿ׶���	
									+ "', HolePor='"		+		sHolePor			  //�������	
									+ "', HoleDia='"		+		sHoleDia			  //ƽ���׾�	
									+ "', CrackNum='"		+		sCrackNum			  //�ѷ�����	
									+ "', CrackLength='"	+		sCrackLength		  //�ѷ쳤��
									+ "', CrackWidth='"		+		sCrackWidth			  //�ѷ���
									+ "', CrackWaterWidth='"+		sCrackWaterWidth	  //�ѷ�ˮ�������
									+ "', CrackPor='"		+		sCrackPor			  //�ѷ������	
									+ "', Describe='"		+		m_Describe 			 //��������
									+ "', FillUser='"		+		m_FillUser			 //�ύ��
									+ "', FillDate='"		+		m_FillDate			 //�ύ����
									+ "', UpperID="			+		sUpperID			 //�洢�ռ���
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

BOOL CGeoResult::sql_delete(DWORD iResultID)
{
	CString cID;
	cID.Format("%lu",iResultID);
	return Execute("DELETE FROM Result WHERE RecordID=" + cID);
}

//���ݾ���Ϣ��Ŷ�ȡ�����ֶ�ֵ
BOOL CGeoResult::GetData(DWORD iResultID)
{
	CString cID;
	cID.Format("%lu",iResultID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Result WHERE RecordID=" + cID);

	//���ظ��е�ֵ
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		m_LayerName			=pRS->GetCollect("LayerName");	
		m_LayerAlias		=pRS->GetCollect("LayerAlias");	
		m_Number			=pRS->GetCollect("Number");
		m_DepSt				=(float)atof((_bstr_t)pRS->GetCollect("DepSt"));   	
		m_DepEn				=(float)atof((_bstr_t)pRS->GetCollect("DepEn"));  	
		m_DepHD				=(float)atof((_bstr_t)pRS->GetCollect("DepHD"));		
		m_DepHD1			=(float)atof((_bstr_t)pRS->GetCollect("DepHD1"));		
		m_POR				=(float)atof((_bstr_t)pRS->GetCollect("POR"));		
		m_PERMA				=(float)atof((_bstr_t)pRS->GetCollect("PERMA"));		
		m_SW				=(float)atof((_bstr_t)pRS->GetCollect("SW"));			
		m_SH				=(float)atof((_bstr_t)pRS->GetCollect("SH"));			
		m_Conclusion		=pRS->GetCollect("Conclusion");
		m_SP				=(float)atof((_bstr_t)pRS->GetCollect("SP"));	 	
		m_GR				=(float)atof((_bstr_t)pRS->GetCollect("GR"));
		m_CGR				=(float)atof((_bstr_t)pRS->GetCollect("CGR"));
		m_DEN				=(float)atof((_bstr_t)pRS->GetCollect("DEN"));		
		m_CNL				=(float)atof((_bstr_t)pRS->GetCollect("CNL"));		
		m_AC				=(float)atof((_bstr_t)pRS->GetCollect("AC"));		
		m_RT				=(float)atof((_bstr_t)pRS->GetCollect("RT"));		
		m_RXO				=(float)atof((_bstr_t)pRS->GetCollect("RXO"));		
		m_RS				=(float)atof((_bstr_t)pRS->GetCollect("RS"));	
		m_PorMax			=(float)atof((_bstr_t)pRS->GetCollect("PorMax"));						
		m_SwMax				=(float)atof((_bstr_t)pRS->GetCollect("SwMax"));							
		m_HoleNum			=(float)atof((_bstr_t)pRS->GetCollect("HoleNum"));						
		m_HolePor			=(float)atof((_bstr_t)pRS->GetCollect("HolePor"));						
		m_HoleDia			=(float)atof((_bstr_t)pRS->GetCollect("HoleDia"));						
		m_CrackNum			=(float)atof((_bstr_t)pRS->GetCollect("CrackNum"));					
		m_CrackLength		=(float)atof((_bstr_t)pRS->GetCollect("CrackLength"));				
		m_CrackWidth		=(float)atof((_bstr_t)pRS->GetCollect("CrackWidth"));				
		m_CrackWaterWidth	=(float)atof((_bstr_t)pRS->GetCollect("CrackWaterWidth"));		
		m_CrackPor			=(float)atof((_bstr_t)pRS->GetCollect("CrackPor"));					
		m_FillUser			=pRS->GetCollect("FillUser");
		m_FillDate			=pRS->GetCollect("FillDate");
		m_Describe			=pRS->GetCollect("Describe");
		m_UpperID			=atol((_bstr_t)pRS->GetCollect("UpperID"));
		return TRUE;
	}
}
