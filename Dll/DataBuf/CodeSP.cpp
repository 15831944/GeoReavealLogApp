// GenFun.cpp: implementation of the CGenFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CodeSP.h"
#include "float.h"
#include <math.h>
#include <sys\stat.h>
#include <direct.h>
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL _declspec(dllexport) TextFileToClipboard(char *ResultFile)
{
	CString cs;
	CFile f;
	if(f.Open(_T(ResultFile),CFile::modeRead)==NULL)
	{
		cs.Format("���ܴ��ļ�%s���ж�!",ResultFile);
		AfxMessageBox(cs);
		return false;
	}
	DWORD fs=f.GetLength();
	char *buf;
	buf=new char[fs+1];
	f.Read(buf,fs);
	buf[fs]=0;
	TextToClipboard(buf);
	return true;
}
BOOL _declspec(dllexport) TextToClipboard(char *szBuf)
{
	if( !OpenClipboard(NULL) )
	{
		AfxMessageBox("���ܴ򿪼����壡");
		return FALSE;
	}
	HGLOBAL hMem=GlobalAlloc(GMEM_MOVEABLE,strlen(szBuf)+1);
	char *lpDest=(char*)GlobalLock(hMem);
	strcpy(lpDest,szBuf);
	GlobalLock(hMem);
	EmptyClipboard();
	SetClipboardData(CF_TEXT,hMem);
	CloseClipboard();
	return TRUE;
}
void _declspec(dllexport) StrTrimLeft(char *str)
{
	CString s=str;
	s.TrimLeft();
	strcpy(str,(const char*)s);
}
void _declspec(dllexport) StrTrimRight(char *str)
{
	CString s=str;
	s.TrimRight();
	strcpy(str,(const char*)s);
}
void _declspec(dllexport) StrTrimLeftRight(char *str)
{
	CString s=str;
	s.TrimRight();
	s.TrimLeft();
	strcpy(str,(const char*)s);
}


bool _declspec(dllexport) RegKeyValueQuery(const CString szKey,const CString lpszEntry,CString &strValue)
{
	HKEY hSecKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_QUERY_VALUE,
		&hSecKey) == ERROR_SUCCESS)
	{
		DWORD dwType, dwCount;
		LONG lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType,
			           NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			//ASSERT(dwType == REG_SZ); //A null-terminated string
			LONG lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType,
				(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
			strValue.ReleaseBuffer();
		}
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			//ASSERT(dwType == REG_SZ);
			return true;
		}	
	}
	return false;
}
bool _declspec(dllexport) RegKeyValueQueryDword(const CString szKey,const CString lpszEntry,DWORD *dwFeatureSet)
{
	HKEY hSecKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_QUERY_VALUE,
		&hSecKey) == ERROR_SUCCESS)
	{
		DWORD dwType, dwCount;
		LONG lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType,
			           NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_DWORD); //A null-terminated string
			unsigned char strValue[4];
			LONG lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType,
				strValue, &dwCount);
			(*dwFeatureSet)=strValue[0]+256*strValue[1]+65536*strValue[2]+16777216*strValue[3];
		}
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_DWORD);
			return true;
		}	
	}
	return false;
}

