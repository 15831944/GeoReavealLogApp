// Qh716R.cpp: implementation of the CQh716R class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Qh716R.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQh716R::CQh716R()
{

}

CQh716R::~CQh716R()
{

}
bool _declspec(dllexport) IsQH716(CString sDataFile)
{
	FILE *fd;
	fd=fopen(sDataFile,"rb");
	if(fd==NULL) return false;
	QH716_HEAD qh;
	fread(&qh.Head,4,1,fd);    SunI4TOPC(qh.Head);
	fread(&qh.ECC,4,1,fd);     SunR4TOPC(qh.ECC);
	fread(&qh.CorpName,80,1,fd);
	fread(&qh.WellName,80,1,fd);
	fread(&qh.NumLog,2,1,fd);  SunI2TOPC(qh.NumLog);
	fread(&qh.IZ,2,1,fd);      SunI2TOPC(qh.IZ);
	for(int i=0;i<20;i++)
	{
		fread(qh.CurveName[i],4,1,fd);
	}
	fread(&qh.Stdep,4,1,fd);     SunR4TOPC(qh.Stdep);
	fread(&qh.Stdep,4,1,fd);  	 SunR4TOPC(qh.Stdep);
	fread(&qh.Rlev,4,1,fd);  	 SunR4TOPC(qh.Rlev);
	fread(&qh.IDC,2,1,fd);       SunI2TOPC(qh.IDC);
	fread(&qh.IZZ,2,1,fd);       SunI2TOPC(qh.IZZ);
	fread(&qh.Feet,4,1,fd);  	 SunR4TOPC(qh.Feet);
	if(qh.NumLog >0 && qh.NumLog <= 128)
	{
		for(i=0;i<4;i++)
		{
		  fread(&qh.DumFull[i],2,1,fd);
		  SunI2TOPC(qh.DumFull[i]);
		}
		if(qh.NumLog>20)
		{
			for(i=20;i<qh.NumLog;i++)
			{
				fread(qh.CurveName[i],4,1,fd);
			}
		}
	}
	else
	{
		qh.NumLog=0;
	}
	fread(&qh.End,4,1,fd);    SunI4TOPC(qh.End);
	fclose(fd);
	if(qh.End!=qh.Head) return false; //头长度与尾长度不相等
	if(qh.NumLog<1 || qh.NumLog>128) return false; //曲线数不符合要求
	return true;
}

bool CQh716R::Open(CString sDataFile)
{
	if(fQh716.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fQh716.GetLength();
	return true;
}
void CQh716R::Close()
{
	fQh716.Close();
}
bool CQh716R::ReadHead()
{
	fQh716.SeekToBegin();
	fQh716.Read(&Head.Head,4);   SunI4TOPC(Head.Head);
	fQh716.Read(&Head.ECC,4);     SunR4TOPC(Head.ECC);
	fQh716.Read(&Head.CorpName,80);
	fQh716.Read(&Head.WellName,80);
	fQh716.Read(&Head.NumLog,2);  SunI2TOPC(Head.NumLog);
	fQh716.Read(&Head.IZ,2);      SunI2TOPC(Head.IZ);
	for(int i=0;i<20;i++)
	{
		fQh716.Read(Head.CurveName[i],4);
	}
	fQh716.Read(&Head.Stdep,4);   SunR4TOPC(Head.Stdep);
	fQh716.Read(&Head.Endep,4);   SunR4TOPC(Head.Endep);
	fQh716.Read(&Head.Rlev,4);    SunR4TOPC(Head.Rlev);
	fQh716.Read(&Head.IDC,2);     SunI2TOPC(Head.IDC);
	fQh716.Read(&Head.IZZ,2);     SunI2TOPC(Head.IZZ);
	fQh716.Read(&Head.Feet,4);    SunR4TOPC(Head.Feet);
	if(Head.NumLog >0 && Head.NumLog <= 128)
	{
		for(i=0;i<4;i++)
		{
		  fQh716.Read(&Head.DumFull[i],2);
		  SunI2TOPC(Head.DumFull[i]);
		}
		if(Head.NumLog>20)
		{
			for(i=20;i<Head.NumLog;i++)
			{
				fQh716.Read(Head.CurveName[i],4);
			}
		}
	}
	else
	{
		Head.NumLog=0;
	}
	fQh716.Read(&Head.End,4);    SunI4TOPC(Head.End);
	for(i=0;i<Head.NumLog;i++)
	{
		char str[5];
		sprintf(str,"%-4.4s",Head.CurveName[i]);
		strupr(str);
		memcpy(Head.CurveName[i],str,4);
	}
	NumPoint=int((Head.Endep-Head.Stdep)/Head.Rlev+1.5);
	NumBlock=(NumPoint-1)/int(Head.Feet)+1;
	BlockSize=4+4*Head.NumLog*int(Head.Feet)+4;
	HeadSize=Head.Head+8;
	return true;
}
void CQh716R::Prt_Header(FILE *fp)
{
	fprintf(fp,"----------------------------Header Start----------------------------\n");
	fprintf(fp,"HEADER LENGTH: %d\n",HeadSize);
	fprintf(fp,"    WELL NAME: %-.80s\n",Head.WellName);
	fprintf(fp," Company NAME: %-.80s\n",Head.CorpName);
	fprintf(fp,"        STDEP: %G\n",Head.Stdep);
	fprintf(fp,"        ENDEP: %G\n",Head.Endep);
	fprintf(fp,"         RLEV: %G\n",Head.Rlev);
	fprintf(fp,"         FEET: %G\n",Head.Feet);
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
void CQh716R::ReadCurve(int index,float *buf)
{
	int iFeet=int(Head.Feet);
	for(int i=0;i<NumBlock;i++)
	{
		int pos=HeadSize+i*BlockSize+4+index*iFeet*4;
		fQh716.Seek(pos,CFile::begin);
		fQh716.Read(&buf[i*iFeet],iFeet*4);
	}
	for(i=0;i<NumPoint;i++)
	{
		SunR4TOPC(buf[i]);
	}
}