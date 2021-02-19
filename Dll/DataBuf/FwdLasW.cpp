// FwdLasW.cpp: implementation of the CFwdLasW class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FwdLasW.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFwdLasW::CFwdLasW()
{

}

CFwdLasW::~CFwdLasW()
{

}
bool CFwdLasW::Open(CString sDataFile)
{
	m_sFileName=sDataFile;
	fLas=fopen(sDataFile,"w");
	if(fLas==NULL)
	{
		return false;
	}
	return true;
}
void CFwdLasW::Close()
{
	WriteData();
	fclose(fLas);
	remove(sMidFile);
}

void CFwdLasW::ResetHead()
{
	strcpy(LasHd.Mark,"~VERSION INFORMATION SECTION\nVERS. 2.0 : CWLS LOG ASCII STANDARD - VERSION 2.0\nWRAP. NO  : ONE LINE PER DEPTH STEP");
    LasHd.Stdep=0;
	LasHd.Endep=0;
	LasHd.Rlev=0.125;
    LasHd.NumLog=0;
    for(int i=0;i<512;i++) memset(LasHd.CurveName[i],'\0',16);
	LasHd.NumPoint=0;
}

void CFwdLasW::WriteHead()
{
	//写版本信息
	fprintf(fLas,"%s\n",LasHd.Mark);
	CString str;
	
	//写井信息
	fprintf(fLas,"#\n");
	fprintf(fLas,"~WELL INFORMATION SECTION\n");
	fprintf(fLas,"#MNEM.   UNIT VALUE/NAME                           DESCRIPTION\n");
	fprintf(fLas,"#-----   ---------------------------------------   ---------------\n");

	str.Format("%12.4f",LasHd.Stdep);
	while(str.GetLength()<39)
		str+=" ";
	fprintf(fLas,"STRT.M   %s   :START DEPTH\n",str);
	str.Format("%12.4f",LasHd.Endep);
	while(str.GetLength()<39)
		str+=" ";
	fprintf(fLas,"STOP.M   %s   :STOP DEPTH\n",str);
	str.Format("%12.4f",LasHd.Rlev);
	while(str.GetLength()<39)
		str+=" ";     
	fprintf(fLas,"STEP.M   %s   :STEP\n",str);           
	fprintf(fLas,"NULL.    -9999                                     :NULL VALUE\n");   
	fprintf(fLas,"COMP.    --                                        :COMPANY\n");
	int xW=m_sFileName.ReverseFind ('\\');
	if(xW>-1)
	{
		str=m_sFileName.Mid (xW+1);
		xW=str.ReverseFind ('.');
		if(xW>-1)
			str=str.Left (xW);
	}
	while(str.GetLength()<39)
		str+=" ";
	fprintf(fLas,"WELL.    %s   :WELL\n",str);           
    fprintf(fLas,"FLD .                                              :FIELD\n");          
	fprintf(fLas,"LOC .                                              :LOCATION\n");       
	fprintf(fLas,"LATI.                                              :SURF. LATITUDE\n"); 
	fprintf(fLas,"LONG.                                              :SURF. LONGITUDE\n");
	fprintf(fLas,"EKB .M                                             :KB ELEVATION\n");   
	fprintf(fLas,"EGL .M                                             :GROUND LEVEL\n");   
	fprintf(fLas,"CTRY.                                              :COUNTRY\n");        
	fprintf(fLas,"SRVC.    SICHUAN LOGGING SLC                       :SERVICE COMPANY\n");
	fprintf(fLas,"DATE.    --                                        :LOG DATE\n");       
	fprintf(fLas,"UWI .                                              :UNIQUE WELL ID\n");

	//写曲线信息
	fprintf(fLas,"#\n");
	fprintf(fLas,"~CURVE INFORMATION SECTION\n");
	fprintf(fLas,"#MNEM.   UNIT        CURVE DESCRIPTION\n");
	fprintf(fLas,"#-----   ----------- -------------------------------\n");
	fprintf(fLas,"DEPTH    .M                    :1 Measured Depth\n"); 
	int i;
	for(i=0;i<LasHd.NumLog;i++)
	{
		str=LasHd.CurveName[i];
		while(str.GetLength()<9)
			str+=" ";
		fprintf(fLas,"%s.none                 :%d Normal\n",str,i+2); 
	}
	//写其它信息
	fprintf(fLas,"#\n");
	fprintf(fLas,"~OTHER INFORMATION\n");
    fprintf(fLas,"   FILE SOURCE: GeoReveal SOFTWARE VERSION 2007 LAS LOG EXPORT\n");
	fprintf(fLas,"~A    MD");
	for(i=0;i<LasHd.NumLog;i++)
		fprintf(fLas," %10s",LasHd.CurveName[i]);
	fprintf(fLas,"\n");
	StartWrite();
}
void CFwdLasW::StartWrite()
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
	LasHd.NumPoint=int(fabs((LasHd.Endep-LasHd.Stdep)/LasHd.Rlev)+0.5)+1;
	BlockSize=LasHd.NumLog*4;
	int fs=LasHd.NumPoint*BlockSize;
	fTmp.SetLength(fs);
}
void CFwdLasW::WriteChannel(int index,float *buf)
{
	for(int i=0;i<LasHd.NumPoint;i++)
	{
		int pos=i*BlockSize+index*4;
		fTmp.Seek(pos,CFile::begin);
		fTmp.Write(&buf[i],4);
	}
}
void CFwdLasW::WriteData()
{
	fTmp.Close();
	if(fTmp.Open(sMidFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) 
	{
		return;
	}
	float *buf=new float[LasHd.NumLog];
	for(int i=0;i<LasHd.NumPoint;i++)
	{
		fTmp.Read(buf,BlockSize);
		float depth=LasHd.Stdep+i*LasHd.Rlev;
		fprintf(fLas,"%-12.4f",depth);
		for(int j=0;j<LasHd.NumLog;j++)
		{
			fprintf(fLas," %11.4f",buf[j]);
		}
		fprintf(fLas,"\n");
	}
	delete []buf; buf=NULL;
}
void CFwdLasW::Prt_Header(FILE *fp)
{
	fprintf(fp,"%s\n",LasHd.Mark);
	fprintf(fp,"STDEP    = %12.5f\n",LasHd.Stdep);
	fprintf(fp,"ENDEP    = %12.5f\n",LasHd.Endep);
	fprintf(fp,"RLEV     = %12.5f\n",LasHd.Rlev);
	fprintf(fp,"NUMPOINT = %6d\n",LasHd.NumPoint);
	fprintf(fp,"NUMLOG   = %6d\n",LasHd.NumLog);
	fprintf(fp,"CURVENAME=");
	for(int i=0;i<LasHd.NumLog;i++)
	{
		if(i>0)
		{
			fprintf(fp,",");
		}
		fprintf(fp," %s",LasHd.CurveName[i]);
	}
	fprintf(fp,"\n");
}
