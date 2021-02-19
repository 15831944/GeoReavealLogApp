#include "stdafx.h"
#include <math.h>
#include "FIDio.h"
#include "CreditDialog.h"
#include <io.h>
#include "SBDestination.h"
#include "CurveView.h"
#include "Message.h"
#include "Database.h"
#include "Iphlpapi.h"
#include "SysDisk.h"
BOOL Register()
{
	char strHardwareCode[256];
	char strLoginCode[256];	
	CString m_IDString,m_RegistNum;
	CString m_sHardCode,m_sLoginCode;
	//读注册表中的产品序列号信息
	CString szLoginKey="SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User";  //键名
	if(!GetLogTopRegString(szLoginKey,_T("ProductionID2008") ,m_RegistNum))
	{
		AfxMessageBox("\n测井解释平台没有注册,请与该软件供应商联系   \n");
		return FALSE;
	}
	GetNetwareCode(strHardwareCode);
	m_IDString.Format("%s",strHardwareCode);
	sprintf(strHardwareCode,m_IDString);
	NetwareCode2LoginCode(strHardwareCode,strLoginCode);
	if(m_RegistNum.Compare(strLoginCode)!=0)
	{
		AfxMessageBox("产品序列号不正确!请与该软件供应商联系        \n");
		return FALSE;
	}
	return TRUE;
}

void GetNetwareCode(char *NetwareCode)
{
	unsigned long nError,IP_len;
	PIP_ADAPTER_INFO pIPinfo=NULL;
	nError	=	GetAdaptersInfo(pIPinfo,&IP_len);
	if (nError==ERROR_BUFFER_OVERFLOW)
	{
		pIPinfo= (PIP_ADAPTER_INFO)malloc(IP_len);
		nError	=	GetAdaptersInfo(pIPinfo,&IP_len);				
		if (nError==0)
		{
			sprintf(NetwareCode,"%02X%02X%02X%02X%02X%02X",pIPinfo->Address[0],
					pIPinfo->Address[1],pIPinfo->Address[2],pIPinfo->Address[3],
					pIPinfo->Address[4],pIPinfo->Address[5]);
			return;
		}
	}
	AfxMessageBox("查找网络适配器失败         ");
}

void NetwareCode2LoginCode(char *HardwareCode,char *LoginCode)
{
	static char Mask[13]="ZZMZHAOHUA36";	//12字节掩码
	char NetwareCode[13];
	char strSerial[13];
	strncpy(strSerial,&HardwareCode[0],12);;
	for(int i=0;i<12;i++)
		NetwareCode[i]=Mask[i]^strSerial[i];
	//将位运算后的结果放入4个短整型数中
	unsigned short kSer1=NetwareCode[0]+256*NetwareCode[1];
	unsigned short kSer2=NetwareCode[2]+256*NetwareCode[3];
	unsigned short kSer3=NetwareCode[4]+256*NetwareCode[5];
	unsigned short kSer4=NetwareCode[6]+256*NetwareCode[7];
	unsigned short kSer5=NetwareCode[8]+256*NetwareCode[9];
	unsigned short kSer6=NetwareCode[10]+256*NetwareCode[11];
	//输出形成硬件代码
	sprintf(LoginCode,"%04X-%04X-%04X-%04X-%04X-%04X",kSer6,kSer5,kSer4,kSer3,kSer2,kSer1);
}

