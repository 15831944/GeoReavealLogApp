// FidRead.cpp: implementation of the CFidRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FidRead.h"
#include <math.h>
#include "CodeSP.h"
#include <sys/stat.h>
#include "io.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidRead::CFidRead()
{
	FidChannel=NULL;
	bWaveOpen=false;
}

CFidRead::~CFidRead()
{
	if(FidChannel)
	{
		delete []FidChannel; 
		FidChannel=NULL;
	}
	if(bWaveOpen)
		CloseWave();
}

void _declspec(dllexport) GetorkSpace(CString &m_sWorkSpace)
{
	CString sWorkDisk,sWorkSpace;
	//取得FID系统的当前工区名
	RegKeyValueQuery(_T("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User"),_T("WorkDisk") ,sWorkDisk);	
	RegKeyValueQuery(_T("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User"),_T("WorkSpace") ,sWorkSpace);	
	if(sWorkDisk.IsEmpty())
	{
		return;
	}
	m_sWorkSpace=sWorkDisk+"\\WorkSpace";
	if(!sWorkSpace.IsEmpty())
	{
		m_sWorkSpace+="\\";
		m_sWorkSpace+=sWorkSpace;
	}
}

bool _declspec(dllexport) IsFID(CString sDataFile)
{
	CString s;
	s=MyGetFileExt(sDataFile);
	if(s.CompareNoCase("FID")!=0)
	{
		return false; //FID文件名的扩展名是固定的
	}
	CStdioFile f;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone|CFile::typeText)==FALSE) return false;
	f.ReadString(s);
	f.Close();
	s.TrimLeft();
	s.TrimRight();
	int n=s.Find(":");
	if(n<0)
		return false;
	CString FidMark=s.Left(n);
	if(FidMark.CompareNoCase("Oil_Fild")!=0 && FidMark.CompareNoCase("Oil Fild")!=0)
		return false;
	return true;
}
void CFidRead::ResetHead()
{
	for(int i=0;i<80;i++) FidHead.Oil_Field[i]=0;
	for(i=0;i<80;i++) FidHead.Region[i]=0;
	for(i=0;i<80;i++) FidHead.Company[i]=0;
	for(i=0;i<80;i++) FidHead.Well[i]=0;
	for(i=0;i<40;i++) FidHead.X_Coordinate[i]=0;
	for(i=0;i<40;i++) FidHead.Y_Coordinate[i]=0;
	for(i=0;i<80;i++) FidHead.Exformation[i]=0;
}
	
