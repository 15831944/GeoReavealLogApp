#ifndef __PROCESS_H__
#define __PROCESS_H__

#define MAX_BUFFER 1024

struct INCURVE 
{
public:
	char name[20];
	char unit[20];
	int repcode;
	int length;
	int sample;
    int number;
	float sdep;
	float edep;
	float rlev;
	INCURVE(const char* = NULL);
};

struct OUTCURVE
{
public:
	char name[20];
	char unit[20];
	int repcode;
	int length;
	int sample;
    int number;
	int flag;
	float sdep;
	float edep;
	float rlev;
	OUTCURVE(const char* name1=NULL);
};

typedef char INPNAME[10];

class CInFID : public CFIDIO
{
public:
	CInFID(const char* szName, INCURVE& curve,char* in, size_t nBufferPointNum);
    CInFID(){};
    ~CInFID();
// attribite
protected:
	char* m_Buffer,*m_DefaultBuffer,*in_ptr;
	size_t m_BufferPointNum,m_ReadPointNum;
	INCURVE m_Curve;
	FIDCONTENT m_CONTENT;
	int bExist;
	float m_Sdep,m_Edep;

//operations
public:                                            
    void DatIn(float dep);
    void DatIn(float dep,char* p, size_t count);
    size_t GetBufferPointNum();
    size_t SetBufferPointNum(size_t number);
    long GetBufferSize();
    long SetBufferSize(long size);        
    char* GetBuffer();
    void ReadBuffer(float dep);
	void CreateDefaultBuffer();
};    
    
class COutFID :public CFIDIO
{
public:
	COutFID(const char* szName, OUTCURVE& curve,char* out, size_t nBufferPointNum);
	COutFID(const char* szName, OUTCURVE& curve,char* out, size_t nBufferPointNum,FIDINDEX* pINDEX);
    COutFID(){};
    ~COutFID();
// attribite
protected:
	char* m_Buffer,*out_ptr;
	size_t m_BufferPointNum,m_ReadPointNum;
	OUTCURVE m_Curve;
	FIDCONTENT m_CONTENT;
	int bExist;       
	float m_Sdep,m_Edep;

//operations
public:
    void DatOut(float dep);
    void DatOut(float dep,char* p, size_t count);
    size_t SetBufferPointNum(size_t number);
    size_t GetBufferPointNum();
    long GetBufferSize();
    long SetBufferSize(long size);        
    char* GetBuffer();
    void ReadBuffer(float dep);
    void WriteBuffer();
};    
    
    
class CLogProcess
{
public:
//szLogFileName－井名*.FID，szInpFileName－参数文件名POR.CRD
	CLogProcess(const char* szLogFileName, const char *szInpFileName);
	~CLogProcess();

// Attribute
public:
	CString m_MethodName;
	FILE *m_InpFile,*m_ResultFile;
	char m_InpFileName[255];
	char m_LogName[255];
	int m_InCurveNum,m_OutCurveNum,m_ParaNum ;
	int m_InFlag,m_OutFlag,m_InpFlag;
	size_t m_BufferPointNum;
	INCURVE *m_InCurve;
	OUTCURVE *m_OutCurve;       
	CInFID **m_InFID;
	COutFID **m_OutFID;
	char *in_ptr, *out_ptr;
	INPNAME *m_NameC;
	float *m_Para;
	char m_DepthUnit[10];
public:
	float EndDep,StartDep,DepLevel;
	float Dep,Sdep,Edep,SSdep,EEdep;

//Operations
public:
	virtual int GetUpdateFlag();
	//初始化输入曲线
	int  InitIn(int num, INCURVE *curve, char *value);
	//读取一个深度点数据
	int  DatIn();
    int  DatIn(int num, char* p, size_t count);
    int  DatIn(char* name, char* p, size_t count);

	//初始化输出曲线
	int  InitOut(int num, OUTCURVE *curve, char *value);
	//输出一个深度点数据
	int  DatOut();
    int  DatOut(int num, char* p, size_t count);
    int  DatOut(char* name, char* p, size_t count);
	

	//初始化处理参数
	int  InitPara(int num, INPNAME *name, float *value);
	//打开处理参文件
	int  OpenInpCard();
	//读取处理参数文件中一深度段处理参数值
	int  ReadInpCard();
	//关闭处理参数文件
	void CloseInpCard();
	//编辑处理参数文件
	virtual BOOL ParametEdit();
	//初始化数据缓冲区
	void InitBuffer();
    void SetBufferPointNum(size_t number);
    long GetBufferSize();
    long SetBufferSize(long size);        
	int  WriteBuffer();
	void DeleteBlank(char* s);
	FIDCONTENT* GetCurveCONTENT(char* name);
	int IsCurveExist(char* name);
	int	DeleteFIDCurve(char* name);
};

#endif //__PROCESS_H__