BOOL Message(const char* MsgStr,BOOL bOK,BOOL bCancel)
{
	CMessage MesDlg(bOK,bCancel);
	MesDlg.m_szMessage=MsgStr;
	if(MesDlg.DoModal( )==IDOK)
		return TRUE;
	else
		return FALSE;
}
/********************************************************************/
/*																	*/
/* Function name : SelectWell								  	    */		
/* Description   : Browse for folder using SHBrowseForFolder.		*/
/*																	*/
/********************************************************************/
CString SelectWell(HWND hWnd)
{
	CSBDestination dlg(hWnd);
	CString PathStr=GetCurrentUserPath();
	dlg.SetFlags(BIF_RETURNONLYFSDIRS);
	dlg.SetInitialSelection(PathStr);
	CString strResult="";
	if (dlg.SelectFolder())
	{
		strResult = dlg.GetSelectedFolder();
	}
	return strResult;
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

CString GetServerIP()
{
	CString ServerIP;
	if(!GetLogTopRegString("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User","ServerIP",ServerIP))
		ServerIP="";
	return ServerIP;
}

CString GetDBInitUserName()
{
	CString DBInitUserName;
	if(!GetLogTopRegString("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User","DBInitUserName",DBInitUserName))
		DBInitUserName="";
	return DBInitUserName;
}

CString GetDBInitPassword()
{
	CString DBInitPassword;
	if(!GetLogTopRegString("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User","DBInitPassword",DBInitPassword))
		DBInitPassword="";
	return DBInitPassword;
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

void SoftAbout(const char* Editer)
{	
	CCreditDialog About((char*)Editer);
	About.DoModal ();
}

void LogView(const char* CurverName)
{
	CCurveView ViewDlg((char*)CurverName);
	ViewDlg.DoModal ();
}

BOOL DetectFileExist(const char* s)
{
    struct _finddata_t c_file;
	if(_findfirst((char*)s, &c_file) != -1L)
		return TRUE;
	else
		return FALSE;
}

BOOL RefurbishFID(char PathName[256])
{
	ReWriteCurveDBF(PathName);
	CStringArray StaticStr;
	StaticStr.SetSize (7);
	StaticStr.SetAt (0,"Oil_Fild:");
	StaticStr.SetAt (1,"Region:");
	StaticStr.SetAt (2,"Company:");
	StaticStr.SetAt (3,"Well:");
	StaticStr.SetAt (4,"X_Coordinate:");
	StaticStr.SetAt (5,"Y_Coordinate:");
	StaticStr.SetAt (6,"Exformation:");

	CString ss,s1;
	CFile  file;
	int i=0;
	if (file.Open(PathName,CFile::modeRead))
	{
		DWORD len = file.GetLength();
		file.Read(ss.GetBufferSetLength(len),len);
		ss.ReleaseBuffer ();
		file.Close();
		int xxx=ss.Find ("\n");
		while((xxx!=-1)&&(i<7))
		{
			s1=ss.Left (xxx-1);
			ss=ss.Mid (xxx+1);
			xxx=ss.Find ("\n");
			if(s1.Find (StaticStr[i])!=-1)
				StaticStr[i]=s1;
			else
				continue;			
			i++;
		}
	}
	char szFIDName[40];
	//得到该口井的路径
	CString szDir;
	CFIDIO::GetPath(PathName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();

	struct _finddata_t c_file;    //Microsoft C++ 的文件结构
	long hFile;
	int CurveNum=0;CString FullCurveName;
	if ((hFile = _findfirst(szDir+"\\I*.*", &c_file )) != -1L)
	{ 
		CFIDIO::GetFIDName(c_file.name,szFIDName);//得到曲线名
		sprintf(szFIDName,"%-10s ",szFIDName);
		FullCurveName+=szFIDName;CurveNum++;
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			CFIDIO::GetFIDName(c_file.name,szFIDName);
			sprintf(szFIDName,"%-10s ",szFIDName);
			FullCurveName+=szFIDName;CurveNum++;
			if(CurveNum%10==0) 
				FullCurveName+="\n";
		}	       
		_findclose( hFile );
	}
	sprintf(szFIDName,"Curve_Number: %d\n",CurveNum);
	FILE* fp=fopen(PathName,"wt");
	if(fp!=NULL)
	{
		for(i=0;i<7;i++)
			fprintf(fp,"%13s\n",StaticStr[i]);
		fprintf(fp,"%s",szFIDName);
		fprintf(fp,"%s",FullCurveName);
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

void ReWriteCurveDBF(char PathName[256])
{	
	struct _finddata_t c_file;    
	long hFile;
	int dim,i=0;
	CString str;
	char szFIDName[40];
	FIDCONTENT *FIDCont;
	CFIDIO* pFIDIO=NULL;
	CString m_WellName=PathName;
	m_WellName=m_WellName.Left (m_WellName.ReverseFind ('\\')+1);
	CString DBFFileName=m_WellName+"Curve.dbf";
	CString IndexFile=GetWorkPath("Config")+"Curve.dbf";
	if(!CopyFile(IndexFile,DBFFileName,FALSE))
	{
		str.Format ("%s\n创建井索引文件失败        ",DBFFileName);
		AfxMessageBox(str);
	}
	if((hFile = _findfirst(m_WellName+"I*.*", &c_file )) != -1L)
	{ 
		CDbaseFile Base;		
		if(Base.Open(DBFFileName)==DBASE_NO_FILE)
		{
			_findclose( hFile );
			str.Format ("%s\n打开井索引文件失败    ",DBFFileName);
			AfxMessageBox(str);
			return;
		}
		DWORD dwCount= Base.GetRecordCount();
		dwCount++;
		Base.AddRecord();
		if(Base.GetRecord(dwCount) == DBASE_SUCCESS)
		{
			CFIDIO::GetFIDName(c_file.name,szFIDName);
			pFIDIO = new CFIDIO(m_WellName+c_file.name);         
			if(pFIDIO->Open())
			{	
				str=szFIDName;
				Base.PutCharField("曲线名称",str);
				dim=pFIDIO->GetDimenSion();
				if(dim>2) str="阵列";
				else      str="常规";
				Base.PutCharField("曲线类型",str);
				
				FIDCont = pFIDIO->GetContent(1);
				Base.PutFloatField("Z轴最小值",FIDCont->min);     
				Base.PutFloatField("Z轴最大值",FIDCont->max);
				str=FIDCont->unit;
				Base.PutCharField("Z轴单位",str);
				Base.PutFloatField("Z轴步长",FIDCont->rlev);
				Base.PutNumericField("Z轴类型",FIDCont->length);
				Base.PutFloatField("Z轴采样",FIDCont->npw);
				

				FIDCont = pFIDIO->GetContent(2);
				Base.PutFloatField("X轴最小值",FIDCont->min);     
				Base.PutFloatField("X轴最大值",FIDCont->max);
				str=FIDCont->unit;
				Base.PutCharField("X轴单位",str);
				Base.PutFloatField("X轴步长",FIDCont->rlev);
				Base.PutNumericField("X轴类型",FIDCont->length);
				Base.PutFloatField("X轴采样",FIDCont->npw);
				if(dim>2)
				{
					FIDCont = pFIDIO->GetContent(dim);
					Base.PutFloatField("Y轴最小值",FIDCont->min);     
					Base.PutFloatField("Y轴最大值",FIDCont->max);
					str=FIDCont->unit;
					Base.PutCharField("Y轴单位",str);
					Base.PutFloatField("Y轴步长",FIDCont->rlev);
					Base.PutNumericField("Y轴类型",FIDCont->length);
					Base.PutFloatField("Y轴采样",FIDCont->npw);
				}
				
				CTime date(c_file.time_write);
				str=date.Format("%Y-%m-%d %H:%M:%S");
				Base.PutCharField("修改时间",str);
				Base.PutRecord(dwCount);
			}
			delete pFIDIO;
		}
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			dwCount++;
			Base.AddRecord();
			if (Base.GetRecord(dwCount) == DBASE_SUCCESS)
			{
				CFIDIO::GetFIDName(c_file.name,szFIDName);
				pFIDIO = new CFIDIO(m_WellName+c_file.name);         
				if (pFIDIO->Open())
				{	
					str=szFIDName;
					Base.PutCharField("曲线名称",str);
					dim=pFIDIO->GetDimenSion();
					if(dim>2) str="阵列";
					else      str="常规";
					Base.PutCharField("曲线类型",str);
					
					FIDCont = pFIDIO->GetContent(1);
					Base.PutFloatField("Z轴最小值",FIDCont->min);     
					Base.PutFloatField("Z轴最大值",FIDCont->max);
					str=FIDCont->unit;
					Base.PutCharField("Z轴单位",str);
					Base.PutFloatField("Z轴步长",FIDCont->rlev);
					Base.PutNumericField("Z轴类型",FIDCont->length);
					Base.PutFloatField("Z轴采样",FIDCont->npw);
					

					FIDCont = pFIDIO->GetContent(2);
					Base.PutFloatField("X轴最小值",FIDCont->min);     
					Base.PutFloatField("X轴最大值",FIDCont->max);
					str=FIDCont->unit;
					Base.PutCharField("X轴单位",str);
					Base.PutFloatField("X轴步长",FIDCont->rlev);
					Base.PutNumericField("X轴类型",FIDCont->length);
					Base.PutFloatField("X轴采样",FIDCont->npw);
					if(dim>2)
					{
						FIDCont = pFIDIO->GetContent(dim);
						Base.PutFloatField("Y轴最小值",FIDCont->min);     
						Base.PutFloatField("Y轴最大值",FIDCont->max);
						str=FIDCont->unit;
						Base.PutCharField("Y轴单位",str);
						Base.PutFloatField("Y轴步长",FIDCont->rlev);
						Base.PutNumericField("Y轴类型",FIDCont->length);
						Base.PutFloatField("Y轴采样",FIDCont->npw);
					}
					
					CTime date(c_file.time_write);
					str=date.Format("%Y-%m-%d %H:%M:%S");
					Base.PutCharField("修改时间",str);
					Base.PutRecord(dwCount);
				}
				delete pFIDIO;
			}
		}
		Base.Close();
	}
	_findclose( hFile );
}

CFIDIO::CFIDIO(const char* pszPathName ,const FIDINDEX* pIndex)
{  
	DatID = 0;
	long num0;
	if (sizeof(int) == 4) num0 = 1024*1024;
	else num0 = 10240;
	char m_name[_MAX_PATH];
	openflag = 0;   
	m_Contentlist = NULL;
	m_Index.adi = NULL;
	strcpy(m_pszPathName,pszPathName);
	GetFileName(m_pszPathName,m_name,'I');
	if ((fp = fopen(m_name,"w+t")) != NULL)
	{
		WriteIndex(fp,pIndex);
		fclose(fp);
		fp = fopen(m_name,"r+t");
		ReadIndex(fp);
		fclose(fp);
		GetFileName(m_pszPathName,m_name,'D');
		if ((fp = fopen(m_name,"w+b")) != NULL)
		{
			int length = 0;
			for (long j=0; j< m_Index.dimension; j++)
			{
				length += m_Contentlist[j].length * m_Contentlist[j].npw;
			}
			if (length == 0) return;
			if (fabs(m_Contentlist[0].rlev)<0.000001) return;;
			long num = Round((m_Contentlist[0].max - m_Contentlist[0].min) / m_Contentlist[0].rlev) + 1;
			if (num0 > num) num0 = num;
			char *buffer = new char[num0*length];

			GetDefaultContentsValue(buffer,num0); 
			
			long k = num / num0;
			for (j=0; j<k; j++) fwrite(buffer,length,num0,fp);
			k = num - k*num0;
			if (k != 0) fwrite(buffer,length,k,fp);
			delete buffer;
			fclose(fp);
		}
		Open();
	}
}

CFIDIO::CFIDIO(const char* pszPathName)
{                         
	int i = 0;
	DatID = 0;
	openflag = 0;   
	m_Contentlist = NULL;
	m_Index.adi = NULL;
	m_Index.Contentlist = NULL;
	m_Index.adi = NULL;
	strcpy(m_pszPathName, pszPathName);
	Open();
}               

CFIDIO::~CFIDIO()
{
	Close();
	if (m_Contentlist != NULL) 
	{
		delete m_Contentlist;
		m_Contentlist = NULL;
	}	
	if (m_Index.adi != NULL) delete m_Index.adi;
}

void CFIDIO::GetDefaultContentsValue(char* p,size_t num)
{

	if (num <=0) return;
	int len1 = 0, len = 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (m_Contentlist[i].npw == 0) continue;
		len = m_Contentlist[i].length;
		if (len == 0) return;
		switch (m_Contentlist[i].type)
		{
		case REPR_CHAR:
			*((char*)p) = -127;
			break;
		case REPR_SHORT:
			*((short*)p) = -9999;
			break;
		case REPR_INT:
			*((int*)p) = -9999;
			break;
		case REPR_LONG:
			*((long*)p) = -9999;
			break;
		case REPR_FLOAT:
			*((float*)p) = -9999.f;
			break;
		case REPR_DOUBLE:
			*((double*)p) = -9999;
			break;
		default:
			*p = 0;
			break;
		}
		if (m_Contentlist[i].npw > 1) memcpy(p+(m_Contentlist[i].npw-1)*len,p,len); 
		len1 += len;
	}
	if (len1 == 0) return;
    for (unsigned int j=1; j<num; j*=2)
    {        
		if (2*j > num) break;
    	memcpy(p+j*len1,p,j*len1);
    }
    if (j < num)
    {
    	memcpy(p+j*len1,p,(num-j)*len1);
    }
}


int CFIDIO::Open()
{
	char m_name[512];
	filetype = 0;
	if (openflag) 
	{
	 	SeekToTop();
	 	return openflag;
	} 	
	GetFileName(m_pszPathName,m_name,'I');
	if ((fp = fopen(m_name,"rt")) != NULL)
	{
		if (!ReadIndex(fp)) 
		{
			fclose(fp);
			return openflag;
		}
		fclose(fp);
	}
	else
	{
		openflag = 0;
		filetype = 2;
		return 0;
	}
	GetFileName(m_pszPathName,m_name,'D');
	if ((fp = fopen(m_name,"r+b")) != NULL)
	{
		openflag = 1;
		filetype = 2;
	}
	else 
	{
		GetFileName(m_pszPathName,m_name,'I');
		if ((fp = fopen(m_name,"r+t")) != NULL)
		{
			openflag = 1;
			filetype = 1;
		}
	}	
	m_RecNo = 1;
	position = 0;
	return openflag;
}

int CFIDIO::Close()
{
	if (openflag)
	{
		fclose(fp);
		openflag = 0;
	}
	return openflag;	
}

int CFIDIO::ReadIndex(FILE* fp)
{
	if (m_Contentlist != NULL) 
	{
		delete m_Contentlist;
		m_Contentlist = NULL;
	}	
	if (m_Index.adi != NULL) 
	{
		delete m_Index.adi;
		m_Index.adi = NULL;
	}
	int i = 0; int j = 0; int k =0;
	char s[20];
	m_Index.dimension = 2;
	int bID_Found = 0;
	while (fscanf(fp,"%s",s) != EOF)
	{
	 	if (i == 1) 
	 	{
	 		m_Index.dimension = atoi(s); 
	 		if (m_Index.dimension < 1) return 0; 
	 		m_Contentlist = new FIDCONTENT[m_Index.dimension];
			m_Index.Contentlist = m_Contentlist;
	 	}
	 	k = (i - 2) / 18;
	 	j = i - k*18 - 2;
	 	if (k >= m_Index.dimension) 
	 	{                                            
			if (!stricmp(s,"==END==")) 
			{
				m_Index.adi = new char[1024];
				m_Index.adi[0] = 0;
				while (fscanf(fp,"%s",s) != EOF)
				{
					strcat(m_Index.adi,s);
					strcat(m_Index.adi," ");
				}
				break;
			}
			continue;
	 	}
	 	switch (j)
		{
		case 1:	strcpy(m_Contentlist[k].name,s);	break;
	 	case 3:	strcpy(m_Contentlist[k].unit,s);  break;
	 	case 5:	{
	 				if (!stricmp(s,"float")) m_Contentlist[k].type = REPR_FLOAT;
	 				if (!stricmp(s,"int")) m_Contentlist[k].type = REPR_INT;
	 				if (!stricmp(s,"char")) m_Contentlist[k].type = REPR_CHAR;
	 				if (!stricmp(s,"string")) m_Contentlist[k].type = REPR_STRING;
	 			} break;
		case 7:	{
					m_Contentlist[k].length = atoi(s);
					if (m_Contentlist[k].type == REPR_INT)
					{                                
						if (m_Contentlist[k].length == 2) m_Contentlist[k].type = REPR_SHORT;
						if (m_Contentlist[k].length == 4) m_Contentlist[k].type = REPR_LONG;
					}
					if (m_Contentlist[k].type == REPR_FLOAT)
					{
						if (m_Contentlist[k].length == 4) m_Contentlist[k].type = REPR_FLOAT;
						if (m_Contentlist[k].length == 8) m_Contentlist[k].type = REPR_DOUBLE;
					}
				}
				break;
		case 9: m_Contentlist[k].nps = atoi(s);	break;
		case 11: m_Contentlist[k].npw = atoi(s);	break;
		case 13: m_Contentlist[k].min = (float) atof(s);	break;
		case 15: m_Contentlist[k].max = (float) atof(s);	break;
		case 17: m_Contentlist[k].rlev =(float) atof(s);	break;
		}	
		i++;
	}                              
	j = 0; 
	for (i=0; i<m_Index.dimension; i++)
	{
		if (j != -1) 
		{
			m_Contentlist[i].offset = j;
			if (m_Contentlist[i].npw != 0) j += m_Contentlist[i].length*m_Contentlist[i].npw;
			if (m_Contentlist[i].length == -1) j = -1;
		}
		else m_Contentlist[i].offset = -1;	
	}       
	m_Index.Contentlist = m_Contentlist;                          
	m_RecLen = j; 
	m_RecNo = 0;
	return 1; 
}

void CFIDIO::WriteIndex(FILE* fp, const FIDINDEX* pIndex)
{
	int i,k;          
	fprintf(fp,"DIMENSION %i\n",pIndex->dimension);
	for (i=0; i<pIndex->dimension; i++)
	{
		k = i + 1;
		fprintf(fp," INDEX-%i-NAME %s",k,pIndex->Contentlist[i].name);
		fprintf(fp," UNIT %s",pIndex->Contentlist[i].unit);
		fprintf(fp," %s","TYPE");
		if (pIndex->Contentlist[i].type == REPR_CHAR) fprintf(fp," char");
		if (pIndex->Contentlist[i].type == REPR_SHORT) fprintf(fp," int");
		if (pIndex->Contentlist[i].type == REPR_INT) fprintf(fp," int");
		if (pIndex->Contentlist[i].type == REPR_LONG) fprintf(fp," int");
		if (pIndex->Contentlist[i].type == REPR_FLOAT) fprintf(fp," float");
		if (pIndex->Contentlist[i].type == REPR_DOUBLE) fprintf(fp," float");
		if (pIndex->Contentlist[i].type == REPR_STRING) fprintf(fp," string");
		fprintf(fp," LENGTH %i",pIndex->Contentlist[i].length);
		fprintf(fp," NPS %i",pIndex->Contentlist[i].nps);
		fprintf(fp," NPW %i",pIndex->Contentlist[i].npw);
		if(k==1)
		{
			fprintf(fp," SDEP %f",pIndex->Contentlist[i].min);
			fprintf(fp," EDEP %f",pIndex->Contentlist[i].max);
		}
		else
		{
			fprintf(fp," MIN %f",pIndex->Contentlist[i].min);
			fprintf(fp," MAX %f",pIndex->Contentlist[i].max);
		}
		fprintf(fp," RLEV %f\n",pIndex->Contentlist[i].rlev);
	}   
	fprintf(fp," %s","==END==");
}

int CFIDIO::GetDimenSion()
{
	return m_Index.dimension;
}

FIDCONTENT* CFIDIO::GetContent(int dimension)
{
 	if (m_Contentlist == NULL) return NULL;
 	if ((dimension < 1) || (dimension > m_Index.dimension)) return NULL;
 	return m_Contentlist+dimension-1;
}

FIDCONTENT* CFIDIO::GetContent(const char* name)
{
 	if (m_Contentlist == NULL) return NULL;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) return m_Contentlist+i;
	}
 	return NULL;
}   

