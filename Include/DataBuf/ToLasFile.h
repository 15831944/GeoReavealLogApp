// ToLasFile.h: interface for the ToLasFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOLASFILE_H_INCLUDED_)
#define AFX_TOLASFILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FidToLas.h"

class AFX_EXT_CLASS CToLasFile  
{
public:
	CToLasFile();
	virtual ~CToLasFile();
	bool ToLasFile(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,int m_nConvEncode,float m_FixRlev,FILE *fo);
	FILE* fp;
private:
	CFidToLas FidToLas;
};

#endif // !defined(AFX_TOLASFILE_H_INCLUDED_)
