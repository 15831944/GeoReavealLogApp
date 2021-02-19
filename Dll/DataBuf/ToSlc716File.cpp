// ToSlc716File.cpp: implementation of the CToSlc716File class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToSlc716File.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToSlc716File::CToSlc716File()
{
}

CToSlc716File::~CToSlc716File()
{
}
bool CToSlc716File::ToSlc716(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,int m_nConvEncode,float m_FixRlev,FILE* fo)
{
	fp=fo;
	bool bResult=false;
	//根据文件类型确定转换函数
	if(sInputFormat.CompareNoCase("SLC716")==0)
	{
		bResult=Slc716ToSlc716.Slc716ToSlc716(SourceFile,ObjectPath,m_nCurvesRadio,fp);
	}
	else if(sInputFormat.CompareNoCase("BIT")==0)
	{
		bResult=BitToSlc716.BitToSlc716(SourceFile,ObjectPath,m_nCurvesRadio,fp);
	}
	else if(sInputFormat.CompareNoCase("SKC92")==0)
	{
		bResult=Skc92ToSlc716.Skc92ToSlc716(SourceFile,ObjectPath,m_nCurvesRadio,fp);
	}
	else if(sInputFormat.CompareNoCase("SKC98")==0)
	{
		bResult=Skc98ToSlc716.Skc98ToSlc716(SourceFile,ObjectPath,m_nCurvesRadio,fp);
	}
	else if(sInputFormat.CompareNoCase("SKH2000")==0)
	{
		bResult=Skh2000ToSlc716.Skh2000ToSlc716(SourceFile,ObjectPath,m_nCurvesRadio,fp);
	}
	else if(sInputFormat.CompareNoCase("FID")==0)
	{
		bResult=FidToSlc716.FidToSlc716(SourceFile,ObjectPath,m_nCurvesRadio,m_nConvEncode,m_FixRlev,fp);
	}
	else if(sInputFormat.CompareNoCase("DLIS")==0)
	{
		bResult=DLisToSlc716.DLisToSlc716(SourceFile,ObjectPath,m_nCurvesRadio,m_nConvEncode,m_FixRlev,fp);
	}
	else if(sInputFormat.CompareNoCase("WIS")==0)
	{
		bResult=WisToSlc716.WisToSlc716(SourceFile,ObjectPath,m_nCurvesRadio,m_nConvEncode,m_FixRlev,fp);
	}
	else if(sInputFormat.CompareNoCase("XTF")==0)
	{
		bResult=XtfToSlc716.XtfToSlc716(SourceFile,ObjectPath,m_nCurvesRadio,m_nConvEncode,m_FixRlev,fp);
	}

	return bResult;
}