void CFIDIO::Convert(char* buf,size_t len,size_t bytes)
{
#ifdef _UNIX_
	if (DatID == 1) return;
#else
	if (DatID == 0) return;
#endif
	if (bytes <= 1) return;
	char* tmp = new char[bytes];          
	long j=0;
	for (size_t i=0; i<len; i++)
	{
		memcpy(tmp,buf+j,bytes);
		for (size_t k=0; k< bytes; k++) buf[j+k] = tmp[bytes-k-1];
		j += bytes;
	}       
	delete tmp;
}

int CFIDIO::ReverseDepth()
{
	long i,j;  
	FILE* fp1;
	if (!Open()) return 0;

	char m_name2[512];
	GetFileName(m_pszPathName,m_name2,'D');
	strcat(m_name2,"%%%");
	if ((fp1 = fopen(m_name2,"w+b")) == NULL)
	{
		DeleteFile(m_name2);
		return 0;
	}

	long nBufRecNum;
//	if (sizeof(int) == 4) nBufRecNum = 1024*1024/m_RecLen;
//	else 
		nBufRecNum = 102400;
	long nTotalRecNum = Round((m_Index.Contentlist[0].max - m_Index.Contentlist[0].min) / m_Index.Contentlist[0].rlev) + 1;
	if (nBufRecNum > nTotalRecNum) nBufRecNum = nTotalRecNum;
	char *buffer = new char[nBufRecNum*m_RecLen];
	char *tmpbuf = new char[m_RecLen];
	long nBufNum = nTotalRecNum / nBufRecNum;
	fseek(fp,0,SEEK_END);
	for (j=0; j<nBufNum; j++) 
	{
		fseek(fp,-nBufRecNum * m_RecLen,SEEK_CUR);
		fread(buffer,m_RecLen,nBufRecNum,fp);
		long nBufNum2 = nBufRecNum / 2;
		long pos1 = 0; 
		long pos2 = (nBufRecNum-1)*m_RecLen;
		for (i=0; i<nBufNum2; i++)
		{
			memcpy(tmpbuf,buffer+pos1,m_RecLen);
			memcpy(buffer+pos1,buffer+pos2,m_RecLen);
			memcpy(buffer+pos2,tmpbuf,m_RecLen);
			pos1 += m_RecLen;
			pos2 -= m_RecLen;
		}
		fwrite(buffer,m_RecLen,nBufRecNum,fp1);
	}
	nBufRecNum = nTotalRecNum - nBufNum*nBufRecNum;
	if (nBufRecNum>0) 
	{
		fseek(fp,0,0);
		fread(buffer,m_RecLen,nBufRecNum,fp);
		long nBufNum2 = nBufRecNum / 2;
		long pos1 = 0; long pos2 = (nBufRecNum-1)*m_RecLen;
		for (i=0; i<nBufNum2; i++)
		{
			memcpy(tmpbuf,buffer+pos1,m_RecLen);
			memcpy(buffer+pos1,buffer+pos2,m_RecLen);
			memcpy(buffer+pos2,tmpbuf,m_RecLen);
			pos1 += m_RecLen;
			pos2 -= m_RecLen;
		}
		fwrite(buffer,m_RecLen,nBufRecNum,fp1);
	}
	fclose(fp1);
	delete buffer;
	delete tmpbuf;
	Close();
	char m_name[512];

	//reset Index
	float dep = m_Contentlist[0].min;
	m_Contentlist[0].min = m_Contentlist[0].max;
	m_Contentlist[0].max = dep;
	m_Contentlist[0].rlev *= -1;
	GetFileName(m_pszPathName,m_name,'I');
	if ((fp = fopen(m_name,"w+t")) != NULL)
	{
		WriteIndex(fp,&m_Index);
		fclose(fp);
	}
	else return 0;

	//reset body
	GetFileName(m_pszPathName,m_name,'D');
	DeleteFile(m_name);
	rename(m_name2,m_name);
	return 1;
}

