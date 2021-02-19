// FlowNode.cpp: implementation of the CFlowNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowChart.h"
#include "FlowNode.h"
#include "FlowChartDoc.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CFlowNode, CObject, 0)

CFlowNode::CFlowNode()
{
	m_szNodeName=_T("");
	m_szMessage=_T("");
	m_bResult=FALSE;
	m_pDoc=NULL;
	m_Ver=1.00f;
}

CFlowNode::~CFlowNode()
{

}

BOOL CFlowNode:: Run()
{
	m_bResult=TRUE;
	return TRUE;
}

void CFlowNode::InitPara(CFlowChartDoc* pDoc)
{
	m_pDoc=pDoc;
}

void CFlowNode::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_szNodeName;
		ar << m_bResult;
		ar << m_szMessage;
		ar << m_Ver;
	}
	else
	{
		ar >> m_szNodeName;
		if(m_szNodeName=="��Чͼ����ǿ����"||m_szNodeName=="ͼ��ȫ���ڻָ�")
			m_szNodeName="����ͼ����ά�ؽ�";
		if(m_szNodeName=="���ģʽʶ��")
			m_szNodeName="ȫ����ͼ��ָ�";
		ar >> m_bResult;
		ar >> m_szMessage;
		ar >> m_Ver;
	}	
}