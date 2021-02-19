// FidWrite.cpp: implementation of the CFidWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FidWrite.h"
#include "CodeSP.h"
#include "FidRead.h"
#include <direct.h>
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidWrite::CFidWrite()
{
	bWaveOpen=false;
	FidChannel=NULL;
	ClaerHead();
}

CFidWrite::~CFidWrite()
{
	if(FidChannel)
	{
		delete []FidChannel; 
		FidChannel=NULL;
	}
}
void CFidWrite::SetFidFileName(CString m_sObjectPath,CString sWellName)
{
	sObjectPath=m_sObjectPath;
	m_sOutWellName=sWellName;//输出FID井名
	m_sOutWellName.MakeUpper();
	if(m_sOutWellName.Left (1)=="D") 
		m_sOutWellName="X"+m_sOutWellName;
	if(m_sOutWellName.Left (1)=="I") 
		m_sOutWellName="X"+m_sOutWellName;
	m_sOutWellPath=sObjectPath+"\\#"+m_sOutWellName; //输出路径
	m_sFidFile=m_sOutWellPath+"\\"+m_sOutWellName+".FID"; //输出FID文件名
}
void CFidWrite::ClaerHead()
{
	for(int i=79;i>=0;i--)
	{
		if(FidHead.Oil_Field[i]!=' ')break;
		FidHead.Oil_Field[i]=0;
	}
	for(i=79;i>=0;i--)
	{
		if(FidHead.Region[i]!=' ')break;
		FidHead.Region[i]=0;
	}
	for(i=79;i>=0;i--)
	{
		if(FidHead.Company[i]!=' ')break;
		FidHead.Company[i]=0;
	}
	for(i=79;i>=0;i--)
	{
		if(FidHead.Well[i]!=' ')break;
		FidHead.Well[i]=0;
	}
	for(i=39;i>=0;i--)
	{
		if(FidHead.X_Coordinate[i]!=' ')break;
		FidHead.X_Coordinate[i]=0;
	}
	for(i=39;i>=0;i--)
	{
		if(FidHead.Y_Coordinate[i]!=' ')break;
		FidHead.Y_Coordinate[i]=0;
	}
	for(i=79;i>=0;i--)
	{
		if(FidHead.Exformation[i]!=' ')break;
		FidHead.Exformation[i]=0;
	}
}
void CFidWrite::InitFidChannel()
{
	if(FidChannel)
	{
		delete FidChannel;
		FidChannel=NULL;
	}
	if(FidHead.NumLog<1) 
	{
		return; //无曲线
	}
	FidChannel=new FID_CHANNEL[FidHead.NumLog];
	for(int i=0;i<FidHead.NumLog;i++)
	{
		strcpy(FidChannel[i].CurveName,"none");
		FidChannel[i].NumOfDimension=2;
		for(int j=0;j<FID_MAX_CURVE_DIM;j++)
		{
			strcpy(FidChannel[i].DimInfo[j].Name,"none");
			strcpy(FidChannel[i].DimInfo[j].Unit,"none");
			FidChannel[i].DimInfo[j].RepCode=4;
			FidChannel[i].DimInfo[j].CodeLen=0;
			FidChannel[i].DimInfo[j].Nps=0;
			FidChannel[i].DimInfo[j].Npw=0;
			FidChannel[i].DimInfo[j].Start=0;
			FidChannel[i].DimInfo[j].Stop=0;
			FidChannel[i].DimInfo[j].Rlev=0;
		}
	}
}
bool CFidWrite::WriteFidInfo()
{
	CString s;
	m_sOutWellPath=sObjectPath+"\\#"+m_sOutWellName;
	_mkdir(m_sOutWellPath);
	
//	::CreateDirectory(m_sOutWellPath,NULL);
	if(!IsDirectory(m_sOutWellPath))
	{
		s.Format ("无法创建目录:\n\n%s",m_sOutWellPath);
		AfxMessageBox(s);
		return false;
	}
	ClaerHead();
	FILE *fp;
	fp=fopen((const char*)m_sFidFile,"w");
	if(fp==NULL)
	{
		return false;
	}
	fprintf(fp,"%+13s %-.80s\n","Oil_Fild:",FidHead.Oil_Field);
	fprintf(fp,"%+13s %-.80s\n","Region:",FidHead.Region);
	fprintf(fp,"%+13s %-.80s\n","Company:",FidHead.Company);
  	fprintf(fp,"%+13s %-.80s\n","Well:",FidHead.Well);
	fprintf(fp,"%+13s %-.40s\n","X_Coordinate:",FidHead.X_Coordinate);
	fprintf(fp,"%+13s %-.40s\n","Y_Coordinate:",FidHead.Y_Coordinate);
  	fprintf(fp,"%+13s %-.80s\n","Exformation:",FidHead.Exformation);
	fprintf(fp,"%+13s %d\n","Curve_Number:",FidHead.NumLog);
	for(int i=0;i<FidHead.NumLog;i++)
	{
		if(i>0 && i%5==0)
		{
			fprintf(fp,"\n");
		}
		fprintf(fp,"%-11.32s",FidChannel[i].CurveName);
	}
	fprintf(fp,"\n");
	fclose(fp);
	return true;
}
bool CFidWrite::FefurbishFidInfo()
{
	CFidRead FidRead;
	CString s;
	CString sCurIndexFile,SCurDataFile;
	CFileFind find;
	BOOL bRet;
	CString sWildCard =m_sOutWellPath+"\\I"+m_sOutWellName+".*";
	CStringArray AllCurveName;
	bRet = find.FindFile (sWildCard,0);
	while(bRet)
	{
		bRet = find.FindNextFile ();
		if(find.IsDots () || find.IsDirectory ())
		{
			continue; //忽略.和..和目录
		}
		sCurIndexFile=find.GetFilePath();
		if(FidRead.IsIndexFile(sCurIndexFile))
		{
			//是索引文件
			SCurDataFile=MyGetPathName(sCurIndexFile)+"\\D"+(find.GetFileName()).Mid(1);
			if(GetFileLength(SCurDataFile)>0)
			{
				//存在与索引文件对应的数据文件
				//取曲线名,曲线名在井名加点后的所有字符
				CString sTemp =m_sOutWellPath+"\\I"+m_sOutWellName+".";
				CString sCurveName=sCurIndexFile.Mid(sTemp.GetLength());
				AllCurveName.Add(sCurveName);
			}
		}
	}

	//重新输出
	FILE *fp;
	fp=fopen((const char*)m_sFidFile,"w");
	if(fp==NULL)
	{
		return false;
	}
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
		{
			fprintf(fp,"\n");
		}
		fprintf(fp,"%-11.32s",(const char*)(AllCurveName.GetAt(i)));
	}
	fprintf(fp,"\n");
	fclose(fp);
	AllCurveName.RemoveAll();

	return true;
}
bool CFidWrite::WriteGenData(int Index,int Num,float *buf)
{
	if(!WriteIndexFile(Index))
	{
		return false;
	}
	if(!WriteDataFile(Index,Num,buf))
	{
		return false;
	}
	return true;
}
bool CFidWrite::WriteIndexFile(int Index)
{
	CString s,sCurveName,sIndexFile;
	sCurveName.Format ("%-.32s",FidChannel[Index].CurveName);
	sCurveName.Trim();
	sIndexFile=m_sOutWellPath+"\\I"+m_sOutWellName+"."+sCurveName;
	FILE *fp;
	fp=fopen((const char*)sIndexFile,"w");
	if(fp==NULL)
	{
		return false;
	}
	fprintf(fp,"DIMENSION %d\n",FidChannel[Index].NumOfDimension);
	for(int i=0;i<FidChannel[Index].NumOfDimension;i++)
	{
		s.Format ("%-.32s",FidChannel[Index].DimInfo[i].Name);
		s.TrimRight();
		fprintf(fp,"INDEX-%d-NAME %-.32s",i+1,(const char*)s);
		s.Format ("%-.32s",FidChannel[Index].DimInfo[i].Unit);
		s.TrimRight();
		fprintf(fp," %s %-.32s","UNIT",(const char*)s);
		fprintf(fp," %s %-.8s","TYPE",(const char*)RepCodeToStr(FidChannel[Index].DimInfo[i].RepCode));
		fprintf(fp," %s %d","LENGTH",FidChannel[Index].DimInfo[i].CodeLen);
		fprintf(fp," %s %d","NPS",FidChannel[Index].DimInfo[i].Nps);
		fprintf(fp," %s %d","NPW",FidChannel[Index].DimInfo[i].Npw);
		if(i==0)
		{
			fprintf(fp," %s %G","SDEP",FidChannel[Index].DimInfo[i].Start);
			fprintf(fp," %s %G","EDEP",FidChannel[Index].DimInfo[i].Stop);
		}
		else
		{
			fprintf(fp," %s %G","MIN",FidChannel[Index].DimInfo[i].Start);
			fprintf(fp," %s %G","MAX",FidChannel[Index].DimInfo[i].Stop);
		}
		fprintf(fp," %s %G","RLEV",FidChannel[Index].DimInfo[i].Rlev);
		fprintf(fp,"\n");
	}
	fprintf(fp," ==END==\n");
	fclose(fp);
	return true;
}
CString CFidWrite::RepCodeToStr(int RepCode)
{
	const int NumType=7;
//	赵中明软件中对于整型数标只能是“INT”，长短区分用字节数2或4
//	static char RepCodeStr[NumType][10]={"INT","SHORT","LONG","FLOAT","DOUBLE","STRING","CHAR"};
	static char RepCodeStr[NumType][10]={"INT","INT","INT","FLOAT","DOUBLE","STRING","INT"};
	if(RepCode>0  && RepCode<=NumType)
	{
		return _T(RepCodeStr[RepCode-1]);
	}
	else
	{
		return _T("none");
	}
}
bool CFidWrite::WriteDataFile(int Index,int Num,float *buf)
{
	CString sCurveName;
	CString sDataFile;
	sCurveName.Format ("%-.32s",FidChannel[Index].CurveName);
	sCurveName.TrimRight();
	sDataFile=m_sOutWellPath+"\\D"+m_sOutWellName+"."+sCurveName;
	
	if(fw.Open(sDataFile,CFile::modeCreate|CFile::modeWrite)==FALSE)
	{
		return false;
	}
	DWORD pos=0;
	WORD nRepCode=FidChannel[Index].DimInfo[1].RepCode;
	WORD nCodeLen=FidChannel[Index].DimInfo[1].CodeLen;
	DWORD count=Num;
	WriteDataToFid(pos,nRepCode,nCodeLen,count,buf);
	fw.Close();
	return true;
}
void CFidWrite::PrintHead(FILE *fp)
{
	CString s;
	//输出转换结果信息
	fprintf(fp,"%+12s: %-.80s\n","Oil_Field",FidHead.Oil_Field);
	fprintf(fp,"%+12s: %-.80s\n","Region",FidHead.Region);
	fprintf(fp,"%+12s: %-.80s\n","Company",FidHead.Company);
	fprintf(fp,"%+12s: %-.80s\n","Well",FidHead.Well);
	fprintf(fp,"%+12s: %-.40s\n","X_Coordinate",FidHead.X_Coordinate);
	fprintf(fp,"%+12s: %-.40s\n","Y_Coordinate",FidHead.Y_Coordinate);
	fprintf(fp,"%+12s: %-.80s\n","Exformation",FidHead.Exformation);
	fprintf(fp,"%+12s: %d\n","Curve_Number",FidHead.NumLog);
	fprintf(fp,"%-10s %-3s %-8.8s %-8.8s %-8s %-8s %-8s %-9s %-9s\n",
		"Name","Dim","DepUnit","Unit","Start","Stop","Rlev","Dimension","Axis");
	for(int i=0;i<FidHead.NumLog;i++)
	{
		int NumDim=FidChannel[i].NumOfDimension-1;
		fprintf(fp,"%-10.32s",FidChannel[i].CurveName);
		fprintf(fp," %-3d",NumDim);
		fprintf(fp," %-8.8s",FidChannel[i].DimInfo[0].Unit);
		fprintf(fp," %-8.8s",FidChannel[i].DimInfo[NumDim].Unit);
		fprintf(fp," %-8G",FidChannel[i].DimInfo[0].Start);
		fprintf(fp," %-8G",FidChannel[i].DimInfo[0].Stop);
		fprintf(fp," %-8G",FidChannel[i].DimInfo[0].Rlev);
		CString sValue="[";
		for(int j=1;j<NumDim;j++)
		{
			if(j>1)
			{
				sValue+=",";
			}
			s.Format ("%d",FidChannel[i].DimInfo[j].Nps);
			sValue+=s;
		}
		sValue+="]";
		if(NumDim==1)
		{
			sValue="[1]";
		}
		fprintf(fp," %-9s",sValue);
		///////////////////////////////////////////////
		sValue="[";
		for(j=1;j<NumDim;j++)
		{
			if(j>1)
			{
				sValue+=",";
			}
			s.Format ("%G",FidChannel[i].DimInfo[j].Rlev);
			sValue+=s;
		}
		sValue+="]";
		if(NumDim==1)
		{
			sValue="0";
		}
		fprintf(fp," %-9s",sValue);
		fprintf(fp,"\n");
	}
	fprintf(fp,"---- End Head information ---\n\n");
}
bool CFidWrite::OpenWave(int Index)
{
	bWaveOpen=false;
	IndexCurve=Index;
	if(!WriteIndexFile(Index))
	{
		return false;
	}
	CString sCurveName;
	CString sDataFile;
	sCurveName.Format ("%-.32s",FidChannel[Index].CurveName);
	sCurveName.TrimRight();
	sDataFile=m_sOutWellPath+"\\D"+m_sOutWellName+"."+sCurveName;
	if(fw.Open(sDataFile,CFile::modeCreate|CFile::modeWrite)==FALSE)
	{
		return false;
	}
	DWORD fs=GetChannelSize(Index);
	fw.SetLength(fs);
	fw.SeekToBegin();
	bWaveOpen=true;
	return true;
}
DWORD CFidWrite::GetChannelSize(int Index)
{
	int NumPoint=int(fabs((FidChannel[Index].DimInfo[0].Stop-FidChannel[Index].DimInfo[0].Start)/FidChannel[Index].DimInfo[0].Rlev)+0.5)+1;
	DWORD NumSam=1;
	for(int i=0;i<(FidChannel[Index].NumOfDimension-1);i++)
	{
		NumSam*=FidChannel[Index].DimInfo[i].Nps;
	}
	DWORD fs=NumSam*NumPoint*FidChannel[Index].DimInfo[i].CodeLen;
	return fs;
}