bool CFidRead::ReWriteFidFromData(const CString FidFileName)
{
	bool bResult=ReadFidInfo(FidFileName);
	if(!bResult) return false;
	//根据盘中文件对FID文件进行更新
	m_sInWellPath=MyGetPathName(FidFileName);
	m_sInWellName=MyGetFileTitle(FidFileName);
	CString s;
	CString sCurIndexFile,SCurDataFile;
	CString sWildCard =m_sInWellPath+"\\I"+m_sInWellName+".*";
	CStringArray AllCurveName;

	struct _finddata_t c_file;    //Microsoft C++ 的文件结构
	long hFile;
	CString sTemp,sCurveName;
	if ((hFile = _findfirst(sWildCard, &c_file )) != -1L)
	{ 
		sCurIndexFile=m_sInWellPath+"\\"+c_file.name;
		if(IsIndexFile(sCurIndexFile))
		{
			//是索引文件
			CString sTemp=c_file.name;
			SCurDataFile=MyGetPathName(sCurIndexFile)+"\\D"+sTemp.Mid(1);			
			if(GetFileLength(SCurDataFile)>0)
			{
				//存在与索引文件对应的数据文件
				//取曲线名,曲线名在井名加点后的所有字符
				sTemp =m_sInWellPath+"\\I"+m_sInWellName+".";
				sCurveName=sCurIndexFile.Mid(sTemp.GetLength());
				sCurveName.MakeUpper();
				AllCurveName.Add(sCurveName);
			}
		}
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			sCurIndexFile=m_sInWellPath+"\\"+c_file.name;
			if(IsIndexFile(sCurIndexFile))
			{
				//是索引文件
				CString sTemp=c_file.name;
				SCurDataFile=MyGetPathName(sCurIndexFile)+"\\D"+sTemp.Mid(1);			
				if(GetFileLength(SCurDataFile)>0)
				{
					//存在与索引文件对应的数据文件
					//取曲线名,曲线名在井名加点后的所有字符
					sTemp =m_sInWellPath+"\\I"+m_sInWellName+".";
					sCurveName=sCurIndexFile.Mid(sTemp.GetLength());
					sCurveName.MakeUpper();
					AllCurveName.Add(sCurveName);
				}
			}
		}
	}
	_findclose( hFile );

	//重新输出
	FILE *fp;
	fp=fopen((const char*)FidFileName,"w");
	if(fp==NULL)
		return false;
	fprintf(fp,"%+13s %-.80s\n","Oil_Fild:",FidHead.Oil_Field);
	fprintf(fp,"%+13s %-.80s\n","Region:",FidHead.Region);
	fprintf(fp,"%+13s %-.80s\n","Company:",FidHead.Company);
  	fprintf(fp,"%+13s %-.80s\n","Well:",FidHead.Well);
	fprintf(fp,"%+13s %-.40s\n","X_Coordinate:",FidHead.X_Coordinate);
	fprintf(fp,"%+13s %-.40s\n","Y_Coordinate:",FidHead.Y_Coordinate);
  	fprintf(fp,"%+13s %-.80s\n","Exformation:",FidHead.Exformation);
	fprintf(fp,"%+13s %d\n","Curve_Number:",AllCurveName.GetSize());
	for(int i=0;i<AllCurveName.GetSize();i++)
	{
		if(i>0 && i%5==0)
			fprintf(fp,"\n");
		fprintf(fp,"%-11.32s",(const char*)(AllCurveName.GetAt(i)));
	}
	fprintf(fp,"\n");
	fclose(fp);
	AllCurveName.RemoveAll();
	return true;
}

bool CFidRead::ReadFidInfo(const CString FidFileName)
{
	CString s,s1;
	char str[1024];
	FILE *fd;
	fd=fopen((const char*)FidFileName,"r");
	if(fd==NULL)
		return false;
	m_sInWellPath=MyGetPathName(FidFileName); //FID曲线数据所在目录
	m_sInWellName=MyGetFileTitle(FidFileName); //当前读取数据的井名
	
	ResetHead(); //图头复位
	int NumCurve=0;
	while(1)
	{
		if(fgets(str,1024,fd)==NULL) break;
		s.Format("%-.1024s",str);
		s.TrimLeft();
		s.TrimRight();
		int n=s.Find(":");
		if(n<0) continue;
		if(s.Left(n).CompareNoCase("Oil_Fild")==0)
		{
			s1=s.Mid(n+1); s1.TrimLeft(); s1.TrimRight();
			sprintf(FidHead.Oil_Field,"%-.80s",s1);
		}
		if(s.Left(n).CompareNoCase("Region")==0)
		{
			s1=s.Mid(n+1); s1.TrimLeft(); s1.TrimRight();
			sprintf(FidHead.Region,"%-.80s",s1);
		}
		if(s.Left(n).CompareNoCase("Company")==0)
		{
			s1=s.Mid(n+1); s1.TrimLeft(); s1.TrimRight();
			sprintf(FidHead.Company,"%-.80s",s1);
		}
		if(s.Left(n).CompareNoCase("Well")==0)
		{
			s1=s.Mid(n+1); s1.TrimLeft(); s1.TrimRight();
			sprintf(FidHead.Well,"%-.80s",s1);
		}
		if(s.Left(n).CompareNoCase("X_Coordinate")==0)
		{
			s1=s.Mid(n+1); s1.TrimLeft(); s1.TrimRight();
			sprintf(FidHead.X_Coordinate,"%-.40s",s1);
		}
		if(s.Left(n).CompareNoCase("Y_Coordinate")==0)
		{
			s1=s.Mid(n+1); s1.TrimLeft(); s1.TrimRight();
			sprintf(FidHead.Y_Coordinate,"%-.40s",s1);
		}
		if(s.Left(n).CompareNoCase("Exformation")==0)
		{
			s1=s.Mid(n+1); s1.TrimLeft(); s1.TrimRight();
			sprintf(FidHead.Exformation,"%-.80s",s1);
		}
		if(s.Left(n).CompareNoCase("Curve_Number")==0)
		{
			s1=s.Mid(n+1); s1.TrimLeft(); s1.TrimRight();
			NumCurve=atoi((const char*)(s1));
			break;
		}
	}
	fclose(fd);
	if(!ReadIndex())
		return false;
	return true;	
}