bool _declspec(dllexport) RegKeyValueWrite(const CString szKey, const CString lpszEntry,const CString strValue)
{
	if(szKey.GetLength()==0 || lpszEntry.GetLength()==0 || strValue.GetLength()==0)
	{
		return false;
	}
	unsigned char *buf;
	buf=new unsigned char[strValue.GetLength()+1];
	for(int i=0;i<strValue.GetLength();i++)
	{
		buf[i]=strValue.GetAt(i);
	}
	buf[strValue.GetLength()]=0;
	HKEY hSecKey; //Handle to a registry key.
	RegCreateKey(HKEY_LOCAL_MACHINE,szKey,&hSecKey);
	LONG lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_SZ,
				buf, strValue.GetLength());
	RegCloseKey(hSecKey);
	delete []buf; buf=NULL;
	return lResult == ERROR_SUCCESS;
}
void _declspec(dllexport) GetBitR4(float &r4)
{
	union R4CH { float r; BYTE ch[4];};
	union R4CH w;
	long s,jm,ws;
	w.r=r4;
	if((w.ch[0]>>7)==0x01)
	{
		s=-1;
	}
	else
	{
		s=1;
	}
	jm=w.ch[0]&0x7f;
	ws=256L*256*w.ch[1]+256L*w.ch[2]+w.ch[3];
	//  r4=(float)(s*pow(16.0,jm-64)*ws/pow(2.0,24.0));
	r4=float(s*pow(2.0,4*jm-280)*ws); //��ʽ�����иı������Ŀ���Ǽ��ټ�����
}
void _declspec(dllexport) SunI2TOPC(short &i2)
{
	union I2CH { short i; char ch[2]; };
	union I2CH w;
	char c;
	w.i=i2;
	c=w.ch[0];
	w.ch[0]=w.ch[1];
	w.ch[1]=c;
	i2=w.i;
}
void _declspec(dllexport) SunI2TOPC(unsigned short &i2)
{
	union I2CH { unsigned short i; char ch[2]; };
	union I2CH w;
	char c;
	w.i=i2;
	c=w.ch[0];
	w.ch[0]=w.ch[1];
	w.ch[1]=c;
	i2=w.i;
}
void _declspec(dllexport) SunI4TOPC(int &i4)
{
	union I4CH { int i; char ch[4]; };
	union I4CH w;
	char c;
	w.i=i4;
	c=w.ch[0];
	w.ch[0]=w.ch[3];
	w.ch[3]=c;
	c=w.ch[1];
	w.ch[1]=w.ch[2];
	w.ch[2]=c;
	i4=w.i;
}
void _declspec(dllexport) SunI4TOPC(unsigned int &i4)
{
	union I4CH { unsigned int i; char ch[4]; };
	union I4CH w;
	char c;
	w.i=i4;
	c=w.ch[0];
	w.ch[0]=w.ch[3];
	w.ch[3]=c;
	c=w.ch[1];
	w.ch[1]=w.ch[2];
	w.ch[2]=c;
	i4=w.i;
}
void _declspec(dllexport) SunR4TOPC(float &r4)
{
	union R4CH { float r; char ch[4];};
	union R4CH w;
	char c;
	w.r=r4;
	for(int i=0;i<2;i++)
	{
		c=w.ch[i];
		w.ch[i]=w.ch[3-i];
		w.ch[3-i]=c;
	}
	r4=w.r;
}
void _declspec(dllexport) SunR8TOPC(double &r8)
{
	union R8CH { double r; char ch[8];};
	union R8CH w;
	char c;
	w.r=r8;
	for(int i=0;i<4;i++)
	{
		c=w.ch[i];
		w.ch[i]=w.ch[7-i];
		w.ch[7-i]=c;
	}
	r8=w.r;
}

void _declspec(dllexport) MSunI2TOPC(int count,short *i2)
{
	union I2CH { short i; char ch[2]; };
	union I2CH w;
	char c;
	for(int ii=0;ii<count;ii++)
	{
		w.i=i2[ii];
		c=w.ch[0];
		w.ch[0]=w.ch[1];
		w.ch[1]=c;
		i2[ii]=w.i;
	}
}
void _declspec(dllexport) MSunI2TOPC(int count,unsigned short *i2)
{
	union I2CH { unsigned short i; char ch[2]; };
	union I2CH w;
	char c;
	for(int ii=0;ii<count;ii++)
	{
		w.i=i2[ii];
		c=w.ch[0];
		w.ch[0]=w.ch[1];
		w.ch[1]=c;
		i2[ii]=w.i;
	}
}

void _declspec(dllexport) MSunI4TOPC(int count,int *i4)
{
	union I4CH {int i; char ch[4]; };
	union I4CH w;
	char c;
	for(int ii=0;ii<count;ii++)
	{
		w.i=i4[ii];
		c=w.ch[0];
		w.ch[0]=w.ch[3];
		w.ch[3]=c;
		c=w.ch[1];
		w.ch[1]=w.ch[2];
		w.ch[2]=c;
		i4[ii]=w.i;
	}
}
void _declspec(dllexport) MSunI4TOPC(int count,unsigned int *i4)
{
	union I4CH {unsigned int i; char ch[4]; };
	union I4CH w;
	char c;
	for(int ii=0;ii<count;ii++)
	{
		w.i=i4[ii];
		c=w.ch[0];
		w.ch[0]=w.ch[3];
		w.ch[3]=c;
		c=w.ch[1];
		w.ch[1]=w.ch[2];
		w.ch[2]=c;
		i4[ii]=w.i;
	}
}
void _declspec(dllexport) MSunR4TOPC(int count,float *r4)
{
	union R4CH { float r; char ch[4];};
	union R4CH w;
	char c;
	for(int ii=0;ii<count;ii++)
	{
		w.r=r4[ii];
		for(int i=0;i<2;i++)
		{
			c=w.ch[i];
			w.ch[i]=w.ch[3-i];
			w.ch[3-i]=c;
		}
		r4[ii]=w.r;
	}
}
void _declspec(dllexport) MSunR8TOPC(int count,double *r8)
{
	union R8CH { double r; char ch[8];};
	union R8CH w;
	char c;
	for(int ii=0;ii<count;ii++)
	{
		w.r=r8[ii];
		for(int i=0;i<4;i++)
		{
			c=w.ch[i];
			w.ch[i]=w.ch[7-i];
			w.ch[7-i]=c;
		}
		r8[ii]=w.r;
	}
}

