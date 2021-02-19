// DlisInput.cpp: implementation of the CDlisInput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DlisInput.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlisInput::CDlisInput()
{
	
}

CDlisInput::~CDlisInput()
{
	
}

void CDlisInput::UnitsTrans(char Units[],float &Factor)
{
	CString sUnits=Units;
	//删除单位串中中前后的空字符
	sUnits.TrimLeft();
	sUnits.TrimRight();
	sUnits.MakeLower();
	//找出串中空格
	int n=sUnits.Find(' ');
	double xa=1.0;
	if(n<0)
	{	
		//没有空格存在，单位串是个纯单位
		xa=1.0;
		n=0;
	}
	else
	{
		//有空格存在，单位串是个复合单位
		xa=atof((const char*)(sUnits.Left(n)));
		n++;
	}
	char str[80];
	sprintf(str,"%s",sUnits.Mid(n));
	double xb;
	//长度单位转换
	bool bResult=LengthUnitsToMetersFactor(str,xb);
	if(bResult)
	{
		strcpy(str,"m");
	}
	else
	{
		bResult=TimeUnitsToSecondFactor(str,xb);
		//如果不是长度单位，则用时间单位转换
		if(bResult)
		{
			strcpy(str,"s");	
		}
	}
	///////////////////////////////
	Factor=float(xa*xb);
	strcpy(Units,str);
}
bool CDlisInput::LengthUnitsToMetersFactor(char *str,double &xb)
{
	bool bResult=true;
	xb=1.0;
	if(strcmp(str,"km")==0)
	{
		xb=1000.0;
	}
	else if(strcmp(str,"m")==0)
	{
		xb=1.0;
	}
	else if(strcmp(str,"dm")==0)
	{
		xb=0.1;
	}
	else if(strcmp(str,"cm")==0)
	{
		xb=0.01;
	}
	else if(strcmp(str,"mm")==0)
	{
		xb=0.001;
	}
	else if(strcmp(str,"um")==0)
	{
		xb=1e-6;
	}
	else if(strcmp(str,"nm")==0)
	{
		xb=1e-9;
	}
	else if(strcmp(str,"ft")==0)
	{
		xb=0.3048;
	}
	else if(strcmp(str,"in")==0)
	{
		xb=0.0254;
	}
	else
	{
		bResult=false;
	}
	return bResult;
}
bool CDlisInput::TimeUnitsToSecondFactor(char *str,double &xb)
{
	bool bResult=true;
	xb=1.0f;
	if(strcmp(str,"d")==0)
	{
		xb=24*60*60; //天
	}
	else if(strcmp(str,"h")==0)
	{
		xb=60*60; //时
	}
	else if(strcmp(str,"min")==0)
	{
		xb=60; //分
	}
	else if(strcmp(str,"s")==0)
	{
		xb=1; //秒
	}
	else if(strcmp(str,"ms")==0)
	{
		xb=1e-3; //毫秒
	}
	else if(strcmp(str,"us")==0)
	{
		xb=1e-6; //微秒
	}
	else if(strcmp(str,"ns")==0)
	{
		xb=1e-9; //纳秒
	}
	else
	{
		bResult=false;
	}
	return bResult;
}