int CFIDIO::SetDepth(float sdep, float edep)
{
	if (!Open()) return 0;
	if (m_RecLen <= 0) return 0;

	long m_RecNo1,m_RecNo2;
	float m_Sdep = m_Contentlist[0].min;
	float m_Edep = m_Contentlist[0].max;
	float m_Rlev = m_Contentlist[0].rlev;
	if ((m_Rlev == -99999) || (fabs(m_Rlev)<0.00001)) return 0;

	FILE* fp1;
	char m_name2[512];
	GetFileName(m_pszPathName,m_name2,'D');
	strcat(m_name2,"%%%");
	if ((fp1 = fopen(m_name2,"w+b")) == NULL)
	{
		DeleteFile(m_name2);
		return 0;
	}

	long srcRecNum = Round((m_Edep - m_Sdep) / m_Rlev) + 1;
	m_RecNo1 = Round((sdep - m_Sdep) / m_Rlev) + 1;
	m_RecNo2 = Round((edep - m_Sdep) / m_Rlev) + 1;
	if (m_RecNo1 > m_RecNo2) return 0;
	if ((m_RecNo1 == 1) && (m_RecNo2 == srcRecNum)) return 1;
	if ((m_RecNo2 < 1) || (m_RecNo1 > srcRecNum)) return 0;
	
	char* buffer;
	long nBufNum,nBufRecNum,nTotalRecNum;

	if (m_RecNo1 < 1)  
	{
		if (sizeof(int) == 4) nBufRecNum = 1024*1024/m_RecLen;
		else nBufRecNum = 10240;
		nTotalRecNum = 1 - m_RecNo1;
		if (nBufRecNum > nTotalRecNum) nBufRecNum = nTotalRecNum;
		buffer = new char[nBufRecNum*m_RecLen];
		GetDefaultContentsValue(buffer,nBufRecNum); 
		nBufNum = nTotalRecNum / nBufRecNum;
		for (long i=0; i<nBufNum; i++) fwrite(buffer,m_RecLen,nBufRecNum,fp1);
		nBufRecNum = nTotalRecNum - nBufNum*nBufRecNum;
		if (nBufRecNum != 0) fwrite(buffer,m_RecLen,nBufRecNum,fp1);
		delete buffer;
		m_RecNo1 = 1;
	}

	if (sizeof(int) == 4) nBufRecNum = 1024*1024/m_RecLen;
	else nBufRecNum = 10240;
	if (m_RecNo2 < srcRecNum) nTotalRecNum = m_RecNo2 - m_RecNo1 + 1;
	else nTotalRecNum = srcRecNum;
	if (nBufRecNum > nTotalRecNum) nBufRecNum = nTotalRecNum;
	buffer = new char[nBufRecNum*m_RecLen];
	nBufNum = nTotalRecNum / nBufRecNum;
	fseek(fp,(m_RecNo1-1)*m_RecLen,0);
	for (long i=0; i<nBufNum; i++) 
	{
		fread(buffer,m_RecLen,nBufRecNum,fp);
		fwrite(buffer,m_RecLen,nBufRecNum,fp1);
	}
	nBufRecNum = nTotalRecNum - nBufNum*nBufRecNum;
	if (nBufRecNum != 0) 
	{
		fread(buffer,m_RecLen,nBufRecNum,fp);
		fwrite(buffer,m_RecLen,nBufRecNum,fp1);
	}
	delete buffer;

	if (m_RecNo2 > srcRecNum)
	{
		if (sizeof(int) == 4) nBufRecNum = 1024*1024/m_RecLen;
		else nBufRecNum = 10240;
		nTotalRecNum = m_RecNo2 - srcRecNum;
		if (nBufRecNum > nTotalRecNum) nBufRecNum = nTotalRecNum;
		buffer = new char[nBufRecNum*m_RecLen];
		GetDefaultContentsValue(buffer,nBufRecNum); 
		nBufNum = nTotalRecNum / nBufRecNum;
		for (long i=0; i<nBufNum; i++) fwrite(buffer,m_RecLen,nBufRecNum,fp1);
		nBufRecNum = nTotalRecNum - nBufNum*nBufRecNum;
		if (nBufRecNum != 0) fwrite(buffer,m_RecLen,nBufRecNum,fp1);
		delete buffer;
	}
	fclose(fp1);
	Close();
	char m_name[512];

	//reset Index
	m_Contentlist[0].min = sdep;
	m_Contentlist[0].max = edep;
	GetFileName(m_pszPathName,m_name,'I');
	if ((fp = fopen(m_name,"w+t")) != NULL)
	{
		WriteIndex(fp,&m_Index);
		fclose(fp);
	}
	else return 0;

	//reset body
	GetFileName(m_pszPathName,m_name,'D');
	DeleteFile(m_name);
	rename(m_name2,m_name);
	return 1;
}

size_t CFIDIO::Read(int dim, size_t count, float* buffer)
{
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dim < 1) || (dim > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dim-1];
	if (m_Content.npw == 0)
	{                                                         
		long j = m_RecNo;
		if (m_Content.rlev != -99999.)
		{
			for (i=0; i<count; i++) buffer[i] = m_Content.min + (j+i)*m_Content.rlev;
			return count;
		}   
		else 
			return 0;
	}
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*m_Content.npw*num];
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_ReadCount = 0;
	if (offset == 0) 
	{
		m_ReadCount = fread(tmpbuf,m_Content.length,count,fp);
		if (m_ReadCount < 0) goto rtn;
	}	
	else                       
	{
		fseek(fp,m_Content.offset,SEEK_CUR);
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fread(tmpbuf+m_ReadCount*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_ReadCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_ReadCount += m_Content.npw;
		}
		if ((num2 != 0) && (i == num))
		{
			if (!feof(fp))
			{
				j = fread(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
				if (j > 0) m_ReadCount += j;
			}	
		}       
	}                
	Convert(tmpbuf,m_ReadCount,m_Content.length);	
	if (m_Content.type == REPR_FLOAT)
	{   
		memcpy(buffer,tmpbuf,m_ReadCount*m_Content.length);
	}
	else
	{
		char *s = tmpbuf;
		for (j=0; j<m_ReadCount; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_CHAR: 	
				buffer[j] = (float) (*((unsigned char *)s));
				break;
			case REPR_SHORT: 	
				buffer[j] = (float) (*((short*)s));
				break;
			case REPR_LONG:
				buffer[j] = (float) (*((long*)s));	
				break;
			case REPR_DOUBLE:
				buffer[j] = (float) (*((double*)s));
				break;
			} 
			s += m_Content.length;
		}
	}
	for (j=m_ReadCount; j<count; j++) 
		buffer[j] = -99999.f;
rtn:
	delete [] tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_ReadCount;
}

size_t CFIDIO::Read(int dim, size_t count, double* buffer)
{
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dim < 1) || (dim > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dim-1];
	if (m_Content.npw == 0)
	{                                                         
		long j = m_RecNo;
		if (m_Content.rlev != -99999.)
		{
			for (i=0; i<count; i++) buffer[i] = m_Content.min + (j+i)*m_Content.rlev;
			return count;
		}   
		else 
			return 0;
	}
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*m_Content.npw*num];
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_ReadCount = 0;
	if (offset == 0) 
	{
		m_ReadCount = fread(tmpbuf,m_Content.length,count,fp);
		if (m_ReadCount < 0) goto rtn;
	}	
	else                       
	{
		fseek(fp,m_Content.offset,SEEK_CUR);
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fread(tmpbuf+m_ReadCount*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_ReadCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_ReadCount += m_Content.npw;
		}
		if ((num2 != 0) && (i == num))
		{
			if (!feof(fp))
			{
				j = fread(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
				if (j > 0) m_ReadCount += j;
			}	
		}       
	}                
	Convert(tmpbuf,m_ReadCount,m_Content.length);	
	if (m_Content.type == REPR_DOUBLE)
	{   
		memcpy(buffer,tmpbuf,m_ReadCount*m_Content.length);
	}
	else
	{
		char *s = tmpbuf;
		for (j=0; j<m_ReadCount; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_CHAR: 	
				buffer[j] = (double) (*((unsigned char *)s));
				break;
			case REPR_SHORT: 	
				buffer[j] = (double) (*((short*)s));
				break;
			case REPR_LONG:
				buffer[j] = (double) (*((long*)s));	
				break;
			case REPR_FLOAT:
				buffer[j] = (double) (*((float*)s));
				break;
			} 
			s += m_Content.length;
		}
	}
	for (j=m_ReadCount; j<count; j++) 
		buffer[j] = -99999.;