CString _declspec(dllexport) MyGetPathName(CString szFilename)
{
	int n=szFilename.GetLength();
	for(int i=n-1;i>=0;i--)
	{
		if(szFilename.GetAt(i)=='\\')
		{
			return szFilename.Left(i);
		}
	}
	return _T("");
}
CString _declspec(dllexport) MyGetFileName(CString szFilename)
{
	int n=szFilename.GetLength();
	for(int i=n-1;i>=0;i--)
	{
		if(szFilename.GetAt(i)=='\\')
		{
			return szFilename.Right(n-i-1);
		}
	}
	return _T("");
}
CString _declspec(dllexport) MyGetFileExt(CString szFilename)
{
	int n=szFilename.GetLength();
	for(int i=n-1;i>=0;i--)
	{
		if(szFilename.GetAt(i)=='.')
		{
			return szFilename.Right(n-i-1);
		}
	}
	return _T("");
}

CString _declspec(dllexport) MyGetFileTitle(CString szFilename)
{
	int n=szFilename.GetLength();
	int n2=n;
	int n1=0;
	for(int i=n-1;i>=0;i--)
	{
		if(szFilename.GetAt(i)=='.')
		{
			n2=i;
			break;
		}
	}
	for(i=n-1;i>=0;i--)
	{
		if(szFilename.GetAt(i)=='\\')
		{
			n1=i+1;
			break;
		}
	}
	return szFilename.Mid(n1,n2-n1);
}
CString _declspec(dllexport) MyGetPathNameFileTitle(const CString szFilename)
{
	int n=szFilename.GetLength();
	for(int i=n-1;i>=0;i--)
	{
		if(szFilename.GetAt(i)=='.')
		{
			return szFilename.Left(i);
		}
	}
	return szFilename;
}
bool _declspec(dllexport) IsDirectory(const CString sDirName)
{
	struct _stat FileInf;
	int result = _stat(sDirName, &FileInf );
	if( result != 0 ) return false; //������
	if(FileInf.st_mode & _S_IFDIR ) 
	{
		return true; //Ŀ¼��
	}
	else
	{
		return false;
	}
}
bool _declspec(dllexport) IsFileName(const CString sFileName)
{
	struct _stat FileInf;
	int result = _stat(sFileName, &FileInf );
	if( result != 0 ) return false; //������
	if(FileInf.st_mode & _S_IFDIR ) 
	{
		return false; //Ŀ¼��
	}
	else
	{
		return true; //�ļ���
	}
}
bool _declspec(dllexport) IsFilePathName(const CString sName)
{
	struct _stat FileInf;
	int result = _stat(sName, &FileInf );
	if( result != 0 ) return false; //������
	return true;
}
int _declspec(dllexport) GetFileLength(CString SCurDataFile)
{
	if(!IsFileName(SCurDataFile)) return 0; //�����ļ�
	struct _stat buf;
	int result = _stat( SCurDataFile, &buf );
	if( result != 0 )
	{
		return 0;
	}
	return buf.st_size;
}
//�����������²�����NumX���ΪNumY��
void _declspec(dllexport) FloatResample(float *buf,int NumX,int NumY)
{
	if(NumX<2 || NumY<2 || NumX==NumY)
	{
		return ;
	}
	float *vo=new float[NumY];
	for(int i=0;i<NumY;i++)
	{
		if(i==(NumY-1))
		{
			vo[NumY-1]=buf[NumX-1];
		}
		else if(i==0)
		{
			vo[0]=buf[0];
		}
		else
		{
			float xk=float(i)*(NumX-1)/(NumY-1);
			int k=int(xk);
			vo[i]=buf[k]+(xk-k)*(buf[k+1]-buf[k]);
		}
	}
	for(i=0;i<NumY;i++)
	{
		buf[i]=vo[i];
	}
	delete []vo; vo=0;
}
//�����������²�����buf2��buf1�в���
void _declspec(dllexport) FloatResample(float Start1,float Stop1,float Rlev1,float *buf1,float Start2,float Stop2,float Rlev2,float *buf2)
{
	int Num1=int((Stop1-Start1+0.5*Rlev1)/Rlev1)+1;
	int Num2=int((Stop2-Start2+0.5*Rlev2)/Rlev2)+1;
	//���μ����������ͬ
	if(Start1==Start2 && Stop1==Stop2 && Rlev1==Rlev2)
	{
		for(int i=0;i<Num1;i++)
		{
			buf2[i]=buf1[i];
		}
		return;
	}
	double er=1e-5;
	//���λ���������ͬ
	for(int i=0;i<Num2;i++)
	{
		float dep=Start2+i*Rlev2;
		if(dep<Start1 || dep>Stop1)
		{
			buf2[i]=-999.25;
			continue;
		}
		float xk=(dep-Start1)*(Num1-1)/(Stop1-Start1);
		int k1=int(xk);
		int k2=k1+1;
		if(k2>(Num1-1))
		{
			k2=Num1-1;
		}
		if((buf1[k1]+9999)<er || (buf1[k1]+999.25)<er)
		{
			buf2[i]=buf1[k2];
		}
		else if((buf1[k2]+9999)<er || (buf1[k2]+999.25)<er)
		{
			buf2[i]=buf1[k1];
		}
		else
		{
			buf2[i]=buf1[k1]+(xk-k1)*(buf1[k2]-buf1[k1]);
		}
	}
}
void _declspec(dllexport) SelectOneFile(const char *szTitle,bool bOpen,const char *szFilter,char *szFileName,const char *ExtName,const char *OldFile)
{
	CString csAiFileName;  //�ļ�������·����
	CString s;            //��ʱ���� 
	CFileDialog dlg(bOpen,  //����Ϊ�򿪶Ի���
		ExtName,  //Ҫ���ӵ���չ����ֵΪNULL���ʾ��������չ��
		_T(OldFile),  //��ʼ���Ի���ʱ���ļ���������ΪNULL
		OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,  //���ƶԻ���Ŀ������
		_T(szFilter), //�ļ�������
		NULL);
	dlg.m_ofn.lpstrTitle=szTitle; //�Ի������
	
    char szCurDir[_MAX_PATH];  //��ŵ�ǰĿ¼
    char szNewDir[_MAX_PATH];  //����µ�Ŀ¼
    DWORD dwNumberofChar;
    BOOL bDirChange = FALSE;  //�Ƿ�ı���Ŀ¼
	strcpy(szFileName,"");
	dwNumberofChar=GetCurrentDirectory(sizeof(szCurDir), (LPSTR)szCurDir);  //ȡ�õ�ǰĿ¼
	if(dwNumberofChar>0)  //�ж�ȡ�õ�ǰĿ¼�ɹ����
	{
		if(SetCurrentDirectory((LPSTR)szNewDir))  //����ָ��·��Ϊ��ǰ·��
		{
			bDirChange = true;  //��ʾ�Ѹı��˵�ǰĿ¼
		}
	}
    //�򿪶Ի���
	if( dlg.DoModal()==IDOK )
	{
		csAiFileName = dlg.GetPathName();  //ȡ�ú�·�����ļ���
		strcpy(szFileName,csAiFileName);
		GetCurrentDirectory(sizeof(szCurDir), (LPSTR)szCurDir);  //ȡ�õ�ǰĿ¼
	}
	if(bDirChange)  //���Ŀ¼�Ѿ��ı�,����ǰĿ¼����Ϊԭ��Ŀ¼
	{
		if(SetCurrentDirectory((LPSTR)szCurDir))
		{
			bDirChange = true;
		}
		else
		{
			AfxMessageBox("�ָ�<��ǰĿ¼��>ʧ�ܣ�",MB_ICONINFORMATION);
		}
	}	
}
bool _declspec(dllexport) FileUnite(const CString sObjectFile,const CString SourceFile)
{
	if(!IsFileName(SourceFile))
	{
		return FALSE; //Դ�ļ�������
	}
	if(IsFileName(sObjectFile))
	{
		//���������ļ�
		CFile f;
		//��Դ�ļ�
		f.Open(SourceFile,CFile::modeRead|CFile::shareDenyNone);
		UINT fs=f.GetLength();
		char *buf=new char[fs];
		f.Read(buf,fs);
		f.Close();
		
		//�����Ŀ���ļ�
		f.Open(sObjectFile,CFile::modeWrite);
		f.SeekToEnd();
		f.Write(buf,fs);
		f.Close();
		delete []buf; buf=0;
	}
	else
	{
		//��Դ�ļ�������Ŀ���ļ�
		::CopyFile(SourceFile,sObjectFile,FALSE);
	}
	return true;
}
bool _declspec(dllexport) PutStringToFile(const CString sText,const CString sFile,BOOL bWarnIfExists)
{ 
	CString s;
	if(IsFileName(sFile) && bWarnIfExists)
	{
		//�ļ��Ѵ���
		s.Format ("%s\n�Ѵ��ڣ�������?",sFile);
		int i=AfxMessageBox(s,MB_YESNO|MB_ICONQUESTION);
		if(i!=IDYES) return FALSE;  //���ûѡ��Yes�򷵻�
		CFile::Remove(sFile);
	}
	CFile f;
	if(f.Open(sFile,CFile::modeCreate|CFile::modeWrite)==FALSE)
	{
		return false;
	}
	f.Write((const char*)sText,sText.GetLength());
	f.Close();
	return true;
}


