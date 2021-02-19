// ToLisFile.h: interface for the CToLisFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOLISFILE_H_INCLUDED_)
#define AFX_TOLISFILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FidToLis.h"
#include "DlisToLis.h"

class AFX_EXT_CLASS CToLisFile  
{
public:
	CToLisFile();
	virtual ~CToLisFile();
	bool ToLisFile(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,FILE* fo);
	FILE* fp;
private:
	CFidToLis FidToLis;
	CDlisToLis DlisToLis;

};

#endif // !defined(AFX_TOLISFILE_H_INCLUDED_)
