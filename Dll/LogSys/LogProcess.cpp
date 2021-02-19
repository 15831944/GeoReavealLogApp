#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include "FIDio.h"
#include "LogSysres.h"
#include "LogProcess.h"
#include "ParameterEdit.h"

#define MAX_INT 6500000
static void SplitPath(const char* path, char* drive, char* dir, char* fname, char* ext);
void SplitPath(const char* path, char* drive, char* dir, char* fname, char* ext)
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

INCURVE::INCURVE(const char* name1)
{
	if (name1 == NULL) strcpy(name,"none");
	else strcpy(name,name1);
	strcpy(unit,"none");
	repcode=REPR_FLOAT,
	sample=1,number=1,sdep=0.f,edep=0.f,rlev=0.f;
	length = CFIDIO::GetTypeLength(repcode);
}

OUTCURVE::OUTCURVE(const char* name1)
{
	if (name1 == NULL) strcpy(name,"none");
	else strcpy(name,name1);
	strcpy(unit,"none");
	repcode=REPR_FLOAT,
	flag = 1;
	sample=1;
	number=1;
	sdep=0.f;
	edep=0.f;
	rlev=0.f;
	length = CFIDIO::GetTypeLength(repcode);
}


CInFID::CInFID(const char* szName, INCURVE& curve,char* in, size_t nBufferPointNum)
			:CFIDIO(szName)
{
	m_BufferPointNum = nBufferPointNum;
	in_ptr = in;
	m_Curve = curve;                     
	bExist = 0;
	m_Sdep = m_Edep = 0.f;     
	m_Buffer = NULL;
	m_DefaultBuffer = NULL;
	if (Open() && GetContent(m_Curve.name)) 
	{
		m_CONTENT = *GetContent(m_Curve.name);
		m_CONTENT.min = GetContent(1)->min;
		m_CONTENT.max = GetContent(1)->max;
		m_CONTENT.rlev = GetContent(1)->rlev;
		unsigned long k = Round((GetContent(1)->max - GetContent(1)->min) / GetContent(1)->rlev) + 1;
		if (k > nBufferPointNum) k = nBufferPointNum;
		if (k*m_Curve.sample > MAX_INT)
			k = MAX_INT / (m_Curve.sample);
		m_BufferPointNum = (size_t) k;
		m_ReadPointNum = 0;
		m_Buffer = new char[ k * m_Curve.sample*m_Curve.length];
		bExist = 1;	
	}	
	CreateDefaultBuffer();
}   

CInFID::~CInFID()
{          
	if (m_Buffer != NULL) delete m_Buffer;
	if (m_DefaultBuffer != NULL) delete m_DefaultBuffer;
}		  

void CInFID::CreateDefaultBuffer()
{
	m_DefaultBuffer = new char[m_Curve.sample * m_Curve.length];
	char* p = m_DefaultBuffer;
	for (int j=0; j<m_Curve.sample; j++)  
	{
		switch (m_Curve.repcode)
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
		p += m_Curve.length;
	}
}

void CInFID::ReadBuffer(float dep)
{
	size_t k,i;
	unsigned long pnum;
	char* p = m_Buffer;
	m_Sdep = dep;
	m_Edep = m_Sdep + (m_BufferPointNum - 1)* m_CONTENT.rlev;
	m_ReadPointNum = 0;
	if (!bExist) return;  
	if (m_Sdep < m_CONTENT.min)
	{
	    pnum = Round((m_CONTENT.min - m_Sdep) / m_CONTENT.rlev);
		if (pnum >= m_BufferPointNum) pnum = m_BufferPointNum;
		for (i=0; i< pnum; i++)
		{
			memcpy(p,m_DefaultBuffer,m_Curve.sample*m_Curve.length);
			p += m_Curve.sample*m_Curve.length; 
		}
		m_ReadPointNum = i;       
		m_Sdep += m_CONTENT.rlev * m_ReadPointNum;
		if (m_ReadPointNum  == m_BufferPointNum) goto l100;
	}	
	if (m_Sdep < m_CONTENT.max) 
	{
		pnum = Round((m_CONTENT.max - m_Sdep) / m_CONTENT.rlev);
		if (pnum+m_ReadPointNum >= m_BufferPointNum) pnum = m_BufferPointNum - m_ReadPointNum;
	}            
	else goto l100;
	k = m_Curve.sample;               
	Open(); 
	SeekToDepth(m_Sdep);
	if (m_Curve.sample == m_CONTENT.npw)
	{
		size_t num = 0;                
		k *= (size_t)pnum;
		switch (m_Curve.repcode)
		{
		case REPR_CHAR:
			num = Read(m_Curve.name,k,(char*)p);
			break;
		case REPR_SHORT:
			num = Read(m_Curve.name,k,(short*)p);
			break;
		case REPR_INT:
			num = Read(m_Curve.name,k,(int*)p);
			break;
		case REPR_LONG:
			num = Read(m_Curve.name,k,(long*)p);
			break;
		case REPR_FLOAT:
			num = Read(m_Curve.name,k,(float*)p);
			break;
		case REPR_DOUBLE:
			num = Read(m_Curve.name,k,(double*)p);
			break;
		default :
			break;                          
		}                           
		if (num > 0) m_ReadPointNum += num / m_Curve.sample;
	}
	else 
	{
		if (k > (size_t)m_CONTENT.npw) k = m_CONTENT.npw;
		for (size_t j=0; j<(size_t)pnum; j++)
		{
			size_t num = 0;
			switch (m_Curve.repcode)
			{
			case REPR_CHAR:
				num = Read(m_Curve.name,k,(char*)p);
				break;
			case REPR_SHORT:
				num = Read(m_Curve.name,k,(short*)p);
				break;
			case REPR_INT:
				num = Read(m_Curve.name,k,(int*)p);
				break;
			case REPR_LONG:
				num = Read(m_Curve.name,k,(long*)p);
				break;
			case REPR_FLOAT:
				num = Read(m_Curve.name,k,(float*)p);
				break;
			case REPR_DOUBLE:
				num = Read(m_Curve.name,k,(double*)p);
				break;
			default :
				break;                          
			}
			if (num < 0) break;
			p += num * m_Curve.length;
			int i = (m_Curve.sample - num) * m_Curve.length;
			if (i > 0) 
			{
				memcpy(p,m_DefaultBuffer,i);
				p += i;
			}	
			m_ReadPointNum ++;
			Next();
		}	                  
	}
l100:
	p = m_Buffer + m_ReadPointNum * m_Curve.sample*m_Curve.length; 
	for (i=m_ReadPointNum; i<m_BufferPointNum; i++)
	{
		memcpy(p,m_DefaultBuffer,m_Curve.sample*m_Curve.length);
		p += m_Curve.sample*m_Curve.length; 
	}
	m_ReadPointNum = m_BufferPointNum;
	m_Sdep = dep;
	m_Edep = m_Sdep + m_ReadPointNum * m_CONTENT.rlev;
	Close();
	return;
}   

