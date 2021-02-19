// ToLisFile.cpp: implementation of the CToLisFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToLisFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToLisFile::CToLisFile()
{
}

CToLisFile::~CToLisFile()
{
	
}
bool CToLisFile::ToLisFile(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sInFormat,FILE* fo)
{	
	fp=fo;
	bool bResult=false;
	//根据文件类型确定转换函数
	if(sInFormat.CompareNoCase("FID")==0)
	{
		bResult=FidToLis.FidToLis(SourceFile,sObjectPath,nCurvesRadio,fp);
	}	
	else if(sInFormat.CompareNoCase("DLIS")==0)
	{
		bResult=DlisToLis.DlisToLis(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	return bResult;
}

