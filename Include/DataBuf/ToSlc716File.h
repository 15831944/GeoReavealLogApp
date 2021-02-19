// TransformFile.h: interface for the CTransformFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOSLC716FILE_H_INCLUDED_)
#define AFX_TOSLC716FILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Slc716ToSlc716.h"
#include "BitToSlc716.h"
#include "Skc92ToSlc716.h"
#include "Skc98ToSlc716.h"
#include "Skh2000ToSlc716.h"
#include "FidToSlc716.h"
#include "DLisToSlc716.h"
#include "WisToSlc716.h"
#include "XtfToSlc716.h"

class AFX_EXT_CLASS CToSlc716File  
{
public:
	CToSlc716File();
	virtual ~CToSlc716File();
	bool ToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,int m_nConvEncode,float m_FixRlev,FILE* fo);
	FILE* fp;
private:
	CSlc716ToSlc716 Slc716ToSlc716;
	CBitToSlc716 BitToSlc716;
	CSkc92ToSlc716 Skc92ToSlc716;
	CSkc98ToSlc716 Skc98ToSlc716;
	CSkh2000ToSlc716 Skh2000ToSlc716;
	CFidToSlc716 FidToSlc716;
	CDLisToSlc716 DLisToSlc716;
	CWisToSlc716 WisToSlc716;
	CXtfToSlc716 XtfToSlc716;
};

#endif // !defined(AFX_TOSLC716FILE_H_INCLUDED_)
