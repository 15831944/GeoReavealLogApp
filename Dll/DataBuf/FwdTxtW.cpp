// FwdTxtW.cpp: implementation of the CFwdTxtW class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FwdTxtW.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFwdTxtW::CFwdTxtW()
{

}

CFwdTxtW::~CFwdTxtW()
{

}
bool CFwdTxtW::Open(CString sDataFile)
{
	m_sFileName=sDataFile;
	fTxt=fopen(sDataFile,"w");
	if(fTxt==NULL)
	{
		return false;
	}
	return true;
}
void CFwdTxtW::Close()
{
	WriteData();
	fclose(fTxt);
	remove(sMidFile);
}

void CFwdTxtW::ResetHead()
{
	strcpy(TxtHd.Mark,"LOGEXPRESSTOP_TEXT_FORMAT");
    TxtHd.Stdep=0;
	TxtHd.Endep=0;
	TxtHd.Rlev=0.125;
    TxtHd.NumLog=0;
    for(int i=0;i<512;i++) memset(TxtHd.CurveName[i],'\0',16);
	TxtHd.NumPoint=0;
}

void CFwdTxtW::WriteHead()
{
	CString sWellName;
	int xW=m_sFileName.ReverseFind ('\\');
	if(xW>-1)
	{
		sWellName=m_sFileName.Mid (xW+1);
		xW=sWellName.ReverseFind ('.');
		if(xW>-1)
			sWellName=sWellName.Left (xW);
	}

	fprintf(fTxt,"%s\n",TxtHd.Mark);
	fprintf(fTxt,"WELLNAME  = %s\n",sWellName);
	fprintf(fTxt,"STDEP     = %12.4f\n",TxtHd.Stdep);
	fprintf(fTxt,"ENDEP     = %12.4f\n",TxtHd.Endep);
	fprintf(fTxt,"RLEV      = %12.4f\n",TxtHd.Rlev);
	fprintf(fTxt,"CURVENAME =");
	for(int i=0;i<TxtHd.NumLog;i++)
	{
		if(i>0)
		{
			fprintf(fTxt,",");
		}
		fprintf(fTxt," %s",TxtHd.CurveName[i]);
	}
	fprintf(fTxt,"\n%-8s","#DEPTH");
	for(i=0;i<TxtHd.NumLog;i++)
	{
		fprintf(fTxt," %11s",TxtHd.CurveName[i]);
	}
	fprintf(fTxt,"\n");
	StartWrite();
}
void CFwdTxtW::StartWrite()
{
	CString s;
	if(IsFileName(sMidFile))
	{
		remove((const char*)sMidFile);
	}
	TCHAR szTempPath  [MAX_PATH];
	DWORD dwResult=:: GetTempPath (MAX_PATH, szTempPath);
	ASSERT (dwResult);
	sMidFile.Format("%s~LisData.tmp",szTempPath);
	if(fTmp.Open(sMidFile,CFile::modeCreate|CFile::modeWrite)==FALSE) 
	{
		s.Format("创建临时文件 %s 出错!",sMidFile);
		AfxMessageBox(s,MB_ICONSTOP);
		return;
	}
	TxtHd.NumPoint=int(fabs((TxtHd.Endep-TxtHd.Stdep)/TxtHd.Rlev)+0.5)+1;
	BlockSize=TxtHd.NumLog*4;
	int fs=TxtHd.NumPoint*BlockSize;
	fTmp.SetLength(fs);
}
void CFwdTxtW::WriteChannel(int index,float *buf)
{
	for(int i=0;i<TxtHd.NumPoint;i++)
	{
		int pos=i*BlockSize+index*4;
		fTmp.Seek(pos,CFile::begin);
		fTmp.Write(&buf[i],4);
	}
}
void CFwdTxtW::WriteData()
{
	fTmp.Close();
	if(fTmp.Open(sMidFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) 
	{
		return;
	}
	float *buf=new float[TxtHd.NumLog];
	for(int i=0;i<TxtHd.NumPoint;i++)
	{
		fTmp.Read(buf,BlockSize);
		float depth=TxtHd.Stdep+i*TxtHd.Rlev;
		fprintf(fTxt,"%-12.4f",depth);
		for(int j=0;j<TxtHd.NumLog;j++)
		{
			fprintf(fTxt," %11.4f",buf[j]);
		}
		fprintf(fTxt,"\n");
	}
	delete []buf; buf=NULL;
}
void CFwdTxtW::Prt_Header(FILE *fp)
{
	fprintf(fp,"%s\n",TxtHd.Mark);
	fprintf(fp,"STDEP    = %12.5f\n",TxtHd.Stdep);
	fprintf(fp,"ENDEP    = %12.5f\n",TxtHd.Endep);
	fprintf(fp,"RLEV     = %12.5f\n",TxtHd.Rlev);
	fprintf(fp,"NUMPOINT = %6d\n",TxtHd.NumPoint);
	fprintf(fp,"NUMLOG   = %6d\n",TxtHd.NumLog);
	fprintf(fp,"CURVENAME=");
	for(int i=0;i<TxtHd.NumLog;i++)
	{
		if(i>0)
		{
			fprintf(fp,",");
		}
		fprintf(fp," %s",TxtHd.CurveName[i]);
	}
	fprintf(fp,"\n");
}
