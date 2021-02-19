// DlisInput.h: interface for the CDlisInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISINPUT_H_INCLUDED_)
#define AFX_DLISINPUT_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDlisInput  
{
public:
	CDlisInput();
	virtual ~CDlisInput();
	void UnitsTrans(char Units[],float &Factor);
	bool LengthUnitsToMetersFactor(char *str,double &xb);
	bool TimeUnitsToSecondFactor(char *str,double &xb);
	bool Decode(unsigned char BUFF[], DWORD &IP, int ICOUNT, BYTE IRCD,
			int IVALUE[], float VALUE[], char CVALUE[], int LCVALUE[],
			DWORD IORIGIN[],BYTE ICOPYNUM[]);
	void DLIS_FSHORT(unsigned char CHA[2], float& VALUE, int& NBYTE);
	void DLIS_FSINGL(unsigned char CHA[4], float& VALUE, int& NBYTE);
	void DLIS_FSING1(unsigned char CHA[8], float& VALUE, float& AVALUE, int& NBYTE);
	void DLIS_FSING2(unsigned char CHA[12], float& VALUE, float& AVALUE, float& BVALUE, int& NBYTE);
	void DLIS_ISINGL(unsigned char CHA[4], float& VALUE, int& NBYTE);
	void DLIS_VSINGL(unsigned char CHA[4], float& VALUE, int& NBYTE);
	void DLIS_FDOUBL(unsigned char CHA[8], double& VALUE, int& NBYTE);
	void DLIS_FDOUB1(unsigned char CHA[16],double& VALUE, double& AVALUE, int& NBYTE);
	void DLIS_FDOUB2(unsigned char CHA[24], double& VALUE, double& AVALUE, double& BVALUE, int& NBYTE);
	void DLIS_CSINGL(unsigned char CHA[8], float& RVALUE, float& FVALUE, int& NBYTE);
	void DLIS_CDOUBL(unsigned char CHA[16], double& RVALUE, double& FVALUE, int& NBYTE);
	void DLIS_SSHORT(unsigned char CHA[1], int& IVALUE, int& NBYTE);
	void DLIS_SNORM(unsigned char CHA[2], int& IVALUE, int& NBYTE);
	void DLIS_SLONG(unsigned char CHA[4], int& IVALUE,int& NBYTE);
	void DLIS_USHORT(unsigned char CHA[1], int& IVALUE, int& NBYTE);
	void DLIS_UNORM(unsigned char CHA[2], int& IVALUE, int& NBYTE);
	void DLIS_ULONG(unsigned char CHA[4], int& IVALUE, int& NBYTE);
	void DLIS_UVARI(unsigned char CHA[], int &NVALUE, int &NBYTE);
	void DLIS_IDENT(unsigned char CHA[], int& NBYTE, int& LIDENT);
	void DLIS_ASCII(unsigned char CHA[], int &NBYTE, int &LASCII);
	void DLIS_DTIME(unsigned char CHA[8], int& ITZ, int& IY, int& IM, int& ID,
				int& IH, int& IMN, int& IS, int& IMS, int& NBYTE);
	void DLIS_ORIGIN(unsigned char CHA[], int& NBYTE, int& LORIGIN);
	void DLIS_OBNAME(unsigned char CHA[], int &IVORG, int &IVCOPY, int &NBYTE, int &LOBNAME);
	void DLIS_OBJREF(unsigned char CHA[], int& NBYTET, int& LTYPE,
				 int& IVORG, int& IVCOPY, int& NBYTEO, int& LOBNAME);
	void DLIS_ATTREFD(unsigned char CHA[], int& NBYTET, int& LTYPE,
				int& IVORG, int& IVCOPY, int& NBYTEO, int& LOBNAME,
				int& NBYTEL, int& LLABEL);
	void DLIS_STATUS(unsigned char CHA[1], int& NBYTE, char* Status);
	void DLIS_UNITS(unsigned char CHA[], int& NBYTE, int& LUNITS);
};

#endif // !defined(AFX_DLISINPUT_H_INCLUDED_)
