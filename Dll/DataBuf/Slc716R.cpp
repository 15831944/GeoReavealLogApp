// Slc716R.cpp: implementation of the CSlc716R class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Slc716R.h"
#include <sys\stat.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlc716R::CSlc716R()
{

}

CSlc716R::~CSlc716R()
{

}
bool _declspec(dllexport) IsSLC716(CString sDataFile)
{
	//从外观检查
	struct _stat FileInf;
	int result = _stat(sDataFile, &FileInf );
	if( result != 0 ) return false; //文件不存在
	if( FileInf.st_size<1024) return false; //文件大小不符合
	
	//从内部结构检查
	CFile f;
	SLC716_HEAD slc716;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Read(&slc716,sizeof(slc716));
	f.Close();
	
	//检查文件头信息
	if(slc716.NumLog<=0) return false;
	if(slc716.NumLog>128) return false;
	if(fabs(slc716.Stdep)<0.001 && fabs(slc716.Endep)<0.001) return false;
	if(slc716.Stdep>slc716.Endep) return false;
	if(fabs(slc716.Spcpr-64.0)>0.1) return false;
	if(slc716.Rlev<=0.0) return false;
	for(int i=0;i<slc716.NumLog;i++)
	{
		if(!isalpha(slc716.CurveName[i][0])) return false; //曲线名首字符必须是字母
	}
	return true;
}
bool CSlc716R::Open(CString sDataFile)
{
	if(fSlc716.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fSlc716.GetLength();
	return true;
}
bool CSlc716R::ReadHead()
{
	fSlc716.SeekToBegin();
	fSlc716.Read(&Slc716Head,sizeof(Slc716Head));
	NumPoint=int((Slc716Head.Endep-Slc716Head.Stdep+0.5*Slc716Head.Rlev)/Slc716Head.Rlev)+1;
	NumBlock=(NumPoint-1)/64+1;
	return true;
}
void CSlc716R::ClearHead()
{
	for(int i=79;i>=0;i--)
	{
		if(Slc716Head.WellName[i]!=' ' && Slc716Head.WellName[i]!=0x0)break;
		Slc716Head.WellName[i]=0;
	}
	for(i=79;i>=0;i--)
	{
		if(Slc716Head.CompanyName[i]!=' ' && Slc716Head.CompanyName[i]!=0x0)break;
		Slc716Head.CompanyName[i]=0;
	}

	//对头信息进行过滤
	for(i=0;i<72;i++) Slc716Head.Reserverd[i]=0;
	for(i=Slc716Head.NumLog;i<MAXLOG;i++)
	{
		for(int j=0;j<4;j++)
		{
			Slc716Head.CurveName[i][j]=32;
		}
	}
	for(i=0;i<256;i++) Slc716Head.Reserverd2[i]=0;
}

bool CSlc716R::ReadChannel(int Index,float *buf)
{
	for(int i=0;i<NumBlock;i++)
	{
		int pos=sizeof(Slc716Head)+(i*Slc716Head.NumLog+Index)*256;
		fSlc716.Seek(pos,CFile::begin);
		fSlc716.Read(&buf[i*64],256);
	}
	return true;
}
bool CSlc716R::ReadChannel(CString sCurveName,float *buf)
{
	int Index=GetCurveIndex(sCurveName);
	if(Index>=0)
	{
		return ReadChannel(Index,buf);
	}
	return false;
}
int CSlc716R::GetCurveIndex(CString sCurveName)
{
	CString s;
	for(int i=0;i<Slc716Head.NumLog;i++)
	{
		s.Format("%-.4s",Slc716Head.CurveName[i]);
		s.TrimRight();
		if(s.CompareNoCase(sCurveName)==0)
		{
			return i;
		}
	}
	return -1;
}

void CSlc716R::Close()
{
	fSlc716.Close();
}