bool CFidWrite::OpenArray(int Index)
{
	return OpenWave(Index);
}

void CFidWrite::WriteDataToFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,float *buffer)
{
	char *cBuf=NULL;
	short *hBuf=NULL;
	long *lBuf=NULL;
	double *dfBuf=NULL;
	DWORD i;
	DWORD BlockSize=count*nCodeLen;
	fw.Seek(pos,CFile::begin);
	if(nRepCode==FID_REPR_FLOAT)
	{
		fw.Write(buffer,BlockSize);
	}
	else if((nRepCode==FID_REPR_SHORT)||(nRepCode==FID_REPR_INT && nCodeLen==2))
	{	//短整型
		hBuf=new short[count];
		for(i=0;i<count;i++)
		{
			hBuf[i]=short(buffer[i]);
		}
		fw.Write(hBuf,BlockSize);
		delete []hBuf; hBuf=NULL;
	}
	else if((nRepCode==FID_REPR_LONG)||(nRepCode==FID_REPR_INT && nCodeLen==4))
	{	//长整型
		lBuf=new long[count];
		for(i=0;i<count;i++)
		{
			lBuf[i]=long(buffer[i]);
		}
		fw.Write(lBuf,BlockSize);
		delete []lBuf; lBuf=NULL;
	}
	else if(nRepCode==FID_REPR_DOUBLE)
	{	//双精度浮点
		dfBuf=new double[count];
		for(i=0;i<count;i++)
		{
			dfBuf[i]=double(buffer[i]);
		}
		fw.Write(dfBuf,BlockSize);
		delete []dfBuf; dfBuf=NULL;
	}
	else if(nRepCode==FID_REPR_CHAR)
	{	//单字节数
		cBuf=new char[count];
		for(i=0;i<count;i++)
		{
			cBuf[i]=char(buffer[i]);
		}
		fw.Write(cBuf,BlockSize);
		delete []cBuf; cBuf=NULL;
	}
}
void CFidWrite::WriteDataToFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,char *buffer)
{
	if(nRepCode!=FID_REPR_CHAR)
	{
		AfxMessageBox("错误调用:WriteDataToFid(),数据类型应为Char");
		return;
	}
	DWORD BlockSize=count*nCodeLen;
	fw.Seek(pos,CFile::begin);
	fw.Write(buffer,BlockSize);
}
void CFidWrite::WriteDataToFid(DWORD pos,WORD nRepCode,WORD nCodeLen,DWORD count,short *buffer)
{
	if(nRepCode!=FID_REPR_SHORT)
	{
		AfxMessageBox("错误调用:WriteDataToFid(),数据类型应为Short");
		return;
	}
	DWORD BlockSize=count*nCodeLen;
	fw.Seek(pos,CFile::begin);
	fw.Write(buffer,BlockSize);
}
bool CFidWrite::WriteWave(float Depth,float *buf)
{
	CString s;
	if(!bWaveOpen)
	{
		AfxMessageBox("输出文件未打开!");
		return false;
	}
	int np=int((Depth-FidChannel[IndexCurve].DimInfo[0].Start)/FidChannel[IndexCurve].DimInfo[0].Rlev+0.5);
	if(np<0)
	{
		return false;
	}
	int BlockSize=FidChannel[IndexCurve].DimInfo[2].CodeLen*FidChannel[IndexCurve].DimInfo[2].Npw;
	UINT pos=np*BlockSize;
	int nRepCode=FidChannel[IndexCurve].DimInfo[2].RepCode;
	WORD count=FidChannel[IndexCurve].DimInfo[2].Npw;
	WORD nCodeLen=FidChannel[IndexCurve].DimInfo[2].CodeLen;
	WriteDataToFid(pos,nRepCode,nCodeLen,count,buf);
	return true;
}
bool CFidWrite::WriteWave(float Depth,short *buf)
{
	CString s;
	if(!bWaveOpen)
	{
		AfxMessageBox("输出文件未打开!");
		return false;
	}
	int np=int((Depth-FidChannel[IndexCurve].DimInfo[0].Start)/FidChannel[IndexCurve].DimInfo[0].Rlev+0.5);
	if(np<0)
	{
		return false;
	}
	int BlockSize=FidChannel[IndexCurve].DimInfo[2].CodeLen*FidChannel[IndexCurve].DimInfo[2].Npw;
	UINT pos=np*BlockSize;
	int nRepCode=FidChannel[IndexCurve].DimInfo[2].RepCode;
	WORD count=FidChannel[IndexCurve].DimInfo[2].Npw;
	WORD nCodeLen=FidChannel[IndexCurve].DimInfo[2].CodeLen;
	WriteDataToFid(pos,nRepCode,nCodeLen,count,buf);
	return true;
}
bool CFidWrite::WriteWave(float Depth,char *buf)
{
	CString s;
	if(!bWaveOpen)
	{
		AfxMessageBox("输出文件未打开!");
		return false;
	}
	int np=int((Depth-FidChannel[IndexCurve].DimInfo[0].Start)/FidChannel[IndexCurve].DimInfo[0].Rlev+0.5);
	if(np<0)
	{
		return false;
	}
	int BlockSize=FidChannel[IndexCurve].DimInfo[2].CodeLen*FidChannel[IndexCurve].DimInfo[2].Npw;
	UINT pos=np*BlockSize;
	int nRepCode=FidChannel[IndexCurve].DimInfo[2].RepCode;
	WORD count=FidChannel[IndexCurve].DimInfo[2].Npw;
	WORD nCodeLen=FidChannel[IndexCurve].DimInfo[2].CodeLen;
	WriteDataToFid(pos,nRepCode,nCodeLen,count,buf);
	return true;
}
void CFidWrite::CloseWave()
{
	if(bWaveOpen)
	{
		fw.Close();
	}
	bWaveOpen=false;
}
void CFidWrite::CloseArray()
{
	CloseWave();
}
bool CFidWrite::WriteArray(float Depth,float *buf)
{
	CString s;
	if(!bWaveOpen)
	{
		AfxMessageBox("输出文件未打开!");
		return false;
	}
	int np=int((Depth-FidChannel[IndexCurve].DimInfo[0].Start)/FidChannel[IndexCurve].DimInfo[0].Rlev+0.5);
	if(np<0)
	{
		return false;
	}
	int NumD=FidChannel[IndexCurve].NumOfDimension; //维数临时变量
	WORD nCodeLen=FidChannel[IndexCurve].DimInfo[NumD-1].CodeLen; //代码长度
	int nRepCode=FidChannel[IndexCurve].DimInfo[NumD-1].RepCode; //表示码
	int Count=1; //采样总数
	for(int i=0;i<(NumD-1);i++)
	{
		Count*=FidChannel[IndexCurve].DimInfo[i].Nps;
	}
	int BlockSize=Count*nCodeLen; //一个采样点的长度
	UINT pos=np*BlockSize;
	WriteDataToFid(pos,nRepCode,nCodeLen,Count,buf);
	return true;
}