void CInFID::DatIn(float dep)
{
	int pointno = 0;
	if (bExist) 
	{
		pointno = Round((dep - m_Sdep) / m_CONTENT.rlev);
		if ((pointno < 0) || (pointno + m_Curve.number > (int)m_ReadPointNum))
		{
			ReadBuffer(dep);
			pointno = 0;
		}
		int i = 0;
		while (i+(int)m_BufferPointNum < m_Curve.number)
		{
			memcpy(in_ptr+i*m_Curve.length * m_Curve.sample,m_Buffer,
				   m_Curve.length * m_Curve.sample * m_BufferPointNum);
			ReadBuffer(dep + i*m_CONTENT.rlev);
			i += m_BufferPointNum;
		}
		int k = m_Curve.number - i;
		if (k > 0) memcpy(in_ptr + i*m_Curve.length * m_Curve.sample,
			m_Buffer + pointno * m_Curve.length * m_Curve.sample,
			m_Curve.length * m_Curve.sample * k);

	}
	else
	{
		for (int i=0; i< m_Curve.number; i++)
			memcpy(in_ptr + i * m_Curve.length * m_Curve.sample,m_DefaultBuffer,m_Curve.length * m_Curve.sample);
	}	
}                  

void CInFID::DatIn(float dep, char*p, size_t number)
{
	int pointno = 0;
	if (bExist) 
	{
		pointno = Round((dep - m_Sdep) / m_CONTENT.rlev);
		if ((pointno < 0) || (pointno + number > m_ReadPointNum))
		{
			ReadBuffer(dep);
			pointno = 0;
		}
		int i = 0;
		while (i+m_BufferPointNum < number)
		{
			memcpy(p+i*m_Curve.length * m_Curve.sample,m_Buffer,m_Curve.length * m_Curve.sample * m_BufferPointNum);
			ReadBuffer(dep + i*m_CONTENT.rlev);
			i += m_BufferPointNum;
		}
		int k = number - i;
		if (k > 0) memcpy(p+i*m_Curve.length * m_Curve.sample,m_Buffer + pointno * m_Curve.length * m_Curve.sample,m_Curve.length * m_Curve.sample * k);
	}
	else
	{
		for (int i=0; i<m_Curve.number; i++)
			memcpy(p + i * m_Curve.length * m_Curve.sample,m_DefaultBuffer,m_Curve.length * m_Curve.sample);
	}	
}                  

char* CInFID::GetBuffer()
{
	return m_Buffer;
}                   

size_t CInFID::GetBufferPointNum()
{
	return m_BufferPointNum;
}                  

long CInFID::GetBufferSize()
{
	return m_BufferPointNum * m_Curve.length * m_Curve.sample;
}

long CInFID::SetBufferSize(long size)
{
	long k = size;  
	if (k < 0) k = 0;
	if (k / m_Curve.length > MAX_INT) k = MAX_INT * m_Curve.length;
	size_t nBufferPointNum = (size_t) (k / (m_Curve.length * m_Curve.sample));
	SetBufferPointNum(nBufferPointNum);
	return GetBufferSize();
}

size_t CInFID::SetBufferPointNum(size_t nBufferPointNum)
{                             
	if (!bExist) return 1;
	if (m_Buffer != NULL) delete m_Buffer;
	m_ReadPointNum = 0;
	m_Sdep = 0.f; m_Edep = 0.f;
	long k = nBufferPointNum;  
	if (k < 0) k = 0;
	if (k*m_Curve.sample > MAX_INT)
		k = MAX_INT / (m_Curve.sample);
	m_BufferPointNum = (size_t)k;
	m_Buffer = new char[m_Curve.sample*m_Curve.length*m_BufferPointNum];
	return m_BufferPointNum;
}

COutFID::COutFID(const char* szName, OUTCURVE& curve,char* out, size_t nBufferPointNum)
		: CFIDIO(szName)
{
	m_BufferPointNum = nBufferPointNum;
	out_ptr = out;
	m_Curve = curve;                        
	m_ReadPointNum = 0;
	m_Sdep = 0.f;
	m_Edep = 0.f;                
	m_Buffer = NULL;    
	if (Open() && GetContent(m_Curve.name)) 
	{
		m_CONTENT = *GetContent(m_Curve.name);
		m_CONTENT.min = GetContent(1)->min;
		m_CONTENT.max = GetContent(1)->max;
		m_CONTENT.rlev = GetContent(1)->rlev;
		unsigned long k = Round((GetContent(1)->max - GetContent(1)->min) / GetContent(1)->rlev) + 1;
		if (k > nBufferPointNum) k = nBufferPointNum;
		if (k*m_Curve.sample > MAX_INT)
			k = MAX_INT / (m_Curve.sample);
		m_BufferPointNum = (size_t) k;
		m_Buffer = new char[m_Curve.sample*m_Curve.length*m_BufferPointNum];
	}	
}               