//�Ӵ���ȡ����n������
CString _declspec(dllexport) MyGetSubStr(const CString szSource,int NumItem)
{
	char *str=new char[szSource.GetLength()+3];
	str[0]=' ';
	strcpy(&str[1],(const char*)szSource);
	strcat(str," ");
	int Num=0;
	int nStart=0;
	int nStop=0;
	for(int i=0;i<int(strlen(str))-1;i++)
	{
		if((str[i]>0 && str[i]<=32) && str[i+1]>32)
		{
			Num++;
			nStart=i+1;
		}
		if(str[i]>32 && (str[i+1]>0 && str[i+1]<=32))
		{
			nStop=i+1;
			if(Num==NumItem)
			{
				delete []str; str=0;
				return szSource.Mid(nStart-1,nStop-nStart);
			}
		}
	}
	delete []str; str=0;
	return _T("");
}

//4�ֽڳ��������븡���ת��
float _declspec(dllexport) Int4ToFloat(int i4)
{
	union R4CH { float r4; int i4;};
	union R4CH w;
	w.i4=i4;
	return w.r4;
}
void _declspec(dllexport) MInt4ToFloat(int count,int *i4,float *r4)
{
	union R4CH { float r4; int i4;};
	union R4CH w;
	for(int i=0;i<count;i++)
	{
		w.i4=i4[i];
		r4[i]=w.r4;
	}
}

