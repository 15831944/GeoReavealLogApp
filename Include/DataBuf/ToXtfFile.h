// ToXtfFile.h: interface for the CToXtfFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOXTFFILE_H_INCLUDED_)
#define AFX_TOXTFFILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FidToXtf.h"
#include "Slc716ToXtf.h"
#include "WisToXtf.h"
#include "DlisToXtf.h"

class AFX_EXT_CLASS CToXtfFile  
{
public:
	CToXtfFile();
	virtual ~CToXtfFile();
	bool ToXtfFile(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString nInputFormat,FILE* fo);
	FILE* fp;
private:
	CFidToXtf FidToXtf;
	CSlc716ToXtf Slc716ToXtf;
	CWisToXtf WisToXtf;
	CDLisToXtf DlisToXtf;
};

#endif // !defined(AFX_TOXTFFILE_H_INCLUDED_)