COutFID::COutFID(const char* szName, OUTCURVE& curve,char* out, size_t nBufferPointNum,FIDINDEX* pINDEX)
		: CFIDIO(szName,pINDEX)
{
	out_ptr = out;
	m_Curve = curve;                     
	m_ReadPointNum = 0;
	m_Sdep = m_Edep = 0.f;
	m_Buffer = NULL;
	if (Open() && GetContent(m_Curve.name)) 
	{
		m_CONTENT = *GetContent(m_Curve.name);
		m_CONTENT.min = GetContent(1)->min;
		m_CONTENT.max = GetContent(1)->max;
		m_CONTENT.rlev = GetContent(1)->rlev;
		unsigned long k = Round((GetContent(1)->max - GetContent(1)->min) / GetContent(1)->rlev) + 1;
		if (k > nBufferPointNum) k = nBufferPointNum;
		if (k*m_Curve.sample > MAX_INT)
			k = MAX_INT / (m_Curve.sample);
		m_BufferPointNum = (size_t) k;
		m_Buffer = new char[m_Curve.sample*m_Curve.length*m_BufferPointNum];
	}	
}

COutFID::~COutFID()
{          
	WriteBuffer();
	if (m_Buffer != NULL) delete m_Buffer;
}           

void COutFID::ReadBuffer(float dep)
{
	if (m_BufferPointNum <= 0) return;
	size_t k;      
	k = m_Curve.sample;               
	m_Sdep = dep;
	m_Edep = m_Sdep + (m_BufferPointNum - 1)* m_CONTENT.rlev;
	if ((dep < m_CONTENT.min) || (dep > m_CONTENT.max)) 
	{
		m_ReadPointNum = 0 ;
		return;
	}
	Open();
	SeekToDepth(m_Sdep);  
	if (k == (size_t)m_CONTENT.npw)
	{
		size_t num = 0;                
		k *= m_BufferPointNum;
		char* p = m_Buffer;
		switch (m_Curve.repcode)
		{
		case REPR_CHAR:
			num = Read(m_Curve.name,k,(char*)p);
			break;
		case REPR_SHORT:
			num = Read(m_Curve.name,k,(short*)p);
			break;
		case REPR_INT:
			num = Read(m_Curve.name,k,(int*)p);
			break;
		case REPR_LONG:
			num = Read(m_Curve.name,k,(long*)p);
			break;
		case REPR_FLOAT:
			num = Read(m_Curve.name,k,(float*)p);
			break;
		case REPR_DOUBLE:
			num = Read(m_Curve.name,k,(double*)p);
			break;
		default :
			break;                          
		}                           
		if (num > 0) m_ReadPointNum = num / m_Curve.sample;
		else m_ReadPointNum = 0;
	}
	else
	{
		if (k > (size_t)m_CONTENT.npw) k = m_CONTENT.npw;
		char* p = m_Buffer;
		for (size_t j=0; j<m_BufferPointNum; j++)
		{
			size_t num = 0;
			switch (m_Curve.repcode)
			{
			case REPR_CHAR:
				num = Read(m_Curve.name,k,(char*)p);
				break;
			case REPR_SHORT:
				num = Read(m_Curve.name,k,(short*)p);
				break;
			case REPR_INT:
				num = Read(m_Curve.name,k,(int*)p);
				break;
			case REPR_LONG:
				num = Read(m_Curve.name,k,(long*)p);
				break;
			case REPR_FLOAT:
				num = Read(m_Curve.name,k,(float*)p);
				break;
			case REPR_DOUBLE:
				num = Read(m_Curve.name,k,(double*)p);
				break;
			default :
				break;                          
			}
			if (num <= 0) break;
			p += m_Curve.sample * m_Curve.length;
			Next();
		}
		m_ReadPointNum = j;
	}	   
	Close();
	return;
}   

void COutFID::WriteBuffer(void)
{
	size_t k;
	if (m_ReadPointNum <= 0) return;
	Open();
	SeekToDepth(m_Sdep);
	char* p = m_Buffer;
	k = m_Curve.sample;               
	if (k == (size_t)m_CONTENT.npw)
	{
		k *= m_ReadPointNum;
		switch (m_Curve.repcode)
		{
		case REPR_CHAR:
			Write(m_Curve.name,k,(char*)p);
			break;
		case REPR_INT:
			Write(m_Curve.name,k,(int*)p);
			break;
		case REPR_SHORT:
			Write(m_Curve.name,k,(short*)p);
			break;
		case REPR_LONG:
			Write(m_Curve.name,k,(long*)p);
			break;
		case REPR_FLOAT:
			Write(m_Curve.name,k,(float*)p);
			break;
		case REPR_DOUBLE:
			Write(m_Curve.name,k,(double*)p);
			break;
		default:
			break;
		}                                             
	}
	else 
	{
		if (k > (size_t)m_CONTENT.npw) k = m_CONTENT.npw;
		for (size_t j=0; j<m_ReadPointNum; j++)
		{
			switch (m_Curve.repcode)
			{
			case REPR_CHAR:
				Write(m_Curve.name,k,(char*)p);
				break;
			case REPR_INT:
				Write(m_Curve.name,k,(int*)p);
				break;
			case REPR_SHORT:
				Write(m_Curve.name,k,(short*)p);
				break;
			case REPR_LONG:
				Write(m_Curve.name,k,(long*)p);
				break;
			case REPR_FLOAT:
				Write(m_Curve.name,k,(float*)p);
				break;
			case REPR_DOUBLE:
				Write(m_Curve.name,k,(double*)p);
				break;
			default:
				break;
			}                                             
			Next();
			p += m_Curve.length * m_Curve.sample;
		}	
	}	
	Close();
	m_ReadPointNum = 0;
}       