bool CFidRead::ReadIndex()
{
	CString s;
	CString sCurIndexFile,SCurDataFile;
		
	struct _finddata_t c_file;    //Microsoft C++ 的文件结构
	long hFile;
	CString sWildCard =m_sInWellPath+"\\I"+m_sInWellName+".*";
	CStringArray AllCurveName;
	CString sTemp,sCurveName;
	if ((hFile = _findfirst(sWildCard, &c_file )) != -1L)
	{ 
		sCurIndexFile=m_sInWellPath+"\\"+c_file.name;
		if(IsIndexFile(sCurIndexFile))
		{
			//是索引文件
			CString sTemp=c_file.name;
			SCurDataFile=MyGetPathName(sCurIndexFile)+"\\D"+sTemp.Mid(1);			
			if(GetFileLength(SCurDataFile)>0)
			{
				//存在与索引文件对应的数据文件
				//取曲线名,曲线名在井名加点后的所有字符
				sTemp =m_sInWellPath+"\\I"+m_sInWellName+".";
				sCurveName=sCurIndexFile.Mid(sTemp.GetLength());
				sCurveName.MakeUpper();
				AllCurveName.Add(sCurveName);
			}
		}
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			sCurIndexFile=m_sInWellPath+"\\"+c_file.name;
			if(IsIndexFile(sCurIndexFile))
			{
				//是索引文件
				CString sTemp=c_file.name;
				SCurDataFile=MyGetPathName(sCurIndexFile)+"\\D"+sTemp.Mid(1);			
				if(GetFileLength(SCurDataFile)>0)
				{
					//存在与索引文件对应的数据文件
					//取曲线名,曲线名在井名加点后的所有字符
					sTemp =m_sInWellPath+"\\I"+m_sInWellName+".";
					sCurveName=sCurIndexFile.Mid(sTemp.GetLength());
					sCurveName.MakeUpper();
					AllCurveName.Add(sCurveName);
				}
			}
		}
	}
	_findclose( hFile );
	FidHead.NumLog=AllCurveName.GetSize();
	if(FidHead.NumLog==0)
		return false;
	FidChannel=new FID_CHANNEL[FidHead.NumLog];
	InitFidChannel();
	for(int i=0;i<FidHead.NumLog;i++)
	{
		sprintf(FidChannel[i].CurveName,"%-.32s",(const char*)(AllCurveName.GetAt(i)));
		ReadFidChannelInfo(i,AllCurveName.GetAt(i));
	}
	AllCurveName.RemoveAll();
	return true;
}
void CFidRead::InitFidChannel()
{
	for(int i=0;i<FidHead.NumLog;i++)
	{
		strcpy(FidChannel[i].CurveName,"none");
		FidChannel[i].NumOfDimension=2;
		for(int j=0;j<FID_MAX_CURVE_DIM;j++)
		{
			strcpy(FidChannel[i].DimInfo[j].Name,"none");
			strcpy(FidChannel[i].DimInfo[j].Unit,"none");
			FidChannel[i].DimInfo[j].RepCode=FID_REPR_FLOAT;
			FidChannel[i].DimInfo[j].CodeLen=4;
			FidChannel[i].DimInfo[j].Nps=1;
			FidChannel[i].DimInfo[j].Npw=0;
			FidChannel[i].DimInfo[j].Start=0;
			FidChannel[i].DimInfo[j].Stop=0;
			FidChannel[i].DimInfo[j].Rlev=0;
		}
	}
}
bool CFidRead::IsIndexFile(CString sCurIndexFile)
{
	CString sBuf,s;
	s=MyGetFileExt(sCurIndexFile);
	if(s.IsEmpty()) return false;
	CStdioFile f;
	if(f.Open(sCurIndexFile,CFile::modeRead|CFile::shareDenyNone|CFile::typeText)==FALSE) 
	{
		s.Format ("%s\n不能打开读!",sCurIndexFile);
		AfxMessageBox(s);
		return false;
	}
	f.ReadString(sBuf);
	f.Close();
	s=MyGetSubStr(sBuf,1);
	if(s.CompareNoCase("DIMENSION")==0)
		return true;
	return false;
}
bool CFidRead::ReadFidChannelInfo(int Index,CString sCurveName)
{
	CString s,sCurIndexFile;
	sCurIndexFile.Format("%s\\I%s.%s",m_sInWellPath,m_sInWellName,sCurveName);
	if(!IsFileName(sCurIndexFile))
		return false;
	CString sBuf;
	char str[256];
	CStdioFile f;
	if(f.Open(sCurIndexFile,CFile::modeRead|CFile::shareDenyNone|CFile::typeText)==FALSE) 
	{
		s.Format ("%s\n不能打开读!",sCurIndexFile);
		AfxMessageBox(s);
		return false;
	}
	f.ReadString(sBuf);
	sscanf((const char*)sBuf,"%s%d",str,&FidChannel[Index].NumOfDimension);
	s=str;
	if(s.CompareNoCase("DIMENSION")!=0 || FidChannel[Index].NumOfDimension>FID_MAX_CURVE_DIM)
	{
		f.Close();
		s.Format ("%s\n维定义不正确!",sCurIndexFile);
		AfxMessageBox(s);
		return false;
	}
	for(int i=0;i<FidChannel[Index].NumOfDimension;i++)
	{
		f.ReadString(sBuf);
		strcpy(FidChannel[Index].DimInfo[i].Name,(const char*)MyGetSubStr(sBuf,2));
		strcpy(FidChannel[Index].DimInfo[i].Unit,(const char*)MyGetSubStr(sBuf,4));
		FidChannel[Index].DimInfo[i].CodeLen=atoi((const char*)MyGetSubStr(sBuf,8));
		CString sRepCode=MyGetSubStr(sBuf,6);
		FidChannel[Index].DimInfo[i].RepCode=RepCodeStrToInt(sRepCode,FidChannel[Index].DimInfo[i].CodeLen);
		FidChannel[Index].DimInfo[i].Nps=atoi((const char*)MyGetSubStr(sBuf,10));
		FidChannel[Index].DimInfo[i].Npw=atoi((const char*)MyGetSubStr(sBuf,12));
		FidChannel[Index].DimInfo[i].Start=(float)atof((const char*)MyGetSubStr(sBuf,14));
		FidChannel[Index].DimInfo[i].Stop=(float)atof((const char*)MyGetSubStr(sBuf,16));
		FidChannel[Index].DimInfo[i].Rlev=(float)atof((const char*)MyGetSubStr(sBuf,18));
	}
	f.Close();
	if(FidChannel[Index].NumOfDimension==2)
		FidChannel[Index].DimInfo[1].Rlev=0;
	return true;
}
int CFidRead::RepCodeStrToInt(CString sRepCode,int CodeLen)
{
	if(sRepCode.CompareNoCase("INT")==0 
		|| sRepCode.CompareNoCase("LONG")==0 
		|| sRepCode.CompareNoCase("SHORT")==0)
	{
		if(CodeLen==1) return FID_REPR_CHAR;
		if(CodeLen==2) return FID_REPR_SHORT;
		if(CodeLen==4) return FID_REPR_LONG;
	}
	if(sRepCode.CompareNoCase("FLOAT")==0)
		return FID_REPR_FLOAT;
	if(sRepCode.CompareNoCase("DOUBLE")==0)
		return FID_REPR_DOUBLE;
	if(sRepCode.CompareNoCase("STRING")==0)
		return FID_REPR_STRING;
	return -1;
}
void CFidRead::ReadDataFromFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,float *buffer)
{
	CString s;
	char *cBuf=NULL;
	short *hBuf=NULL;
	long *lBuf=NULL;
	double *dfBuf=NULL;
	DWORD i;
	DWORD BlockSize=count*nCodeLen;
	fR.Seek(pos,CFile::begin);
	if(nRepCode==FID_REPR_FLOAT)
		fR.Read(buffer,BlockSize);
	else if((nRepCode==FID_REPR_SHORT)||(nRepCode==FID_REPR_INT && nCodeLen==2))
	{	//短整型
		hBuf=new short[count];
		fR.Read(hBuf,BlockSize);
		for(i=0;i<count;i++)
		{
			buffer[i]=float(hBuf[i]);
		}
		delete []hBuf; hBuf=NULL;
	}
	else if((nRepCode==FID_REPR_LONG)||(nRepCode==FID_REPR_INT && nCodeLen==4))
	{	//长整型
		lBuf=new long[count];
		fR.Read(lBuf,BlockSize);
		for(i=0;i<count;i++)
		{
			buffer[i]=float(lBuf[i]);
		}
		delete []lBuf; lBuf=NULL;
	}
	else if(nRepCode==FID_REPR_DOUBLE)
	{	//双精度浮点
		dfBuf=new double[count];
		fR.Read(dfBuf,BlockSize);
		for(i=0;i<count;i++)
		{
			buffer[i]=float(dfBuf[i]);
		}
		delete []dfBuf; dfBuf=NULL;
	}
	else if(nRepCode==FID_REPR_CHAR)
	{	//单字节数
		cBuf=new char[count];
		fR.Read(cBuf,BlockSize);
		for(i=0;i<count;i++)
		{
			buffer[i]=float(cBuf[i]);
		}
		delete []cBuf; cBuf=NULL;
	}
}
bool CFidRead::ReadGenData(const CString sCurveName,int NumR,float *buf)
{
	CString s,sCurDataFile;
	for(int i=0;i<NumR;i++) buf[i]=-999.25;
	sCurDataFile.Format("%s\\D%s.%s",m_sInWellPath,m_sInWellName,sCurveName);
	if(!IsFileName(sCurDataFile))
		return false;
	int Index=GetIndexFromCurveName(sCurveName);
	if(Index<0)
		return false;

	if(FidChannel[Index].NumOfDimension != 2)
		return false;
	if(fabs(FidChannel[Index].DimInfo[0].Rlev)<0.0001)
		return false;
	if(FidChannel[Index].DimInfo[0].Npw != 0)
		return false; //第一维索引信息不记录
	if(FidChannel[Index].DimInfo[1].Npw != 1)
		return false; //第二维（数据）每点记录必须是1

	if(FidChannel[Index].DimInfo[1].RepCode==FID_REPR_STRING)
		return false; //数据代码为串，目前没有解释
	if(fR.Open(sCurDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
		return false;
	int fs=fR.GetLength();	
	int Size=NumR*FidChannel[Index].DimInfo[1].CodeLen;
	if(Size >fs) 
	{
		NumR=fs/FidChannel[Index].DimInfo[1].CodeLen;
		Size=NumR*FidChannel[Index].DimInfo[1].CodeLen;
	}
	DWORD pos=0;
	WORD nRepCode=FidChannel[Index].DimInfo[1].RepCode;
	WORD nCodeLen=FidChannel[Index].DimInfo[1].CodeLen;
	DWORD count=NumR;
	ReadDataFromFid(pos,nRepCode,nCodeLen,count,buf);
	fR.Close();
	return true;
}
int CFidRead::GetIndexFromCurveName(const CString sCurveName)
{
	if(FidHead.NumLog<=0)
	{
		return -1;
	}
	CString s;
	for(int i=0;i<FidHead.NumLog;i++)
	{
		s.Format ("%-.32s",FidChannel[i].CurveName);
		s.TrimRight();
		if(s.CompareNoCase(sCurveName)==0)
		{
			return i;
		}
	}
	return -1;
}
bool CFidRead::OpenArray(CString sCurveName)
{
	return OpenWave(sCurveName);
}
bool CFidRead::OpenWave(CString sCurveName)
{
	CString s;
	if(bWaveOpen)
	{
		CloseWave();
	}
	IndexCurve=GetIndexFromCurveName(sCurveName);
	if(FidChannel[IndexCurve].NumOfDimension<3)
	{
		return false; //不是波形数据
	}
	sWaveFile.Format("%s\\D%s.%s",m_sInWellPath,m_sInWellName,sCurveName);
	if(fR.Open(sWaveFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) 
	{
		bWaveOpen=false;
		return false;
	}
	bWaveOpen=true;
	return true;
}
bool CFidRead::ReadWave(float Depth,float *buf)
{
	CString s;
	if(!bWaveOpen)
	{
		return false;
	}
	int np=int((Depth-FidChannel[IndexCurve].DimInfo[0].Start)/FidChannel[IndexCurve].DimInfo[0].Rlev+0.5);
	if(np<0)
	{
		return false;
	}
	WORD nRepCode=FidChannel[IndexCurve].DimInfo[2].RepCode;
	WORD nCodeLen=FidChannel[IndexCurve].DimInfo[2].CodeLen;
	DWORD count=FidChannel[IndexCurve].DimInfo[2].Npw;
	int BlockSize=nCodeLen*count;
	DWORD pos=np*BlockSize;
	if(pos>=fR.GetLength())
	{
		return false;
	}
	ReadDataFromFid(pos,nRepCode,nCodeLen,count,buf);
	return true;
}
bool CFidRead::ReadArray(float Depth,float *buf)
{
	CString s;
	if(!bWaveOpen)
	{
		return false;
	}
	int NumDim=FidChannel[IndexCurve].NumOfDimension;
	int np=int((Depth-FidChannel[IndexCurve].DimInfo[0].Start)/FidChannel[IndexCurve].DimInfo[0].Rlev+0.5);
	if(np<0)
	{
		return false;
	}
	DWORD Count=1;
	for(int i=1;i<(NumDim-1);i++)
	{
		Count*=FidChannel[IndexCurve].DimInfo[i].Nps;
	}
	WORD nRepCode=FidChannel[IndexCurve].DimInfo[NumDim-1].RepCode;
	WORD nCodeLen=FidChannel[IndexCurve].DimInfo[NumDim-1].CodeLen;
	int BlockSize=nCodeLen*Count;
	DWORD pos=np*BlockSize;
	if(pos>=fR.GetLength())
		return false;
	ReadDataFromFid(pos,nRepCode,nCodeLen,Count,buf);
	return true;
}
void CFidRead::CloseWave()
{
	if(bWaveOpen)
	{
		fR.Close();
		bWaveOpen=false;
	}
}	
void CFidRead::CloseArray()
{
	CloseWave();
}
void CFidRead::CleanArrayCurve()
{
	int NumOk=FidHead.NumLog;
	for(int index=FidHead.NumLog-1;index>=0;index--)
	{
		if(FidChannel[index].NumOfDimension>3)
		{
			for(int k=index;k<(NumOk-1);k++)
			{
				FidChannel[k]=FidChannel[k+1];
			}
			NumOk--;
		}
	}
	FidHead.NumLog=NumOk;
}
void CFidRead::CleanWaveCurve()
{
	int NumOk=FidHead.NumLog;
	for(int index=FidHead.NumLog-1;index>=0;index--)
	{
		if(FidChannel[index].NumOfDimension==3)
		{
			for(int k=index;k<(NumOk-1);k++)
			{
				FidChannel[k]=FidChannel[k+1];
			}
			NumOk--;
		}
	}
	FidHead.NumLog=NumOk;
}
void CFidRead::CleanConvCurve()
{
	int NumOk=FidHead.NumLog;
	for(int index=FidHead.NumLog-1;index>=0;index--)
	{
		if(FidChannel[index].NumOfDimension==2)
		{
			for(int k=index;k<(NumOk-1);k++)
			{
				FidChannel[k]=FidChannel[k+1];
			}
			NumOk--;
		}
	}
	FidHead.NumLog=NumOk;
}