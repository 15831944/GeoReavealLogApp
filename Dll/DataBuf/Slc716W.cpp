// Slc716W.cpp: implementation of the CSlc716W class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Slc716W.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlc716W::CSlc716W()
{

}

CSlc716W::~CSlc716W()
{

}
bool CSlc716W::Open(CString sDataFile)
{
	if(fSlc716.Open(sDataFile,CFile::modeCreate|CFile::modeWrite)==FALSE)
	{
		return false;
	}
	return true;
}
void CSlc716W::ResetHead() //头信息清除
{
	Slc716Head.Ecc=0;
	Slc716Head.Stdep=0;
	Slc716Head.Endep=0;
	Slc716Head.Rlev=0.125;
	Slc716Head.Spcpr=64;
	for(int i=79;i>=0;i--)
	{
		if(Slc716Head.WellName[i]!=' ')break;
		Slc716Head.WellName[i]=0;
	}
	for(i=79;i>=0;i--)
	{
		if(Slc716Head.CompanyName[i]!=' ')break;
		Slc716Head.CompanyName[i]=0;
	}

	for(i=0;i<72;i++) Slc716Head.Reserverd[i]=0;
	Slc716Head.NumLog=0;
	for(i=0;i<MAXLOG;i++)
	{
		for(int j=0;j<4;j++)
		{
			Slc716Head.CurveName[i][j]=32;
		}
	}
	for(i=0;i<256;i++) Slc716Head.Reserverd2[i]=0;
}

void CSlc716W::ClearHead() //清输出文件头
{
	for(int i=79;i>=0;i--)
	{
		if(Slc716Head.WellName[i]!=' ')break;
		Slc716Head.WellName[i]=0;
	}
	for(i=79;i>=0;i--)
	{
		if(Slc716Head.CompanyName[i]!=' ')break;
		Slc716Head.CompanyName[i]=0;
	}

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

bool CSlc716W::WriteHead()
{ 
	CString s;
	//根据井段等曲线信息设置文件长度
	NumPoint=int((Slc716Head.Endep-Slc716Head.Stdep+0.5*Slc716Head.Rlev)/Slc716Head.Rlev)+1;
	NumBlock=(NumPoint-1)/64+1;
	UINT fs=sizeof(Slc716Head)+NumBlock*Slc716Head.NumLog*256;
	fSlc716.SetLength(fs);
	//写文件头信息
	fSlc716.SeekToBegin();
	fSlc716.Write(&Slc716Head,sizeof(Slc716Head));
	return true;
}

void CSlc716W::WriteChannel(int Index,float *buf)
{
	NumPoint=int((Slc716Head.Endep-Slc716Head.Stdep+0.5*Slc716Head.Rlev)/Slc716Head.Rlev)+1;
	NumBlock=(NumPoint-1)/64+1;

	for(int i=0;i<NumBlock;i++)
	{
		int pos=sizeof(Slc716Head)+(i*Slc716Head.NumLog+Index)*256;
		fSlc716.Seek(pos,CFile::begin);
		fSlc716.Write(&buf[i*64],256);
	}
}
void CSlc716W::Close()
{
	fSlc716.Close();
}
void CSlc716W::PrintHead(FILE *fp)
{
	//输出转换结果信息
	fprintf(fp,"Well Name: %-.80s\n",Slc716Head.WellName);
	fprintf(fp,"Company name: %-.80s\n",Slc716Head.CompanyName);
	fprintf(fp,"Start Depth: %G\n",Slc716Head.Stdep);
	fprintf(fp,"Stop Depth: %G\n",Slc716Head.Endep);
	fprintf(fp,"Sample Interval: %G\n",Slc716Head.Rlev);
	fprintf(fp,"Curve Number: %d\n",Slc716Head.NumLog);
	fprintf(fp,"Curve Name:\n");
	for(int i=0;i<Slc716Head.NumLog;i++)
	{
		if((i>0) && (i%10==0))
		{
			fprintf(fp,"\n");
		}
		fprintf(fp," %-4.4s",Slc716Head.CurveName[i]);
	}
	fprintf(fp,"\n---- End Head information ---\n\n");
}