int _declspec(dllexport) FloatToInt4(float r4)
{
	union R4CH { float r4; int i4;};
	union R4CH w;
	w.r4=r4;
	return w.i4;
}
void _declspec(dllexport) MFloatToInt4(int count,float *r4,int *i4)
{
	union R4CH { float r4; int i4;};
	union R4CH w;
	for(int i=0;i<count;i++)
	{
		w.r4=r4[i];
		i4[i]=w.i4;
	}
}
int _declspec(dllexport) WisCodeLen(DWORD RepCode)
{
	if(RepCode==WIS_REPR_INT) return sizeof(int);
	if(RepCode==WIS_REPR_SHORT) return sizeof(short);
	if(RepCode==WIS_REPR_LONG) return sizeof(long);
	if(RepCode==WIS_REPR_FLOAT) return sizeof(float);
	if(RepCode==WIS_REPR_DOUBLE) return sizeof(double);
	if(RepCode==WIS_REPR_STRING) return 0; //�ַ�����������
	if(RepCode==WIS_REPR_CHAR) return sizeof(char);
	if(RepCode==WIS_REPR_UCHAR) return sizeof(unsigned char);
	if(RepCode==WIS_REPR_USHORT) return sizeof(unsigned short);
	if(RepCode==WIS_REPR_UINT) return sizeof(unsigned int);
	if(RepCode==WIS_REPR_ULONG) return sizeof(unsigned long);
	return 0;
}

