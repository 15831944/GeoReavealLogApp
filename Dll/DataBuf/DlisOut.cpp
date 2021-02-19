// DlisOut.cpp: implementation of the CDlisOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DlisOut.h"
#include "string.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlisOut::CDlisOut()
{

}

CDlisOut::~CDlisOut()
{

}
void CDlisOut::Encode_DEFAULT(int &IP,BYTE *buf)
{
	buf[0]=0X00;
	IP++;
}
void CDlisOut::Encode_NULL(int &IP,BYTE *buf)
{
	buf[0]=0X20;
	IP++; 
}
void CDlisOut::Encode_OBNAME(int &IP,BYTE *buf,const int *Origin,const BYTE *CopyNum,const char *Name,const int Count)
{
	int IpntC=0;
	int NBYTE;
	buf[0]=0X20;
	IpntC++;
	if(Count>1)
	{
		buf[0]|=0X08;
		DLIS_UVARI(Count,NBYTE,&buf[IpntC]);
		IpntC+=NBYTE;
	}
	if(Count>0)
	{
		buf[0]|=0X01;
	}
	int pos=0;
	for(int i=0;i<Count;i++)
	{
		DLIS_UVARI(Origin[i],NBYTE,&buf[IpntC]);
		IpntC+=NBYTE;
		buf[IpntC]=CopyNum[i];
		IpntC++;
		int sl=strlen(&Name[pos]);
		DLIS_IDENT(&Name[pos],NBYTE,&buf[IpntC]);
		pos+=sl+1;
		IpntC+=NBYTE;
	}
	IP+=IpntC;
}
void CDlisOut::Encode_ASCII(int &IP,BYTE *buf,const char *str,char *Units,const BYTE RepC,const int Count)
{
	int IpntC=0;
	int NBYTE;
	Encode_CRU(Count,RepC,Units,IpntC,&buf[IpntC]);
	int pos=0;
	for(int i=0;i<Count;i++)
	{
		int sl=strlen(&str[pos]);
		DLIS_ASCII(&str[pos],NBYTE,&buf[IpntC]);
		pos+=sl+1;
		IpntC+=NBYTE;
	}
	IP+=IpntC;
}
void CDlisOut::Encode_IDENT(int &IP,BYTE *buf,const char *str,char*Units,const BYTE RepC,const int Count)
{
	int IpntC=0;
	int NBYTE;
	Encode_CRU(Count,RepC,Units,IpntC,&buf[IpntC]);
	int pos=0;
	for(int i=0;i<Count;i++)
	{
		int sl=strlen(&str[pos]);
		DLIS_IDENT(&str[pos],NBYTE,&buf[IpntC]);
		pos+=sl+1;
		IpntC+=NBYTE;
	}
	IP+=IpntC;
}
void CDlisOut::Encode_UNITS(int &IP,BYTE *buf,const char *str,char *Units,const BYTE RepC,const int Count)
{
	int IpntC=0;
	int NBYTE;
	Encode_CRU(Count,RepC,Units,IpntC,&buf[IpntC]);
	int pos=0;
	for(int i=0;i<Count;i++)
	{
		int sl=strlen(&str[pos]);
		DLIS_IDENT(&str[pos],NBYTE,&buf[IpntC]);
		pos+=sl+1;
		IpntC+=NBYTE;
	}
	IP+=IpntC;
}

void CDlisOut::Encode_UVARI(int &IP,BYTE *buf,int *NVALUE,char *Units,const BYTE RepC,const int Count)
{
	int IpntC=0;
	int NBYTE;
	Encode_CRU(Count,RepC,Units,IpntC,&buf[IpntC]);
	for(int i=0;i<Count;i++)
	{
		DLIS_UVARI(NVALUE[i],NBYTE,&buf[IpntC]);
		IpntC+=NBYTE;
	}
	IP+=IpntC;
}
void CDlisOut::Encode_DTIME(WORD Year,BYTE Month,BYTE Day,BYTE Hour,BYTE Minute,BYTE Second,int &IP,BYTE *buf)
{
	int IpntC=0;
	buf[0]=0X21;
	IpntC++;
	int NBYTE;
	DLIS_DTIME(Year,Month,Day,Hour,Minute,Second,NBYTE,&buf[IpntC]);
	IpntC+=NBYTE;
	IP+=IpntC;
}
void CDlisOut::Encode_UNORM(int &IP,BYTE *buf,const int *NVALUE,char *Units,const BYTE RepC,const int Count)
{
	int IpntC=0;
	int NBYTE;
	Encode_CRU(Count,RepC,Units,IpntC,&buf[IpntC]);
	for(int i=0;i<Count;i++)
	{
		DLIS_UNORM(NVALUE[i],NBYTE,&buf[IpntC]);
		IpntC+=NBYTE;
	}
	IP+=IpntC;
}
void CDlisOut::Encode_FSINGL(int &IP,BYTE *buf,const float *VALUE,char *Units,const BYTE RepC,const int Count)
{
	int IpntC=0;
	int NBYTE;
	Encode_CRU(Count,RepC,Units,IpntC,&buf[IpntC]);
	for(int i=0;i<Count;i++)
	{
		DLIS_FSINGL(VALUE[0],NBYTE,&buf[IpntC]);
		IpntC+=NBYTE;
	}
	IP+=IpntC;
}
void CDlisOut::Encode_USHORT(int &IP,BYTE *buf,const BYTE *NVALUE,char *Units,const BYTE RepC,const int Count)
{
	int IpntC=0;
	Encode_CRU(Count,RepC,Units,IpntC,&buf[IpntC]);
	for(int i=0;i<Count;i++)
	{
		buf[IpntC]=NVALUE[i];
	}
	IpntC+=Count;
	IP+=IpntC;
}

