// ManPro.h: interface for the CManPro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MANPRO_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
#define AFX_MANPRO_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BackupData.h"
class 	CFlowChartDoc;
class CManPro : public CBackupData  
{
	DECLARE_SERIAL(CManPro);
public:
	CManPro();
	CManPro(CString szNodename,BOOL bResult);
	virtual ~CManPro();
	// Attributes
public:
	CString	m_szWellName;             //�ļ���
	CString m_szSeries;              //�⾮ϵ��
	float   m_Bits;                  //���ڽ�������ģ���һ��ʶ��
	float   m_StProDep;
	float   m_EnProDep;
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	virtual void InitPara(CFlowChartDoc* pDoc);
};

#endif // !defined(AFX_MANPRO_H__F9352E9B_0517_4B95_A3A6_48C924AC7631__INCLUDED_)
