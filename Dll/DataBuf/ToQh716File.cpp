// ToQh716File.cpp: implementation of the ToQh716File class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToQh716File.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToQh716File::CToQh716File()
{

}

CToQh716File::~CToQh716File()
{

}
bool CToQh716File::ToQh716File(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,int m_nConvEncode,float m_FixRlev,FILE *fo)
{
	fp=fo;
	bool bResult=false;
	//根据文件类型确定转换函数
	if(sInputFormat.CompareNoCase("FID")==0)
	{
		bResult=FidToQh716.FidToQh716(SourceFile,ObjectPath,m_nCurvesRadio,m_nConvEncode,m_FixRlev,fp);
	}
	else if(sInputFormat.CompareNoCase("WIS")==0)
	{
		bResult=WisToQh716.WisToQh716(SourceFile,ObjectPath,m_nCurvesRadio,m_nConvEncode,m_FixRlev,fp);
	}
	return bResult;
}
