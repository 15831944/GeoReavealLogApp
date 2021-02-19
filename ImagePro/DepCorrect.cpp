// DepCorrect.cpp: implementation of the CDepCorrect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowChart.h"
#include "DepCorrect.h"
#include "FlowChartDoc.h"
#include "FlowNode.h"
#include "BackupData.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CDepCorrect, CFlowNode, 0)
CDepCorrect::CDepCorrect()
{
	m_pDoc=NULL;
}

CDepCorrect::~CDepCorrect()
{

}

void CDepCorrect::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);		
}

void CDepCorrect::InitPara(CFlowChartDoc* pDoc)
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
				m_CalNum=((CBackupData*)pObj)->m_CalNum;
				m_StProDep=((CBackupData*)pObj)->m_StProDep;
				m_EnProDep=((CBackupData*)pObj)->m_EnProDep;
				m_Rlev=((CBackupData*)pObj)->m_Rlev;
				m_ButtonMid=12;
			}
		}
	}
	m_pDoc=pDoc;
}