// ToWisFile.cpp: implementation of the CToWisFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToWisFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToWisFile::CToWisFile()
{
}

CToWisFile::~CToWisFile()
{
	
}
bool CToWisFile::ToWisFile(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sInFormat,FILE* fo)
{
	fp=fo;
	bool bResult=false;
	//根据文件类型确定转换函数
	if(sInFormat.CompareNoCase("FID")==0)
	{
		bResult=FidToWis.FidToWis(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("DLIS")==0)
	{
		bResult=DlisToWis.DLisToWis(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("SLC716")==0)
	{
		bResult=Slc716ToWis.Slc716ToWis(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("XTF")==0)
	{
		bResult=XtfToWis.XtfToWis(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("XB716")==0)
	{
		bResult=Xb716ToWis.Xb716ToWis(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	return bResult;
}