rtn:
	delete [] tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_ReadCount;
}

size_t CFIDIO::Read(int dim, size_t count, short* buffer)
{
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dim < 1) || (dim > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dim-1];
	if (m_Content.npw == 0)
	{                                                         
		long j = m_RecNo;
		if (m_Content.rlev != -99999.)
		{
			for (i=0; i<count; i++) buffer[i] = (short)(m_Content.min + (j+i)*m_Content.rlev);
			return count;
		}   
		else 
			return 0;
	}
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*m_Content.npw*num];
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_ReadCount = 0;
	if (offset == 0) 
	{
		m_ReadCount = fread(tmpbuf,m_Content.length,count,fp);
		if (m_ReadCount < 0) goto rtn;
	}	
	else                       
	{
		fseek(fp,m_Content.offset,SEEK_CUR);
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fread(tmpbuf+m_ReadCount*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_ReadCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_ReadCount += m_Content.npw;
		}
		if ((num2 != 0) && (i == num))
		{
			if (!feof(fp))
			{
				j = fread(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
				if (j > 0) m_ReadCount += j;
			}	
		}       
	}                
	Convert(tmpbuf,m_ReadCount,m_Content.length);	
	if (m_Content.type == REPR_SHORT)
	{   
		memcpy(buffer,tmpbuf,m_ReadCount*m_Content.length);
	}
	else
	{
		long k = 0;
		for (j=0; j<m_ReadCount; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_CHAR: 	
				buffer[j] = (short) ((unsigned char)tmpbuf[k]);
				break;
			case REPR_LONG:
				buffer[j] = (short) (*((long*)(tmpbuf+k)));	
				break;
			case REPR_FLOAT:
				buffer[j] = (short) (*((float*)(tmpbuf+k)));
				break;
			case REPR_DOUBLE: 	
				buffer[j] = (short) (*((double*)(tmpbuf+k)));
				break;
			} 
			k += m_Content.length;
		}
	}
	for (j=m_ReadCount; j<count; j++) 
		buffer[j] = -9999;
rtn:
	delete [] tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_ReadCount;
}

size_t CFIDIO::Read(int dim, size_t count, int* buffer)
{
	if (sizeof(int) == 2) return Read(dim,count,(short*)buffer);
	else if (sizeof(int) == 4) return Read(dim,count,(long*)buffer);
	else return 0;
}


size_t CFIDIO::Read(int dim, size_t count, long* buffer)
{
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dim < 1) || (dim > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dim-1];
	if (m_Content.npw == 0)
	{                                                         
		long j = m_RecNo;
		if (m_Content.rlev != -99999.)
		{
			for (i=0; i<count; i++) buffer[i] = (long)(m_Content.min + (j+i)*m_Content.rlev);
			return count;
		}   
		else 
			return 0;
	}
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*m_Content.npw*num];
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_ReadCount = 0;
	if (offset == 0) 
	{
		m_ReadCount = fread(tmpbuf,m_Content.length,count,fp);
		if (m_ReadCount < 0) goto rtn;
	}	
	else                       
	{
		fseek(fp,m_Content.offset,SEEK_CUR);
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fread(tmpbuf+m_ReadCount*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_ReadCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_ReadCount += m_Content.npw;
		}
		if ((num2 != 0) && (i == num))
		{
			if (!feof(fp))
			{
				j = fread(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
				if (j > 0) m_ReadCount += j;
			}	
		}       
	}                
	Convert(tmpbuf,m_ReadCount,m_Content.length);	
	if (m_Content.type == REPR_LONG)
	{   
		memcpy(buffer,tmpbuf,m_ReadCount*m_Content.length);
	}
	else
	{
		long k = 0;
		for (j=0; j<m_ReadCount; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_CHAR: 	
				buffer[j] = (long) (unsigned char)tmpbuf[k];
				break;
			case REPR_SHORT:
				buffer[j] = (long) (*((short*)(tmpbuf+k)));	
				break;
			case REPR_FLOAT:
				buffer[j] = (long) (*((float*)(tmpbuf+k)));
				break;
			case REPR_DOUBLE: 	
				buffer[j] = (long) (*((double*)(tmpbuf+k)));
				break;
			} 
			k += m_Content.length;
		}
	}
	for (j=m_ReadCount; j<count; j++) 
		buffer[j] = -99999;
rtn:
	delete [] tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_ReadCount;
}

size_t CFIDIO::Read(int dim, size_t count, char* buffer)
{
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dim < 1) || (dim > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dim-1];
	if (m_Content.npw == 0)
	{                                                         
		long j = m_RecNo;
		if (m_Content.rlev != -99999.)
		{
			for (i=0; i<count; i++) buffer[i] = (char)(m_Content.min + (j+i)*m_Content.rlev);
			return count;
		}   
		else 
			return 0;
	}
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*m_Content.npw*(num+1)];
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_ReadCount = 0;
	if (offset == 0) 
	{
		m_ReadCount = fread(tmpbuf,m_Content.length,count,fp);
		if (m_ReadCount < 0) goto rtn;
	}	
	else                       
	{
		fseek(fp,m_Content.offset,SEEK_CUR);
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fread(tmpbuf+m_ReadCount*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_ReadCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_ReadCount += m_Content.npw;
		}
		if ((num2 != 0) && (i == num))
		{
			if (!feof(fp))
			{
				j = fread(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
				if (j > 0) m_ReadCount += j;
			}	
		}       
	}                
	Convert(tmpbuf,m_ReadCount,m_Content.length);	
	if (m_Content.type == REPR_CHAR)
	{   
		memcpy(buffer,tmpbuf,m_ReadCount*m_Content.length);
	}
	else
	{
		long k = 0;
		for (j=0; j<m_ReadCount; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_SHORT: 	
				buffer[j] = (char) (*((short*)(tmpbuf[k])));
				break;
			case REPR_LONG:
				buffer[j] = (char) (*((long*)(tmpbuf+k)));	
				break;
			case REPR_FLOAT:
				buffer[j] = (char) (*((float*)(tmpbuf+k)));
				break;
			case REPR_DOUBLE: 	
				buffer[j] = (char) (*((double*)(tmpbuf+k)));
				break;
			} 
			k += m_Content.length;
		}
	}
	for (j=m_ReadCount; j<count; j++) 
		buffer[j] = -99;
rtn:
	delete tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_ReadCount;
}