int _declspec(dllexport) PcCodeLen(DWORD RepCode)
{
	if(RepCode==PC_REPR_INT) return sizeof(int);
	if(RepCode==PC_REPR_SHORT) return sizeof(short);
	if(RepCode==PC_REPR_LONG) return sizeof(long);
	if(RepCode==PC_REPR_FLOAT) return sizeof(float);
	if(RepCode==PC_REPR_DOUBLE) return sizeof(double);
	if(RepCode==PC_REPR_STRING) return 0; //�ַ�����������
	if(RepCode==PC_REPR_CHAR) return sizeof(char);
	if(RepCode==PC_REPR_UCHAR) return sizeof(unsigned char);
	if(RepCode==PC_REPR_USHORT) return sizeof(unsigned short);
	if(RepCode==PC_REPR_UINT) return sizeof(unsigned int);
	if(RepCode==PC_REPR_ULONG) return sizeof(unsigned long);
	return 0;
}

BOOL _declspec(dllexport) IsNullString(const char *str)
{
	CString s=str;
	s.TrimLeft();
	s.TrimRight();
	s.Remove(' ');
	return s.IsEmpty();
}
BOOL _declspec(dllexport) IsAsciiString(CString Source)
{
	int sl=Source.GetLength();
	char *str=new char[sl+1];
	memcpy(str,Source,sl);
	str[sl]='\0';
	StrTrimLeftRight(str);
	bool bResult=true;
	sl=strlen(str);
	if(sl>0)
	{
		for(int i=0;i<sl;i++)
		{
			if(str[i]>127 || str[i]<0)
			{
				bResult=false;
				break;
			}
		}
	}
	else
	{
		bResult=false;
	}
	delete []str; str=NULL;
	return bResult;
}

BOOL _declspec(dllexport) CheckLoginCode()
{
	//�������������Ƿ񵯳�ע��Ի���
	CString strValue;  //��ֵ(����)
	CString szLoginKey="SYSTEM\\Windows";  //����
	BOOL bResult=RegKeyValueQuery(szLoginKey,_T("DataBufPro"),strValue);
	if(bResult)
	{
		//��ע���룬�������Ƿ���ȷ
		char HardwareCode[256],LoginCode[256];
		GetHardwareCode(HardwareCode);
		HardwareCode2LoginCode(HardwareCode,LoginCode);
		if(strcmp(LoginCode,(const char*)strValue)==0)
			return TRUE; //ע������ȷ
	}
	return FALSE; //ע���벻��ȷ
}

#include "SysDisk.h"
void GetHardwareCode(char *HardwareCode)
{
	CSysDisk pSysDisk;
	CString m_DiskSerialNum;
	char HardDriveSerialNumber [1024];
    // �õ�Ӳ�����кź�Ӳ������
    int done = pSysDisk.ReadPhysicalDriveInNT();
    strcpy (HardDriveSerialNumber,pSysDisk.ConvertToString(10,19));
	long ComputerHardDriveId=pSysDisk.GetHardDriveComputerID();
	sprintf(HardwareCode,"%04X",ComputerHardDriveId);
}

void HardwareCode2LoginCode(char *HardwareCode,char *LoginCode)
{
	static char Mask[9]="ZHZM6736";	//8�ֽ�����
	char NetwareCode[9];
	char strSerial[9];
	strncpy(strSerial,&HardwareCode[0],12);;
	for(int i=0;i<8;i++)
		NetwareCode[i]=Mask[i]^strSerial[i];
	//��λ�����Ľ������4������������
	unsigned short kSer1=NetwareCode[0]+256*NetwareCode[1];
	unsigned short kSer2=NetwareCode[2]+256*NetwareCode[3];
	unsigned short kSer3=NetwareCode[4]+256*NetwareCode[5];
	unsigned short kSer4=NetwareCode[6]+256*NetwareCode[7];
	//����γ�Ӳ������
	sprintf(LoginCode,"%04X-%04X-%04X-%04X",kSer4,kSer3,kSer2,kSer1);
}

CString _declspec(dllexport) GetWorkPath(CString flag)
{
	CString WorkPath;
	if(!RegKeyValueQuery("SOFTWARE\\�Ĵ�ʯ�͹���ֲ⾮��˾\\�⾮����ƽ̨\\User","WorkDisk",WorkPath))
		WorkPath="";
	else
		WorkPath+="\\"+flag+"\\";
	return WorkPath;
}

#include "_Comext.h"
#include "ModiFileNameDlg.h"