void COutFID::DatOut(float dep)
{
	int number = m_Curve.number;
	if (m_BufferPointNum < 0) return ;
	if (dep > m_CONTENT.max) return ;
	if (dep < m_CONTENT.min) 
	{
		int k = Round((m_CONTENT.min - dep) / m_CONTENT.rlev);
		if (k >= m_Curve.number) return ;
		number = m_Curve.number - k;
		dep = m_CONTENT.min;
	}
	if (m_ReadPointNum == 0) 
		ReadBuffer(dep);
	int pointno =  Round((dep - m_Sdep) / m_CONTENT.rlev);
	if ((pointno < 0) || (pointno + number >= (int)m_BufferPointNum))
	{
		WriteBuffer();
		ReadBuffer(dep);
		pointno = 0;
	}
	int i = 0;
	while (i+(int)m_BufferPointNum < number)
	{
		memcpy(m_Buffer,out_ptr+m_Curve.length * m_Curve.sample*i,m_Curve.length * m_Curve.sample * m_BufferPointNum);
		WriteBuffer();
		ReadBuffer(dep + i*m_CONTENT.rlev);
		i += m_BufferPointNum;
	}
	int k = number - i; 
	if (k > 0) memcpy(m_Buffer+pointno*m_Curve.length * m_Curve.sample,out_ptr+i*m_Curve.length * m_Curve.sample,m_Curve.length * m_Curve.sample * k);
}   

void COutFID::DatOut(float dep, char* p, size_t number)
{
	if (m_BufferPointNum <= 0) return ;
	if (dep > m_CONTENT.max) return ;
	if (dep < m_CONTENT.min) 
	{
		int k = Round((m_CONTENT.min - dep) / m_CONTENT.rlev);
		if (k >= (int)number) return ;
		number -= k;
		dep = m_CONTENT.min;
	}
	if (m_ReadPointNum == 0) 
		ReadBuffer(dep);
	int pointno = Round((dep - m_Sdep) / m_CONTENT.rlev);
	if ((pointno < 0) || (pointno + number >= m_BufferPointNum))
	{
		WriteBuffer();
		ReadBuffer(dep);
		pointno = 0;
	}
	size_t i=0;
	while (i+m_BufferPointNum < number)
	{
		memcpy(m_Buffer,p+m_Curve.length * m_Curve.sample*i,m_Curve.length * m_Curve.sample * m_BufferPointNum);
		WriteBuffer();
		ReadBuffer(dep + i*m_CONTENT.rlev);
		i += m_BufferPointNum;
	}
	int k = number - i; 
	if (k > 0) memcpy(m_Buffer+pointno*m_Curve.length * m_Curve.sample,p+i*m_Curve.length * m_Curve.sample,m_Curve.length * m_Curve.sample * k);
}   

char* COutFID::GetBuffer()
{
	return m_Buffer;
}                   

size_t COutFID::GetBufferPointNum()
{
	return m_BufferPointNum;
}                  

long COutFID::GetBufferSize()
{
	return m_BufferPointNum * m_Curve.length * m_Curve.sample;
}

long COutFID::SetBufferSize(long size)
{
	long k = size;  
	if (k < 0) k = 0;
	if (k / m_Curve.length > MAX_INT) k = MAX_INT * m_Curve.length;
	size_t nBufferPointNum = (size_t) (k / (m_Curve.length * m_Curve.sample));
	SetBufferPointNum(nBufferPointNum);
	return GetBufferSize();
}

size_t COutFID::SetBufferPointNum(size_t nBufferPointNum)
{                             
	WriteBuffer(); 
	if (!bExist) return 1;
	if (m_Buffer != NULL) delete m_Buffer;
	m_ReadPointNum = 0;
	m_Sdep = 0.f; m_Edep = 0.f;
	long k = nBufferPointNum;  
	if (k < 0) k = 0;
	if (k*m_Curve.sample > MAX_INT)
		k = MAX_INT / (m_Curve.sample);
	m_BufferPointNum = (size_t)k;
	m_Buffer = new char[m_Curve.sample*m_Curve.length*m_BufferPointNum];
	return m_BufferPointNum;
}

CLogProcess::CLogProcess(const char *strWellName, const char* strInpName)
{
	m_MethodName = "";
	m_InpFile = NULL;
	strcpy(m_LogName,strWellName);
	strcpy(m_InpFileName,"");
	strcpy(m_DepthUnit,"m");
	if ((strInpName != NULL) && (strcmp(strInpName,"")))
	{
		strcpy(m_InpFileName,strInpName);
	    OpenInpCard();
    }
	m_InCurveNum = m_OutCurveNum = 0;
	m_InpFlag = m_InFlag = m_OutFlag = 0;       
	m_ResultFile = NULL;
	m_InFID = NULL;
	m_OutFID = NULL;
	m_InCurve = NULL;
	m_OutCurve = NULL;
	Sdep = Edep = EEdep = SSdep = 0.f;
	StartDep = EndDep = DepLevel = 0.f;
}

CLogProcess::~CLogProcess()
{
	CloseInpCard();
	if (m_OutFID != NULL) 
	{
		for (int i=0; i<m_OutCurveNum; i++)
		 if (m_OutFID[i] != NULL) delete m_OutFID[i];
		delete m_OutFID;
		m_OutFID = NULL; 
    }
	if (m_InFID != NULL)
	{
		for (int i=0; i<m_InCurveNum; i++) 
		 if (m_InFID[i] != NULL) delete m_InFID[i];
	 	delete m_InFID; 
	 	m_InFID = NULL;
	}	
}

int CLogProcess::OpenInpCard(void)
{
	CloseInpCard();
	if ((m_InpFile = fopen(m_InpFileName,"rt")) == NULL)
    {
    	m_InpFlag = 0;
		return 0;
    };  
	m_InpFlag = 1;
	return 1;
}

void CLogProcess::CloseInpCard(void)
{
	if (m_InpFile != NULL) fclose(m_InpFile);
	m_InpFile = NULL;
	m_InpFlag = 0;
}

int CLogProcess::InitPara(int num, INPNAME *name, float *value)
{
	int i;
	char *s;
	m_ParaNum = num;
	m_NameC = name;
	m_Para = value;
	for (i=0; i<m_ParaNum; i++)
	{
		s = strchr(m_NameC[i],' ');
		if (s != NULL) *s='\0';
	} 
	if (!OpenInpCard()) return 1;
	if (m_InFlag == 0)
	{
		StartDep = 0.f; 
		EndDep = 99999.f; 
	}

	m_InpFlag = 2;	
	ReadInpCard();

	OpenInpCard();
	ReadInpCard();
	SSdep = Sdep;
	while (!ReadInpCard()) ;
	EEdep = Edep;

	OpenInpCard();
	return 0;
}

