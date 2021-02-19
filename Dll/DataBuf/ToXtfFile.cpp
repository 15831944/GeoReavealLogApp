// ToXtfFile.cpp: implementation of the CToXtfFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToXtfFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToXtfFile::CToXtfFile()
{

}

CToXtfFile::~CToXtfFile()
{

}
bool CToXtfFile::ToXtfFile(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sInFormat,FILE* fo)
{
	fp=fo;
	bool bResult=false;
	if(sInFormat.CompareNoCase("FID")==0)
	{
		bResult=FidToXtf.FidToXtf(SourceFile,sObjectPath,nCurvesRadio,fp);	
	}
	else if(sInFormat.CompareNoCase("SLC716")==0)
	{
		bResult=Slc716ToXtf.Slc716ToXtf(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("WIS")==0)
	{
		bResult=WisToXtf.WisToXtf(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("DLIS")==0)
	{
		bResult=DlisToXtf.DlisToXtf(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	return bResult;
}