BOOL _declspec(dllexport) DelDirAllFile(CString DirName)
{
	CString DelFileName=DirName+"\\*.*";
	struct _finddata_t c_file;
    long hFile;

    if( (hFile = _findfirst( DelFileName, &c_file )) != -1L )
    {
		_findnext( hFile, &c_file );
        while( _findnext( hFile, &c_file ) == 0 )
		{
			DelFileName=DirName+"\\"+c_file.name;
			if(c_file.attrib&_A_SUBDIR)
			{
				DelDirAllFile(DelFileName);
				_rmdir(DelFileName);
			}
		    else
				if(remove(DelFileName)==-1)
				{
					_chmod(DelFileName,_S_IREAD | _S_IWRITE);
					if(remove(DelFileName)==-1)
						AfxMessageBox(DelFileName+"ɾ��ʧ��    ");
				}
		}      
	}
	_findclose( hFile );
	return TRUE;
}

void _declspec(dllexport) ModiFileName(const char* Editer)
{
	if(!CheckLoginCode()) return;
	CString sFileName=(char*)Editer;
	if(!sp_bManualFileName)
	{
		if(!IsFilePathName(sFileName))
			return; //�����Զ������ļ�������,�򷵻�
		else
		{
			if(sp_bWriteExist)
			{
				if(IsFileName(sFileName))
					CFile::Remove(sFileName);
				return; //�����Զ������ļ����ڣ���ɾ���󷵻�
			}
		}
	}
	
	CModiFileNameDlg ModiFileNameDlg((char*)Editer);
	if(ModiFileNameDlg.DoModal()==IDOK)
		sFileName=ModiFileNameDlg.m_ManualFileName;
	Editer=sFileName;
}

void _declspec(dllexport) LoadDataBufProPara()
{
	sp_Break_Status=FALSE;
	sp_bAllIgnore=TRUE;              //�Ƿ�����޸�             DLIS���������Ϣ
    sp_bManualFileName=FALSE;        //�Զ���������ļ���
    sp_bWriteExist=TRUE;             //�ļ������Ƿ�ֱ�Ӹ���
    sp_bAddCopyNum=FALSE;            //DLISͬ���������ӿ�����   �������
    sp_bAddFrameNo=FALSE;            //DLISͬ����������֡��     �������
    sp_bAddOrigin=FALSE;             //DLISͬ����������Դ       �������
    sp_bDirectory=FALSE;             //�Ƿ���Ŀ¼

	CString s;int n;
	CStdioFile fPara;	
	CString sParaFile=GetWorkPath("Config")+"DataBufPro.ini";
	if(fPara.Open(sParaFile,CFile::modeRead|CFile::shareDenyNone|CFile::typeText))
	{		
		CString flag,Value;
		while(fPara.ReadString(s))
		{
			n=s.Find("=");
			if(n<1) continue;
			flag=s.Left(n);
			Value=s.Mid(n+1);
			flag.TrimLeft();flag.TrimRight();
			Value.TrimLeft();Value.TrimRight();
			if(Value=="TRUE")
			{
				if(flag=="AllIgnore")
					sp_bAllIgnore=TRUE;
				else if(flag=="ManualFileName")
					sp_bManualFileName=TRUE;
				else if(flag=="WriteExist")
					sp_bWriteExist=TRUE;
				else if(flag=="AddCopyNum")
					sp_bAddCopyNum=TRUE;
				else if(flag=="AddFrameNo")
					sp_bAddFrameNo=TRUE;
				else if(flag=="AddOrigin")
					sp_bAddOrigin=TRUE;
				else if(flag=="Directory")
					sp_bDirectory=TRUE;
			}
		}
		fPara.Close();
	}
	
	CString sFilterFile;
	sp_FilterCurves.RemoveAll ();
	sFilterFile=GetWorkPath("Config")+"CurveIO.ini";
	if(IsFileName(sFilterFile))
	{
		//����������������Ϣ
		CStdioFile fFilter;
		if(fFilter.Open(sFilterFile,CFile::modeRead|CFile::shareDenyNone|CFile::typeText))
		{
			while(fFilter.ReadString(s))
			{
				n=s.Find(' ');
				if(n>0) s=s.Left(n);
				s.TrimLeft();
				s.TrimRight();
				if(s.CompareNoCase("END")==0) break;
				if(!s.IsEmpty())
					sp_FilterCurves.Add(s);
			}
			fFilter.Close();
		}
	}
}