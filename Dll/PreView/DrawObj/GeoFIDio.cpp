#include "stdafx.h"
#include <math.h>
#include "GeoFIDio.h"
#include <io.h>
#include "SelfExtractor.h"

long Round(double x)
{
	if (x>=0) return (long) (x+0.5);
	else return (long) (x-0.5);
}

BOOL GetLogTopRegString(CString szSubKey,LPCTSTR lpszEntry,CString& strValue)
{
	ASSERT(lpszEntry != NULL);
	HKEY hSecKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_QUERY_VALUE,
		&hSecKey) == ERROR_SUCCESS)
	{
		DWORD dwType, dwCount;
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
			strValue.ReleaseBuffer();
		}
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SetLogTopRegString(CString szKey,LPCTSTR lpszEntry,LPCTSTR lpszValue)
{
	ASSERT(lpszEntry != NULL);
	HKEY hSecKey;
	RegCreateKey(HKEY_LOCAL_MACHINE,szKey,&hSecKey);
	LONG lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_BINARY,
				(LPBYTE)lpszValue, (lstrlen(lpszValue)+1)*sizeof(TCHAR));
	RegCloseKey(hSecKey);
	return lResult == ERROR_SUCCESS;
}

CString GetCurrentUser()
{
	CString UserName;
	if(!GetLogTopRegString("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User","WorkSpace",UserName))
		UserName="";
	return UserName;
}

CString GetCurrentUserPath()
{	
	CString User=GetCurrentUser();
	CString FilePath=GetWorkPath();
	if(User!="")
		FilePath+=User+"\\";
	else
		FilePath+="[DEFAULT]\\";
	return FilePath;
}

CString GetWorkDisk()
{
	CString WorkDisk;
	if(!GetLogTopRegString("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User","WorkDisk",WorkDisk))
		WorkDisk="";
	return WorkDisk;
}

CString GetWorkPath(CString flag)
{
	CString WorkPath;
	if(!GetLogTopRegString("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User","WorkDisk",WorkPath))
		WorkPath="";
	else
		WorkPath+="\\"+flag+"\\";
	return WorkPath;
}

BOOL DetectFileExist(const char* s)
{
    struct _finddata_t c_file;
	if(_findfirst((char*)s, &c_file) != -1L)
		return TRUE;
	else
		return FALSE;
}

CGeoFIDIO::CGeoFIDIO(const char* pszPathName)
{          
	m_CurveID=0;
	m_ProjectID=0;
	m_WellID=0;
	m_CurveName=_T("");

	m_bOpen = FALSE; 
	m_Contentlist = NULL;
	m_Index.adi = NULL;
	m_Index.Contentlist = NULL;
	m_Index.adi = NULL;
	m_fData=NULL;
	m_iData=NULL;
	m_cData=NULL;
	strcpy(m_pszPathName, pszPathName);
	char szPath[MAX_PATH];
	GetTempPath(MAX_PATH,szPath);
	m_szTempPath.Format("%s\\xLogtemp\\",szPath);
	m_szFIDName=_T("");
	m_bFIDExist=FALSE;
	m_DataSize=0;
	m_DataZip=0;
}               

CGeoFIDIO::~CGeoFIDIO()
{
	Close();
}

