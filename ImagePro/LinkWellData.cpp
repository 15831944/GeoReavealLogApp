// LinkWellData.cpp: implementation of the CLinkWellData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowChart.h"
#include "LinkWellData.h"
#include "FlowChartDoc.h"
#include "FIDio.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CLinkWellData, CFlowNode, 0)

CLinkWellData::CLinkWellData()
{
	m_pDoc=NULL;
}

CLinkWellData::CLinkWellData(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
}

CLinkWellData::~CLinkWellData()
{

}

BOOL CLinkWellData:: Run()
{
	static char BASED_CODE szFilter[] = 
	"测井数据(*.FID)|*.FID|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "测井数据(*.FID)", "*.FID", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if (Browse.DoModal()==IDOK)
	{
		m_szLogName = Browse.GetPathName();
		m_szSeries= FindLogSeries();
		m_bResult=TRUE;
		if(m_pDoc!=NULL)
			m_pDoc->SetModifiedFlag();
	}
	return m_bResult;
}

void CLinkWellData::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_szSeries;
		ar<<m_szLogName;
		ar<<m_szWellName; 
	}
	else
	{
		ar>>m_szSeries;
		ar>>m_szLogName;
		ar>>m_szWellName; 
		CString str=m_szWellName.Mid(m_szWellName.ReverseFind('#')+1);
		str=str.Left(str.ReverseFind('\\'));
		m_szWellName=GetCurrentUserPath()+"#"+str+"\\"+str;
		m_szLogName=m_szWellName+".FID";
	}	
}

CString CLinkWellData::FindLogSeries()
{	
	CString szSeries="";
	m_szWellName=m_szLogName.Left (m_szLogName.ReverseFind ('.'));
	CString Path=m_szLogName.Left (m_szLogName.ReverseFind ('\\'));
	if(DetectFileExist(Path+"\\*.P1BTN"))
			szSeries="STARII";
	else if(DetectFileExist(Path+"\\*.P1BTNQH"))
		szSeries="STARV";
	else if(DetectFileExist(Path+"\\*.PAD1"))
		szSeries="EMI";
	else if(DetectFileExist(Path+"\\*.FCA1"))
	{
		if(DetectFileExist(Path+"\\*.FCA3")
			&&DetectFileExist(Path+"\\*.FCA4"))
			szSeries="FMI_8PAD";
		else
			szSeries="FMI_4PAD";
	}
	else if(DetectFileExist(Path+"\\*.BTN1"))
		szSeries="DIP6";
	else if(DetectFileExist(Path+"\\*.PDD1"))
		szSeries="SED";
	else if(DetectFileExist(Path+"\\*.BHTAQH"))
		szSeries="CBIL";
	else if(DetectFileExist(Path+"\\*.AMP"))
		szSeries="CAST";
	else if(DetectFileExist(Path+"\\*.BZ1")
		&&DetectFileExist(Path+"\\*.BZ2")
		&&DetectFileExist(Path+"\\*.BZ3")
		&&DetectFileExist(Path+"\\*.BZ4")
		&&DetectFileExist(Path+"\\*.BZ5")
		&&DetectFileExist(Path+"\\*.BZ6")
		&&DetectFileExist(Path+"\\*.BZ7")
		&&DetectFileExist(Path+"\\*.BZ8"))
		szSeries="AMK2000";
	else if(DetectFileExist(Path+"\\*.IAI01")
		&&DetectFileExist(Path+"\\*.IAI12")
		&&DetectFileExist(Path+"\\*.IAI03")
		&&DetectFileExist(Path+"\\*.IAI14")
		&&DetectFileExist(Path+"\\*.IAI05")
		&&DetectFileExist(Path+"\\*.IAI16")
		&&DetectFileExist(Path+"\\*.IAI07")
		&&DetectFileExist(Path+"\\*.IAI09")
		&&DetectFileExist(Path+"\\*.IAI11"))
		szSeries="ARI_16";
	return szSeries;
}