bool CDlisInput::Decode(unsigned char BUFF[], DWORD  &IP, int ICOUNT, BYTE IRCD,
						int IVALUE[], float VALUE[], char CVALUE[], int LCVALUE[],
						DWORD IORIGIN[],BYTE ICOPYNUM[])
{
	bool bResult=true;
	int NBYTE, IpntC;
	char* pCVALUE = CVALUE;
	int IVORG, IVCOPY,NBYTET, LTYPE, NBYTEO, LOB;
	float AVALUE, BVALUE, FVALUE;
	double dVALUE, dAVALUE, dBVALUE, dRVALUE, dFVALUE;
	CString cs;
	for(int i=0;i<ICOUNT;i++)
	{
		VALUE[i]=0;
		IVALUE[i]=0;
		CVALUE[i]=0;
		LCVALUE[i]=0;
	}
	IpntC=0;
	switch (IRCD)
	{
	case 1:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_FSHORT(&BUFF[IpntC], VALUE[i], NBYTE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 2:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_FSINGL(&BUFF[IpntC], VALUE[i], NBYTE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 3:
		for (i=0; i<ICOUNT; i++)
		{
			float AVALUE;
			DLIS_FSING1(&BUFF[IpntC], VALUE[i], AVALUE, NBYTE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 4:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_FSING2(&BUFF[IpntC], VALUE[i], AVALUE, BVALUE, NBYTE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 5:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_ISINGL(&BUFF[IpntC], VALUE[i], NBYTE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;  
	case 6:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_VSINGL(&BUFF[IpntC], VALUE[i], NBYTE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 7:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_FDOUBL(&BUFF[IpntC], dVALUE, NBYTE);
			VALUE[i] = float(dVALUE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 8:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_FDOUB1(&BUFF[IpntC], dVALUE, dAVALUE, NBYTE);
			VALUE[i] = float(dVALUE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 9:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_FDOUB2(&BUFF[IpntC], dVALUE, dAVALUE, dBVALUE, NBYTE);
			VALUE[i] = float(dVALUE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 10:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_CSINGL(&BUFF[IpntC], VALUE[i], FVALUE, NBYTE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 11:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_CDOUBL(&BUFF[IpntC], dRVALUE, dFVALUE, NBYTE);
			VALUE[i] = float(dRVALUE);
			IVALUE[i] = int(VALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 12:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_SSHORT(&BUFF[IpntC], IVALUE[i], NBYTE);
			VALUE[i] = float(IVALUE[i]);
			IpntC += NBYTE;
		}
		break;
		
	case 13:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_SNORM(&BUFF[IpntC], IVALUE[i], NBYTE);
			VALUE[i] = float(IVALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 14:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_SLONG(&BUFF[IpntC], IVALUE[i], NBYTE);
			VALUE[i] = (float)IVALUE[i];
			IpntC += NBYTE;
		}
		break;
	case 15:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_USHORT(&BUFF[IpntC], IVALUE[i], NBYTE);
			VALUE[i] = float(IVALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 16:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_UNORM(&BUFF[IpntC], IVALUE[i], NBYTE);
			VALUE[i] = float(IVALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 17:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_ULONG(&BUFF[IpntC], IVALUE[i], NBYTE);
			VALUE[i] = float(IVALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 18:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_UVARI(&BUFF[IpntC], IVALUE[i], NBYTE);
			VALUE[i] = float(IVALUE[i]);
			IpntC += NBYTE;
		}
		break;
	case 19:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_IDENT(&BUFF[IpntC], NBYTE, LCVALUE[i]);
			IpntC += NBYTE;
			memcpy(pCVALUE, &BUFF[IpntC], LCVALUE[i]);
			pCVALUE += LCVALUE[i];
			(*pCVALUE) = '\0';
			pCVALUE ++;
			IpntC += LCVALUE[i];
		}
		break;
	case 20:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_ASCII(&BUFF[IpntC], NBYTE, LCVALUE[i]);
			IpntC += NBYTE;
			memcpy(pCVALUE, &BUFF[IpntC], LCVALUE[i]);
			pCVALUE += LCVALUE[i];
			(*pCVALUE) = '\0';
			pCVALUE ++;
			IpntC += LCVALUE[i];
		}
		break;
	case 21:
		int ITZ,IY,IM,ID,IH,IMN,IS,IMS;
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_DTIME(&BUFF[IpntC], ITZ,IY,IM,ID,IH,IMN,IS,IMS,NBYTE);
			IpntC += NBYTE;
			IY += 1900;
			sprintf(pCVALUE, "%4d/%02d/%02d %02d:%02d:%02d.%d",
				IY,IM,ID,IH,IMN,IS,IMS);
			LCVALUE[i] = strlen(pCVALUE);
			pCVALUE += LCVALUE[i]; 
			(*pCVALUE) = '\0';
			pCVALUE ++;
		}
		break;
	case 22:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_ORIGIN(&BUFF[IpntC], IVALUE[i], NBYTE);
			IpntC += NBYTE;
		}
		break;
	case 23:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_OBNAME(&BUFF[IpntC], IVORG, IVCOPY, NBYTE, LCVALUE[i]);
			IpntC += NBYTE;
			memcpy(pCVALUE, &BUFF[IpntC], LCVALUE[i]);
			pCVALUE += LCVALUE[i];
			(*pCVALUE) = '\0';
			pCVALUE ++;
			IpntC += LCVALUE[i];
			IORIGIN[i]=IVORG;
			ICOPYNUM[i]=IVCOPY;
		}
		break;
	case 24:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_OBJREF(&BUFF[IpntC], NBYTET, LTYPE, IVORG, IVCOPY, NBYTEO, LOB);
			IpntC += NBYTET;
			LCVALUE[i] = LTYPE+1+LOB;
			memcpy(pCVALUE, &BUFF[IpntC], LTYPE);
			pCVALUE += LTYPE;
			(*pCVALUE) = ':';
			pCVALUE ++;
			IpntC += LTYPE+NBYTEO;
			memcpy(pCVALUE, &BUFF[IpntC], LOB);
			pCVALUE += LOB;
			(*pCVALUE) = '\0';
			pCVALUE ++;
			IpntC += LOB;
			IORIGIN[i]=IVORG;
			ICOPYNUM[i]=IVCOPY;
		}
		break;
	case 25:
		(*pCVALUE) = '\0';
		TRACE("\nThis representation code ATTREF can not be present!\n");
		{char ss[64]; TRACE("press Return to quit ..."); gets(ss);}
		break;
	case 26:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_STATUS(&BUFF[IpntC], NBYTE, pCVALUE);
			pCVALUE += NBYTE;
			(*pCVALUE) = '\0';
			pCVALUE ++;
			IpntC += NBYTE;
		}
		break;
	case 27:
		for (i=0; i<ICOUNT; i++)
		{
			DLIS_UNITS(&BUFF[IpntC], NBYTE, LCVALUE[i]);
			IpntC += NBYTE;
			memcpy(pCVALUE, &BUFF[IpntC], LCVALUE[i]);
			pCVALUE += LCVALUE[i];
			(*pCVALUE) = '\0';
			pCVALUE ++;
			IpntC += LCVALUE[i];
		}
		break;  
	default:
		CString s;
		cs.Format("没定义的表示码:%u",IRCD);
		AfxMessageBox(s);
		bResult=false;
		break;
	}
	IP += IpntC;
	return bResult;
}

///////////////////////////////////////////////////////////////////
//
// Code = 1	FSHORT	Low precision floating point (2 bytes)
//
// Byte     |           1             |           2               |
// Bit      | 1   2  3  4  5  6  7  8 | 1  2  3   4    5  6  7  8 |
// Meaning  |-1 |-1 -2 -3 -4 -5 -6 -7  -8 -9 -10 -11 | 3  2  1  0 | 2**I
// Field    | S |              M                     |     E      |
//
// Value = M*2**E
//
void CDlisInput::DLIS_FSHORT(unsigned char CHA[2], float& VALUE, int& NBYTE)
{
	int IX1 = (unsigned)CHA[0];
	int IX2 = (unsigned)CHA[1];
	int IE = IX2%16;
	int IX;
	if (IX1/128==1)
	{
		// Signed 1 (negative digtal)
		IX = (IX1-128)*256+IX2;
		VALUE = -float((IX-IE)*exp((IE-15.)*log(2.)));
	}
	else
	{
		// Signed 0 (Positive digtal)
		IX = IX1*256+IX2;
		VALUE = float((IX-IE)*exp((IE-15.)*log(2.)));
	}
	NBYTE = 2;
}

//////////////////////////////////////////////////////////////////
//
// Code = 2	FSINGL	IEEE SINGLE PRECISION FLOATING POINT (4 bytes)
//
// Byte     |           1             |           2              |
// Bit      | 1   2  3  4  5  6  7  8 | 1    2  3  4  5  6  7  8 |
// Meaning  |-1 | 7  6  5  4  3  2  1   0 | -1 -2 -3 -4 -5 -6 -7 | 2**I
// Field    | S |           E             |          M           |
//
// Byte     |           3             |          4               |
// Bit      | 1   2  3  4  5  6  7  8 | 1    2  3  4  5  6  7  8 |
// Meaning  |-8  -9-10-11-12-13-14-15  -16 -17-18-19-20-21-22-23 | 2**I
// Field    |                       M(continue)                  |
//
// Value =   (-1)**S*(1+M)*2**(E-127) ,  0<E<255 [norm.]
//       =   (-1)**S*M*2**(-126)  ,   E=0, M<>0 [denorm.]
//       =   (-1)**S*0            ,   E=0, M=0
//       =   (-1)**S*G  ,   E=255, M=0 [G, unlimit great number]
//       =   NaN[Not a Number], E=255, M<>0
//
void CDlisInput::DLIS_FSINGL(unsigned char CHA[4], float& VALUE, int& NBYTE)
{
	CString cs;
	int S = ((CHA[0]>>7)==0? 1: -1);
	int E = ((CHA[0]&0x7F)<<1)|(CHA[1]>>7);
	int M = ((CHA[1]&0x7F)<<16)|(CHA[2]<<8)|CHA[3];
	if (E>0 && E<255)
	{
		VALUE = S*(float)((1.0+M*pow(2.0,-23.0))*pow(2.0,E-127.0));
	}
	else if (E==0 && M!=0)
	{
		VALUE = S*float(M*pow(2,-126));
	}
	else if (E==0 && M==0)
	{
		VALUE = 0;
	}
	else if (E==255 && M==0)
	{
		VALUE = S*9999.9f;
	}
	else
	{
		VALUE = -9999.9f;
	}
	NBYTE = 4;
}
//////////////////////////////////////////////////////////////
//
// Code = 3	FSING1	VALIDATED SINGLE PRECISION FLOATING POINT (8 bytes)
//
// Byte     |     1     |     2     |     3     |     4      |
// Field    |                V  (FSINGL)                     |
// Byte     |     5     |     6     |     7     |     8      |
// Field    |                A  (FSINGL)                     |
//
// Value = V,	V is a norminal value with a confidence interval of [V-A, V+A]
//				A >= 0.0
//
void CDlisInput::DLIS_FSING1(unsigned char CHA[8], float& VALUE, float& AVALUE, int& NBYTE)
{
	DLIS_FSINGL(CHA, VALUE, NBYTE);
	DLIS_FSINGL(&CHA[4], AVALUE, NBYTE);
	NBYTE = 8;
}
//////////////////////////////////////////////////////////////
//
// Code = 4	FSING2	TWO-WAY VALIDATED SINGLE PRECISION FLOATING POINT (12 bytes)
//
// Byte     |     1     |     2     |     3     |     4      |
// Field    |                V  (FSINGL)                     |
// Byte     |     5     |     6     |     7     |     8      |
// Field    |                A  (FSINGL)                     |
// Byte     |     9     |    10     |     11    |     12     |
// Field    |                B  (FSINGL)                     |
//
// Value = V,	V is a norminal value with a confidence interval of [V-A, V+B]
//				A,B >= 0.0
//
void CDlisInput::DLIS_FSING2(unsigned char CHA[12], float& VALUE, float& AVALUE, float& BVALUE, int& NBYTE)
{
	DLIS_FSINGL(CHA, VALUE, NBYTE);
	DLIS_FSINGL(&CHA[4], AVALUE, NBYTE);
	DLIS_FSINGL(&CHA[8], BVALUE, NBYTE);
	NBYTE = 12;
}

/////////////////////////////////////////////////////////////////
//
// Code = 5	ISINGL	IBM SINGLE PRECISION FLOATING POINT (4 bytes)
//
// Byte     |           1             |           2             |
// Bit      | 1   2  3  4  5  6  7  8 |  1  2  3  4  5  6  7  8 |
// Meaning  |-1 | 6  5  4  3  2  1  0 | -1 -2 -3 -4 -5 -6 -7 -8 | 2**I
// Field    | S |           E         |           M             |
//
// Byte     |           3             |           4             |
// Bit      | 1   2  3  4  5  6  7  8 |  1  2  3  4  5  6  7  8 |
// Meaning  |-9 -10-11-12-13-14-15-16  -17-18-19-20-21-22-23-24 | 2**I
// Field    |                       M(continue)                 |
//
// Value = (-1)**S*M*16**(E-64)
//       = 0, S=E=M=0
//
void CDlisInput::DLIS_ISINGL(unsigned char CHA[4], float& VALUE, int& NBYTE)
{
	int IX1 = unsigned char(CHA[0]);
	int IX2 = unsigned char(CHA[1]);
	int IX3 = unsigned char(CHA[2]);
	int IX4 = unsigned char(CHA[3]);
	float S = 1;
	if (IX1/128==1)
	{
		IX1 -= 128;
		S = -1;
	}
	int IE = IX1;
	int M = IX2*256*256+IX3*256+IX4;
	if (IE==0 && S>0 && M==0)
	{
		VALUE = 0;
	}
	else
	{
		VALUE = float(IX2*exp((4*IE+16-24-64*4)*log(2.))
			+IX3*exp((4*IE+8-24-64*4)*log(2.))
			+IX4*exp((4*IE-24-64*4)*log(2.)));
		if (S<0) VALUE = -VALUE;
	}
	NBYTE = 4;
}

//////////////////////////////////////////////////////////////////
//
// Code = 6	VSINGL	VAX SINGLE PRECISION FLOATING POINT (4 bytes)
//
// Byte     |           2             |           1              |
// Bit      | 1   2  3  4  5  6  7  8 | 1    2  3  4  5  6  7  8 |
// Meaning  |-1 | 7  6  5  4  3  2  1   0 | -2 -3 -4 -5 -6 -7 -8 | 2**I
// Field    | S |           E             |          M           |
//
// Byte     |           4             |           3              |
// Bit      | 1   2  3  4  5  6  7  8 | 1    2  3  4  5  6  7  8 |
// Meaning  |-9 -10-11-12-13-14-15-16 -17  -18-19-20-21-22-23-24 | 2**I
// Field    |                       M(continue)                  |
//
// Value = (-1)**S*(0.5+M)*2**(E-128),	E>0 [norm.]
//       = 0,							E=S=0, M arbitrary
//       = undefined (reserved),		E=0, S=1
//
void CDlisInput::DLIS_VSINGL(unsigned char CHA[4], float& VALUE, int& NBYTE)
{
	int IX1 = (int)(CHA[1]);
	int IX2 = (int)(CHA[0]);
	float S = 1;
	if (IX1/128==1)
	{
		S = -1;
		IX1 -= 128;
	}
	int IE = IX1*2;
	if (IX2/128==1)
	{
		IE ++;
		IX2 -= 128;
	}
	if (IE>0)
	{
		int M = IX2*65536+CHA[3]*256+CHA[2];
		VALUE = S*float((0.5+M*pow(2,-24))*pow(2,IE-128));
	}
	else if (S>0)
	{
		VALUE = 0;
	}
	else
	{
		VALUE = -9999;
	}
	NBYTE = 4;
}
///////////////////////////////////////////////////////////////////
//
// Code = 7	FDOUBL	IEEE DOUBLE PRECISION FLOATING POINT (8 bytes)
//
// Byte     |                1         |               2          |
// Bit      |  1   2  3  4  5  6  7  8 | 1  2  3  4    5  6  7  8 |
// Meaning  | -1| 10  9  8  7  6  5  4   3  2  1  0 | -1 -2 -3 -4 | 2**I
// Field    | S |                    E              |       M     |
// Byte     |               3          |               4          |
// Bit      |  1   2  3  4  5  6  7  8 | 1  2  3  4    5  6  7  8 |
// Meaning  | -5  -6 -7 -8 -9-10-11-12 -13-14-15-16  -17-18-19-20 | 2**I
// Field    |                        M(continue)                  |
// Byte     |              5           |               6          |
// Bit      |  1   2  3  4  5  6  7  8 | 1  2  3  4    5  6  7  8 |
// Meaning  |-21 -22-23-24-25-26-27-28 -29-30-31-32  -33-34-35-36 | 2**I
// Field    |                        M(continue)                  |
// Byte     |              7           |               8          |
// Bit      |  1   2  3  4  5  6  7  8 | 1  2  3  4    5  6  7  8 |
// Meaning  |-37 -38-39-40-41-42-43-44 -45-46-47-48  -49-50-51-52 | 2**I
// Field    |                        M(continue)                  |
//
// Value = (-1)**S*(1+M)*2**(E-1023),	0<E<2047  [norm.]
//       = (-1)**S*M*2**(-1022),		E=0, M<>0 [denorm.]
//       = (-1)**S*0,					E=0, M=0
//       = (-1)**S*G,					E=2047, M=0 [G, unlimit great number]
//       = NaN[Not a Number]			E=2047, M<>0
//
void CDlisInput::DLIS_FDOUBL(unsigned char CHA[8], double& VALUE, int& NBYTE)
{
	int IX1 = unsigned char(CHA[0]);
	int IX2 = unsigned char(CHA[1]);
	int IX3 = unsigned char(CHA[2]);
	int IX4 = unsigned char(CHA[3]);
	int IX5 = unsigned char(CHA[4]);
	int IX6 = unsigned char(CHA[5]);
	int IX7 = unsigned char(CHA[6]);
	int IX8 = unsigned char(CHA[7]);
	float S = 1;
	if (IX1/128==1)
	{
		S = -1;
		IX1 -= 128;
	}
	int IX2L4 = IX2%16;
	int IE = IX1*16+IX2-IX2L4;
	int M = 1;
	if (IX2L4==0 && IX3==0 && IX4==0 && IX5==0 && IX6==0 && IX7==0 && IX8==0)
	{
		M = 0;
	}
	if (IE>0 && IE<2047)
	{
		VALUE = float(IX2*exp((IE+48-52-1023)*log(2.))
			+IX3*exp((IE+40-52-1023)*log(2.))
			+IX4*exp((IE+32-52-1023)*log(2.))
			+IX5*exp((IE+24-52-1023)*log(2.))
			+IX6*exp((IE+16-52-1023)*log(2.))
			+IX7*exp((IE+8-52-1023)*log(2.))
			+IX8*exp((IE-52-1023)*log(2.))
			+exp((IE-1023)*log(2.)));
		if (S<0) VALUE = -VALUE;
	}
	else if (IE==0 && M!=0)
	{
		VALUE = float(IX2*exp((48-52-1022)*log(2.))
			+IX3*exp((40-52-1022)*log(2.))
			+IX4*exp((32-52-1022)*log(2.))
			+IX5*exp((24-52-1022)*log(2.))
			+IX6*exp((16-52-1022)*log(2.))
			+IX7*exp((8-52-1022)*log(2.))
			+IX8*exp((-52-1022)*log(2.)));
		if (S<0) VALUE = -VALUE;
	}
	else if (IE==0 && M==0)
	{
		VALUE = 0;
	}
	else if (IE==2047)
	{
		// TRACE("Digtal Overflow !\n");
		VALUE = -99999;
	}
	NBYTE = 8;
}

/////////////////////////////////////////////////////////////
//
// Code = 8	FDOUB1	VALIDATED DOUBLE PRECISION FLOATING POINT (16 bytes)
//
// Byte     |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
// Field    |                V  (FDOUBL)                    |
// Byte     |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  16 |
// Field    |                A  (FDOUBL)                    |
//
// Value = V	V is a norminal value with a confidence interval of [V-A, V+A]
//				A>=0.0
//
void CDlisInput::DLIS_FDOUB1(unsigned char CHA[16],double& VALUE, double& AVALUE, int& NBYTE)
{
	DLIS_FDOUBL(CHA, VALUE, NBYTE);
	DLIS_FDOUBL(&CHA[8], AVALUE, NBYTE);
	NBYTE = 16;
}

/////////////////////////////////////////////////////////////
//
// Code = 9	FDOUB2	VALIDATED DOUBLE PRECISION FLOATING POINT (24 bytes)
//
// Byte     |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
// Field    |                V  (FDOUBL)                    |
// Byte     |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  16 |
// Field    |                A  (FDOUBL)                    |
// Byte     |  17 |  18 |  19 |  20 |  21 |  22 |  23 |  24 |
// Field    |                B  (FDOUBL)                    |
//
// Value = V	V is a norminal value with a confidence interval of [V-A, V+B]
//				A, B >= 0.0
//
void CDlisInput::DLIS_FDOUB2(unsigned char CHA[24], double& VALUE, double& AVALUE, double& BVALUE, int& NBYTE)
{
	DLIS_FDOUBL(CHA, VALUE, NBYTE);
	DLIS_FDOUBL(&CHA[8], AVALUE, NBYTE);
	DLIS_FDOUBL(&CHA[16], BVALUE, NBYTE);
	NBYTE = 24;
}

//////////////////////////////////////////////////////////////
//
// Code = 10	CSINGL	SINGLE PRECISION COMPLEX (8 bytes)
//
// Byte     |     1     |     2     |     3     |     4      |
// Field    |                   R (FSINGL)                   |
// Byte     |     5     |     6     |     7     |     8      |
// Field    |                   I (FSINGL)                   |
//
// Value = R+i*I
//					i=sqrt(-1)
//
void CDlisInput::DLIS_CSINGL(unsigned char CHA[8], float& RVALUE, float& FVALUE, int& NBYTE)
{
	DLIS_FSINGL(CHA, RVALUE, NBYTE);
	DLIS_FSINGL(&CHA[4], FVALUE, NBYTE);
	NBYTE = 8;
}

/////////////////////////////////////////////////////////////
//
// Code = 11	CDOUBL	DOUBLE PRECISION COMPLEX (16 bytes)
//
// Byte     |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
// Field    |                R  (FDOUBL)                    |
// Byte     |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  16 |
// Field    |                I  (FDOUBL)                    |
//
// Value = R+i*I
//					i=sqrt(-1)
//
void CDlisInput::DLIS_CDOUBL(unsigned char CHA[16], double& RVALUE, double& FVALUE, int& NBYTE)
{
	DLIS_FDOUBL(CHA, RVALUE, NBYTE);
	DLIS_FDOUBL(&CHA[8], FVALUE, NBYTE);
	NBYTE = 16;
}

/////////////////////////////////////////////////
//
// Code = 12	SSHORT	SHORT SIGNED INTEGER (1 byte)
//
// Byte     |                 1                 |
// Bit      |   1   2   3   4   5   6   7   8   |
// Meaning  |-2**7  6   5   4   3   2   1   0   | 2**I
// Field    |                I                  |
//
// Value = I
//
void CDlisInput::DLIS_SSHORT(unsigned char CHA[1], int& IVALUE, int& NBYTE)
{
	int IX = unsigned char(CHA[0]);
	if (IX>=128)
	{
		IVALUE = -128+(IX-128);
	}
	else
	{
		IVALUE = IX;
	}
	NBYTE = 1;
}

///////////////////////////////////////////////////////////////////
//
// Code = 13	SNORM	NORMAL SIGNED INTEGER (2 bytes)
//
// Byte     |              1             |           2            |
// Bit      |     1  2  3  4  5  6  7  8 | 1  2  3  4  5  6  7  8 |
// Meaning  |-2**15 14 13 12 11 10  9  8 | 7  6  5  4  3  2  1  0 | 2**I
// Field    |                           I                         |
//
// Value = I
//
void CDlisInput::DLIS_SNORM(unsigned char CHA[2], int& IVALUE, int& NBYTE)
{
	union
	{
		unsigned char cc[2];
		short sv;
	} c2s;
	c2s.cc[0] = CHA[1];
	c2s.cc[1] = CHA[0];
	IVALUE = c2s.sv;
	NBYTE = 2;
}

///////////////////////////////////////////////////////////////////////
//
// Code = 14	SLONG	LONG SIGNED INTEGER (4 bytes)
//
// Byte     |                 1              |           2            |
// Bit      |   1   2   3   4   5   6   7  8 | 1  2  3  4  5  6  7  8 |
// Meaning  |-2**31 30  29  28  27  26  25 24| 23 22 21 20 19 18 17 16| 2**I
// Field    |                               I                         |
// Byte     |                 3              |           3            |
// Bit      |  1  2   3   4   5   6   7  8   | 1  2  3  4  5  6  7  8 |
// Meaning  | 15  14  13  12  11  10  9  8   | 7  6  5  4  3  2  1  0 | 2**I
// Field    |                              I(cont)                    |
//
// Value = I
//
void CDlisInput::DLIS_SLONG(unsigned char CHA[4], int& IVALUE,int& NBYTE)
{
	union
	{
		unsigned char cc[4];
		int iv;
	} c2i;
	c2i.cc[0] = CHA[3];
	c2i.cc[1] = CHA[2];
	c2i.cc[2] = CHA[1];
	c2i.cc[3] = CHA[0];
	IVALUE = c2i.iv;
	NBYTE = 4;
}

///////////////////////////////////////////////
//
// Code = 15	USHORT	SHORT UNSIGNED INTEGER (1 byte)
//
// Byte     |               1                 |
// Bit      |  1  2   3   4   5   6   7   8   |
// Meaning  |  7  6   5   4   3   2   1   0   |  2**I
// Field    |               I                 |
//
// Value = I
//
void CDlisInput::DLIS_USHORT(unsigned char CHA[1], int& IVALUE, int& NBYTE)
{
	IVALUE = (int)(CHA[0]);
	NBYTE = 1;
}

/////////////////////////////////////////////////////////////////////
//
// Code = 16	UNORM	NORMAL UNSIGNED INTEGER (2 bytes)
//
// Byte     |               1              |           2            |
// Bit      |  1  2   3   4   5   6   7  8 | 1  2  3  4  5  6  7  8 |
// Meaning  | 15 14  13  12  11  10   9  8 | 7  6  5  4  3  2  1  0 | 2**I
// Field    |                           I                           |
//
// Value = I
//
void CDlisInput::DLIS_UNORM(unsigned char CHA[2], int& IVALUE, int& NBYTE)
{
	int IX1 = CHA[0];
	int IX2 = CHA[1];
	IVALUE = IX1*256+IX2;
	NBYTE = 2;
}
///////////////////////////////////////////////////////////////////////
//
// Code = 17	ULONG	LONG UNSIGNED INTEGER (4 bytes)
//
// Byte     |                 1              |           2            |
// Bit      |   1   2   3   4   5   6   7  8 | 1  2  3  4  5  6  7  8 |
// Meaning  |  31  30  29  28  27  26  25 24 | 23 22 21 20 19 18 17 16| 2**I
// Field    |                               I                         |
// Byte     |                 1              |           2            |
// Bit      |  1  2   3   4   5   6   7  8   | 1  2  3  4  5  6  7  8 |
// Meaning  | 15  14  13  12  11  10  9  8   | 7  6  5  4  3  2  1  0 | 2**I
// Field    |                              I(cont)                    |
//
// Value = I
//
void CDlisInput::DLIS_ULONG(unsigned char CHA[4], int& IVALUE, int& NBYTE)
{
	int IX1 = unsigned char(CHA[0]);
	int IX2 = unsigned char(CHA[1]);
	int IX3 = unsigned char(CHA[2]);
	int IX4 = unsigned char(CHA[3]);
	IVALUE = ((IX1*256+IX2)*256+IX3)*256+IX4;
	NBYTE = 4;
}
///////////////////////////////////////
//
// Code = 18	UVARI	VARIABLE_LENGTH UNSIGNED INTEGER (1 or 2 or 4 bytes)
//
// Byte     |           1             |
// Bit      | 1   2  3  4  5  6  7  8 |
// Meaning  | 0 | 6  5  4  3  2  1  0 | (2**I)
// Field    | L |        I            |
//
// Value = I
//
//////////////////////////////////////////////////////////
// Byte     |             1            |        2        |
// Bit      | 1  2 |  3  4  5  6  7  8 | 1 2 3 4 5 6 7 8 |
// Meaning  | 1  0 | 13 12 11 10  9  8 | 7 6 5 4 3 2 1 0 | (2**I)
// Field    |  L   |                        I            |
//
// Value = I
//
//////////////////////////////////////////////////////////////////
// Byte     |             1            |            2            |
// Bit      | 1  2 |  3  4  5  6  7  8 |  1  2  3  4  5  6  7  8 |
// Meaning  | 1  1 | 29 28 27 26 25 24 | 23 22 21 20 19 18 17 16 | (2**I)
// Field    |  L   |                        I                    |
// Byte     |             3            |        4        |
// Bit      | 1   2   3  4  5  6  7  8 | 1 2 3 4 5 6 7 8 |
// Meaning  | 15 14  13 12 11 10  9  8 | 7 6 5 4 3 2 1 0 | (2**I)
// Field    |                     I(continue)            |
//
// Value = I
//
void CDlisInput::DLIS_UVARI(unsigned char CHA[], int &NVALUE, int &NBYTE)
{
	int IX1 = (int)(CHA[0]);
	if (IX1<128)
	{
		NVALUE = IX1;
		NBYTE = 1;
	}
	else 
	{
		if (IX1-128<64)
		{
			int IX2 = (int)(CHA[1]);
			NVALUE = (IX1-128)*256+IX2;
			NBYTE = 2;
		}
		else
		{
			int IX2 = (int)(CHA[1]);
			int IX3 = (int)(CHA[2]);
			int IX4 = (int)(CHA[3]);
			NVALUE = (((IX1-128-64)*256+IX2)*256+IX3)*256+IX4;
			NBYTE = 4;
		}
	}
}
//////////////////////////////////////////////////
//
// Code = 19	IDENT	VARIABLE_LENGTH IDENTif IER (N+1 Bytes)
//
// Byte     |        1        |     2 to (1+N)   |
// Meaning  |   L(USHORT)     |  ASCII Character |
//
//     L = Length of Character String(N)
//
void CDlisInput::DLIS_IDENT(unsigned char CHA[], int& NBYTE, int& LIDENT)
{
	LIDENT = (int)CHA[0];
	NBYTE = 1;
}


/////////////////////////////////////////////////
//
// Code = 20	ASCII	VARIABLE_LENGTH ASCII CHARACTER STRING (N+K Bytes)
//
// Byte     |   1 to k       |  (K+1) to (K+N)  |
// Meaning  |   L(UVARI)     |  ASCII Character |
//
//     L = Length of Character String(N)
//     K = Length of UVARI Bytes
//
void CDlisInput::DLIS_ASCII(unsigned char CHA[], int& NBYTE, int& LASCII)
{
	DLIS_UVARI(CHA, LASCII, NBYTE);
}
////////////////////////////////////////////////////////////////
//
// Code = 21	DTIME	DATE/TIME (8 bytes)
//
// Byte     |            1           |            2            |
// Bit      | 1  2  3  4  5  6  7  8 | 1  2  3  4   5  6  7  8 |
// Meaning  | 7  6  5  4  3  2  1  0 | 3  2  1  0 | 3  2  1  0 | (2**I)
// Field    |            Y           |     TZ     |     M      |
// Byte     |            3           |            4            |
// Bit      | 1  2  3  4  5  6  7  8 | 1  2  3  4   5  6  7  8 |
// Meaning  | 7  6  5  4  3  2  1  0 | 7  6  5  4   3  2  1  0 | (2**I)
// Field    |            D           |            H            |
// Byte     |            5           |            6            |
// Bit      | 1  2  3  4  5  6  7  8 | 1  2  3  4   5  6  7  8 |
// Meaning  | 7  6  5  4  3  2  1  0 | 7  6  5  4   3  2  1  0 | (2**I)
// Field    |           MN           |            S            |
// Byte     |            7           |            8            |
// Bit      | 1  2  3  4  5  6  7  8 | 1  2  3  4   5  6  7  8 |
// Meaning  | 15 14 13 12 11 10 9  8 | 7  6  5  4   3  2  1  0 | (2**I)
// Field    |                        MS                        |
//
//     Y = Years Since 1990 (0-255).
//    TZ = Time Zone ( 0 = Local Standard.
//                     1 = Local Daylight Savings.
//                     2 = Greenwich Mean Time )
//     M = Month of the Year (1-12).
//     D = Day of Month (1-31).
//     H = Hours Since Midnight (0-23).
//    MN = Minutes Past Hours (0-59).
//     S = Seconds Past Minutes (0-59).
//    MS = Milliseconds Past Seconds (0-999)
//
void CDlisInput::DLIS_DTIME(unsigned char CHA[8], int& ITZ, int& IY, int& IM, int& ID,
							int& IH, int& IMN, int& IS, int& IMS, int& NBYTE)
{
	IY = (int)CHA[0];
	int KX = (int)CHA[1];
	IM = KX%16;
	ITZ = KX-IM;
	ID = (int)(CHA[2]);
	IH = (int)(CHA[3]);
	IMN = (int)(CHA[4]);
	IS = (int)(CHA[5]);
	IMS = (int)(CHA[6])*256+(int)(CHA[7]);
	NBYTE = 8;
}
////////////////////////////////////////////////////////////////
//
// Code = 22	ORIGIN	ORIGIN REFERENCE: = UVARI
//
//      (See Code[18] UVARI)
//
void CDlisInput::DLIS_ORIGIN(unsigned char CHA[], int& NBYTE, int& LORIGIN)
{
	DLIS_UVARI(CHA, LORIGIN, NBYTE);
}
////////////////////////////////////////////////////////////
//
// Code = 23	OBNAME	OBJECT NAME( K+2+N Bytes).
//
// Byte     |    1 to K       |     K+1      |   K+2+N     |
// Meaning  |   O(ORIGIN)     |   C(USHORT)  |   I(IDENT)  |
//
//    O =  Origin Reference L (see code[22] ORIGIN).
//    C =  Copy Number.
//    I =  Identifier.
//
void CDlisInput::DLIS_OBNAME(unsigned char CHA[], int& IVORG, int& IVCOPY, int& NBYTE, int& LOBNAME)
{
	DLIS_UVARI(CHA, IVORG, NBYTE);
	IVCOPY = CHA[NBYTE];
	NBYTE ++;
	LOBNAME = CHA[NBYTE];
	NBYTE ++;
}
////////////////////////////////////////////////
//
// Code = 24	OBJREF	OBJECT REFERENCE (K+1+I Bytes)
//
// Byte     |    1 to K      |   K+1 to K+1+I  |
// Meaning  |   T(IDENT)     |     N(OBNAME)   |
//
//    T =  Object Type.
//    N =  Object Name.
//
void CDlisInput::DLIS_OBJREF(unsigned char CHA[], int& NBYTET, int& LTYPE,
							 int& IVORG, int& IVCOPY, int& NBYTEO, int& LOBNAME)
{
	DLIS_IDENT(CHA, NBYTET, LTYPE);
	int LT = NBYTET+LTYPE;
	DLIS_OBNAME(&CHA[LT], IVORG, IVCOPY, NBYTEO, LOBNAME);
}
//////////////////////////////////////////////////////////////
//
// Code = 25	ATTREF	ATTRIBUTION REFERENCE (K+J+1+I bytes)
//
// Byte     |    1 to K       |  K+1 to K+J  | K+J+1 to K+J+1+I |
// Meaning  |   T(IDENT)      |   N(OBNAME)  |      L(IDENT)    |
//
//    T =  Object Type.
//    N =  Object Name.
//    L =  Attribution Label.
//
void CDlisInput::DLIS_ATTREFD(unsigned char CHA[], int& NBYTET, int& LTYPE,
							  int& IVORG, int& IVCOPY, int& NBYTEO, int& LOBNAME,
							  int& NBYTEL, int& LLABEL)
{
	DLIS_IDENT(CHA, NBYTET, LTYPE);
	int LT = NBYTET+LTYPE;
	DLIS_OBNAME(&CHA[LT], IVORG, IVCOPY, NBYTEO, LOBNAME);
	LT = NBYTET+LTYPE+NBYTEO+LOBNAME;
	DLIS_IDENT(&CHA[LT], NBYTEL, LLABEL);
}
/////////////////////////////////////////////////
//
// Code = 26	STATUS	BOOLEAN STATUS VALUE (1 Byte).
//
// Byte     |        1          |
// Meaning  |     S(USHORT)     |
//
// Value = ALLOWED = TRUE = ON,			if  S = 1
//       = DISALLOWED = FALSE = OFF,	if  S = 0
//
void CDlisInput::DLIS_STATUS(unsigned char CHA[1], int& NBYTE, char* Status)
{
	Status[0] = (CHA[0]==0? 'F': 'T');
	NBYTE = 1;
}

/////////////////////////////////////////////////
//
// Code = 27	UNITS	UNITS expRESSION (N+1 Bytes)
//
// Byte     |      1         |      2 to N+1    |
// Meaning  |   L(USHORT)    | ASCII Characters |
//
//     L =  Length of Units expression(N)
//
void CDlisInput::DLIS_UNITS(unsigned char CHA[], int& NBYTE, int& LUNITS)
{
	LUNITS = CHA[0];
	NBYTE = 1;
}