BOOL CGeoFIDIO::Open(CString szServer)
{
	if(szServer.IsEmpty())
		return FALSE;
	m_szSever=szServer;
	if(!m_ado.InitData(m_szSever))
		return FALSE;

	CString str=m_pszPathName;
//	D:\\极睿解释平台2010\\WorkSpace\\[综合解释-MO206]\\#MO206ZH(3070-3240)\\MO206ZH(3070-3240).AC
	int x=str.ReverseFind('.');
	if(x<1) return FALSE;
	//取项目名
	m_CurveName=str.Mid(x+1);

	x=str.ReverseFind('\\');
	if(x<1) return FALSE;
	str=str.Left(x);
	//取项目名
	x=str.ReverseFind('\\');
	if(x<1) return FALSE;
	CString ProjectName=str.Mid(x+1);
	str=str.Left(x);
	//取井名
	x=str.ReverseFind('\\');
	if(x<1) return FALSE;
	CString WellName=str.Mid(x+1);

	//寻找井ID号
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Storage WHERE StorageName='" + WellName + "'");
	if(pRS->adoEOF)
		return FALSE;
	else
		m_WellID=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
	if(m_WellID<1) return FALSE;
	
	//寻找项目ID号
	CString cID;
	cID.Format("%lu",m_WellID);
	pRS = m_ado.GetRS("SELECT * FROM Storage WHERE UpperID=" + cID);
	while(pRS->adoEOF == 0)//执行SELECT语句
	{
		str=(LPCTSTR)(_bstr_t)pRS->GetCollect("StorageName");
		if(str==ProjectName)
		{
			m_ProjectID=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
			break;
		}
		pRS->MoveNext();
	}
	if(m_ProjectID<1)
		return FALSE;

	//寻找曲线ID号
	cID.Format("%lu",m_ProjectID);
	pRS = m_ado.GetRS("SELECT * FROM Curve WHERE UpperID=" + cID);
	while(pRS->adoEOF == 0)//执行SELECT语句
	{
		str=(LPCTSTR)(_bstr_t)pRS->GetCollect("CurveName");
		if(str==m_CurveName)
		{
			m_CurveID=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("CurveID"));
			break;
		}
		pRS->MoveNext();
	}
	if(m_CurveID<1) 
		return FALSE;

	if(!ReadIndex()) 
		return FALSE;

	//查找数据体在本地是否存在
	m_szFIDName=ProjectName.Mid(1);
	m_szFIDName="D"+m_szFIDName+"."+m_CurveName;

	struct _finddata_t c_file;
    intptr_t hFile;
    if((hFile = _findfirst(m_szTempPath+"*.*", &c_file ))!=-1L)
    {
		_findnext( hFile, &c_file );
        while( _findnext( hFile, &c_file ) == 0 )
		{
			if(m_szFIDName==c_file.name)
			{
				m_bFIDExist=TRUE;
				break;
			}
		}      
	}
	_findclose( hFile );

	//右不存在，在数据库中读取
	//若存在，在Read函数中读本地文件
	m_szFIDName=m_szTempPath+m_szFIDName;
	if(!m_bFIDExist)
	{
		if(!GetFullData())
			return FALSE;
	}
	m_bOpen=TRUE;
	m_RecNo = 0;
	return TRUE;
}

BOOL CGeoFIDIO::Close()
{
	if(m_Contentlist != NULL) 
	{
		delete m_Contentlist;
		m_Contentlist = NULL;
	}	
	if(m_Index.adi != NULL) 
		delete m_Index.adi;
	if(m_fData!=NULL)
	{
		delete m_fData;
		m_fData=NULL;
	}
	if(m_iData!=NULL)
	{
		delete m_iData;
		m_iData=NULL;
	}
	if(m_cData!=NULL)
	{
		delete m_cData;
		m_cData=NULL;
	}
	
	m_ado.Disconnect();
	return TRUE;
}