void CDlisOut::Encode_CRU(const int Count,const BYTE RepC,char *Units,int &IpntC,BYTE *buf)
{
	int NBYTE=0;
	buf[0]=0X20;
	IpntC++;
	//计数和
	if(Count>1)
	{
		DLIS_UVARI(Count,NBYTE,&buf[IpntC]); 
		IpntC+=NBYTE;
		buf[0]|=0X08;
	}
	//表示码
	if(RepC>0)
	{
		buf[IpntC]=RepC; 
		IpntC++;
		buf[0]|=0X04;
	}
	//单位
	int sl=strlen(Units);
	if(sl>0)
	{
		buf[IpntC]=sl; IpntC++;
		memcpy(&buf[IpntC],(const char*)Units,sl); IpntC+=sl;
		buf[0]|=0X02;
	}
	if(Count>0)
	{
		buf[0]|=0X01;
	}
}

void CDlisOut::DLIS_UVARI(const int NVALUE, int &NBYTE,BYTE *CHA)
{
	if(NVALUE<128)
	{
		CHA[0]=BYTE(NVALUE);
		NBYTE=1;
	}
	else if(NVALUE<102400)
	{
		CHA[0]=BYTE((NVALUE>>8)|0X80);
		CHA[1]=BYTE((NVALUE)&0XFF);
		NBYTE=2;
	}
	else
	{
		CHA[0]=BYTE((NVALUE>>24)|0XC0);
		CHA[1]=BYTE((NVALUE>>16)&0XFF);
		CHA[2]=BYTE((NVALUE>>8)&0XFF);
		CHA[3]=BYTE((NVALUE)&0XFF);
		NBYTE=4;
	}
}
void CDlisOut::DLIS_UNORM(const WORD NVALUE, int &NBYTE,BYTE *CHA)
{
	CHA[0]=(NVALUE>>8)&0XFF;
	CHA[1]=NVALUE&0XFF;
	NBYTE=2;
}
void CDlisOut::DLIS_SNORM(const int NVALUE, int &NBYTE,BYTE *CHA)
{
	CHA[0]=(NVALUE>>8)&0XFF;
	CHA[1]=NVALUE&0XFF;
	NBYTE=2;
}
void CDlisOut::DLIS_SLONG(const long NVALUE, int &NBYTE,BYTE *CHA)
{
	CHA[0]=(NVALUE>>24)&0XFF;
	CHA[1]=(NVALUE>>16)&0XFF;
	CHA[2]=(NVALUE>>8)&0XFF;
	CHA[3]=NVALUE&0XFF;
	NBYTE=4;
}
void CDlisOut::DLIS_ULONG(const DWORD NVALUE, int &NBYTE,BYTE *CHA)
{
	CHA[0]=BYTE((NVALUE>>24)&0XFF);
	CHA[1]=BYTE((NVALUE>>16)&0XFF);
	CHA[2]=BYTE((NVALUE>>8)&0XFF);
	CHA[3]=BYTE(NVALUE&0XFF);
	NBYTE=4;
}

void CDlisOut::DLIS_FDOUBL(const double VALUE, int &NBYTE,BYTE *CHA)
{
	union R8CH { double r; BYTE ch[8];};
	union R8CH w;
	w.r=VALUE;
	for(int i=0;i<8;i++) CHA[i]=w.ch[7-i];
	NBYTE=8;
}

void CDlisOut::DLIS_DTIME(WORD Year,BYTE Month,BYTE Day,BYTE Hour,BYTE Minute,BYTE Second,int &NBYTE,BYTE *CHA)
{
	CHA[0]=Year-1900;
	CHA[1]=Month;
	CHA[2]=Day;
	CHA[3]=Hour;
	CHA[4]=Minute;
	CHA[5]=Second;
	CHA[6]=0;
	CHA[7]=0;
	NBYTE=8;
}
void CDlisOut::DLIS_ASCII(const char *ASCII,int &NBYTE,BYTE *buf)
{
	int sl=strlen(ASCII);
	DLIS_UVARI(sl,NBYTE,buf);
	memcpy(&buf[NBYTE],ASCII,sl);
	NBYTE+=sl;
}
void CDlisOut::DLIS_IDENT(const char *ASCII,int &NBYTE,BYTE *buf)
{
	NBYTE=strlen(ASCII);
	buf[0]=NBYTE;
	memcpy(&buf[1],ASCII,NBYTE);
	NBYTE++;
}

void CDlisOut::DLIS_OBNAME(const int Origin,const BYTE CopyNum,const char *Name,int& NBYTE,BYTE *buf)
{
	DLIS_UVARI(Origin,NBYTE,buf);
	buf[NBYTE]=CopyNum;
	NBYTE++;
	int sl=strlen(Name);
	buf[NBYTE]=sl;
	NBYTE++;
	memcpy(&buf[NBYTE],Name,sl);
	NBYTE+=sl;
}
void CDlisOut::DLIS_FSINGL(const float VALUE, int &NBYTE,BYTE *CHA)
{
	union R4CH { float r; BYTE ch[4];};
	union R4CH w;
	w.r=VALUE;
	for(int i=0;i<4;i++) CHA[i]=w.ch[3-i];
	NBYTE=4;
}