size_t CFIDIO::Read(const char* name, size_t count, float* buffer)	
{   
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Read(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Read(const char* name, size_t count, double* buffer)	
{   
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Read(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Read(const char* name, size_t count, short* buffer)	
{   
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Read(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Read(const char* name, size_t count, int* buffer)	
{   
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Read(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Read(const char* name, size_t count, long* buffer)	
{   
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Read(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Read(const char* name, size_t count, char* buffer)	
{   
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Read(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Write(int dimension, size_t count, float *buffer)
{                                          
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dimension < 1) || (dimension > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dimension-1];
	if (m_Content.npw == 0) return 0;
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*count];

	if (num < 0) num =0;
	i = num * m_Content.npw;

	if (m_Content.type == REPR_FLOAT)
	{   
		memcpy(tmpbuf,buffer,count*m_Content.length);
	}
	else
	{       
		char *s = tmpbuf;
		long k = 0;
		for (j=0; j<count; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_CHAR: 	
				*s = (char) buffer[k];
				break;
			case REPR_INT: 	
				*((int*)s) = (int) buffer[k];
				break;
			case REPR_SHORT: 	
				*((short*)s) = (short) buffer[k];
				break;
			case REPR_LONG:
				*((long*)s) = (long) buffer[k];	
				break;
			case REPR_DOUBLE:
				*((double*)s) = (double) buffer[k];
				break;
			default:
				break;
			} 
			s += m_Content.length;
			k++;
		}
	}
	Convert(tmpbuf,count,m_Content.length);	
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_WriteCount = 0;
	if (offset == 0) m_WriteCount = fwrite(tmpbuf,m_Content.length,count,fp);
	else 
	{
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fwrite(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_WriteCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_WriteCount += j;
		}
		if (num2 != 0) 
		{
			j = fwrite(tmpbuf+num*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
			if (j > 0) m_WriteCount += j;
		}	
	}                

rtn:	
	delete [] tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_WriteCount;
}

size_t CFIDIO::Write(int dimension, size_t count, double *buffer)
{       
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dimension < 1) || (dimension > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dimension-1];
	if (m_Content.npw == 0) return 0;
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*count];

	if (num < 0) num =0;
	i = num * m_Content.npw;

	if (m_Content.type == REPR_DOUBLE)
	{   
		memcpy(tmpbuf,buffer,count*m_Content.length);
	}
	else
	{       
		char *s = tmpbuf;
		long k = 0;
		for (j=0; j<count; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_CHAR: 	
				*s = (char) buffer[k];
				break;
			case REPR_INT: 	
				*((int*)s) = (int) buffer[k];
				break;
			case REPR_SHORT: 	
				*((short*)s) = (short) buffer[k];
				break;
			case REPR_LONG:
				*((long*)s) = (long) buffer[k];	
				break;
			case REPR_FLOAT:
				*((float*)s) = (float) buffer[k];
				break;
			default:
				break;
			} 
			s += m_Content.length;
			k++;
		}
	}
	Convert(tmpbuf,count,m_Content.length);	
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_WriteCount = 0;
	if (offset == 0) m_WriteCount = fwrite(tmpbuf,m_Content.length,count,fp);
	else 
	{
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fwrite(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_WriteCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_WriteCount += j;
		}
		if (num2 != 0) 
		{
			j = fwrite(tmpbuf+num*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
			if (j > 0) m_WriteCount += j;
		}	
	}                

rtn:	
	delete [] tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_WriteCount;
}

size_t CFIDIO::Write(int dimension, size_t count, char *buffer)
{
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dimension < 1) || (dimension > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dimension-1];
	if (m_Content.npw == 0) return 0;
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*count];

	if (num < 0) num =0;
	i = num * m_Content.npw;

	if (m_Content.type == REPR_CHAR)
	{   
		memcpy(tmpbuf,buffer,count*m_Content.length);
	}
	else
	{       
		char *s = tmpbuf;
		long k = 0;
		for (j=0; j<count; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_INT: 	
				*((int*)s) = (int) buffer[k];
				break;
			case REPR_SHORT: 	
				*((short*)s) = (short) buffer[k];
				break;
			case REPR_LONG:
				*((long*)s) = (long) buffer[k];	
				break;
			case REPR_FLOAT:
				*((float*)s) = (float) buffer[k];
				break;
			case REPR_DOUBLE:
				*((double*)s) = (double) buffer[k];
				break;
			default:
				break;
			} 
			s += m_Content.length;
			k++;
		}
	}
	Convert(tmpbuf,count,m_Content.length);	
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_WriteCount = 0;
	if (offset == 0) m_WriteCount = fwrite(tmpbuf,m_Content.length,count,fp);
	else 
	{
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fwrite(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_WriteCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_WriteCount += j;
		}
		if (num2 != 0) 
		{
			j = fwrite(tmpbuf+num*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
			if (j > 0) m_WriteCount += j;
		}	
	}                

rtn:	
	delete [] tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_WriteCount;
}

size_t CFIDIO::Write(int dimension, size_t count, int *buffer)
{
	if (sizeof(int) == 2) return Write(dimension,count,(short*)buffer);
	else if (sizeof(int) == 4) return Write(dimension,count,(long*)buffer);
	else return 0;
}

size_t CFIDIO::Write(int dimension, size_t count, short *buffer)
{
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dimension < 1) || (dimension > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dimension-1];
	if (m_Content.npw == 0) return 0;
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*count];

	if (num < 0) num =0;
	i = num * m_Content.npw;

	if (m_Content.type == REPR_SHORT)
	{   
		memcpy(tmpbuf,buffer,count*m_Content.length);
	}
	else
	{       
		char *s = tmpbuf;
		long k = 0;
		for (j=0; j<count; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_CHAR: 	
				*s = (char) buffer[k];
				break;
			case REPR_INT: 	
				*((int*)s) = (int) buffer[k];
				break;
			case REPR_LONG:
				*((long*)s) = (long) buffer[k];	
				break;
			case REPR_FLOAT:
				*((float*)s) = (float) buffer[k];
				break;
			case REPR_DOUBLE: 	
				*((double*)s) = (double) buffer[k];
				break;
			default:
				break;
			} 
			s += m_Content.length;
			k++;
		}
	}
	Convert(tmpbuf,count,m_Content.length);	
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_WriteCount = 0;
	if (offset == 0) m_WriteCount = fwrite(tmpbuf,m_Content.length,count,fp);
	else 
	{
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fwrite(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_WriteCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_WriteCount += j;
		}
		if (num2 != 0) 
		{
			j = fwrite(tmpbuf+num*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
			if (j > 0) m_WriteCount += j;
		}	
	}                

rtn:	
	delete [] tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_WriteCount;
}

size_t CFIDIO::Write(int dimension, size_t count, long *buffer)
{
	size_t i,j,num;  
	if (openflag == 0) return 0; 
 	if ((dimension < 1) || (dimension > m_Index.dimension)) return 0;
	FIDCONTENT m_Content = m_Contentlist[dimension-1];
	if (m_Content.npw == 0) return 0;
	if ((m_Content.offset < 0) || (m_Content.length == 0)) return 0; 
    num = count / m_Content.npw;
    size_t num2 = count - num * m_Content.npw;
	char* tmpbuf = new char[((long)m_Content.length)*count];

	if (num < 0) num =0;
	i = num * m_Content.npw;

	if (m_Content.type == REPR_LONG)
	{   
		memcpy(tmpbuf,buffer,count*m_Content.length);
	}
	else
	{       
		char *s = tmpbuf;
		long k = 0;
		for (j=0; j<count; j++)
		{                          
			switch (m_Content.type)    
			{
			case REPR_CHAR: 	
				*s = (char) buffer[k];
				break;
			case REPR_INT: 	
				*((int*)s) = (int) buffer[k];
				break;
			case REPR_SHORT: 	
				*((short*)s) = (short) buffer[k];
				break;
			case REPR_FLOAT:
				*((float*)s) = (float) buffer[k];
				break;
			case REPR_DOUBLE: 	
				*((double*)s) = (double) buffer[k];
				break;
			default:
				break;
			} 
			s += m_Content.length;
			k++;
		}
	}
	Convert(tmpbuf,count,m_Content.length);	
	int offset = m_RecLen - m_Content.length*m_Content.npw;
	size_t m_WriteCount = 0;
	if (offset == 0) m_WriteCount = fwrite(tmpbuf,m_Content.length,count,fp);
	else 
	{
		for (i=0; i<num; i++)
		{
			if (feof(fp)) break;
			j = fwrite(tmpbuf+i*m_Content.npw*m_Content.length,m_Content.length,m_Content.npw,fp);
			if (j != (size_t) m_Content.npw) 
			{
				if (j > 0) m_WriteCount += j;
				if ((i == 0) && (j < 0)) goto rtn;
				break;
			}
			fseek(fp,offset,SEEK_CUR);
			m_WriteCount += j;
		}
		if (num2 != 0) 
		{
			j = fwrite(tmpbuf+num*m_Content.npw*m_Content.length,m_Content.length,num2,fp);
			if (j > 0) m_WriteCount += j;
		}	
	}                

rtn:	
	delete [] tmpbuf;
	fseek(fp,position,SEEK_SET);
	return m_WriteCount;
}

size_t CFIDIO::Write(const char* name, size_t count, float *buffer)
{
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Write(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Write(const char* name, size_t count, double *buffer)
{
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Write(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Write(const char* name, size_t count, char *buffer)
{
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Write(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Write(const char* name, size_t count, short *buffer)
{
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Write(i+1,count,buffer);
		}	
	}
	return 0;
}

size_t CFIDIO::Write(const char* name, size_t count, int *buffer)
{
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Write(i+1,count,buffer);
		}	
	}
	return 0;
}                                                       

size_t CFIDIO::Write(const char* name, size_t count, long *buffer)
{
 	if (openflag == 0) return 0;
	for (int i=0; i<m_Index.dimension; i++)
	{
		if (!stricmp(m_Contentlist[i].name,name)) 
		{
			return Write(i+1,count,buffer);
		}	
	}
	return 0;
}                                                       

int CFIDIO::SeekToTop()
{               
	if (openflag == 0) return -1;
	return fseek(fp,0,SEEK_SET);
}

int CFIDIO::SeekToDepth(float dep)
{                             
	if (openflag == 0) return -1;
	if (m_Contentlist[0].rlev == -99999.) return -1;
	if (m_RecLen == -1) return -1;
	if(dep<m_Contentlist[0].min||dep>m_Contentlist[0].max)
		return -1;
	depth  = dep;                             
	m_RecNo = Round((depth - m_Contentlist[0].min) / m_Contentlist[0].rlev);
	position = m_RecNo * m_RecLen;
	fseek(fp,position,SEEK_SET);
	return 0;
}

int CFIDIO::Next()
{
	if (openflag == 0) return -1;
	if (m_Contentlist[0].rlev == -99999.) return -1;
	if (m_RecLen == -1) return -1;
	depth  += m_Contentlist[0].rlev;                             
	m_RecNo += 1;
	position += m_RecLen;
	fseek(fp,m_RecLen,SEEK_CUR);
	return 0;
}

float CFIDIO::GetDepth()
{          
	return depth;
}

float CFIDIO::GetStartDepth()
{   
	return m_Contentlist[0].min;
}

float CFIDIO::GetEndDepth()
{
	return m_Contentlist[0].max;
}

float CFIDIO::GetDepthLevel()
{
	return m_Contentlist[0].rlev;
}

int CFIDIO::Copy(const char* name)
{
	char szNewIndexName[512];
	char szOldIndexName[512];
	char szNewBodyName[512];
	char szOldBodyName[512];
	GetFileName(m_pszPathName,szOldIndexName,'I');
	GetFileName(m_pszPathName,szOldBodyName,'D');
	GetFileName(name,szNewIndexName,'I');
	GetFileName(name,szNewBodyName,'D');
	Close();
	FILE* fp1 = fopen(szNewIndexName,"wt");
	if (fp1 == NULL) 
		return -1;
	char szNewFIDName[20],szOldFIDName[20];
	GetFIDName(name,szNewFIDName);
	int dim=GetDimenSion();
	FIDCONTENT* pContent = GetContent(dim);
	if (pContent != NULL)	
	{
		strcpy(szOldFIDName,pContent->name);
		strcpy(pContent->name,szNewFIDName);
	}
	WriteIndex(fp1,&m_Index);
	if (pContent != NULL)
	{
		strcpy(pContent->name,szOldFIDName);
	}
	fclose(fp1);
	if (!CopyFile(szOldBodyName,szNewBodyName,FALSE)) 
		return -1;
	Open();
	return 0;
}

int CFIDIO::Rename(const char* name)
{
	char szNewIndexName[512];
	char szOldIndexName[512];
	char szNewBodyName[512];
	char szOldBodyName[512];
	GetFileName(m_pszPathName,szOldIndexName,'I');
	GetFileName(m_pszPathName,szOldBodyName,'D');
	GetFileName(name,szNewIndexName,'I');
	GetFileName(name,szNewBodyName,'D');
	Close();
	FILE* fp1 = fopen(szNewIndexName,"wt");
	if (fp1 == NULL) 
		return -1;
	char szNewFIDName[20];
	GetFIDName(name,szNewFIDName);
	int dim=GetDimenSion();
	FIDCONTENT* pContent = GetContent(dim);
	if (pContent != NULL)	
		strcpy(pContent->name,szNewFIDName);
	WriteIndex(fp1,&m_Index);
	fclose(fp1);
	DeleteFile(szOldIndexName);
	if (!MoveFile(szOldBodyName,szNewBodyName)) 
		return -1;
	return 0;
}

int CFIDIO::Delete()
{
	char m_name[512];
	GetFileName(m_pszPathName,m_name,'I');
	Close();
	if ((fp = fopen(m_name,"rt")) != NULL)
	{
		fclose(fp);
		if (remove(m_name) == -1) return -1;
	}
	GetFileName(m_pszPathName,m_name,'D');
	if ((fp = fopen(m_name,"r+b")) != NULL)
	{
		fclose(fp);
		if (remove(m_name) == -1) return -1;
	}
	else 
	{
		GetFileName(m_pszPathName,m_name,'I');
		if ((fp = fopen(m_name,"r+t")) != NULL)
		{
			fclose(fp);
			if (remove(m_name) == -1) return -1;
		}
		else return 0;
	}	
	return 1;
}

long Round(double x)
{
	if (x>=0) return (long) (x+0.5);
	else return (long) (x-0.5);
}

int CFIDIO::GetTypeLength(int type)
{                                   
	switch (type)
	{
		case REPR_CHAR: return sizeof(char);
		case REPR_FLOAT: return sizeof(float);
		case REPR_DOUBLE: return sizeof(double);
		case REPR_INT: return sizeof(int);
		case REPR_SHORT: return sizeof(short);
		case REPR_LONG: return sizeof(long);
	}   
	return 0;
}

void CFIDIO::GetFileName(const char* path, char* name,char pFileType)
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

void CFIDIO::GetFullFIDName(const char* path, char* name)
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

void CFIDIO::GetFullWellName(const char* path, char* name)
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

void CFIDIO::MakePath(char* path, const char* drive, const char* dir, const char* fname, const char* ext)
{
	if ((path==NULL) || (drive==NULL) || (dir==NULL) || (fname==NULL) || (ext==NULL)) return ;
	strcpy(path,"");
	strcat(path,drive);
	strcat(path,dir);
	strcat(path,fname);									 
	strcat(path,ext);
}

void CFIDIO::SplitPath(const char* path, char* drive, char* dir, char* fname, char* ext)
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
	CString szUncSep = szPathSep;
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

void CFIDIO::GetFIDName(const char* path, char* name)
{
	if ((path == NULL) || (name == NULL)) return;
	char ext[_MAX_EXT]; 
	SplitPath(path, NULL, NULL, NULL, ext);
	if (strcmp(ext,""))
		strcpy(name,ext+1);
	else
		strcpy(name,"");
}

void CFIDIO::GetWellName(const char* path, char* name)
{
	if ((path == NULL) || (name == NULL)) return;
	char fname[_MAX_FNAME];
	SplitPath(path, NULL, NULL, fname, NULL);
	if ((fname[0] == 'I') || (fname[0] == 'D'))
		strcpy(name,fname+1);
	else
		strcpy(name,fname);
}

void CFIDIO::GetPath(const char* path, char* name)
{
	if ((path == NULL) || (name == NULL)) return;
	char drive[64];
	char dir[_MAX_DIR];
	SplitPath(path, drive, dir, NULL, NULL);
	MakePath(name, drive, dir,"","");
}

void CFIDIO::GetDefaultValue(char* p, int repc, size_t num)
{
	if (num <=0) return;
	int len = GetTypeLength(repc);
	if (len == 0) return;
	switch (repc)
	{
	case REPR_CHAR:
		*((char*)p) = -127;
		break;
	case REPR_SHORT:
		*((short*)p) = -9999;
		break;
	case REPR_INT:
		*((int*)p) = -9999;
		break;
	case REPR_LONG:
		*((long*)p) = -9999;
		break;
	case REPR_FLOAT:
		*((float*)p) = -9999.f;
		break;
	case REPR_DOUBLE:
		*((double*)p) = -9999;
		break;
	}
    for (unsigned int j=1; j<num; j*=2)
    {        
		if (2*j > num) break;
    	memcpy(p+j*len,p,j*len);
    }
    if (j < num)
    {
    	memcpy(p+j*len,p,(num-j)*len);
    }
}

int CFIDIO::MoveDepth(float delta)
{
	char szIndexName[512];
	GetFileName(m_pszPathName,szIndexName,'I');
	Close();
	FILE* fp1 = fopen(szIndexName,"wt");
	if (fp1 == NULL) 
	{
		AfxMessageBox("写索引错误",MB_OK | MB_ICONINFORMATION);
		return -1;
	}
	m_Contentlist[0].min += delta;
	m_Contentlist[0].max += delta;
	WriteIndex(fp1,&m_Index);
	fclose(fp1);
	return 0;
} 

int CFIDIO::UnitConvert(float m_f)
{
	char szIndexName[512];
	GetFileName(m_pszPathName,szIndexName,'I');
	Close();	
	CString str=m_Contentlist[0].unit;
	str.MakeUpper ();
	while(str.Left(1)==" ")
		str.TrimLeft (" ");
	while(str.Right(1)==" ")
		str.TrimRight (" ");
	if((str=="M")&&(m_f==1.f))
	{
		strcpy(m_Contentlist[0].unit,"feet");
		m_Contentlist[0].min /= 0.3048f;
		m_Contentlist[0].max /= 0.3048f;
		m_Contentlist[0].rlev/=0.3048f;
	}
	else if((str=="FEET")&&(m_f==0.f))
	{
		strcpy(m_Contentlist[0].unit,"m");
		m_Contentlist[0].min *= 0.3048f;
		m_Contentlist[0].max *= 0.3048f;
		m_Contentlist[0].rlev*=0.3048f;
	}
	else if((str=="M")&&(m_f!=0.f)&&(m_f!=1.f))
	{
		m_Contentlist[0].min *= m_f;
		m_Contentlist[0].max *= m_f;
		m_Contentlist[0].rlev*=m_f;
	}
	else 
		return FALSE;
	FILE* fp1 = fopen(szIndexName,"wt");
	if (fp1 == NULL) 
	{
		AfxMessageBox("写索引错误",MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}
	WriteIndex(fp1,&m_Index);
	fclose(fp1);
	return TRUE;
}

BOOL GetMinMaxValue(CString WellPath,CString Curve,
					float &Min, float &Max,int &LineLog)
{
	int m1=WellPath.ReverseFind ('\\');
	CString m_WellName=WellPath.Mid(m1+1);
	if(m_WellName.ReverseFind ('.')>-1)
		m_WellName=m_WellName.Left (m_WellName.ReverseFind ('.'));
	CString m_IndexFile=WellPath.Left(m1)+"\\I"+m_WellName+"."+Curve;
	CString m_DataFile=WellPath.Left(m1)+"\\D"+m_WellName+"."+Curve;
	//读取索引和数据文件
	CString cs;
	char s[1024],str1[1024],str2[1024],str3[1024];
	int Dimension;  //维数
	float Stdep,Endep,Rlev;  //数据的起止深度及采样间隔
	char DeptUnits[41]; //深度单位
	char CurveName[41],CurveUnits[41]; //曲线名和曲线单位
	int NumSam; //曲线采样点数
	float *CurveData;  //曲线数据
	FILE *fdIndex,*fdData;
	char IndexType[21],DataType[21];
	int IndexLen,DataLen;
	fdIndex=fopen(m_IndexFile,"r");
	if(fdIndex==NULL)
	{
		cs.Format("不能打开索引文件:\n%s",m_IndexFile);
		AfxMessageBox(cs);
		return FALSE;
	}
	fgets(str1,1024,fdIndex);
	fgets(str2,1024,fdIndex);
	fgets(str3,1024,fdIndex);
	fclose(fdIndex);
	sscanf(str1,"%s%d",s,&Dimension);
	sscanf(str2,"%s%s%s%s%s%s%s%d%s%s%s%s%s%f%s%f%s%f",
		  s,s,s,DeptUnits,s,IndexType,s,&DataLen,s,s,s,s,s,&Stdep,s,&Endep,s,&Rlev);
	sscanf(str3,"%s%s%s%s%s%s%s%d%s%s%s%s%s%f%s%f",
		  s,CurveName,s,CurveUnits,s,DataType,s,&IndexLen,s,s,s,s,s,&Min,s,&Max);
	
	if(stricmp(DataType,"float")!=0)
		return FALSE;
	NumSam=(int)((Endep-Stdep+0.5*Rlev)/Rlev)+1;	
	fdData=fopen(m_DataFile,"rb");
	if(fdData==NULL)
	{
		cs.Format("不能打开数据文件:\n%s",m_DataFile);
		AfxMessageBox(cs);
		return FALSE;
	}
	CurveData=new float[NumSam];
	fread(CurveData,NumSam*sizeof(float),1,fdData);
	fclose(fdData);
	typedef struct 
	{
		char CurveName[5];
		float left,right;
		int line_log;
	}CurAttrib;
	//预定曲线的左右刻度
	const NUMCUR=40;  //具有预定刻度的曲线数
	CurAttrib cur[NUMCUR]={
			{"CAL",4.0f,14.0f,0},
			{"CALI",4.0f,14.0f,0},
			{"GR",0.0f,150.0f,0},
			{"CGR",0.0f,150.0f,0},
			{"AC",40.0f,90.0f,0},
			{"DT",40.0f,90.0f,0},
			{"DTC",40.0f,90.0f,0},
			{"CNL",0.f,30.0f,0},
			{"NPHI",0.f,30.f,0},
			{"APLC",0.f,30.f,0},
			{"DEN",2.0f,3.0f,0},
			{"RHOB",2.0f,3.0f,0},
			{"RHOM",2.0f,3.0f,0},
			{"RT",2.0f,10000.0f,1},
			{"LLD",2.0f,10000.0f,1},
			{"RXO",2.0f,10000.0f,1},
			{"LLS",2.0f,10000.0f,1},
			{"SH",0.0f,100.0f,0},
			{"VSH",0.0f,100.0f,0},
			{"POR",0.0f,25.0f,0},
			{"PORT",0.0f,25.0f,0},
			{"CPOR",0.0f,25.0f,0},
			{"PORA",0.0f,25.0f,0},
			{"PORB",0.0f,25.0f,0},
			{"PORD",0.0f,25.0f,0},
			{"PORN",0.0f,25.0f,0},
			{"PORW",0.0f,25.0f,0},
			{"PERM",0.01f,1000.0f,1},
			{"CPER",0.01f,1000.0f,0},
			{"SW",0.0f,100.0f,0},
			{"CSW",0.0f,100.0f,0},
			{"SXO",0.0f,100.0f,0},
			{"U",0.0f,50.0f,0},
			{"TH",0.0f,50.0f,0},
			{"K",0.0f,10.0f,0},
			{"HAZI",0.0f,360.0f,0},
			{"AZIM",0.0f,360.0f,0},
			{"DEVI",0.0f,90.0f,0},
			{"DEV",0.0f,90.0f,0},
	};
	//从隐含的左右刻度值得到绘图的左右刻度值，如果没有则用最大和最小值
	bool bLeft_Right=FALSE;
	for(int i=0;i<NUMCUR;i++)
	{
		cs=cur[i].CurveName;
		if(CurveName==cs)
		{
			Min=cur[i].left;
			Max=cur[i].right;
			LineLog=cur[i].line_log;
			bLeft_Right=true;
			break;
		}
	}
	if(!bLeft_Right)
	{
		//找出最小和最大值，作为横向刻度比例
		float varmin=3.4e38f;  //取最小值变量为正的最大浮点数
		float varmax=-3.4e38f; //取最大值变量为负的最大浮点数
		int Num=0;
		for(i=0;i<NumSam;i++)
		{
			if(CurveData[i]==-9999.0)continue;
			if(CurveData[i]==-999.25)continue;
			Num++;
			if(Num==1)
			{
				Min=CurveData[i];
				Max=CurveData[i];
			}
			else
			{
				Min=__min(Min,CurveData[i]);
				Max=__max(Max,CurveData[i]);
			}
		}
	}
	return TRUE;
}

BOOL ArrayCopy(CString OldName,CString NewName,CProgressCtrl *m_Progress)
{
	BOOL rw=FALSE;
	char szNewIndexName[512];
	char szOldIndexName[512];
	char szNewBodyName[512];
	char szOldBodyName[512];
	CFIDIO::GetFileName(OldName,szOldIndexName,'I');
	CFIDIO::GetFileName(OldName,szOldBodyName,'D');
	CFIDIO::GetFileName(NewName,szNewIndexName,'I');
	CFIDIO::GetFileName(NewName,szNewBodyName,'D');

	char szNewFIDName[20];
	CFIDIO::GetFIDName(NewName,szNewFIDName);

	CFIDIO *pFID=NULL;
	pFID = new CFIDIO(OldName);
	if(!pFID->Open())
	{
		AfxMessageBox("打开文件失败      ");
		delete pFID;
		return FALSE;
	}
	int dim=pFID->GetDimenSion();	
	FILE* fp1= fopen(szNewIndexName,"wt");
	if (fp1==NULL) 
	{
		AfxMessageBox("创建文件失败      ");
		delete pFID;
		return FALSE;
	}
	int i,k;
	FIDCONTENT *Content=NULL;
	fprintf(fp1,"DIMENSION %d\n",dim);
	for(i=0;i<dim;i++)
	{
		k=i+1;
		Content=pFID->GetContent(k);
		if(k==dim) 
			fprintf(fp1," INDEX-%d-NAME %s",k,szNewFIDName);
		else
			fprintf(fp1," INDEX-%d-NAME %s",k,Content->name);
		fprintf(fp1," UNIT %s",Content->unit);
		fprintf(fp1," %s","TYPE");
		if (Content->type == REPR_CHAR) fprintf(fp1," char");
		if (Content->type == REPR_SHORT) fprintf(fp1," int");
		if (Content->type == REPR_INT) fprintf(fp1," int");
		if (Content->type == REPR_LONG) fprintf(fp1," int");
		if (Content->type == REPR_FLOAT) fprintf(fp1," float");
		if (Content->type == REPR_DOUBLE) fprintf(fp1," float");
		if (Content->type == REPR_STRING) fprintf(fp1," string");
		fprintf(fp1," LENGTH %d",Content->length);
		fprintf(fp1," NPS %d",Content->nps);
		fprintf(fp1," NPW %d",Content->npw);
		if(k==1)
		{
			fprintf(fp1," SDEP %f",Content->min);
			fprintf(fp1," EDEP %f",Content->max);
		}
		else
		{
			fprintf(fp1," MIN %f",Content->min);
			fprintf(fp1," MAX %f",Content->max);
		}
		fprintf(fp1," RLEV %f\n",Content->rlev);
	}   
	fprintf(fp1," %s","==END==");
	fclose(fp1);
	delete pFID;

	FILE *fpr,*fpw;
	if((fpr = fopen(szOldBodyName,"rb"))!=NULL)
	{
		if((fpw=fopen(szNewBodyName,"wb"))!=NULL)
		{
			fseek(fpr, 0, SEEK_END);
			int FileLength = ftell(fpr);
			
			char buf[40960];
			int rl=40960;
			fseek(fpr, 0, SEEK_SET);
			if(m_Progress!=NULL)
			{
				m_Progress->SetRange32(0,FileLength/40960+1);
				i=0;
				while(i<FileLength)
				{
					if(i+rl>FileLength)
						rl=FileLength-i;
					fread(buf,rl,1,fpr);
					fwrite(buf,rl,1,fpw);
					i+=rl;
					m_Progress->SetPos(i/40960);
				}
			}
			else
			{
				i=0;
				while(i<FileLength)
				{
					if(i+rl>FileLength)
						rl=FileLength-i;
					rl=fread(buf,rl,1,fpr);
					fwrite(buf,rl,1,fpw);
					i+=rl;
				}
			}
			fclose(fpw);
		}
		fclose(fpr);
	}
	return rw;
}