// FwdLasR.cpp: implementation of the CFwdLasR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FwdLasR.h"
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

CFwdLasR::CFwdLasR()
{

}

CFwdLasR::~CFwdLasR()
{

}

bool _declspec(dllexport) IsFwdLas(CString sDataFile)
{
	//从外观检查
	struct _stat FileInf;
	int result = _stat(sDataFile, &FileInf );
	if( result != 0 ) return false; //文件不存在
	if( FileInf.st_size<20) return false; //文件大小不符合
	//从内部结构检查
	CString s,s1;
	CStdioFile f;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) 
		return false;
	f.ReadString(s);
	f.ReadString(s1);
	f.Close();
	s.TrimRight();s.TrimLeft();
	s.MakeUpper();s1.MakeUpper();
	if((s.Find("~VERSION INFORMATION")>-1)
		&&(s1.Find ("CWLS")>-1))
	{
		return true;
	}
	return false;
}

bool CFwdLasR::Open(CString sDataFile)
{
	if(fFwdLas.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fFwdLas.GetLength();
	return true;
}
void CFwdLasR::Close()
{
	fFwdLas.Close();
}
void CFwdLasR::FwdLasHdReset()
{
    memset(FwdLasHd.Mark,'\0',1024);
    FwdLasHd.Stdep=0;
	FwdLasHd.Endep=0;
	FwdLasHd.Rlev=0.125;
    FwdLasHd.NumLog=0;
    for(int i=0;i<512;i++) memset(FwdLasHd.CurveName[i],'\0',16);
}

bool CFwdLasR::ReadHead()
{
	CString s;
	FwdLasHdReset();
	fFwdLas.SeekToBegin();
	fFwdLas.ReadString(s);
	strcpy(FwdLasHd.Mark,s);
	BOOL bSd,bEd,bRl,bCur;
	bSd=bEd=bRl=bCur=FALSE;
	while(1)
	{
		if(!fFwdLas.ReadString(s)) break;
		if(s.CompareNoCase("END")==0) break;
		if(s.Find("STRT")>=0)
		{
			bSd=TRUE;
			FwdLasHd.Stdep=StringToFloat(s);
		}
		if(s.Find("STOP")>=0)
		{
			bEd=TRUE;
			FwdLasHd.Endep=StringToFloat(s);
		}
		if(s.Find("STEP")>=0)
		{
			bRl=TRUE;
			FwdLasHd.Rlev=StringToFloat(s);
		}
		if(s.Find("~CURVE INFORMATION")>=0)
		{
			bCur=TRUE;
			GetCurveName(s);
		}
		if(bSd&&bEd&&bRl&&bCur) break;
	}
	FwdLasHd.NumPoint=int((FwdLasHd.Endep-FwdLasHd.Stdep)/FwdLasHd.Rlev)+1;
	return true;
}
float CFwdLasR::StringToFloat(CString sVal)
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
void CFwdLasR::GetCurveName(CString sVal)
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
			strcpy(FwdLasHd.CurveName[FwdLasHd.NumLog],sCurveName);
			FwdLasHd.NumLog++;
			break;
		}
		sCurveName=s.Left(n);
		sCurveName.TrimLeft ();
		sCurveName.TrimRight ();
		strcpy(FwdLasHd.CurveName[FwdLasHd.NumLog],sCurveName);
		FwdLasHd.NumLog++;
		CString s1=s.Mid(n+1);
		s=s1;
	}
}
void CFwdLasR::Prt_Header(FILE *fp)
{
	fprintf(fp,"\n%s\n",FwdLasHd.Mark);
	fprintf(fp,"STDEP    = %12.5f\n",FwdLasHd.Stdep);
	fprintf(fp,"ENDEP    = %12.5f\n",FwdLasHd.Endep);
	fprintf(fp,"RLEV     = %12.5f\n",FwdLasHd.Rlev);
	fprintf(fp,"NUMPOINT = %6d\n",FwdLasHd.NumPoint);
	fprintf(fp,"NUMLOG   = %6d\n",FwdLasHd.NumLog);
	fprintf(fp,"CURVENAME=");
	for(int i=0;i<FwdLasHd.NumLog;i++)
	{
		if(i>0)
		{
			fprintf(fp,",");
		}
		fprintf(fp," %s",FwdLasHd.CurveName[i]);
	}
	fprintf(fp,"\n");
}
int CFwdLasR::ReadCurve(float *buf)
{
	CString s;
	char str[102400];
	char str1[256];
	memset(str,'\0',102400);
	str[0]=' ';
	int NumLine=0;
	while(1)
	{
		if(!fFwdLas.ReadString(&str[1],16381)) break;
		if(str[1]=='#') continue; //注释行
		////////////////////////////////////////////////////
		int sl=strlen(str);
		str[sl]=' ';
		str[sl+1]='\0';
		int n=0;
		for(int i=0;i<FwdLasHd.NumLog+1;i++)
		{
			if(!GetSubStr(str,n,str1)) break;
			if(i==0)
			{
				float dept=float(atof(str1));
			}
			else
			{
				int k=(i-1)*FwdLasHd.NumPoint+NumLine;
				buf[k]=float(atof(str1));
			}
			if(n>=sl) 
			{	//本行数据存在问题,被忽略
				break;
			}
		}
		////////////////////////////////////////////////////
		NumLine++;
		if(NumLine>=FwdLasHd.NumPoint) break;
	}
	return NumLine;
}
bool CFwdLasR::GetSubStr(const char *str,int &StartPos,char *SubStr)
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