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
	m_DEN=-999.25f;				//密度
	m_CNL=-999.25f;				//中子
	m_AC=-999.25f;				//声波
	m_RT=-999.25f;				//深电阻率
	m_RXO=-999.25f;				//中电阻率
	m_RS=-999.25f;				//浅电阻率
	m_FillUser= _T("");			//提交人
	m_FillDate= _T("");			//提交日期
	m_Describe= _T("");			//解释描述
	m_UpperID = 0;				//存储空间编号
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
	//获取刚才插入的存储空间信息中的StorageID值(数据库中StorageID字段是自动增加的)
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
									+ "', DEN='"			+		sDEN				  //密度
									+ "', CNL='"			+		sCNL				  //中子
									+ "', AC='"				+		sAC					  //声波
									+ "', RT='"				+		sRT					  //深电阻率
									+ "', RXO='"			+		sRXO				  //中电阻率
									+ "', RS='"				+		sRS					 //浅电阻率						
									+ "', Describe='"		+		m_Describe 			 //解释描述
									+ "', FillUser='"		+		m_FillUser			 //提交人
									+ "', FillDate='"		+		m_FillDate			 //提交日期
									+ "', UpperID="			+		sUpperID			 //存储空间编号
				+ "' WHERE ResultID=" + cID );
	return m_ado.Execute(str);	
}

BOOL CGeoResult::sql_delete(int iResultID)
{
	CString cID;
	cID.Format("%d",iResultID);
	return m_ado.Execute("DELETE FROM Result WHERE ResultID=" + cID);
}

//根据井信息编号读取所有字段值
BOOL CGeoResult::GetData(int iResultID)
{
	CString cID;
	cID.Format("%d",iResultID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Result WHERE ResultID=" + cID);

	//返回各列的值
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