int CGeoFIDIO::ReadIndex()
{
	if(m_Contentlist != NULL) 
	{
		delete m_Contentlist;
		m_Contentlist = NULL;
	}	
	if (m_Index.adi != NULL) 
	{
		delete m_Index.adi;
		m_Index.adi = NULL;
	}

	CString cID;
	cID.Format("%lu",m_CurveID);
	_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Curve WHERE CurveID=" + cID);
	if(pRS->adoEOF)
		return FALSE;
	
	m_Index.dimension=atoi((LPCTSTR)(_bstr_t)pRS->GetCollect("Dimension"));
	if(m_Index.dimension <1) return FALSE;
	m_Length=atoi((LPCTSTR)(_bstr_t)pRS->GetCollect("DataType"));
	if(m_Length <1) return FALSE;
	m_nps=atoi((LPCTSTR)(_bstr_t)pRS->GetCollect("XNPS"));
	if(m_Length<1) return FALSE;
	//数据压缩标识
	m_DataZip=atoi((LPCTSTR)(_bstr_t)pRS->GetCollect("DataZip"));
	m_DataSize=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("DataSize"));

	m_Contentlist = new FIDCONTENT[m_Index.dimension];
	m_Index.Contentlist = m_Contentlist;

	CString str="DEP";
	strcpy(m_Contentlist[0].name,str);
	str= (LPCTSTR)(_bstr_t)pRS->GetCollect("DepUnit");
	str.Trim();
	strcpy(m_Contentlist[0].unit,str);
	m_Contentlist[0].type = REPR_FLOAT;
	m_Contentlist[0].length = 4;
	m_Contentlist[0].nps = 1;
	m_Contentlist[0].npw = 0;
	m_Contentlist[0].min =(float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("DepSt"));
	m_Contentlist[0].max =(float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("DepEn"));
	m_Contentlist[0].rlev =(float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("DepRlev"));

	float XMin= (float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("XMin"));  		
	float XMax= (float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("XMax"));  		
	float XRlev= (float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("XRlev")); 
	int k=m_Index.dimension-1; 
	if(k>1)
	{
		str= (LPCTSTR)(_bstr_t)pRS->GetCollect("XName");
		str.Trim();
		strcpy(m_Contentlist[1].name,str);
		str= (LPCTSTR)(_bstr_t)pRS->GetCollect("XUnit");
		str.Trim();
		strcpy(m_Contentlist[1].unit,str);
		m_Contentlist[1].type = REPR_FLOAT;
		m_Contentlist[1].length = 4;
		m_Contentlist[1].nps = m_nps;
		m_Contentlist[1].npw = 0;
      	m_Contentlist[1].min =XMin;
		m_Contentlist[1].max =XMax;
		m_Contentlist[1].rlev =XRlev;
	}
	str= (LPCTSTR)(_bstr_t)pRS->GetCollect("CurveName");
	str.Trim();
	strcpy(m_Contentlist[k].name,str);
	str= (LPCTSTR)(_bstr_t)pRS->GetCollect("DataUnit");
	str.Trim();
	strcpy(m_Contentlist[k].unit,str);

	if(m_Length == 4)
		m_Contentlist[k].type = REPR_FLOAT;
	else if(m_Length == 2)
		m_Contentlist[k].type = REPR_SHORT;
	else
		m_Contentlist[k].type = REPR_CHAR;
	m_Contentlist[k].length = m_Length;
	m_Contentlist[k].nps = m_nps;
	m_Contentlist[k].npw = m_nps;
	m_Contentlist[k].min =XMin;
	m_Contentlist[k].max =XMax;
	m_Contentlist[k].rlev =XRlev;
	
	m_Index.Contentlist = m_Contentlist;

	m_MaxRecNo=(Round)((m_Contentlist[0].max-m_Contentlist[0].min)/m_Contentlist[0].rlev);
	m_RecLen = m_Length*m_nps; 
	return TRUE; 
}

BOOL CGeoFIDIO::WriteLocalFID(CString SavePath,CString CurveName)
{
	BOOL flag=FALSE;
	//本地项目检索路径 
	int k=SavePath.ReverseFind('\\');
	CString ProjectName=SavePath.Mid(k+1);
	CString Path=SavePath.Left(k);
	CString SaveName=Path+"\\I"+ProjectName+"."+CurveName;
	if(WriteIndex(SaveName))
	{
		//写本地FID数据体
		SaveName=Path+"\\D"+ProjectName+"."+CurveName;
		CFile fl;
		if(fl.Open(SaveName,CFile::modeCreate|CFile::modeWrite))
		{	
			size_t len=m_MaxRecNo*m_nps*m_Length;
			if(m_bFIDExist)
			{
				CFile infp=NULL;
				if(infp.Open(m_szFIDName,CFile::modeRead))
				{
					infp.Seek(0,CFile::begin);
					if(m_Length == 4)
					{
						float *fData=new float[m_MaxRecNo*m_nps];
						infp.Read(fData,len);
						fl.Write(fData,len);
						delete fData;
					}
					else if(m_Length == 2)
					{
						short *iData=new short[m_MaxRecNo*m_nps];
						infp.Read(iData,len);
						fl.Write(iData,len);
						delete iData;
					}
					else
					{
						char *cData=new char[m_MaxRecNo*m_nps];
						infp.Read(cData,len);
						fl.Write(cData,len);
						delete cData;
					}
					infp.Close();
				}
			}
			else
			{
				if(m_Length == 4)
					fl.Write(m_fData,len);
				else if(m_Length == 2)
					fl.Write(m_iData,len);
				else
					fl.Write(m_cData,len);
			}
			fl.Close();
			flag=TRUE;
		}
	}
	return flag;
}

