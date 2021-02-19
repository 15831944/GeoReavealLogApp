// ToDLisFile.h: interface for the CToDLisFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TODLISFILE_H_INCLUDED_)
#define AFX_TODLISFILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FidToDLis.h"
#include "Slc716ToDLis.h"
#include "XtfToDLis.h"
#include "WisToDLis.h"
#include "BitToDLis.h"
#include "Skc92ToDLis.h"
#include "Skc98ToDLis.h"
#include "Skh2000ToDLis.h"
#include "LisToDLis.h"

class AFX_EXT_CLASS CToDLisFile  
{
public:
	CToDLisFile();
	virtual ~CToDLisFile();
	bool ToDlisFile(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sInFormat,CString m_sProducer,FILE* fo);
private:
	CFidToDLis FidToDlis;
	CSlc716ToDLis Slc716ToDlis;
	CXtfToDLis XtfToDLis;
	CWisToDLis WisToDLis;
	CBitToDLis BitToDLis;
	CSkc92ToDLis Skc92ToDLis;
	CSkc98ToDLis Skc98ToDLis;
	CSkh2000ToDLis Skh2000ToDLis;
	CLisToDlis LisToDlis;
	FILE *fp;
};	


#endif // !defined(AFX_TODLISFILE_H_INCLUDED_)
