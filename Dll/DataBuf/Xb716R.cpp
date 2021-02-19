// Qb716R.cpp: implementation of the CQb716R class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Xb716R.h"
#include <sys/stat.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXb716R::CXb716R()
{

}

CXb716R::~CXb716R()
{

}
bool _declspec(dllexport) IsXB716(CString sDataFile)
{
	//从外观检查
	struct _stat FileInf;
	int result = _stat(sDataFile, &FileInf );
	if( result != 0 ) return false; //文件不存在
	if( FileInf.st_size<512) return false; //文件大小不符合

	//从内部结构检查
	XB716_HEAD hd;
	FILE *fd;
	fd=fopen(sDataFile,"rb");
	
	if(!fd) return false;

	fread(&hd,sizeof(hd),1,fd);
	fclose(fd);

	//检查文件头信息
	if(hd.Endep<hd.Stdep) return false;
	if(hd.NumLog<1 || hd.NumLog>64) return false;
	if(hd.Rlev<=1e-5 || hd.Rlev>1000.0f) return false;
	//	if(hd.Feet<1.0 || hd.Feet>1024.0) return false; //有些文件为0
	for(int i=0;i<hd.NumLog;i++)
	{
		char str[5];
		memcpy(str,hd.CurveName[i],4);
		str[4]='\0';
		strupr(str);
		for(int j=0;j<4;j++)
		{
			if((str[j]<48 && str[j]!=32) || (str[j]>57 && str[j]<65) || str[j]>90)
			{
				return false;
			}
		}
	}
	//检查文件太小
	int NumPoint=int((hd.Endep-hd.Stdep+0.5*hd.Rlev)/hd.Rlev)+1;
	if(NumPoint<1 || NumPoint>1000000) return false;
	int fs=512+4*hd.NumLog*NumPoint;
	if(abs(FileInf.st_size-fs) > fs/2) return false;
	return true;
}
bool CXb716R::Open(CString sDataFile)
{
	if(fXb716.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fXb716.GetLength();
	HeadSize=sizeof(Head); //文件头长度
	return true;
}
void CXb716R::Close()
{
	fXb716.Close();
}
bool CXb716R::ReadHead()
{
	fXb716.SeekToBegin();
	fXb716.Read(&Head,sizeof(Head));
	Head.Endep*=0.1f;
	Head.Stdep*=0.1f;
	Head.Rlev*=0.1f;
	NumPoint=int((Head.Endep-Head.Stdep)/Head.Rlev+1.5);
	NumBlock=NumPoint;
	BlockSize=Head.NumLog*4;

	return true;
}
void CXb716R::Prt_Header(FILE *fp)
{
	fprintf(fp,"----------------------------Header Start----------------------------\n");
	fprintf(fp,"  FILE LENGTH: %d bytes\n",FileSize);
	fprintf(fp,"HEADER LENGTH: %d bytes\n",HeadSize);
	fprintf(fp,"    WELL NAME: %-.80s\n",Head.WellName);
	fprintf(fp," Company NAME: %-.80s\n",Head.CorpName);
	fprintf(fp,"        STDEP: %G\n",Head.Stdep);
	fprintf(fp,"        ENDEP: %G\n",Head.Endep);
	fprintf(fp,"         RLEV: %G\n",Head.Rlev);
	fprintf(fp,"     NUMPOINT: %d\n",NumPoint);
	fprintf(fp,"       NUMLOG: %d\n",Head.NumLog);
	fprintf(fp,"CURVES: ");
	for(int i=0;i<Head.NumLog;i++)
	{
		fprintf(fp," %-.4s",Head.CurveName[i]);
		if((i+1)%10==0)
		{
			fprintf(fp,"\n%-8s","");
		}
	}
	fprintf(fp,"\n");
	fprintf(fp,"----------------------------Header Stop----------------------------\n");
}
void CXb716R::ReadCurve(int index,float *buf)
{
	for(int i=0;i<NumPoint;i++)
	{
		int pos=HeadSize+i*BlockSize+index*4;
		fXb716.Seek(pos,CFile::begin);
		fXb716.Read(&buf[i],4);
	}
}