BOOL CGeoFIDIO::WriteIndex(CString szPathName)
{
	FILE *fp=NULL;
	if((fp = fopen(szPathName,"w+t")) != NULL)
	{
		int i,k;          
		fprintf(fp,"DIMENSION %i\n",m_Index.dimension);
		for(i=0; i<m_Index.dimension; i++)
		{
			k = i + 1;
			fprintf(fp," INDEX-%i-NAME %s",k,m_Index.Contentlist[i].name);
			fprintf(fp," UNIT %s",m_Index.Contentlist[i].unit);
			fprintf(fp," %s","TYPE");
			if (m_Index.Contentlist[i].type == REPR_CHAR) fprintf(fp," char");
			if (m_Index.Contentlist[i].type == REPR_SHORT) fprintf(fp," int");
			if (m_Index.Contentlist[i].type == REPR_INT) fprintf(fp," int");
			if (m_Index.Contentlist[i].type == REPR_LONG) fprintf(fp," int");
			if (m_Index.Contentlist[i].type == REPR_FLOAT) fprintf(fp," float");
			if (m_Index.Contentlist[i].type == REPR_DOUBLE) fprintf(fp," float");
			if (m_Index.Contentlist[i].type == REPR_STRING) fprintf(fp," string");
			fprintf(fp," LENGTH %i",m_Index.Contentlist[i].length);
			fprintf(fp," NPS %i",m_Index.Contentlist[i].nps);
			fprintf(fp," NPW %i",m_Index.Contentlist[i].npw);
			if(k==1)
			{
				fprintf(fp," SDEP %f",m_Index.Contentlist[i].min);
				fprintf(fp," EDEP %f",m_Index.Contentlist[i].max);
			}
			else
			{
				fprintf(fp," MIN %f",m_Index.Contentlist[i].min);
				fprintf(fp," MAX %f",m_Index.Contentlist[i].max);
			}
			fprintf(fp," RLEV %f\n",m_Index.Contentlist[i].rlev);
		}   
		fprintf(fp," %s","==END==");
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

int CGeoFIDIO::GetDimenSion()
{
	return m_Index.dimension;
}

FIDCONTENT* CGeoFIDIO::GetContent(int dim)
{
 	if (m_Contentlist == NULL) return NULL;
 	if ((dim< 1) || (dim > m_Index.dimension)) return NULL;
 	return m_Contentlist+dim-1;
}

FIDCONTENT* CGeoFIDIO::GetContent(const char* name)
{
 	if (m_Contentlist == NULL) return NULL;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
			return m_Contentlist+i;
	}
 	return NULL;
}   
//根据曲线ID读取曲线数据DataByte1字段值
size_t CGeoFIDIO::GetDataByte(char *pBuf,size_t lenth,DWORD iCurveID)
{
	CString cID;
	cID.Format("%lu",iCurveID);
	//将曲线据数据库中DataByte读取到pBuf
	_RecordsetPtr  pRS= m_ado.GetRS("SELECT * FROM Curve WHERE CurveID=" + cID);
	//读取DataByte字段的实际大小
	DWORD Size = pRS->GetFields()->GetItem("DataByte1")->ActualSize;		
	BSTR HUGEP *Buffer=NULL;
	HRESULT hr;
	if(lenth>Size) lenth=Size;
	if(Size>0)
	{		
		//从图像字段中读取数据到varBLOB中
		_variant_t varBLOB = pRS->GetFields()->GetItem("DataByte1")->GetChunk(Size);		
		if(varBLOB.vt == (VT_ARRAY | VT_UI1))
		{
			hr=SafeArrayAccessData(varBLOB.parray,(void HUGEP**)&Buffer); 
			if(FAILED(hr)) 
				return 0;
			memcpy(pBuf,Buffer,lenth);
			SafeArrayUnaccessData(varBLOB.parray);
		}
	}
	return lenth;
}

//根据曲线ID读取szBlob字段值
DWORD CGeoFIDIO::GetDataFrom(char *pBuf,DWORD lenth,DWORD iCurveID,char szBlob[MAX_PATH])
{
	CString cID;
	cID.Format("%lu",iCurveID);
	//将曲线据数据库中DataByte读取到pBuf
	_RecordsetPtr  pRS= m_ado.GetRS("SELECT * FROM Curve WHERE CurveID=" + cID);
	//读取DataByte字段的实际大小
	DWORD ByteNum = pRS->GetFields()->GetItem(szBlob)->ActualSize;		
	BSTR HUGEP *Buffer=NULL;
	HRESULT hr;
	if(lenth>ByteNum) lenth=ByteNum;
	if(ByteNum>0)
	{		
		//从图像字段中读取数据到varBLOB中
		_variant_t varBLOB = pRS->GetFields()->GetItem(szBlob)->GetChunk(ByteNum);		
		if(varBLOB.vt == (VT_ARRAY | VT_UI1))
		{
			hr=SafeArrayAccessData(varBLOB.parray,(void HUGEP**)&Buffer); 
			if(FAILED(hr))
				return 0;
			memcpy(pBuf,Buffer,lenth);
			SafeArrayUnaccessData(varBLOB.parray);
		}
	}
	return lenth;
}
//根据曲线ID读取压缩分开存储的曲线数据DataByte1-10字段值
size_t CGeoFIDIO::GetDataByteZip(char *pBuf,size_t lenth,DWORD iCurveID)
{
	char szBlob[MAX_PATH] = {0};
	//一个大字段内存储数据体最大字节数
	DWORD ByteNum=10485760;//10兆
	
	CFile fl=NULL;//创建文件
	CString FileName=m_szTempPath+"PlotData";
	if(fl.Open((const char*)FileName,CFile::modeCreate|CFile::modeWrite))
	{
		//读取DataByte1-5字段内的数据到了文件fl
		char *pTemp=new char[ByteNum];
		DWORD PerByte;
		long DataSize=m_DataSize;
		int k=1;
		while(DataSize>0&&k<11)
		{
			sprintf(szBlob,"DataByte%d",k);
			PerByte= GetDataFrom(pTemp,ByteNum,iCurveID,szBlob);
			fl.Write(pTemp,PerByte);
			
			DataSize-=ByteNum;
			k++;
		}
		fl.Close();
		delete pTemp;pTemp=NULL;

		//大字段数中压缩标识,解压到本地后读文件
		CSelfExtractor	Extractor;
		if(Extractor.ReadTOC(FileName)!= INVALID_SIG)
		{
			Extractor.ExtractAll(m_szTempPath);
			FileName=m_szTempPath+Extractor.GetItem(0).GetFilename();
			DWORD len=0;
			CFile fl1=NULL;
			if(fl1.Open((const char*)FileName, CFile::modeRead | CFile::shareDenyNone))
			{
				len=fl1.GetLength();
				if(lenth>len) lenth=len;
				fl1.Read(pBuf,lenth);
				fl1.Close();
			}
		}
		Extractor.Reset();
	}	
	return lenth;
}

BOOL CGeoFIDIO::GetFullData()
{
	if(m_fData!=NULL)
	{
		delete m_fData;
		m_fData=NULL;
	}
	if(m_iData!=NULL)
	{
		delete m_iData;
		m_iData=NULL;
	}
	if(m_cData!=NULL)
	{
		delete m_cData;
		m_cData=NULL;
	}
	size_t len=m_MaxRecNo*m_nps*m_Length;
	if(m_DataZip==16)
	{
		if(m_Length == 4)
		{
			m_fData=new float[m_MaxRecNo*m_nps+m_nps];
			len=GetDataByteZip((char *)m_fData,len,m_CurveID);
		}
		else if(m_Length == 2)
		{
			m_iData=new short[m_MaxRecNo*m_nps+m_nps];
			len=GetDataByteZip((char *)m_iData,len,m_CurveID);
		}
		else
		{
			m_cData=new char[m_MaxRecNo*m_nps+m_nps];
			len=GetDataByteZip((char *)m_cData,len,m_CurveID);
		}
	}
	else
	{
		if(m_Length == 4)
		{
			m_fData=new float[m_MaxRecNo*m_nps+m_nps];
			len=GetDataByte((char *)m_fData,len,m_CurveID);
		}
		else if(m_Length == 2)
		{
			m_iData=new short[m_MaxRecNo*m_nps+m_nps];
			len=GetDataByte((char *)m_iData,len,m_CurveID);
		}
		else
		{
			m_cData=new char[m_MaxRecNo*m_nps+m_nps];
			len=GetDataByte((char *)m_cData,len,m_CurveID);
		}
	}
	if(len<1) return FALSE;
	m_MaxRecNo=(int)(len/m_Length/m_nps);
	return TRUE;
}

size_t CGeoFIDIO::Read(int dim, size_t count, float* buffer)
{
	if((!m_bOpen)||(dim<1))
		return 0; 
	size_t max=m_MaxRecNo*m_nps;
	size_t ReadCount=count;
	if(count>max) ReadCount=max;

	size_t k=m_RecNo*m_nps;
	if(k+count>max) ReadCount=max-k;
	size_t i;
	if(ReadCount<1)
	{
		for(i=0;i<count;i++)
			buffer[i]=-9999.f;
		return 0;
	}
	//若数据体存在，读本地文件
	if(m_bFIDExist)
	{
		CFile fl=NULL;
		if(fl.Open(m_szFIDName,CFile::modeRead))
		{
			fl.Seek(k*m_Length,CFile::begin);
			fl.Read(buffer,ReadCount*m_Length);
			fl.Close();
		}
	}
	else
	{
		for(i=0;i<ReadCount;i++)
			buffer[i]=m_fData[i+k];
	}
	for(i=ReadCount;i<count;i++)
		buffer[i]=-9999.f;
	return count;
}

size_t CGeoFIDIO::Read(int dim, size_t count, short* buffer)
{
	if((!m_bOpen)||(dim<1)) 
		return 0; 
	size_t max=m_MaxRecNo*m_nps;
	size_t ReadCount=count;
	if(count>max) ReadCount=max;

	size_t k=m_RecNo*m_nps;
	if(k+count>max) ReadCount=max-k;
	size_t i;
	if(ReadCount<1)
	{
		for(i=0;i<count;i++)
			buffer[i]=-9999;
		return 0;
	}
	//若数据体存在，读本地文件
	if(m_bFIDExist)
	{
		CFile fl=NULL;
		if(fl.Open(m_szFIDName,CFile::modeRead))
		{
			fl.Seek(k*m_Length,CFile::begin);
			fl.Read(buffer,ReadCount*m_Length);
			fl.Close();
		}
	}
	else
	{
		for(i=0;i<ReadCount;i++)
			buffer[i]=m_iData[i+k];
	}
	for(i=ReadCount;i<count;i++)
		buffer[i]=-9999;
	return ReadCount;
}

size_t CGeoFIDIO::Read(int dim, size_t count, char* buffer)
{
	if((!m_bOpen)||(dim<1)) 
		return 0; 
	size_t max=m_MaxRecNo*m_nps;
	size_t ReadCount=count;
	if(count>max) ReadCount=max;

	size_t k=m_RecNo*m_nps;
	if(k+count>max) ReadCount=max-k;
	size_t i;
	if(ReadCount<1)
	{
		for(i=0;i<count;i++)
			buffer[i]=' ';
		return 0;
	}
	//若数据体存在，读本地文件
	if(m_bFIDExist)
	{
		CFile fl=NULL;
		if(fl.Open(m_szFIDName,CFile::modeRead))
		{
			fl.Seek(k*m_Length,CFile::begin);
			fl.Read(buffer,ReadCount*m_Length);
			fl.Close();
		}
	}
	else
	{
		for(i=0;i<ReadCount;i++)
			buffer[i]=m_cData[i+k];
	}
	for(i=ReadCount;i<count;i++)
		buffer[i]=' ';
	return ReadCount;
}
                                                    

BOOL CGeoFIDIO::SeekToTop()
{               
	m_RecNo=0;
	m_Dep  = m_Contentlist[0].min;
	return TRUE;
}


BOOL CGeoFIDIO::SeekToDepth(float dep)
{                             
	if(!m_bOpen) return FALSE;
	m_Dep  = dep;                             
	m_RecNo = Round((m_Dep - m_Contentlist[0].min) / m_Contentlist[0].rlev);
	if(m_RecNo>m_MaxRecNo)
	{
		m_RecNo=m_MaxRecNo;
		m_Dep=m_Contentlist[0].max;
	}
	if(m_RecNo<0)
	{
		m_RecNo=0;
		m_Dep=m_Contentlist[0].min;
	}
	return TRUE;
}

BOOL CGeoFIDIO::Next()
{
	if(!m_bOpen) return FALSE;
	m_RecNo += 1;
	m_Dep  = m_RecNo*m_Contentlist[0].rlev;                             
	
	if(m_RecNo>m_MaxRecNo)
	{
		m_RecNo=m_MaxRecNo;
		m_Dep=m_Contentlist[0].max;
	}
	return TRUE;
}

float CGeoFIDIO::GetDepth()
{          
	return m_Dep;
}

float CGeoFIDIO::GetStartDepth()
{   
	return m_Contentlist[0].min;
}

float CGeoFIDIO::GetEndDepth()
{
	return m_Contentlist[0].max;
}

float CGeoFIDIO::GetDepthLevel()
{
	return m_Contentlist[0].rlev;
}

void CGeoFIDIO::GetFileName(const char* path, char* name,char pFileType)
{
	if ((path == NULL) || (name == NULL)) return;
	char drive[64];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT]; 
	char s[_MAX_FNAME];
	SplitPath(path, drive, dir, fname, ext );
	if ((fname[0] == 'I') ||(fname[0] == 'D'))
	{
		fname [0] = pFileType;
		MakePath(name, drive, dir, fname, ext );
	}	
	else
	{
		s[0] = pFileType; s[1] = 0;
		strcat(s,fname);
		MakePath(name, drive, dir, s, ext );
	}	
}

