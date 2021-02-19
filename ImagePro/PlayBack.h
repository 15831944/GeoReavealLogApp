// PlayBack.h: interface for the CPlayBack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYBACK_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
#define AFX_PLAYBACK_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FlowNode.h"
class 	CFlowChartDoc;
class CPlayBack : public CFlowNode  
{
	DECLARE_SERIAL(CPlayBack);
public:
	CPlayBack();
	CPlayBack(CString szNodename,BOOL bResult);
	virtual ~CPlayBack();
	// Attributes
public:
	CString	m_szLogName;             //文件名
	CString m_szSeries;              //测井系列
public:
	virtual BOOL Run();
	virtual void InitPara(CFlowChartDoc* pDoc);
	virtual void Serialize(CArchive& ar);
};

#endif // !defined(AFX_PLAYBACK_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