int ReadLine(FILE* fp,char* m_LineStr)
{
	int ch,i;
	while (1)
	{
		ch = 0;
		i = 0;
		m_LineStr[0] = 0;
		while ((ch != '\n') && (i < 255))
		{
			ch = fgetc(fp);
			if (ch == EOF) return 1;
			if (ch == ' ') continue;//
			if ((ch > 'a') && (ch < 'z')) ch -= 32;
			m_LineStr[i] = ch;
			i++;
		}
		if (ch == '\n') m_LineStr[i-1] = 0;
		else continue;
		if (i < 3) continue;
		if ((m_LineStr[0] == 'I') || (m_LineStr[0] == '$') || (m_LineStr[0] == '#') ||
		    (m_LineStr[0] == '%') || (m_LineStr[0] == '!') || 
			(!strncmp(m_LineStr,"///",2))) continue;
		if (!strncmp(m_LineStr,"END",3)) return 1;
		return 0;
	}
}

int ReadLine0(FILE* fp,char *m_LineStr)  
{
	int ch,i;
	while (1)
	{
		ch = 0;
		i = 0;
		m_LineStr[0] = 0;
		while ((ch != '\n') && (i < 255))
		{
			ch = fgetc(fp);
			if (ch == EOF) return 1;
			if ((ch == ' ') && (i==0)) continue;//
			if ((ch > 'a') && (ch < 'z')) ch -= 32;
			m_LineStr[i] = ch;
			i++;
		}
		if (ch == '\n') m_LineStr[i-1] = 0;
		else continue;
		if (i < 3) continue;
		if ((m_LineStr[0] == ';') || (m_LineStr[0] == 'I') || (m_LineStr[0] == '$')
			|| (m_LineStr[0] == '#') ||(m_LineStr[0] == '%') || (m_LineStr[0] == '!') || 
			(!strncmp(m_LineStr,"///",2))) continue;
		if (!strncmp(m_LineStr,"END",3)) return 1;
		return 0;
	}
}

int CLogProcess::ReadInpCard(void)
{
	int is,ie;char nChar;
	CString szStr1,szCurve,szCurve1;
	char line[256] ;
	char tmpStr[20];
	char *s,*s1;
	int i;
	if (m_InpFlag==0) return 1;
	if (feof(m_InpFile)) return 2;
	while (!ReadLine0(m_InpFile,line))
	{
		szStr1=line;
		if(szStr1.FindOneOf("<>") != -1)
		{
			if(m_InpFlag == 2)
			{
				while (szStr1.FindOneOf("<>") != -1)//重定向行
				{
					is = szStr1.FindOneOf("<>"); 
					ie = szStr1.Find(',');
					if (ie == -1) ie= szStr1.GetLength();
					szCurve = szStr1.Left(is);
					nChar = szStr1.GetAt(is);
					szCurve1= szStr1.Mid(is+1, ie-is-1);
					szStr1 = szStr1.Right(szStr1.GetLength()-ie-1);
				
					if (szCurve1 == "") continue;
					if (!szCurve.CompareNoCase(szCurve1)) continue;
					if (nChar == '<')	            //输入定向
					{
						for (i=0; i<m_InCurveNum ;i++)
						{
							if (!stricmp(szCurve,m_InCurve[i].name))
							{
								strcpy(m_InCurve[i].name,szCurve1);
								break;
							}
						}
					}
					else					        //输出定向
					{
						for (i=0; i<m_OutCurveNum ;i++)
						{
							if (!stricmp(szCurve,m_OutCurve[i].name))
							{	
								strcpy(m_OutCurve[i].name,szCurve1);
								break;
							}
						}
					}
				}
				continue;
			}
			continue;
		}

		for (i=0; i<(int)strlen(line);i++)
		{
			if (line[i] == ',') line[i] = ' ';
		}
		line[i] = ' ';
		sscanf(line,"%f%f",&Sdep,&Edep);
		if (Sdep < StartDep) Sdep = StartDep;
		if (Edep > EndDep) Edep = EndDep;
		if (Edep < Sdep)
		{
			do 
			{
				if (ReadLine(m_InpFile,line)) return 2;
			}while(line[strlen(line)-1] == ',');
			continue;
		}
		BOOL bContinue = TRUE;
		while (bContinue)
		{
			if (ReadLine(m_InpFile,line)) return 2;
			bContinue = line[strlen(line)-1] == ',';
			s = strtok(line,",");
			while (s)
			{
				s1 = strchr(s,'=');
				if (s1)
				{
					*s1 = '\0';
					sscanf(s,"%s8",tmpStr);
					for (i=0; i<m_ParaNum; i++)
						if (!stricmp(tmpStr,m_NameC[i]))
						{
							m_Para[i] = (float)atof(s1+1);
							break;
						}
				}
				s = strtok(NULL,",");
			}
		}
		return 0;
	}
	return 2;
}

