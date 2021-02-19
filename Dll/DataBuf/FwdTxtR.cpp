// FwdTxtR.cpp: implementation of the CFwdTxtR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FwdTxtR.h"
#include <sys/stat.h>
#include <ctype.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFwdTxtR::CFwdTxtR()
{

}

CFwdTxtR::~CFwdTxtR()
{

}

bool _declspec(dllexport) IsFwdTxt(CString sDataFile)
{
	//从外观检查
	struct _stat FileInf;
	int result = _stat(sDataFile, &FileInf );
	if( result != 0 ) return false; //文件不存在
	if( FileInf.st_size<20) return false; //文件大小不符合
	//从内部结构检查
	CString s;
	CStdioFile f;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.ReadString(s);
	f.Close();
	s.TrimRight();
	if((s.CompareNoCase("FORWARD_TEXT_FORMAT_1.0")!=0)
		&&(s.CompareNoCase("LOGEXPRESSTOP_TEXT_FORMAT")!=0))
	{
		return false;
	}
	return true;
}

bool CFwdTxtR::Open(CString sDataFile)
{
	if(fFwdTxt.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fFwdTxt.GetLength();
	return true;
}
void CFwdTxtR::Close()
{
	fFwdTxt.Close();
}
void CFwdTxtR::FwdTxtHdReset()
{
    memset(FwdTxtHd.Mark,'\0',1024);
    FwdTxtHd.Stdep=0;
	FwdTxtHd.Endep=0;
	FwdTxtHd.Rlev=0.125;
    FwdTxtHd.NumLog=0;
    for(int i=0;i<512;i++) memset(FwdTxtHd.CurveName[i],'\0',16);
}

bool CFwdTxtR::ReadHead()
{
	CString s;
	FwdTxtHdReset();
	fFwdTxt.SeekToBegin();
	fFwdTxt.ReadString(s);
	strcpy(FwdTxtHd.Mark,s);
	BOOL bSd,bEd,bRl,bCur;
	bSd=bEd=bRl=bCur=FALSE;
	while(1)
	{
		if(!fFwdTxt.ReadString(s)) break;
		if(s.CompareNoCase("END")==0) break;
		if(s.Find("STDEP")>=0)
		{
			bSd=TRUE;
			FwdTxtHd.Stdep=StringToFloat(s);
		}
		if(s.Find("ENDEP")>=0)
		{
			bEd=TRUE;
			FwdTxtHd.Endep=StringToFloat(s);
		}
		if(s.Find("RLEV")>=0)
		{
			bRl=TRUE;
			FwdTxtHd.Rlev=StringToFloat(s);
		}
		if(s.Find("CURVENAME")>=0)
		{
			bCur=TRUE;
			GetCurveName(s);
		}
		if(bSd&&bEd&&bRl&&bCur) break;
	}
	FwdTxtHd.NumPoint=int((FwdTxtHd.Endep-FwdTxtHd.Stdep)/FwdTxtHd.Rlev)+1;
	return true;
}
float CFwdTxtR::StringToFloat(CString sVal)
{
	int n=sVal.Find("=");
	if(n>=0)
	{
		char str[256];
		strcpy(str,sVal.Mid(n+1));
		return float(atof(str));
	}
	return -999.25f;
}
void CFwdTxtR::GetCurveName(CString sVal)
{
	int n=sVal.Find("=");
	if(n<0) return;
	CString s=sVal.Mid(n+1);
	s.Remove(' ');
	CString sCurveName;
	while(1)
	{
		n=s.Find(',');
		if(n<0)
		{
			sCurveName=s;
			strcpy(FwdTxtHd.CurveName[FwdTxtHd.NumLog],sCurveName);
			FwdTxtHd.NumLog++;
			break;
		}
		sCurveName=s.Left(n);
		sCurveName.TrimLeft ();
		sCurveName.TrimRight ();
		strcpy(FwdTxtHd.CurveName[FwdTxtHd.NumLog],sCurveName);
		FwdTxtHd.NumLog++;
		CString s1=s.Mid(n+1);
		s=s1;
	}
}
void CFwdTxtR::Prt_Header(FILE *fp)
{
	fprintf(fp,"\n%s\n",FwdTxtHd.Mark);
	fprintf(fp,"STDEP    = %12.5f\n",FwdTxtHd.Stdep);
	fprintf(fp,"ENDEP    = %12.5f\n",FwdTxtHd.Endep);
	fprintf(fp,"RLEV     = %12.5f\n",FwdTxtHd.Rlev);
	fprintf(fp,"NUMPOINT = %6d\n",FwdTxtHd.NumPoint);
	fprintf(fp,"NUMLOG   = %6d\n",FwdTxtHd.NumLog);
	fprintf(fp,"CURVENAME=");
	for(int i=0;i<FwdTxtHd.NumLog;i++)
	{
		if(i>0)
		{
			fprintf(fp,",");
		}
		fprintf(fp," %s",FwdTxtHd.CurveName[i]);
	}
	fprintf(fp,"\n");
}
int CFwdTxtR::ReadCurve(float *buf)
{
	CString s;
	char str[102400];
	char str1[256];
	memset(str,'\0',102400);
	str[0]=' ';
	int NumLine=0;
	while(1)
	{
		if(!fFwdTxt.ReadString(&str[1],16381)) break;
		if(str[1]=='#') continue; //注释行
		////////////////////////////////////////////////////
		int sl=strlen(str);
		str[sl]=' ';
		str[sl+1]='\0';
		int n=0;
		for(int i=0;i<FwdTxtHd.NumLog+1;i++)
		{
			if(!GetSubStr(str,n,str1)) break;
			if(i==0)
			{
				float dept=float(atof(str1));
			}
			else
			{
				int k=(i-1)*FwdTxtHd.NumPoint+NumLine;
				buf[k]=float(atof(str1));
			}
			if(n>=sl) 
			{	//本行数据存在问题,被忽略
				break;
			}
		}
		////////////////////////////////////////////////////
		NumLine++;
		if(NumLine>=FwdTxtHd.NumPoint) break;
	}
	return NumLine;
}
bool CFwdTxtR::GetSubStr(const char *str,int &StartPos,char *SubStr)
{
	strcpy(SubStr,"");
	int n1=0;
	int n2=0;
	for(int i=StartPos;i<int(strlen(str)-1);i++)
	{
		if(isspace(str[i]) && !isspace(str[i+1]))
		{
			n1=i+1;
		}
		if(!isspace(str[i]) && isspace(str[i+1]))
		{
			n2=i+1;
			break;
		}
	}
	if(n2>0)
	{
		memcpy(SubStr,&str[n1],n2-n1);
		SubStr[n2-n1]='\0';
		StartPos=n2;
		return true;
	}
	return false;
}