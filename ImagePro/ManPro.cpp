// ManPro.cpp: implementation of the CManPro class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowChart.h"
#include "FlowChartDoc.h"
#include "FlowNode.h"
#include "ManPro.h"
#include "FIDio.h"
#include <process.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CManPro, CFlowNode, 0)

CManPro::CManPro()
{
	m_pDoc=NULL;
}

CManPro::CManPro(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
}

CManPro::~CManPro()
{

}

BOOL CManPro:: Run()
{
	m_bResult=TRUE;
	if(m_pDoc->m_bAunAll)
		return TRUE;
	char str[512];
	char *args[3];	
	CString ExcFileName=GetWorkPath("System")+"ImageMan";
	strcpy(str,ExcFileName);
	args[0]=str;
	CString DataFileName,szdep,szbit;	
	szdep.Format ("<%.1f,%.1f^%s",m_StProDep,m_EnProDep,m_szBufPath);
	
	if(m_Bits>300)
		szbit="_311.LPT";
	else if(m_Bits>200)
		szbit="_216.LPT";
	else
		szbit="_152.LPT";

	DataFileName=m_szWellName+".FID>"+GetWorkPath("Template")+"Image\\"+m_szSeries+szbit+szdep;
	strcpy(str,DataFileName);
	args[1]=str;
	args[2] = NULL;
	_spawnv( _P_NOWAIT, ExcFileName, args );
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	return m_bResult;
}

void CManPro::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
}

void CManPro::InitPara(CFlowChartDoc* pDoc)
{	
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		CFlowNode *pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{		
			if(pObj->IsKindOf(RUNTIME_CLASS(CBackupData)))
			{
				m_szBufPath=((CBackupData*)pObj)->m_szBufPath;
				m_szWellName=((CBackupData*)pObj)->m_szWellName;
				m_szSeries=((CBackupData*)pObj)->m_szSeries;
				m_PadNum=((CBackupData*)pObj)->m_PadNum; 
				m_ButtonNum=((CBackupData*)pObj)->m_ButtonNum; 
				m_ButtonMid=((CBackupData*)pObj)->m_ButtonMid;
//				m_DataNum=((CBackupData*)pObj)->m_DataNum;
				m_CalNum=((CBackupData*)pObj)->m_CalNum;
				m_StProDep=((CBackupData*)pObj)->m_StProDep;
				m_EnProDep=((CBackupData*)pObj)->m_EnProDep;
				m_Rlev=((CBackupData*)pObj)->m_Rlev;
//				m_WinLongth=((CBackupData*)pObj)->m_WinLongth;          
				m_Bits=((CBackupData*)pObj)->m_Bits; 
			}
		}
	}
	m_pDoc=pDoc;
}