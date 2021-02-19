// ScanFile.h: interface for the CScanFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANFILE_H_INCLUDED_)
#define AFX_SCANFILE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Slc716R.h"
#include "BitR.h"
#include "Skc92R.h"
#include "Skc98R.h"
#include "Skh2000R.h"
#include "FidRead.h"
#include "WisRead.h"
#include "XtfRead.h"
#include "LisRead.h"
#include "DLisRead.h"
#include "FwdTxtR.h"
#include "FwdLasR.h"
#include "Qh716R.h"
#include "Xb716R.h"

class AFX_EXT_CLASS  CScanFile  
{
public:
	CScanFile();
	virtual ~CScanFile();
	bool SLC16(CString sDataFile);
	bool BIT(CString sDataFile);
	bool SKC92(CString sDataFile);
	bool SKC98(CString sDataFile);
	bool SKH2000(CString sDataFile);
	bool FID(CString sDataFile);
	bool WIS(CString sDataFile);
	bool XTF(CString sDataFile);
	bool LIS(CString sDataFile,int NumPrtFdata,int NumPrtSample,BOOL m_bPrtBaseInfo);
	bool DLIS(CString sDataFile,int NumPrtFdata,int NumPrtSample,BOOL bPrtBaseInfo);
	bool FWDTXT(CString sDataFile);
	bool FWDLAS(CString sDataFile);
	bool QH716(CString sDataFile);
	bool XB716(CString sDataFile);
	FILE* fInfo;
public:
	CXtfRead XtfRead;
	void PrtXtfCurveHead(char *strCurveName);

};

#endif // !defined(AFX_SCANFILE_H_INCLUDED_)
