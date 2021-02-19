// LinkWellData.h: interface for the CLinkWellData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINKWELLDATA_H__B0EDBE3F_A4DE_4342_B985_3F224D3BE22C__INCLUDED_)
#define AFX_LINKWELLDATA_H__B0EDBE3F_A4DE_4342_B985_3F224D3BE22C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlowNode.h"

class CLinkWellData : public CFlowNode  
{
	DECLARE_SERIAL(CLinkWellData);
public:
	CLinkWellData();
	CLinkWellData(CString szNodename,BOOL bResult);
	virtual ~CLinkWellData();
	// Attributes
public:
	CString	m_szLogName;                   //文件名
	CString m_szWellName;                  //包括路路径的井名
	CString m_szSeries;                    //测井系列号
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	CString FindLogSeries();
};

#endif // !defined(AFX_LINKWELLDATA_H__B0EDBE3F_A4DE_4342_B985_3F224D3BE22C__INCLUDED_)
