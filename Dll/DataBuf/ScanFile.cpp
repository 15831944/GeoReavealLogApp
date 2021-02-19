// ScanFile.cpp: implementation of the CScanFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScanFile::CScanFile()
{

}

CScanFile::~CScanFile()
{

}
bool CScanFile::SLC16(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n\n",(const char*)sDataFile);
	if(!IsSLC716(sDataFile))
	{
		fprintf(fInfo,"Warning: is not SLC716 Format\n\n");
		return false;
	}

	CSlc716R Slc716R;
	if(!Slc716R.Open(sDataFile)) 
	{
		fprintf(fInfo,"\n!!!!链接文件打开失败\n\n");
		return false;
	}
	Slc716R.ReadHead();
	Slc716R.ClearHead();
	Slc716R.Close();
	fprintf(fInfo,"         Well Name: %-.80s\n",Slc716R.Slc716Head.WellName);
	fprintf(fInfo,"      Company Name: %-.80s\n",Slc716R.Slc716Head.CompanyName);
	fprintf(fInfo,"       Start Depth: %G\n",Slc716R.Slc716Head.Stdep);
	fprintf(fInfo,"        Stop Depth: %G\n",Slc716R.Slc716Head.Endep);
	fprintf(fInfo,"   Sample Interval: %G\n",Slc716R.Slc716Head.Rlev);
	fprintf(fInfo,"Samples Per Record: %G\n",Slc716R.Slc716Head.Spcpr);
	fprintf(fInfo,"      Curve Number: %d\n",Slc716R.Slc716Head.NumLog);
	for(int i=0;i<Slc716R.Slc716Head.NumLog;i++)
	{
		if((i>0) && (i%10==0))
		{
			fprintf(fInfo,"\n");
		}
		fprintf(fInfo," %-4.4s",Slc716R.Slc716Head.CurveName[i]);
	}
	return true;
}
bool CScanFile::BIT(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n\n",(const char*)sDataFile);
	if(!IsBIT(sDataFile))
	{
		fprintf(fInfo,"Warning: is not BIT Format\n\n");
		return false;
	}

	CBitR BitR;
	if(!BitR.Open(sDataFile)) 
	{
		fprintf(fInfo,"\n!!!!链接文件打开失败\n\n");
		return false;
	}
	BitR.ReadHead();
	BitR.ClearHead();
	BitR.Close();
	fprintf(fInfo,"            Logging Time:%d-%02d-%02d %02d:%02d:%02d\n",
		BitR.BitHead.nYear,BitR.BitHead.nMonth,BitR.BitHead.nDay,
		BitR.BitHead.nHour,BitR.BitHead.nMinute,BitR.BitHead.nSecond);
	fprintf(fInfo,"               Well Name: %-.72s\n",BitR.BitHead.WellName);
	fprintf(fInfo,"            Company name: %-.72s\n",BitR.BitHead.CompanyName);
	fprintf(fInfo,"             Start Depth: %G m\n",BitR.BitHead.Stdep);
	fprintf(fInfo,"              Stop Depth: %G m\n",BitR.BitHead.Endep);
	fprintf(fInfo,"         Sample Interval: %G m\n",BitR.BitHead.Rlev);
	fprintf(fInfo,"      Samples Per Record: %G\n",BitR.BitHead.Spcpr);
	if(BitR.bWaveform)
	{
		fprintf(fInfo,"                WaveForm: Yes\n");	//是否为波形数据，TRUE表示波形
		fprintf(fInfo,"                DataFlag: %d\n",BitR.BitHead.DataFlag);
		fprintf(fInfo,"         WaveForm Number: %d\n",BitR.BitHead.NumWF);
		fprintf(fInfo,"WaveForm Sample Interval: %d us\n",BitR.BitHead.RlevTime);
		fprintf(fInfo,"  WaveForm Sample Number: %d\n",BitR.BitHead.NumSample);
	}
	fprintf(fInfo,"      Log Service Number: %-.6s\n",BitR.BitHead.LogServ);//测井服务号
	fprintf(fInfo,"         Log File Number: %-.2s\n",BitR.BitHead.FileNum); //文件号

	fprintf(fInfo,"            Curve Number: %d\n",BitR.BitHead.NumLog);
	fprintf(fInfo,"              Curve Name:\n");
	for(int i=0;i<BitR.BitHead.NumLog;i++)
	{
		if((i>0) && (i%10==0))
		{
			fprintf(fInfo,"\n");
		}
		fprintf(fInfo," %-4.4s",BitR.BitHead.CurveName[i]);
	}
	return true;
}
bool CScanFile::SKC92(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n\n",(const char*)sDataFile);
	if(!IsSKC92(sDataFile))
	{
		fprintf(fInfo,"Warning: is not SKC92 Format\n\n");
		return false;
	}
	CSkc92R Skc92R;
	if(!Skc92R.Open(sDataFile)) 
	{
		fprintf(fInfo,"\n!!!!链接文件打开失败\n\n");
		return FALSE;
	}
	Skc92R.ReadHead();
	Skc92R.ClearHead();	
	Skc92R.Close();
	fprintf(fInfo,"   Logging Time:%d-%02d-%02d %02d:%02d:%02d\n",
		Skc92R.Skc92Head.Year,Skc92R.Skc92Head.Month,Skc92R.Skc92Head.Day,
		Skc92R.Skc92Head.Hour,Skc92R.Skc92Head.Minute,Skc92R.Skc92Head.Second);
	fprintf(fInfo,"      Well Name: %-.80s\n",Skc92R.Skc92Head.WellName);
	fprintf(fInfo,"   Company Name: %-.80s\n",Skc92R.Skc92Head.CompanyName);
	fprintf(fInfo,"    Start Depth: %G\n",Skc92R.Skc92Head.Stdep);
	fprintf(fInfo,"     Stop Depth: %G\n",Skc92R.Skc92Head.Endep);
	fprintf(fInfo,"     Block Size: %d\n",Skc92R.nBlockSize);
	fprintf(fInfo,"Sample NumPoint: %d\n",Skc92R.NumPoint);
	fprintf(fInfo,"Record NumBlock: %d\n",Skc92R.NumBlock);
	fprintf(fInfo,"Sample Interval: %G\n",Skc92R.Skc92Head.Rlev);
	fprintf(fInfo,"   Curve Number: %d\n",Skc92R.Skc92Head.NumLog);
	fprintf(fInfo,"     Curve Name:\n");
	for(int i=0;i<Skc92R.Skc92Head.NumLog;i++)
	{
		if((i>0) && (i%10==0))
		{
			fprintf(fInfo,"\n");
		}
		fprintf(fInfo," %-4.4s",Skc92R.Skc92Head.CurveName[i]);
	}
	fprintf(fInfo,"\n");
	if(Skc92R.bCCLFour)
	{
		fprintf(fInfo,"Curve: CCL Level Spacing=0.025m\n");
	}
	
	return true;
}
bool CScanFile::SKC98(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n\n",(const char*)sDataFile);
	if(!IsSKC98(sDataFile))
	{
		fprintf(fInfo,"Warning: is not SKC98 Format\n\n");
		return false;
	}
	CSkc98R Skc98R;
	if(!Skc98R.Open(sDataFile)) 
	{
		fprintf(fInfo,"\n!!!!链接文件打开失败\n\n");
		return false;
	}
	Skc98R.ReadHead();
	Skc98R.ClearHead();
	Skc98R.Close();

	fprintf(fInfo,"       SKCFlag: %d\n",Skc98R.Skc98Head.SKCFlag);
	fprintf(fInfo,"   Logging Time:%-.18s %-.18s\n",
		Skc98R.Skc98Head.LogDate,Skc98R.Skc98Head.LogTime);
	fprintf(fInfo,"      Well Name: %-.80s\n",Skc98R.Skc98Head.WellName);
	fprintf(fInfo,"   Company Name: %-.80s\n",Skc98R.Skc98Head.CompanyName);
	fprintf(fInfo,"      Team Name: %-.50s\n",Skc98R.Skc98Head.TeamName);
	fprintf(fInfo,"      Area Name: %-.50s\n",Skc98R.Skc98Head.AreaName);
	fprintf(fInfo,"    Start Depth: %G\n",Skc98R.Skc98Head.Stdep);
	fprintf(fInfo,"     Stop Depth: %G\n",Skc98R.Skc98Head.Endep);
	fprintf(fInfo,"Sample Interval: %G\n",Skc98R.Skc98Head.Rlev);
	if(Skc98R.Skc98Head.DriverFlag==1)
		fprintf(fInfo,"     DriverFlag: TIME MODE(%d)\n",Skc98R.Skc98Head.DriverFlag);
	else
		fprintf(fInfo,"     DriverFlag: DEPTH MODE(%d)\n",Skc98R.Skc98Head.DriverFlag);
	fprintf(fInfo,"Sample NumPoint: %d\n",Skc98R.NumPoint);
	fprintf(fInfo,"Record NumBlock: %d\n",Skc98R.NumBlock);
	fprintf(fInfo,"   Curve Number: %d\n",Skc98R.Skc98Head.NumLog);
	fprintf(fInfo,"%-8s  %s\n","Name","Length(B)");
	for(int i=0;i<Skc98R.Skc98Head.NumLog;i++)
	{
		fprintf(fInfo,"%-8.4s  %d\n",
			Skc98R.Skc98Head.CurveName[i],
			Skc98R.Skc98Head.CurveTypes[i]);
	}
	fprintf(fInfo,"    Block Size: %d\n",Skc98R.nBlockSize);
	return true;
}

