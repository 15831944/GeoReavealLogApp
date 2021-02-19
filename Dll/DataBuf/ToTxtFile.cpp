// ToTxtFile.cpp: implementation of the ToTxtFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToTxtFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToTxtFile::CToTxtFile()
{

}

CToTxtFile::~CToTxtFile()
{

}
bool CToTxtFile::ToTxtFile(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,int m_nConvEncode,float m_FixRlev,FILE *fo)
{
	fp=fo;
	bool bResult=false;
	//根据文件类型确定转换函数
	if(sInputFormat.CompareNoCase("FID")==0)
	{
		bResult=FidToTxt.FidToTxt(SourceFile,ObjectPath,m_nCurvesRadio,m_nConvEncode,m_FixRlev,fp);
	}
	else if(sInputFormat.CompareNoCase("WIS")==0)
	{
		bResult=WisToTxt.WisToTxt(SourceFile,ObjectPath,m_nCurvesRadio,m_nConvEncode,m_FixRlev,fp);
	}

	return bResult;
}