BOOL CLogProcess::ParametEdit()
{
	if (strcmp(m_InpFileName, "")) {
		CString szExt;
 		SplitPath(m_InpFileName, NULL, NULL, NULL, szExt.GetBuffer(128));
		szExt.ReleaseBuffer();
		if (szExt.GetLength() >0) szExt = szExt.Right(szExt.GetLength()-1);
		
		CParameterEdit dlg(NULL, m_InCurveNum,m_OutCurveNum,m_ParaNum, m_InCurve, m_OutCurve,
		              m_NameC, m_Para);
		dlg.m_MethodName = 	m_MethodName;
		dlg.m_InpFileName = "";
		dlg.m_InpFileName = m_InpFileName;
		dlg.m_ParaExt = szExt;
		dlg.m_SDep = StartDep;
		dlg.m_EDep = EndDep;
		dlg.m_Rlev = DepLevel;
		CloseInpCard();
		if (dlg.DoModal() != IDOK) return FALSE;	
		OpenInpCard();
		return TRUE;
	}
	return FALSE;
}
int CLogProcess::GetUpdateFlag()
{
    struct _finddata_t c_file;
	char s1[512];
	char s[512];
	time_t t;
	CFIDIO::GetFileName(m_LogName, s1 , 'D');
	strcat(s1,".");
	BOOL bFirst = TRUE;

	if (m_OutCurveNum == 0) return 0;
	// 获得输出曲线最早的日期,若某条曲线不存在,则需要更新
	for (int i=0; i<m_OutCurveNum; i++)
	{
	   	strcpy(s,s1);
		strcat(s,m_OutCurve[i].name);
		if  (_findfirst(s, &c_file) != -1L)
		{
		   	if (bFirst)
			{
				t = c_file.time_write;
				bFirst = FALSE;
			}
			else if (c_file.time_write < t) t = c_file.time_write;
		}
		else return TRUE;
	}
	
	// 比较参数卡和输出曲线
	if  (strcmp(m_InpFileName,"") && (_findfirst((char*)m_InpFileName, &c_file) != -1L))
	{
		if (c_file.time_write > t) return TRUE;
	}

	// 比较输入曲线和输出曲线
	for (i=0; i<m_InCurveNum; i++)
	{
	   	strcpy(s,s1);
		strcat(s,m_InCurve[i].name);
		if  (_findfirst(s, &c_file) != -1L)
		{
		   if (c_file.time_write > t) return TRUE;
		}
	}
	return FALSE;
}

