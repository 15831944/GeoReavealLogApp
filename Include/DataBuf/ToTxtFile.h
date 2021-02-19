// ToTxtFile.h: interface for the ToTxtFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOTXTFILE_H_INCLUDED_)
#define AFX_TOTXTFILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FidToTxt.h"
#include "WisToTxt.h"

class AFX_EXT_CLASS CToTxtFile  
{
public:
	CToTxtFile();
	virtual ~CToTxtFile();
	bool ToTxtFile(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,int m_nConvEncode,float m_FixRlev,FILE *fo);
	FILE* fp;
private:
	CFidToTxt FidToTxt;
	CWisToTxt WisToTxt;
};

#endif // !defined(AFX_TOTXTFILE_H_INCLUDED_)
