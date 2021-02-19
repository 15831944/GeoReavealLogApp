
#ifndef __FIDIO_H__
#define __FIDIO_H__

#define REPR_INT 1
#define REPR_SHORT 2
#define REPR_LONG 3
#define REPR_FLOAT 4
#define REPR_DOUBLE 5
#define REPR_STRING 6
#define REPR_CHAR 7

#ifdef _UNIX_
static char szPathSep = '/';
#else
static char szPathSep = '\\';
#endif

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

class CFIDIO
{
public:
	//�½��ļ��Ľṹ����
	CFIDIO(const char* pszPathName,const FIDINDEX* pIndex);
	//�ļ��Ѿ����ڵĽṹ����
	CFIDIO(const char* pszPathName); 
	CFIDIO(){}; 
	~CFIDIO();
	FILE* fp;  

//attribute	
protected:
	FIDINDEX m_Index;

private:
	FIDCONTENT* m_Contentlist;
	FIDCONTENT m_Content;
	unsigned int m_RecNo,m_MaxRecNo;
	long position;
	unsigned int m_RecLen;
	float depth;
	int DatID;
	
//operation
public:
	int Open();
	int Close();
	int Delete();
	int Copy(const char* name);
	int Rename(const char* name);
	FIDCONTENT* GetContent(int dimension);
	FIDCONTENT* GetContent(const char* name);
    int GetDimenSion();    
	size_t Read(int dimension, size_t count, double *buffer);	
	size_t Read(int dimension, size_t count, float *buffer);	
	size_t Read(int dimension, size_t count, char *buffer);	
	size_t Read(int dimension, size_t count, int *buffer);	
	size_t Read(int dimension, size_t count, short *buffer);	
	size_t Read(int dimension, size_t count, long *buffer);	
	size_t Read(const char* name, size_t count, float *buffer);
	size_t Read(const char* name, size_t count, double *buffer);
	size_t Read(const char* name, size_t count, char *buffer);
	size_t Read(const char* name, size_t count, int *buffer);
	size_t Read(const char* name, size_t count, short *buffer);
	size_t Read(const char* name, size_t count, long *buffer);
	size_t Write(int dimension, size_t count, double *buffer);	
	size_t Write(int dimension, size_t count, float *buffer);	
	size_t Write(int dimension, size_t count, char *buffer);	
	size_t Write(int dimension, size_t count, int *buffer);	
	size_t Write(int dimension, size_t count, short *buffer);	
	size_t Write(int dimension, size_t count, long *buffer);	
	size_t Write(const char* name, size_t count, float *buffer);
	size_t Write(const char* name, size_t count, double *buffer);
	size_t Write(const char* name, size_t count, char *buffer);
	size_t Write(const char* name, size_t count, int *buffer);
	size_t Write(const char* name, size_t count, short *buffer);
	size_t Write(const char* name, size_t count, long *buffer);
	int SetDepth(float sdep,float edep);
	int ReverseDepth();
	int SeekToTop();
	int SeekToEnd();
	int SeekToDepth(float depth);	
	float GetDepth();
	float GetStartDepth();
	float GetEndDepth();
	float GetDepthLevel(); 
	void GetDefaultContentsValue(char* p,size_t num);
	int Next();
	int Last();
	int MoveDepth(float delta);
	int UnitConvert(float m_f);
	
protected:        
	int ReadIndex(FILE* fp);
	void WriteIndex(FILE* fp, const FIDINDEX* pIndex);

private:
	void Convert(char*,size_t,size_t);
	char m_pszPathName[_MAX_PATH];
	int openflag;                 
	int filetype;
public: // static member
	static void GetDefaultValue(char* p, int repc, size_t num);
	static void GetFileName(const char* path,char* name,char filetype);
	static void GetFullFIDName(const char* path,char* name);
	static void GetFullWellName(const char* path,char* name);
	static void GetFIDName(const char* path,char* name);
	static void GetWellName(const char* path,char* name);
	static void GetPath(const char* path,char* name);
	static int GetTypeLength(int type);
	static void MakePath(char* path, const char* drive, const char* dir, const char* fname, const char* ext);
	static void SplitPath(const char* path, char* drive, char* dir, char* fname, char* ext);	
};  
BOOL _declspec(dllexport)_cdecl Register();
void _declspec(dllexport)_cdecl GetNetwareCode(char *NetwareCode);
void _declspec(dllexport)_cdecl NetwareCode2LoginCode(char *HardwareCode,char *LoginCode);
//ˢ�����߿��ļ�
void _declspec(dllexport)_cdecl ReWriteCurveDBF(char path[256]);
//ˢ�²⾮��Ŀ��̬�ļ�
BOOL _declspec(dllexport)_cdecl RefurbishFID(char path[256]);
CString _declspec(dllexport)_cdecl ReadRow(FILE* fp);
//����ļ��Ƿ����
BOOL _declspec(dllexport) _cdecl DetectFileExist(const char* s);
void _declspec(dllexport) _cdecl SoftAbout(const char* Editer);
//��ȡ��װĿ¼
CString _declspec(dllexport)_cdecl GetWorkDisk();
//��ȡĿ��Ŀ¼����Ŀ�����ʱΪ����Ŀ¼
CString _declspec(dllexport)_cdecl GetWorkPath(CString flag="WorkSpace");
//��ȡ��ǰĿ�꾮��
CString _declspec(dllexport)_cdecl GetCurrentUser();
CString _declspec(dllexport)_cdecl GetCurrentUserPath();

CString _declspec(dllexport)_cdecl GetServerIP();
CString _declspec(dllexport)_cdecl GetDBInitUserName();
CString _declspec(dllexport)_cdecl GetDBInitPassword();
BOOL _declspec(dllexport)_cdecl GetLogTopRegString(CString szKey,LPCTSTR lpszEntry,CString& strValue);
BOOL _declspec(dllexport)_cdecl SetLogTopRegString(CString szKey,LPCTSTR lpszEntry,LPCTSTR lpszValue);
CString _declspec(dllexport)_cdecl SelectWell(HWND hWnd);
//��������Ԥ��
void _declspec(dllexport) _cdecl LogView(const char* CurverName);
BOOL _declspec(dllexport) _cdecl Message(const char* MsgStr,BOOL bOK,BOOL bCancel);
BOOL _declspec(dllexport) _cdecl GetMinMaxValue(CString WellPath,CString Curve,float &Min, float &Max,int &LineLog);
//�������߸���
BOOL _declspec(dllexport) _cdecl ArrayCopy(CString OldName,CString NewName,CProgressCtrl *m_Progress);

#endif 
