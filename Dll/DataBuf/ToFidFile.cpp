// ToFidFile.cpp: implementation of the CToFidFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToFidFile.h"
#include "ReCurNamDlg.h"
#include "FIDIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToFidFile::CToFidFile()
{
}

CToFidFile::~CToFidFile()
{
//	CurveIndex.RemoveAll();
}
bool CToFidFile::ToFidFile(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sInFormat,BOOL bCurveSplit,BOOL bDelSplitCur,FILE* fo)
{
	fp=fo;
	bool bResult=false;
	//根据文件类型确定转换函数
	if(sInFormat.CompareNoCase("SLC716")==0)
	{
		bResult=Slc716ToFid.Slc716ToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("FID")==0)
	{
		bResult=FidToFid.FidToFid(SourceFile,sObjectPath,nCurvesRadio,fp);	
	}
	else if(sInFormat.CompareNoCase("WIS")==0)
	{
		bResult=WisToFid.WisToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("XTF")==0)
	{
		bResult=XtfToFid.XtfToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("LIS")==0)
	{
		bResult=LisToFid.LisToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("DLIS")==0)
	{
		bResult=DLisToFid.DLisToFid(SourceFile,sObjectPath,nCurvesRadio,bCurveSplit,bDelSplitCur,fp);
	}
	else if(sInFormat.CompareNoCase("BIT")==0)
	{
		bResult=BitToFid.BitToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("SKC92")==0)
	{
		bResult=Skc92ToFid.Skc92ToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("SKC98")==0)
	{
		bResult=Skc98ToFid.Skc98ToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("SKH2000")==0)
	{
		bResult=Skh2000ToFid.Skh2000ToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("TXT")==0)
	{
		bResult=TxtToFid.TxtToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("QH716")==0)
	{
		bResult=Qh716ToFid.Qh716ToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("XB716")==0)
	{
		bResult=Xb716ToFid.Xb716ToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	else if(sInFormat.CompareNoCase("LAS")==0)
	{
		bResult=LasToFid.LasToFid(SourceFile,sObjectPath,nCurvesRadio,fp);
	}
	if(bResult)
	{
		CString WellName=SourceFile;
		WellName=WellName.Mid(WellName.ReverseFind('\\')+1);
		int x=WellName.Find(".");
		if(x>0) WellName=WellName.Left(x);
		WellName.MakeUpper();
		if(WellName.Left (1)=="D") 
			WellName="X"+WellName;
		if(WellName.Left (1)=="I") 
			WellName="X"+WellName;
		WellName=sObjectPath+"\\#"+WellName+"\\"+WellName;
		char TargetCurveName[256];
		strcpy(TargetCurveName,WellName);
		ReWriteCurveDBF(TargetCurveName);
		strcat(TargetCurveName,".FID");
		RefurbishFID(TargetCurveName);
	}
	return bResult;
}
