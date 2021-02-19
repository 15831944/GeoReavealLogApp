// DlisOut.h: interface for the CDlisOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISOUT_H_INCLUDED_)
#define AFX_DLISOUT_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dlis_Head.h"

class CDlisOut  
{
public:
	CDlisOut();
	virtual ~CDlisOut();
	void Encode_DEFAULT(int &IP,BYTE *buf); //缺省设置
	void Encode_NULL(int &IP,BYTE *buf); //空值(无值)
	void Encode_DTIME(WORD Year,BYTE Month,BYTE Day,BYTE Hour,BYTE Minute,BYTE Second,int &IP,BYTE *buf);
	void Encode_CRU(const int Count,const BYTE RepC,char *Units,int &IpntC,BYTE *buf);
	void Encode_FSINGL(int &IP,BYTE *buf,const float *VALUE,char *Units="",const BYTE RepC=0,const int Count=1);
	void Encode_UNORM(int &IP,BYTE *buf,const int *NVALUE,char *Units="",const BYTE RepC=0,const int Count=1);
	void Encode_UVARI(int &IP,BYTE *buf,int *NVALUE,char *Units="",const BYTE RepC=0,const int Count=1);
	void Encode_ASCII(int &IP,BYTE *buf,const char *str,char *Units="",const BYTE RepC=0,const int Count=1);
	void Encode_IDENT(int &IP,BYTE *buf,const char *str,char*Units="",const BYTE RepC=0,const int Count=1);
	void Encode_USHORT(int &IP,BYTE *buf,const BYTE *NVALUE,char *Units="",const BYTE RepC=0,const int Count=1);
	void Encode_UNITS(int &IP,BYTE *buf,const char *str,char *Units="",const BYTE RepC=0,const int Count=1);
	void Encode_OBNAME(int &IP,BYTE *buf,const int *Origin,const BYTE *CopyNum,const char *Name,const int Count=1);

public:
	void DLIS_FSINGL(const float VALUE, int &NBYTE,BYTE *CHA);
	void DLIS_UNORM(const WORD NVALUE, int &NBYTE,BYTE *CHA);
	void DLIS_SNORM(const int NVALUE, int &NBYTE,BYTE *CHA);
	void DLIS_UVARI(const int NVALUE, int &NBYTE,BYTE *CHA);
	void DLIS_ASCII(const char *ASCII,int &NBYTE,BYTE *buf);
	void DLIS_IDENT(const char *ASCII,int &NBYTE,BYTE *buf);
	void DLIS_SLONG(const long NVALUE, int &NBYTE,BYTE *CHA);
	void DLIS_ULONG(const DWORD NVALUE, int &NBYTE,BYTE *CHA);
	void DLIS_FDOUBL(const double VALUE, int &NBYTE,BYTE *CHA);
	void DLIS_DTIME(WORD Year,BYTE Month,BYTE Day,BYTE Hour,BYTE Minute,BYTE Second,int &NBYTE,BYTE *CHA);
	void DLIS_OBNAME(const int Origin,const BYTE CopyNum,const char *ObName,int& NBYTE,BYTE *buf);

};

#endif // !defined(AFX_DLISOUT_H_INCLUDED_)
