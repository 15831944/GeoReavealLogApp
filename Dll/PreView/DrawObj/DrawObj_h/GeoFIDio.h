
#ifndef __GEOFIDIO_H__
#define __GEOFIDIO_H__

#define REPR_INT 1
#define REPR_SHORT 2
#define REPR_LONG 3
#define REPR_FLOAT 4
#define REPR_DOUBLE 5
#define REPR_STRING 6
#define REPR_CHAR 7

static char szPathSep = '\\';

extern long Round(double x);
//���������ļ���ÿά�����ݽṹFIDCONTENT
typedef struct tagFIDCONTENT
{
	char name[20];
	char unit[20];
	int type;
	int length;
	int nps;
	int npw;
	float min;
	float max;
	float rlev;
	int offset;
}FIDCONTENT;              
//���������ļ�����ṹFIDINDEX
typedef struct tagFIDINDEX
{          
	int dimension;
	FIDCONTENT* Contentlist;
	char* adi;
}FIDINDEX;

#include "GeoAdo.h"

class CGeoFIDIO
{
private: //��������
	CGeoAdo		m_ado;

public:
	DWORD		m_CurveID;
	DWORD		m_ProjectID;
	DWORD		m_WellID;
	CString		m_szSever;	
	CString		m_CurveName;

	CString		m_szTempPath,m_szFIDName;
	BOOL		m_bFIDExist;
	//��������ID��ȡ��������DataByte1�ֶ�ֵ
	size_t GetDataByte(char *pBuf,size_t lenth,DWORD iCurveID);
	//��������ID��ȡѹ���ֿ��洢����������DataByte1-5�ֶ�ֵ
	size_t GetDataByteZip(char *pBuf,size_t lenth,DWORD iCurveID);
	//��������ID��ȡszBlob�ֶ�ֵ
	DWORD GetDataFrom(char *pBuf,DWORD lenth,DWORD iCurveID,char szBlob[MAX_PATH]);
	int m_nps,m_Length,m_DataZip;
	DWORD	m_DataSize;

	float	*m_fData;
	short	*m_iData;
	char	*m_cData;
	BOOL	GetFullData();
public:
	CGeoFIDIO(const char* pszPathName); 
	CGeoFIDIO(){}; 
	~CGeoFIDIO(); 

//attribute	
protected:
	FIDINDEX m_Index;

private:
	FIDCONTENT* m_Contentlist;

	unsigned long m_RecNo,m_MaxRecNo;
	unsigned long m_RecLen;
	float m_Dep;
	
//operation
public:
	BOOL Open(CString szServer);
	BOOL Close();

	FIDCONTENT* GetContent(int dim);
	FIDCONTENT* GetContent(const char* name);
    int GetDimenSion(); 

	size_t Read(int dimension, size_t count, float *buffer);	
	size_t Read(int dimension, size_t count, char *buffer);		
	size_t Read(int dimension, size_t count, short *buffer);	

	BOOL Next();
	BOOL SeekToTop();
	BOOL SeekToDepth(float depth);	
	float GetDepth();
	float GetStartDepth();
	float GetEndDepth();
	float GetDepthLevel(); 
	
	//д���ߵ�����
	BOOL CGeoFIDIO::WriteLocalFID(CString SavePath,CString CurveName);
protected:        
	int		ReadIndex();
	BOOL	WriteIndex(CString szPathName);
private:
	char m_pszPathName[_MAX_PATH];
	BOOL m_bOpen;                 

public: // static member
	static void GetFileName(const char* path,char* name,char filetype);
	static void GetFullFIDName(const char* path,char* name);
	static void GetFullWellName(const char* path,char* name);
	static void GetFIDName(const char* path,char* name);
	static void GetWellName(const char* path,char* name);
	static void GetPath(const char* path,char* name);
	static void MakePath(char* path, const char* drive, const char* dir, const char* fname, const char* ext);
	static void SplitPath(const char* path, char* drive, char* dir, char* fname, char* ext);	
};  

//����ļ��Ƿ����
BOOL DetectFileExist(const char* s);
//��ȡ��װĿ¼
CString GetWorkDisk();
//��ȡĿ��Ŀ¼����Ŀ�����ʱΪ����Ŀ¼
CString GetWorkPath(CString flag="WorkSpace");
//��ȡ��ǰĿ�꾮��
CString GetCurrentUser();
CString GetCurrentUserPath();
//ע������
BOOL GetLogTopRegString(CString szKey,LPCTSTR lpszEntry,CString& strValue);
BOOL SetLogTopRegString(CString szKey,LPCTSTR lpszEntry,LPCTSTR lpszValue);
#endif 
