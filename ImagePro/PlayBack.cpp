// PlayBack.cpp: implementation of the CPlayBack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowChart.h"
#include "FlowChartDoc.h"
#include "FlowNode.h"
#include "LinkWellData.h"
#include "PlayBack.h"
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
IMPLEMENT_SERIAL(CPlayBack, CFlowNode, 0)

CPlayBack::CPlayBack()
{
	m_pDoc=NULL;
}

CPlayBack::CPlayBack(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
}

CPlayBack::~CPlayBack()
{

}

BOOL CPlayBack:: Run()
{
	char str[512];
	char *args[3];	
	CString ExcFileName=GetWorkPath("System")+"ImageMan";
	strcpy(str,ExcFileName);
	args[0]=str;
	CString DataFileName=m_szLogName+">"+GetWorkPath("Template")+"Image\\"+m_szSeries+"_Curve.LPT";
	strcpy(str,DataFileName);
	args[1]=str;
	args[2] = NULL;
	_spawnv( _P_NOWAIT, ExcFileName, args );
	m_bResult=TRUE;
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	return m_bResult;
}

void CPlayBack::InitPara(CFlowChartDoc* pDoc)
{
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		CFlowNode *pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{		
			if(pObj->IsKindOf(RUNTIME_CLASS(CLinkWellData)))
			{
				m_szLogName=((CLinkWellData*)pObj)->m_szLogName;
				m_szSeries=((CLinkWellData*)pObj)->m_szSeries;
			}
		}
	}
	m_pDoc=pDoc;
}

void CPlayBack::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_szLogName;
		ar<<m_szSeries; 
	}
	else
	{
		ar>>m_szLogName;
		ar>>m_szSeries; 
	}	
}