bool CScanFile::SKH2000(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n\n",(const char*)sDataFile);
	if(!IsSKH2000(sDataFile))
	{
		fprintf(fInfo,"Warning: is not SKH2000 Format\n\n");
		return false;
	}
	CSkh2000R Skh2000R;
	if(!Skh2000R.Open(sDataFile)) 
	{
		fprintf(fInfo,"\n!!!!链接文件打开失败\n\n");
		return FALSE;
	}
	Skh2000R.ReadHead();
	Skh2000R.ClearHead();
	Skh2000R.Close();

	fprintf(fInfo,"   Logging Time:%02X%02X-%02X-%02X %02X:%02X\n",
		Skh2000R.Skh2000Head.YEAR[0],Skh2000R.Skh2000Head.YEAR[1],
		Skh2000R.Skh2000Head.MONTH,Skh2000R.Skh2000Head.DATE,
		Skh2000R.Skh2000Head.HOUR,Skh2000R.Skh2000Head.MINUTE);
	fprintf(fInfo,"      Well Name: %-.40s\n",Skh2000R.Skh2000Head.WellName);
	fprintf(fInfo,"   Company Name: %-.40s\n",Skh2000R.Skh2000Head.CompanyName);
	fprintf(fInfo,"      Team Name: %-.20s\n",Skh2000R.Skh2000Head.TeamName);
	fprintf(fInfo,"      Well Site: %-.20s\n",Skh2000R.Skh2000Head.WellSite);
	fprintf(fInfo,"    Start Depth: %G\n",Skh2000R.Skh2000Head.Stdep);
	fprintf(fInfo,"     Stop Depth: %G\n",Skh2000R.Skh2000Head.Endep);
	fprintf(fInfo,"Sample Interval: %G\n",Skh2000R.Skh2000Head.Rlev);
	fprintf(fInfo,"Sample NumPoint: %d\n",Skh2000R.NumPoint);
	fprintf(fInfo,"Record NumBlock: %d\n",Skh2000R.NumBlock);
	fprintf(fInfo,"     Block Size: %d\n",Skh2000R.nBlockSize);
	fprintf(fInfo,"     Trail Size: %d\n",Skh2000R.nTrailSize);
	fprintf(fInfo,"   Curve Number: %d\n",Skh2000R.Skh2000Head.NumLog);
	for(int i=0;i<Skh2000R.Skh2000Head.NumLog;i++)
	{
		if((i>0) && (i%10==0))
		{
			fprintf(fInfo,"\n");
		}
		fprintf(fInfo," %-4.4s",Skh2000R.Skh2000Head.CurveName[i]);
	}
	fprintf(fInfo,"\n");
	
	return true;
}
bool CScanFile::FID(CString sDataFile)
{
	CString s;
	fprintf(fInfo,"\nDataBuf: %s\n\n",(const char*)sDataFile);
	if(!IsFID(sDataFile))
	{
		fprintf(fInfo,"Warning: is not FID Format\n\n");
		return false;
	}
	CFidRead FidRead;
	if(!FidRead.ReadFidInfo(sDataFile)) 
	{
		fprintf(fInfo,"\n!!!!链接文件打开失败\n\n");
		return FALSE;
	}
	////////////////////////////////
	fprintf(fInfo,"%+12s: %-.80s\n","Oil_Field",FidRead.FidHead.Oil_Field);
	fprintf(fInfo,"%+12s: %-.80s\n","Region",FidRead.FidHead.Region);
	fprintf(fInfo,"%+12s: %-.80s\n","Company",FidRead.FidHead.Company);
	fprintf(fInfo,"%+12s: %-.80s\n","Well",FidRead.FidHead.Well);
	fprintf(fInfo,"%+12s: %-.40s\n","X_Coordinate",FidRead.FidHead.X_Coordinate);
	fprintf(fInfo,"%+12s: %-.40s\n","Y_Coordinate",FidRead.FidHead.Y_Coordinate);
	fprintf(fInfo,"%+12s: %-.80s\n","Exformation",FidRead.FidHead.Exformation);
	fprintf(fInfo,"%+12s: %d\n","Curve_Number",FidRead.FidHead.NumLog);
	fprintf(fInfo,"%-16s %-9s %-8s %-8s %-8s %-8s %-8s %-9s %-9s\n",
		"Name","Dimension","VerUnit","HorUnit","Start","Stop","VerRlev","HorRlev","Samples");
	for(int i=0;i<FidRead.FidHead.NumLog;i++)
	{
		int NumDim=FidRead.FidChannel[i].NumOfDimension-1;
		fprintf(fInfo,"%-16.32s",FidRead.FidChannel[i].CurveName);
		fprintf(fInfo," %-9d",NumDim+1);
		fprintf(fInfo," %-8.8s",FidRead.FidChannel[i].DimInfo[0].Unit);
		fprintf(fInfo," %-8.8s",FidRead.FidChannel[i].DimInfo[NumDim].Unit);
		fprintf(fInfo," %-8G",FidRead.FidChannel[i].DimInfo[0].Start);
		fprintf(fInfo," %-8G",FidRead.FidChannel[i].DimInfo[0].Stop);
		fprintf(fInfo," %-8G",FidRead.FidChannel[i].DimInfo[0].Rlev);
		///////////////////////////////////////////////
		CString sValue="";
		for(int j=1;j<NumDim;j++)
		{
			s.Format ("%G",FidRead.FidChannel[i].DimInfo[j].Rlev);
			sValue+=s;
			if(j>1) sValue+=",";
		}
		if(NumDim==1)
			sValue="--";
		fprintf(fInfo," %-9s",sValue);
		for(j=1;j<NumDim;j++)
		{
			s.Format ("%d",FidRead.FidChannel[i].DimInfo[j].Nps);
			sValue+=s;
			if(j>1) sValue+=",";
		}
		if(NumDim==1)
		{
			sValue="1";
		}
		fprintf(fInfo," %-9s",sValue);	
		fprintf(fInfo,"\n");
	}
	return true;
}
bool CScanFile::WIS(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n\n",(const char*)sDataFile);
	if(!IsWIS(sDataFile))
	{
		fprintf(fInfo,"Warning: is not WIS Format\n\n");
		return false;
	}
	CWisRead WisRead;
	if(!WisRead.Open(sDataFile)) 
	{
		fprintf(fInfo,"\n!!!!链接文件打开失败\n\n");
		return false;
	}
	WisRead.GetHead();
	WisRead.GetObjectEntry();
	int NumOk=0; //正常对象数
	int NumDiscard=0; //抛弃对象数
	int NumDeleted=0; //删除对象数
	WisRead.Prt_HeaderInfo(fInfo);
	WisRead.Prt_ObjectInfo(fInfo);
	WisRead.Prt_ChannelInfo(fInfo);//打印通道信息
	WisRead.Close();
	
	return true;
}
bool CScanFile::XTF(CString sDataFile)
{
	CString s;
	fprintf(fInfo,"\n\nDataBuf: %s\n\n",(const char*)sDataFile);
	if(!IsXTF(sDataFile))
	{
		fprintf(fInfo,"Warning: is not XTF Format\n\n");
		return false;
	}
	if(!XtfRead.Open(sDataFile)) 
	{
		fprintf(fInfo,"\n!!!!链接文件打开失败\n\n");
		return false;
	}
	XtfRead.GetHead();
	fprintf(fInfo,"\n------- XTF FILE HEADER ---------\n");
	fprintf(fInfo,"%+25s: %d\n","System Code",XtfRead.XtfHead.System_Code);
	fprintf(fInfo,"%+25s: %-.80s\n","Fully expanded filename",XtfRead.XtfHead.FileFullName);
	fprintf(fInfo,"%+25s: %-.8s\n","Depth Units",XtfRead.XtfHead.Depth_Unit);
	fprintf(fInfo,"%+25s: %d\n","Number of Curves",XtfRead.XtfHead.isnumcv);
	fprintf(fInfo,"%+25s: %d\n","Max    of Curves",XtfRead.XtfHead.ismaxcv);
	fprintf(fInfo,"%+25s: %G\n","Default Start Depth",XtfRead.XtfHead.wttop);
	fprintf(fInfo,"%+25s: %G\n","Default End Depth",XtfRead.XtfHead.wtbottom);
	fprintf(fInfo,"%+25s: %G\n","Default Level Spacing",XtfRead.XtfHead.wtlevel);
	fprintf(fInfo,"%+25s: %d (%d Bytes)\n","Number of Record",XtfRead.XtfHead.FileRecord,XtfRead.XtfHead.FileRecord*4096);

	fprintf(fInfo,"\n------- XTF SELF-DEFINED DATATYPE ---------\n");
	fprintf(fInfo," %15s %15s %15s\n","DataType Name","Start Record","End Record");
	for(int i=0;i<64;i++)
	{
		s.Format ("%-.8s",XtfRead.XtfHead.UserData[i].Name);
		s.TrimLeft();
		s.TrimRight();
		if(s.IsEmpty()) continue;
		fprintf(fInfo," %15s %15d %15d\n",
			(const char*)s,XtfRead.XtfHead.UserData[i].StartRecord,
			XtfRead.XtfHead.UserData[i].StopRecord);
	}

	//井场信息
	fprintf(fInfo,"\n---------------------  Wellsite information  -------------------\n");	
	fprintf(fInfo,"%+40s: %-.80s\n","Well Name",XtfRead.XtfHead.Wellname);
 	fprintf(fInfo,"%+40s: %-.80s\n","Field Name",XtfRead.XtfHead.Fieldname);
	fprintf(fInfo,"%+40s: %-.80s\n","Company Name",XtfRead.XtfHead.Companyname);
	fprintf(fInfo,"%+40s: %-.32s\n","Country Name",XtfRead.XtfHead.Countryname);
 	fprintf(fInfo,"%+40s: %-.32s\n","Country or Parish Name",XtfRead.XtfHead.Countyname);
	fprintf(fInfo,"%+40s: %-.16s\n","State Name",XtfRead.XtfHead.Statename);
	fprintf(fInfo,"%+40s: %-.8s\n","Operational Control Table Name",XtfRead.XtfHead.OCTname);
 	fprintf(fInfo,"%+40s: %-.40s\n","Well Location",XtfRead.XtfHead.Welllocation);
	fprintf(fInfo,"%+40s: %-.4s\n","Section",XtfRead.XtfHead.Section);
	fprintf(fInfo,"%+40s: %-.4s\n","TownShip",XtfRead.XtfHead.Township);
 	fprintf(fInfo,"%+40s: %-.4s\n","Range",XtfRead.XtfHead.Range);
	fprintf(fInfo,"%+40s: %-.2s\n","Permanent datum (GL or SL)",XtfRead.XtfHead.Permanentdatum);
	fprintf(fInfo,"%+40s: %-.2s\n","Drilling mesurement datum (KB,GL,etc)",XtfRead.XtfHead.Drillingmeasurementdatum);
	fprintf(fInfo,"%+40s: %-.4s\n","Log Analysis Center",XtfRead.XtfHead.LogAnalysisCenter);
	fprintf(fInfo,"%+40s: %-.8s\n","Wellsite units for elevations",XtfRead.XtfHead.Wellunits);

	fprintf(fInfo,"%+40s: %G\n","Latitude",XtfRead.XtfHead.Latitude);
	fprintf(fInfo,"%+40s: %G\n","Longitude",XtfRead.XtfHead.Longitude);
	fprintf(fInfo,"%+40s: %G\n","Kelly Bushing",XtfRead.XtfHead.Kellybushing);
	fprintf(fInfo,"%+40s: %G\n","Derrick Floor",XtfRead.XtfHead.Derrickfloor);
	fprintf(fInfo,"%+40s: %G\n","Elevation",XtfRead.XtfHead.Elevation);
	fprintf(fInfo,"%+40s: %G\n","Magnetic Declination",XtfRead.XtfHead.MagneticDeclination);
	fprintf(fInfo,"%+40s: %G\n","ECC",XtfRead.XtfHead.ECC);
	fprintf(fInfo,"%+40s: %G\n","Ground to sea level elevation",XtfRead.XtfHead.Groundtosealevelelevation);

	fprintf(fInfo,"%+40s: %s\n","Metric Flag",XtfRead.XtfHead.Metricflag==0? "Metric":"EngLish");	
	fprintf(fInfo,"%+40s: %d\n","Maximum Curve versions (1-99)",XtfRead.XtfHead.Maximumcurveversion);

	//曲线信息
	fprintf(fInfo,"\n----------------------  XTF Curve Information  -------------------\n");
	fprintf(fInfo,"%-8s %-4s %-8s %-8s %8s %8s %8s %8s %5s %4s %4s %4s %4s %4s %4s %4s\n",
		"Name","Type","DepUnit","Units","Start","Stop","Rlev","Num","Offs",
		"#Dim","#El1","#El2","#El3",
		"RepC","Hor","Ver");
	CString sCurveName;
	int NumCurveHeadErr=0; //曲线头有错的曲线数
	for(i=0;i<XtfRead.XtfHead.isnumcv;i++)
	{
		sCurveName.Format ("%-.8s",XtfRead.XtfHead.CurveName[i]);
		int nResult=XtfRead.OpenCurve(sCurveName);
		fprintf(fInfo,"%-.8s",XtfRead.XtfHead.CurveName[i]);
		if(XtfRead.XtfHead.CurveAttrib[i].CurType==1)
		{
			fprintf(fInfo," %-4s","C");
		}
		else if(XtfRead.XtfHead.CurveAttrib[i].CurType==2)
		{
			fprintf(fInfo," %-4s","WF");
		}
		else
		{
			fprintf(fInfo," %-4s","MATR");
		}
		if(nResult>=0)
		{
			fprintf(fInfo," %-8.8s",XtfRead.XtfCurve.DepthUnits);
			fprintf(fInfo," %-8.8s",XtfRead.XtfCurve.Units);
		}
		else
		{
			NumCurveHeadErr++;
			fprintf(fInfo," %-8.8s","×");
			fprintf(fInfo," %-8d",nResult);
		}
		fprintf(fInfo," %8G",XtfRead.XtfHead.Sdep[i]);
		fprintf(fInfo," %8G",XtfRead.XtfHead.Edep[i]);
		fprintf(fInfo," %8G",XtfRead.XtfHead.Level[i]);
		fprintf(fInfo," %8u",XtfRead.XtfHead.LevelNumber[i]);
		fprintf(fInfo," %5u",XtfRead.XtfHead.StartOffset[i]);
		fprintf(fInfo," %4u",XtfRead.XtfHead.ndimension[i]);
		fprintf(fInfo," %4u",XtfRead.XtfHead.idimen1[i]);
		fprintf(fInfo," %4u",XtfRead.XtfHead.idimen2[i]);
		fprintf(fInfo," %4u",XtfRead.XtfHead.idimen3[i]);
		fprintf(fInfo," %4u",XtfRead.XtfHead.CurveAttrib[i].RepCode);
		fprintf(fInfo," %4u",XtfRead.XtfHead.CurveAttrib[i].HorType);
		fprintf(fInfo," %4u",XtfRead.XtfHead.CurveAttrib[i].VerType);
		fprintf(fInfo,"\n");

		////////////////////////////////////////////////////////
		//PrtXtfCurveHead(XtfRead.XtfHead.CurveName[i]);
	}
	if(NumCurveHeadErr>0)
	{
		fprintf(fInfo,"\nNumber of Curve Header Error: %d\n",NumCurveHeadErr);
	}
	fprintf(fInfo,"\n");
	

	/////////////////
	XtfRead.Close();
	return true;
}
void CScanFile::PrtXtfCurveHead(char *strCurveName)
{
	CString sCurveName,s;
	char str[4096];
	sCurveName.Format ("%-.8s",strCurveName);
	sCurveName.TrimRight();

	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.Name);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.Units);
	fprintf(fInfo,"%-.24s\n",XtfRead.XtfCurve.Comment);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.ServiceCompany);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.ToolType);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.CurveClassType);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.DepthUnits);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.TimeIncrementUnits);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.StartTimeUnits);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.RawTapeHeaderMnemonic);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.UnitsOfStationary);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.CorrelogramParamterUnits);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.CbilOrientationMnemonic);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.LoggingDirection);
	fprintf(fInfo,"%-.24s\n",XtfRead.XtfCurve.AifCurveName);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.OCT);
	fprintf(fInfo,"%-.48s\n",XtfRead.XtfCurve.ToolNames); //8个6字节串
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.strunused);
	fprintf(fInfo,"%-.8s\n",XtfRead.XtfCurve.strunused2);
	fprintf(fInfo,"%-.4s\n",XtfRead.XtfCurve.LisName);
	fprintf(fInfo,"%-.4s\n",XtfRead.XtfCurve.strunused3);
	fprintf(fInfo,"%-.16s\n",XtfRead.XtfCurve.CreationRoutineName);
	fprintf(fInfo,"%-.16s\n",XtfRead.XtfCurve.ModifiedRoutineName);
	fprintf(fInfo,"%-.28s\n",XtfRead.XtfCurve.strunused4);
	fprintf(fInfo,"%-.20s\n",XtfRead.XtfCurve.LongName);
	fprintf(fInfo,"%-.712s\n",XtfRead.XtfCurve.strunused5);
	fprintf(fInfo,"\n");

	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.deptop);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.depbot);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.rlevcv);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.curvmin);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.curvmax);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.curvavg);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.timeinc);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.starttm);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.stadepth);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.stddev);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.cpwinbeg);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.cpwinstp);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.cpwinlen);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.cbilodeg);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.IntervalTR);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.IntervalRR);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.RawOffset);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.TimeDelay);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.Angle);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.XPad);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.YPad);
	for(int i=0;i<6;i++)
	{
		sprintf(str," %G",XtfRead.XtfCurve.LossMinor[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	
	for(i=0;i<101;i++)
	{
		sprintf(str," %G",XtfRead.XtfCurve.funused[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	fprintf(fInfo,"\n");
	
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.dblinit);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.dbtopdep);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.dbbotdep);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.dbrlevel);
	for(i=0;i<6;i++)
	{
		sprintf(str," %G",XtfRead.XtfCurve.smvalsdb[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.idxtop2);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.idxbot2);
	fprintf(fInfo,"%G\n",XtfRead.XtfCurve.idxrlev2);
	for(i=0;i<51;i++)
	{
		sprintf(str," %G",XtfRead.XtfCurve.dunused[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	fprintf(fInfo,"\n");
	
	
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.EncodedCreationDate);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.EncodedCreationTime);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.EncodedLastAccessDate);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.EncodedLastAccessTime);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.NumberOfCurveHeaderrecords);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.RecordNumberOfLastCurvedataRecord);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.DatumInitializationVariable);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.NumberOfLevelsForTheCurve);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.CurrentEncodedDateSignature);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.CurrentEncodedTimeSignature);
	for(i=0;i<6;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.LossMinor2[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	
	fprintf(fInfo,"%d %d\n",XtfRead.XtfCurve.Arrays2[0],XtfRead.XtfCurve.Arrays2[1]);
	fprintf(fInfo,"%d %d\n",XtfRead.XtfCurve.Arrays3[0],XtfRead.XtfCurve.Arrays3[1]);
	for(i=0;i<108;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.iunused[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	fprintf(fInfo,"\n");
	
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.SurvlibMajor);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.SurvlibMinor);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Ihcurv);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Ictype);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.RepCode);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Ivtype);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Ihtype);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Ndims);
	for(i=0;i<7;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.Idims[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.I2init);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Intflfix);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Isign);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Numbits);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Lisrepcd);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Lisnsamp);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Lisrcsiz);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Lisnsize);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Istartfv);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Igainfv);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Igainepx);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Igainmth);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Igainapp);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Lntaphed);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Ntools);
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.Idxingrp);
	for(i=0;i<6;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.Smvalsi2[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	for(i=0;i<219;i++)
	{
		sprintf(str,"%d",XtfRead.XtfCurve.hunused[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	fprintf(fInfo,"\n");
	
	for(i=0;i<24;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.TransmitterID[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	for(i=0;i<24;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.ReceiverID[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	for(i=0;i<276;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.RawTapeHeaderInfo[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	for(i=0;i<6;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.LossMinor3[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	for(i=0;i<6;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.LossMinor4[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	for(i=0;i<678;i++)
	{
		sprintf(str," %d",XtfRead.XtfCurve.bunused[i]);
		s+=str;
	}
	fprintf(fInfo,"%s\n",(const char*)s);
	s.Empty();
	fprintf(fInfo,"%d\n",XtfRead.XtfCurve.SystemCode);
}
bool CScanFile::LIS(CString sDataFile,int NumPrtFdata,int NumPrtSample,BOOL bPrtBaseInfo)
{
	CString s;
	fprintf(fInfo,"\nDataBuf: %s\n\n",(const char*)sDataFile);
	if(!IsLIS(sDataFile))
	{
		fprintf(fInfo,"Warning: is not LIS Format\n\n");
		return false;
	}
	CString sProducer; //公司名
	CString OriginOfdata; //数据来源
	CLisRead LisRead;
	if(!LisRead.Open(sDataFile)) 
	{
		fprintf(fInfo,"\n!!!!链接文件打开失败\n\n");
		return false;
	}

	LisRead.ScanLisFile(NumPrtFdata,NumPrtSample,bPrtBaseInfo,fInfo);
	LisRead.GetTapeProducer(sProducer,OriginOfdata);
	LisRead.Close();
	return true;
}
bool CScanFile::DLIS(CString sDataFile,int NumPrtFdata,int NumPrtSample,BOOL bPrtBaseInfo)
{
	CString s;
	fprintf(fInfo,"\nDataBuf: %s\n",(const char*)sDataFile);
	if(!IsDLIS(sDataFile))
	{
		fprintf(fInfo,"Warning: is not DLIS Format\n\n");
		return false;
	}
	CDlisRead DlisRead;
	DlisRead.NumPrtFdata=NumPrtFdata;
	DlisRead.NumPrtSample=NumPrtSample;
	DlisRead.bPrtBaseInfo=bPrtBaseInfo;

	DlisRead.ScanDlisFile(sDataFile,fInfo);
	fprintf(fInfo,"\n");
	return true;
}
bool CScanFile::FWDTXT(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n",(const char*)sDataFile);
	if(!IsFwdTxt(sDataFile))
	{
		fprintf(fInfo,"Warning: is not FORWARD_TEXT Format\n\n");
		return false;
	}
	CFwdTxtR FwdTxtR;
	if(!FwdTxtR.Open(sDataFile)) return false;
	FwdTxtR.ReadHead();
	FwdTxtR.Close();
	FwdTxtR.Prt_Header(fInfo);
	return true;
}
bool CScanFile::FWDLAS(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n",(const char*)sDataFile);
	if(!IsFwdLas(sDataFile))
	{
		fprintf(fInfo,"Warning: is not FORWARD_TEXT Format\n\n");
		return false;
	}
	CFwdLasR FwdLasR;
	if(!FwdLasR.Open(sDataFile)) 
		return false;
	FwdLasR.ReadHead();
	FwdLasR.Close();
	FwdLasR.Prt_Header(fInfo);
	return true;
}
bool CScanFile::QH716(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n",(const char*)sDataFile);
	if(!IsQH716(sDataFile))
	{
		fprintf(fInfo,"Warning: is not QH716 Format\n\n");
		return false;
	}
	CQh716R Qh716R;
	if(!Qh716R.Open(sDataFile)) return false;
	Qh716R.ReadHead();
	Qh716R.Close();
	Qh716R.Prt_Header(fInfo);
	return true;
}
bool CScanFile::XB716(CString sDataFile)
{
	fprintf(fInfo,"\nDataBuf: %s\n",(const char*)sDataFile);
	if(!IsXB716(sDataFile))
	{
		fprintf(fInfo,"Warning: is not XB716 Format\n\n");
		return false;
	}
	CXb716R Xb716R;
	if(!Xb716R.Open(sDataFile)) return false;
	Xb716R.ReadHead();
	Xb716R.Close();
	Xb716R.Prt_Header(fInfo);
	return true;
}