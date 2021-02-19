// ToDLisFile.cpp: implementation of the CToDLisFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToDLisFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToDLisFile::CToDLisFile()
{

}

CToDLisFile::~CToDLisFile()
{

}
bool CToDLisFile::ToDlisFile(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sInFormat,CString sProducer,FILE* fo)
{
	fp=fo;
	bool bResult=false;
	//根据文件类型确定转换函数
	if(sInFormat.CompareNoCase("FID")==0)
	{
		bResult=FidToDlis.FidToDlis(SourceFile,sObjectPath,nCurvesRadio,sProducer,fp);
	}
	else if(sInFormat.CompareNoCase("SLC716")==0)
	{
		bResult=Slc716ToDlis.Slc716ToDlis(SourceFile,sObjectPath,nCurvesRadio,sProducer,fp);
	}
	else if(sInFormat.CompareNoCase("XTF")==0)
	{
		bResult=XtfToDLis.XtfToDLis(SourceFile,sObjectPath,nCurvesRadio,sProducer,fp);
	}
	else if(sInFormat.CompareNoCase("WIS")==0)
	{
		bResult=WisToDLis.WisToDLis(SourceFile,sObjectPath,nCurvesRadio,sProducer,fp);
	}
	else if(sInFormat.CompareNoCase("BIT")==0)
	{
		bResult=BitToDLis.BitToDLis(SourceFile,sObjectPath,nCurvesRadio,sProducer,fp);
	}
	else if(sInFormat.CompareNoCase("SKC92")==0)
	{
		bResult=Skc92ToDLis.Skc92ToDLis(SourceFile,sObjectPath,nCurvesRadio,sProducer,fp);
	}
	else if(sInFormat.CompareNoCase("SKC98")==0)
	{
		bResult=Skc98ToDLis.Skc98ToDLis(SourceFile,sObjectPath,nCurvesRadio,sProducer,fp);
	}
	else if(sInFormat.CompareNoCase("SKH2000")==0)
	{
		bResult=Skh2000ToDLis.Skh2000ToDLis(SourceFile,sObjectPath,nCurvesRadio,sProducer,fp);
	}
	else if(sInFormat.CompareNoCase("LIS")==0)
	{
		bResult=LisToDlis.LisToDlis(SourceFile,sObjectPath,nCurvesRadio,sProducer,fp);
	}

	return bResult;
}

