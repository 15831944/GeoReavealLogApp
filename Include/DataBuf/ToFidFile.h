// ToFidFile.h: interface for the CToFidFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOFIDFILE_H_INCLUDED_)
#define AFX_TOFIDFILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Slc716ToFid.h"
#include "FidToFid.h"
#include "WisToFid.h"
#include "XtfToFid.h"
#include "LisToFid.h"
#include "DLisToFid.h"
#include "BitToFid.h"
#include "Skc92ToFid.h"
#include "Skc98ToFid.h"
#include "Skh2000ToFid.h"
#include "TxtToFid.h"
#include "Qh716ToFid.h"
#include "Xb716ToFid.h"
#include "LasToFid.h"
class AFX_EXT_CLASS CToFidFile  
{
public:
	CToFidFile();
	virtual ~CToFidFile();
	bool ToFidFile(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString nInputFormat,BOOL bCurveSplit,BOOL bDelSplitCur,FILE* fo);
	CSlc716ToFid Slc716ToFid;
	CFidToFid FidToFid;
	CWisToFid WisToFid;
	CXtfToFid XtfToFid;
	CLisToFid LisToFid;
	CDLisToFid DLisToFid;
	CBitToFid BitToFid;
	CSkc92ToFid Skc92ToFid;
	CSkc98ToFid Skc98ToFid;
	CSkh2000ToFid Skh2000ToFid;
	CTxtToFid TxtToFid;
	CLasToFid LasToFid;
	CQh716ToFid Qh716ToFid;
	CXb716ToFid Xb716ToFid;
	FILE* fp;
};

#endif // !defined(AFX_TOFIDFILE_H_INCLUDED_)
