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
	m_LayerName= _T("");		//层位名称
	m_LayerAlias= _T("");		//层位别名
	m_Number= _T("");			//标注序号
	m_DepSt=0.f;  				//起始深度
	m_DepEn=0.f;  				//结束深度
	m_DepHD=0.f;				//厚度
	m_DepHD1=0.f;				//有效厚度
	m_POR=-999.25f;				//孔隙度
	m_PERMA=-999.25f;			//渗透率
	m_SW=-999.25f;				//含水饱和度
	m_SH=-999.25f;				//泥质含量
	m_Conclusion= _T("");		//解释结论
	m_SP=-999.25f;				//自然电位
	m_GR=-999.25f;				//自然伽玛
	m_CGR=-999.25f;				//无铀伽玛
	m_DEN=-999.25f;				//密度
	m_CNL=-999.25f;				//中子
	m_AC=-999.25f;				//声波
	m_RT=-999.25f;				//深电阻率
	m_RXO=-999.25f;				//中电阻率
	m_RS=-999.25f;				//浅电阻率
	m_PorMax=-999.25f;			//最大孔隙度
	m_SwMax=-999.25f;			//最大含水饱和度
	m_HoleNum=-999.25f;			//总孔洞数
	m_HolePor=-999.25f;			//总面孔率
	m_HoleDia=-999.25f;			//平均孔径
	m_CrackNum=-999.25f;		//裂缝条数
	m_CrackLength=-999.25f;		//裂缝长度
	m_CrackWidth=-999.25f;		//裂缝宽度
	m_CrackWaterWidth=-999.25f;	//裂缝水动力宽度
	m_CrackPor=-999.25f;		//裂缝面孔率
	m_FillUser= _T("");			//提交人
	m_FillDate= _T("");			//提交日期
	m_Describe= _T("");			//解释描述
	m_UpperID = 0;				//存储空间编号
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
	CString sPorMax;			sPorMax.Format("%.2f",			m_PorMax);			//最大孔隙度
	CString sSwMax;				sSwMax.Format("%.2f",			m_SwMax);			//最大含水饱和度
	CString sHoleNum;			sHoleNum.Format("%.2f",			m_HoleNum);			//总孔洞数
	CString sHolePor;			sHolePor.Format("%.2f",			m_HolePor);			//总面孔率
	CString sHoleDia;			sHoleDia.Format("%.2f",			m_HoleDia);			//平均孔径
	CString sCrackNum;			sCrackNum.Format("%.2f",		m_CrackNum);		//裂缝条数
	CString sCrackLength;		sCrackLength.Format("%.2f",		m_CrackLength);		//裂缝长度
	CString sCrackWidth;		sCrackWidth.Format("%.2f",		m_CrackWidth);		//裂缝宽度
	CString sCrackWaterWidth;	sCrackWaterWidth.Format("%.2f",	m_CrackWaterWidth);	//裂缝水动力宽度
	CString sCrackPor;			sCrackPor.Format("%.2f",		m_CrackPor);		//裂缝面孔率

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
	CString sPorMax;			sPorMax.Format("%.2f",			m_PorMax);			//最大孔隙度
	CString sSwMax;				sSwMax.Format("%.2f",			m_SwMax);			//最大含水饱和度
	CString sHoleNum;			sHoleNum.Format("%.2f",			m_HoleNum);			//总孔洞数
	CString sHolePor;			sHolePor.Format("%.2f",			m_HolePor);			//总面孔率
	CString sHoleDia;			sHoleDia.Format("%.2f",			m_HoleDia);			//平均孔径
	CString sCrackNum;			sCrackNum.Format("%.2f",		m_CrackNum);		//裂缝条数
	CString sCrackLength;		sCrackLength.Format("%.2f",		m_CrackLength);		//裂缝长度
	CString sCrackWidth;		sCrackWidth.Format("%.2f",		m_CrackWidth);		//裂缝宽度
	CString sCrackWaterWidth;	sCrackWaterWidth.Format("%.2f",	m_CrackWaterWidth);	//裂缝水动力宽度
	CString sCrackPor;			sCrackPor.Format("%.2f",		m_CrackPor);		//裂缝面孔率

	CString sUpperID;	sUpperID.Format("%lu",m_UpperID);
	CString cID;
	cID.Format("%lu",iResultID);
	CString str("UPDATE Result SET	  LayerName='"		+		m_LayerName	          //层位名称   
									+ "', LayerAlias='"		+		m_LayerAlias		  //层位别名	
									+ "', Number='"			+		m_Number			  //标注序号
									+ "', DepSt='"  		+		sDepSt   			  //起始深度
									+ "', DepEn='"  		+		sDepEn  			  //结束深度
									+ "', DepHD='"			+		sDepHD				  //厚度
									+ "', DepHD1='"			+		sDepHD1				  //有效厚度
									+ "', POR='"			+		sPOR				  //孔隙度		
									+ "', PERMA='"			+		sPERMA				  //渗透率
									+ "', SW='"				+		sSW					  //含水饱和度
									+ "', SH='"				+		sSH					  //泥质含量
									+ "', Conclusion='"		+		m_Conclusion		  //解释结论
									+ "', SP='"				+		sSP					  //自然电位
									+ "', GR='"				+		sGR					  //自然伽玛
									+ "', CGR='"			+		sCGR				  //无铀伽玛
									+ "', DEN='"			+		sDEN				  //密度
									+ "', CNL='"			+		sCNL				  //中子
									+ "', AC='"				+		sAC					  //声波
									+ "', RT='"				+		sRT					  //深电阻率
									+ "', RXO='"			+		sRXO				  //中电阻率
									+ "', RS='"				+		sRS					  //浅电阻率	
									+ "', PorMax='"			+		sPorMax				  //最大孔隙度
									+ "', SwMax='"			+		sSwMax				  //最大含水饱和度		
									+ "', HoleNum='"		+		sHoleNum			  //总孔洞数	
									+ "', HolePor='"		+		sHolePor			  //总面孔率	
									+ "', HoleDia='"		+		sHoleDia			  //平均孔径	
									+ "', CrackNum='"		+		sCrackNum			  //裂缝条数	
									+ "', CrackLength='"	+		sCrackLength		  //裂缝长度
									+ "', CrackWidth='"		+		sCrackWidth			  //裂缝宽度
									+ "', CrackWaterWidth='"+		sCrackWaterWidth	  //裂缝水动力宽度
									+ "', CrackPor='"		+		sCrackPor			  //裂缝面孔率	
									+ "', Describe='"		+		m_Describe 			 //解释描述
									+ "', FillUser='"		+		m_FillUser			 //提交人
									+ "', FillDate='"		+		m_FillDate			 //提交日期
									+ "', UpperID="			+		sUpperID			 //存储空间编号
				+ "' WHERE RecordID=" + cID );
	return Execute(str);	
}

BOOL CGeoResult::sql_delete(DWORD iResultID)
{
	CString cID;
	cID.Format("%lu",iResultID);
	return Execute("DELETE FROM Result WHERE RecordID=" + cID);
}

//根据井信息编号读取所有字段值
BOOL CGeoResult::GetData(DWORD iResultID)
{
	CString cID;
	cID.Format("%lu",iResultID);
	_RecordsetPtr pRS = GetRecordSet("SELECT * FROM Result WHERE RecordID=" + cID);

	//返回各列的值
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