void CGeoFIDIO::GetFullFIDName(const char* path, char* name)
{
	if ((path == NULL) || (name == NULL)) return;
	char drive[64];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT]; 
	SplitPath(path, drive, dir, fname, ext );
	if ((fname[0] == 'I') || (fname[0] == 'D'))
		MakePath(name, drive, dir, fname+1, ext);
	else
		strcpy(name,path);
}

void CGeoFIDIO::GetFullWellName(const char* path, char* name)
{
	if ((path == NULL) || (name == NULL)) return;
	char drive[64];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT]; 
	SplitPath(path, drive, dir, fname, ext );
	if ((fname[0] == 'I') || (fname[0] == 'D'))
		MakePath(name, drive, dir, fname+1,"");
	else
		MakePath(name, drive, dir, fname,"");
}

void CGeoFIDIO::MakePath(char* path, const char* drive, const char* dir, const char* fname, const char* ext)
{
	if ((path==NULL) || (drive==NULL) || (dir==NULL) || (fname==NULL) || (ext==NULL)) return ;
	strcpy(path,"");
	strcat(path,drive);
	strcat(path,dir);
	strcat(path,fname);									 
	strcat(path,ext);
}

void CGeoFIDIO::SplitPath(const char* path, char* drive, char* dir, char* fname, char* ext)
{
	char szPath[256],*ss;
	CString str;
	int k;
	if (path == NULL) return;
	if (drive != NULL) strcpy(drive,"");
	if (dir != NULL) strcpy(dir,"");
	if (fname != NULL) strcpy(fname,"");
	if (ext != NULL) strcpy(ext,"");
	strcpy(szPath,path);
	CString szUncSep="\\";
	szUncSep = szUncSep + szUncSep;
	if (!strncmp(szPath,szUncSep,2)) 
	{
		ss = strchr(szPath+2,szPathSep);
		if (ss != NULL) ss = strchr(ss+1,szPathSep);
	}
	else ss = strchr(szPath,':');
	if (ss != NULL) 
	{
		ss++;
		k = ss-szPath;
		if (drive != NULL) 
		{
			strncpy(drive,szPath,k);
			drive[k]=0;
		}
		str = ss;
	}
	else str = szPath;
	k = str.ReverseFind('.');
	if (k != -1) 
	{
		if (ext != NULL) strcpy(ext,str.Mid(k));
		str = str.Left(k);
	}
	k = str.ReverseFind(szPathSep);
	if (k != -1)
	{
		if (fname != NULL) strcpy(fname,str.Mid(k+1));
		str = str.Left(k+1);
	}
	else
	{
        if (fname != NULL) strcpy(fname,str);
		return;
	}
	if (dir != NULL) strcpy(dir,str);
}

void CGeoFIDIO::GetFIDName(const char* path, char* name)
{
	if ((path == NULL) || (name == NULL)) return;
	char ext[_MAX_EXT]; 
	SplitPath(path, NULL, NULL, NULL, ext);
	if (strcmp(ext,""))
		strcpy(name,ext+1);
	else
		strcpy(name,"");
}

void CGeoFIDIO::GetWellName(const char* path, char* name)
{
	if ((path == NULL) || (name == NULL)) return;
	char fname[_MAX_FNAME];
	SplitPath(path, NULL, NULL, fname, NULL);
	if ((fname[0] == 'I') || (fname[0] == 'D'))
		strcpy(name,fname+1);
	else
		strcpy(name,fname);
}

void CGeoFIDIO::GetPath(const char* path, char* name)
{
	if ((path == NULL) || (name == NULL)) return;
	char drive[64];
	char dir[_MAX_DIR];
	SplitPath(path, drive, dir, NULL, NULL);
	MakePath(name, drive, dir,"","");
}