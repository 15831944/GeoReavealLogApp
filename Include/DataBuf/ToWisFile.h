// ToWisFile.h: interface for the CToWisFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOWISFILE_H_INCLUDED_)
#define AFX_TOWISFILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FidToWis.h"
#include "DLisToWis.h"
#include "Slc716ToWis.h"
#include "XtfToWis.h"
#include "Xb716ToWis.h"

class AFX_EXT_CLASS CToWisFile  
{
public:
	CToWisFile();
	virtual ~CToWisFile();
	bool ToWisFile(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,FILE* fo);
	FILE* fp;
private:
	CFidToWis FidToWis;
	CDLisToWis DlisToWis;
	CSlc716ToWis Slc716ToWis;
	CXtfToWis XtfToWis;
	CXb716ToWis Xb716ToWis;

};

#endif // !defined(AFX_TOWISFILE_H_INCLUDED_)
