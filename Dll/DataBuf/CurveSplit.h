// CurveSplit.h: interface for the CCurveSplit class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CURVESPIT_H__
#define __CURVESPIT_H__

#include "CodeSP.h"
#include "FidRead.h"
#include "FidWrite.h"

class AFX_EXT_CLASS CCurveSplit  
{
public:
	CCurveSplit();
	virtual ~CCurveSplit();
	bool CurveSplit(CString FidFile,CString sCurveName);

private:
	CFidRead *FidRead; 
	CFidWrite *FidWrite; 
	CString sWellName; //����
	int Index; //����������
	CString sPathName; //·����
};

#endif 
