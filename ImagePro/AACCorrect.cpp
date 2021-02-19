// AACCorrect.cpp: implementation of the CAACCorrect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "AACCorrect.h"
#include "FlowChartDoc.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CAACCorrect, CDepCorrect, 0)
CAACCorrect::CAACCorrect()
{
	m_pDoc=NULL;
}

CAACCorrect::CAACCorrect(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_szBufPath=_T("");
	m_szWellName=_T("");
	m_szSeries=_T("");
	m_szAACName=_T("");
	m_PadNum=6; 
	m_ButtonNum=24;
	m_ButtonMid=12;
	m_CalNum=6; 
	m_StProDep=m_EnProDep=-9999.f;
}


CAACCorrect::~CAACCorrect()
{

}

void CAACCorrect::Serialize(CArchive& ar)
{
	CDepCorrect::Serialize(ar);	
	if (ar.IsStoring())
		ar<<m_szAACName;
	else
		ar>>m_szAACName;
}

BOOL CAACCorrect::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	m_bResult=TRUE;
	return m_bResult;
}

