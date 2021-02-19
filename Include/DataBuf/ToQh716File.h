// ToQh716File.h: interface for the ToQh716File class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOQH716FILE_H_INCLUDED_)
#define AFX_TOQH716FILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FidToQh716.h"
#include "WisToQh716.h"

class AFX_EXT_CLASS CToQh716File  
{
public:
	CToQh716File();
	virtual ~CToQh716File();
	bool ToQh716File(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,int m_nConvEncode,float m_FixRlev,FILE *fo);
	FILE* fp;
private:
	CFidToQh716 FidToQh716;
	CWisToQh716 WisToQh716;

};

#endif // !defined(AFX_TOQH716FILE_H_INCLUDED_)