void CLogProcess::InitBuffer()
{
	int i = 0;
	unsigned long k = 0;
	if (m_OutFID != NULL) 
	{
		for (int i=0; i<m_OutCurveNum; i++)
		 if (m_OutFID[i] != NULL) delete m_OutFID[i];
		delete m_OutFID;
		m_OutFID = NULL; 
    }
	if (m_InFID != NULL)
	{
		for (int i=0; i<m_InCurveNum; i++) 
		 if (m_InFID[i] != NULL) delete m_InFID[i];
	 	delete m_InFID; 
	 	m_InFID = NULL;
	}	
	for (i=0; i<m_InCurveNum; i++)
	{
		if (m_InCurve[i].number <= 0) m_InCurve[i].number = 1;
		if (m_InCurve[i].sample <= 0) m_InCurve[i].sample = 1;
		if (m_InCurve[i].repcode == 0) m_InCurve[i].repcode = REPR_FLOAT;
		int j = CFIDIO::GetTypeLength(m_InCurve[i].repcode);
		if (j != 0) m_InCurve[i].length = j;
		k += m_InCurve[i].sample * m_InCurve[i].length * m_InCurve[i].number;
	}
	for (i=0; i<m_OutCurveNum; i++)
	{
		if (m_OutCurve[i].number <= 0) m_OutCurve[i].number = 1;
	    if (m_OutCurve[i].sample <= 0) m_OutCurve[i].sample = 1;
		if (m_OutCurve[i].repcode == 0) m_OutCurve[i].repcode = REPR_FLOAT;
		int j = CFIDIO::GetTypeLength(m_OutCurve[i].repcode);
		if (j != 0) m_OutCurve[i].length = j; 
		k += m_OutCurve[i].sample * m_OutCurve[i].length * m_OutCurve[i].number;
	}
	if (k <= 0) return;
	unsigned long m_BufferPointNum = Round(1024.0 * MAX_BUFFER / k);
	if (m_BufferPointNum < 0) m_BufferPointNum = 1;

	char s[255];  
	unsigned long number;
	if (m_InCurveNum > 0) m_InFID = new CInFID* [m_InCurveNum];
	for (i=0; i<m_InCurveNum ;i++)
    {
		if (m_InCurve[i].length == 0)
		{
			m_InFID[i] = NULL;
			continue;
		}	
		strcpy(s,m_LogName); 
		strcat(s,".");
		strcat(s,m_InCurve[i].name); 
		number = m_BufferPointNum * m_InCurve[i].number;
		if (number > MAX_INT)	number = MAX_INT;
		CInFID* pFID = new CInFID(s,m_InCurve[i],in_ptr,(size_t)number);
		if (pFID->Open() && (pFID->GetContent(m_InCurve[i].name) != NULL)) 
		{
			if (!m_InFlag)
			{
				m_InFlag = 1;                                  
				StartDep = pFID->GetContent(1)->min;
				EndDep = pFID->GetContent(1)->max;
				DepLevel = pFID->GetContent(1)->rlev;
				strcpy(m_DepthUnit,pFID->GetContent(1)->unit);
			}	        
			m_InCurve[i].sdep = pFID->GetContent(1)->min;	
			m_InCurve[i].edep = pFID->GetContent(1)->max;	
			m_InCurve[i].rlev = pFID->GetContent(1)->rlev;	
			strncpy(m_InCurve[i].unit,pFID->GetContent(2)->unit,10);	
			m_InCurve[i].unit[9]=0;	
		}
		pFID->Close();
		m_InFID[i] = pFID;
		in_ptr += m_InCurve[i].sample * m_InCurve[i].length * m_InCurve[i].number;
	}       
	
	if (m_OutCurveNum > 0) m_OutFID = new COutFID*[m_OutCurveNum];
	m_OutFlag = 1;
	
	for (i=0; i<m_OutCurveNum ;i++)
    {
		if ((m_OutCurve[i].length == 0) || (m_OutCurve[i].flag == 2))
		{   
			m_OutFID[i] = NULL;
			out_ptr += m_OutCurve[i].sample * m_OutCurve[i].length * m_OutCurve[i].number;
			continue;          
		}	
		strcpy(s,m_LogName);
		strcat(s,".");
		strcat(s,m_OutCurve[i].name); 
		number = m_BufferPointNum * m_OutCurve[i].number;
		if (number > MAX_INT)	number = MAX_INT;
		COutFID* pFID = new COutFID(s,m_OutCurve[i],out_ptr,(size_t)number);
		if ((m_OutCurve[i].flag == 1) || (!pFID->Open()) || (pFID->GetContent(m_OutCurve[i].name) == NULL))
		{
			delete pFID; 
			pFID = NULL;

			FIDCONTENT m_CONTENT[2] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
	    		                   {"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f}};
			FIDINDEX m_INDEX;
			char adi[] = "DATID 0";
			m_INDEX.dimension = 2;
			m_INDEX.Contentlist = m_CONTENT;
			m_INDEX.adi = adi;
			if (m_OutCurve[i].sdep != 0) m_CONTENT[0].min = m_OutCurve[i].sdep;
			else if (m_InFlag) m_CONTENT[0].min = StartDep;
			else if (m_InpFlag) m_CONTENT[0].min = SSdep;  
			else m_CONTENT[0].min = StartDep;
			if (m_OutCurve[i].edep != 0) m_CONTENT[0].max = m_OutCurve[i].edep;
			else if (m_InFlag) m_CONTENT[0].max = EndDep;
			else if (m_InpFlag) m_CONTENT[0].max = EEdep;
			else m_CONTENT[0].max = EndDep;
			if (m_OutCurve[i].rlev != 0) m_CONTENT[0].rlev = m_OutCurve[i].rlev;
			else if (m_InFlag) m_CONTENT[0].rlev = DepLevel;
			else m_CONTENT[0].rlev = DepLevel;
			strcpy(m_CONTENT[0].unit,m_DepthUnit);
			m_CONTENT[1].rlev = m_CONTENT[0].rlev;
			strcpy(m_CONTENT[1].name, m_OutCurve[i].name);
			strcpy(m_CONTENT[1].unit, m_OutCurve[i].unit);
			m_CONTENT[1].type = m_OutCurve[i].repcode;
			m_CONTENT[1].length = m_OutCurve[i].length; 
			m_CONTENT[1].npw = m_OutCurve[i].sample; 
			if ((m_CONTENT[0].rlev != 0) && (m_CONTENT[0].min < m_CONTENT[0].max))
				pFID = new COutFID(s,m_OutCurve[i],out_ptr,(size_t)number,&m_INDEX);
		}                          
		if ((pFID != NULL) && (pFID->GetContent(1) != NULL))
		{
			m_OutCurve[i].sdep = pFID->GetContent(1)->min;	
			m_OutCurve[i].edep = pFID->GetContent(1)->max;	
			m_OutCurve[i].rlev = pFID->GetContent(1)->rlev;	
			if (DepLevel == 0) DepLevel = pFID->GetContent(1)->rlev;
			pFID->Close();
		}
		m_OutFID[i] = pFID;
		out_ptr += m_OutCurve[i].sample * m_OutCurve[i].length * m_OutCurve[i].number;
    }       
	struct _finddata_t c_file;
	if (_findfirst(m_InpFileName,&c_file) == -1L)
	{
		SSdep = StartDep;
		EEdep = EndDep;
	}
}

int CLogProcess::InitIn(int num, INCURVE *curve, char *buffer)
{
	char s[255];
	if (m_InFID != NULL)
	{
		for (int i=0; i<m_InCurveNum; i++) 
		 if (m_InFID[i] != NULL) delete m_InFID[i];
	 	delete m_InFID;
	 	m_InFID = NULL;
	}	
	m_InCurveNum = num;
	if (m_InCurveNum < 0) m_InCurveNum = 0;
	m_InCurve = curve;   
	m_InFlag = 0;
	for (int i=0; i<m_InCurveNum; i++)
	{
		DeleteBlank(curve[i].name);
		if (m_InCurve[i].number <= 0) m_InCurve[i].number = 1;
		if (m_InCurve[i].sample <= 0) m_InCurve[i].sample = 1;
		if (m_InCurve[i].repcode == 0) m_InCurve[i].repcode = REPR_FLOAT;
		int j = CFIDIO::GetTypeLength(m_InCurve[i].repcode);
		if (j != 0) m_InCurve[i].length = j;
		strcpy(s,m_LogName); 
		strcat(s,".");
		strcat(s,m_InCurve[i].name); 
		CFIDIO* pFID = new CFIDIO(s);
		if (pFID->Open() && (pFID->GetContent(m_InCurve[i].name) != NULL)) 
		{
			if (!m_InFlag)
			{
				m_InFlag = 1;                                  
				DepLevel = pFID->GetContent(1)->rlev;
				StartDep = 99999.f;
				EndDep = 0.f;
			}
			if(DepLevel >pFID->GetContent(1)->rlev)
				DepLevel = pFID->GetContent(1)->rlev;
			if (StartDep > pFID->GetContent(1)->min) 
				StartDep = pFID->GetContent(1)->min;
			if (EndDep < pFID->GetContent(1)->max) 
				EndDep = pFID->GetContent(1)->max;
			m_InCurve[i].sdep = pFID->GetContent(1)->min;	
			m_InCurve[i].edep = pFID->GetContent(1)->max;	
			m_InCurve[i].rlev = pFID->GetContent(1)->rlev;	
			strncpy(m_InCurve[i].unit,pFID->GetContent(1)->unit,10);	
			m_InCurve[i].unit[9]=0;	
		}
		pFID->Close();
		delete pFID;
	}       
	in_ptr = buffer;
	return 0;
}

int CLogProcess::DatIn()
{
	for (int i=0; i<m_InCurveNum; i++)
	{
		if (m_InFID[i] != NULL) 
			m_InFID[i]->DatIn(Dep);
	}
	return 0;
}

int CLogProcess::DatIn(int num,char* p, size_t number)
{
	if ((num < 0) && (num > m_InCurveNum)) 
		return -1;
	if (m_InFID[num-1] != NULL) 
		m_InFID[num-1]->DatIn(Dep,p,number);
	return 0;
}             

int CLogProcess::DatIn(char* name,char* p, size_t number)
{
	char s[255];
	strcpy(s,name);
	DeleteBlank(s);
	for (int num=0; num<m_OutCurveNum; num++)
	{
		if (!stricmp(s,m_OutCurve[num].name)) 
			return DatIn(num+1,p,number);
	}                           
	return 0;
}             

int CLogProcess::InitOut(int num, OUTCURVE *curve, char *buffer)
{
	if (m_OutFID != NULL) 
	{
		for (int i=0; i<m_OutCurveNum; i++)
		    if (m_OutFID[i] != NULL) delete m_OutFID[i];
		delete m_OutFID; 
		m_OutFID = NULL;
    }
	m_OutCurveNum = num;
	if (m_OutCurveNum < 0) m_OutCurveNum = 0;
	m_OutFlag = 0;
	m_OutCurve = curve;
	for (int i=0; i<m_OutCurveNum; i++)
	{
		DeleteBlank(curve[i].name);
	}	                                      
	out_ptr = buffer;
	return 0;
}                                                      

int CLogProcess::DatOut()
{
	for (int i=0; i<m_OutCurveNum; i++)
	{
		if (m_OutFID[i] != NULL) m_OutFID[i]->DatOut(Dep);
	}
	return 0;
}       

int CLogProcess::DatOut(int num,char* p, size_t number)
{
	if ((num < 0) && (num > m_OutCurveNum)) return -1;
	if (m_OutFID[num-1] != NULL) m_OutFID[num-1]->DatOut(Dep,p,number);
	return 0;
}             


int CLogProcess::DatOut(char* name, char* p, size_t number)
{
	char s[255];
	strcpy(s,name);
	DeleteBlank(s);
	for (int num=0; num<m_OutCurveNum; num++)
	{
		if (!stricmp(s,m_OutCurve[num].name)) 
			return DatOut(num+1,p,number);
	}
	return 0;
}             

int CLogProcess::WriteBuffer()
{
	for (int i=0; i<m_OutCurveNum; i++)
	{
		if (m_OutFID[i] != NULL) m_OutFID[i]->WriteBuffer();
	}
	return 0;
}             

void CLogProcess::SetBufferPointNum(size_t nBufferPointNum)
{
	unsigned long number;
	for (int i=0; i<m_OutCurveNum; i++)
		if (m_OutFID[i] != NULL) 
		{
			number = nBufferPointNum * m_OutCurve[i].number;
			if (number > MAX_INT)	number = MAX_INT;
			m_OutFID[i]->SetBufferPointNum((size_t)number);
		}
	for (i=0; i<m_InCurveNum; i++)
		if (m_InFID[i] != NULL) 
		{
			number = nBufferPointNum * m_InCurve[i].number;
			if (number > MAX_INT)	number = MAX_INT;
			m_InFID[i]->SetBufferPointNum((size_t)number);
		}
}       

int CLogProcess::IsCurveExist(char* name)
{
	char s[255],s1[20];
	strcpy(s1,name);
	DeleteBlank(s1);
	strcpy(s,m_LogName); 
	strcat(s,".");
	strcat(s,s1);     
	CFIDIO* pFID = new CFIDIO(s);
	int bExist = pFID->Open();
	delete pFID;
	return bExist;
}                 

void CLogProcess::DeleteBlank(char* s)
{
	int j = strlen(s);
	while ((j>0) && (s[j-1] == ' ')) j-- ;
	*(s+j) = 0;
}

FIDCONTENT* CLogProcess::GetCurveCONTENT(char* name)
{
	char s[255],s1[20];
	strcpy(s1,name);
	DeleteBlank(s1);
	strcpy(s,m_LogName); 
	strcat(s,".");
	strcat(s,s1);     
	CFIDIO* pFID = new CFIDIO(s);
	if (pFID->Open()) 
	{
		return pFID->GetContent(s1);
	}	
	delete pFID;
	return NULL;
}  

long CLogProcess::SetBufferSize(long size)
{
	int i;
	unsigned long k = 0;
	for (i=0; i<m_InCurveNum; i++)
		if (m_InFID[i] != NULL) k += m_InCurve[i].sample * m_InCurve[i].length * m_InCurve[i].number;
	for (i=0; i<m_OutCurveNum; i++)
		if (m_OutFID[i] != NULL) k += m_OutCurve[i].sample * m_OutCurve[i].length * m_OutCurve[i].number;
	if (k <= 0) return 0;
	unsigned long number;
	unsigned long nBufferPointNum = Round(1024.0 * size / k);
	for (i=0; i<m_OutCurveNum; i++)
		if (m_OutFID[i] != NULL) 
		{
			number = nBufferPointNum * m_OutCurve[i].number;
			if (number > MAX_INT)	number = MAX_INT;
			m_OutFID[i]->SetBufferPointNum((size_t)number);
		}
	for (i=0; i<m_InCurveNum; i++)
		if (m_InFID[i] != NULL) 
		{
			number = nBufferPointNum * m_InCurve[i].number;
			if (number > MAX_INT)	number = MAX_INT;
			m_InFID[i]->SetBufferPointNum((size_t)number);
		}
	return GetBufferSize();
}         

long CLogProcess::GetBufferSize()
{
	unsigned long k = 0;
	for (int i=0; i<m_OutCurveNum; i++)
		if (m_OutFID[i] != NULL) k += m_OutFID[i]->GetBufferSize();
	for (i=0; i<m_InCurveNum; i++)
		if (m_InFID[i] != NULL) k += m_InFID[i]->GetBufferSize();
	return k;
}                

int CLogProcess::DeleteFIDCurve(char* name)
{
	char s[255],s1[20];
	strcpy(s1,name);
	DeleteBlank(s1);
	strcpy(s,m_LogName); 
	strcat(s,".");
	strcat(s,s1);     
	CFIDIO* pFID = new CFIDIO(s);
	if (pFID->Open())
	{
		pFID->Delete();
		delete pFID;
		return 1;
	}
	delete pFID;
	return 0;
}
